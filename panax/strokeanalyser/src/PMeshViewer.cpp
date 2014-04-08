/* PMeshViewer.cpp

   3D mesh viewer.
   
   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#include "PMeshViewer.h"
#include <QtGui>
#include "vtkCommand.h"
#include "vtkOBJReader.h"
#include "vtkPLYReader.h"
#include "vtkSTLReader.h"
#include "vtkPLYWriter.h"
#include "vtkSTLWriter.h"
#include "vtkWindowToImageFilter.h"
#include "vtkJPEGWriter.h"
#include "vtkPNGWriter.h"
#include "vtkTIFFWriter.h"
#include "vtkCamera.h"
#include "vtkProperty.h"
#include "vtkCellArray.h"

#include <unistd.h>
using namespace std;


// Callback class

class PMeshViewerCallback: public vtkCommand
{
public:
    static PMeshViewerCallback *New() { return new PMeshViewerCallback; }
    void Execute(vtkObject *caller, unsigned long eventId, void *callData);
    PMeshViewer *viewer;

protected:
    PMeshViewerCallback();
    ~PMeshViewerCallback();
    
// private:
//     vtkPointData *pointData;
};


// PMeshViewer class

PMeshViewer::PMeshViewer()
{
    // Initialisation
    appName = QString("Mesh Viewer");
    projectName = QString("Untitled");
    loaded = false;
    hideFrontFace = 0;
    
    // Create GUI
    createWidgets();
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    initSize();
}


PMeshViewer::~PMeshViewer()
{
    uninstallPipeline();
    style->Delete();
}


// Event handling

bool PMeshViewer::eventFilter(QObject *target, QEvent *event)
{
    if (target == meshTable and event->type() == QEvent::KeyPress)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Delete)
        {
            deleteMesh();
            return true;
        }
    }
    return QMainWindow::eventFilter(target, event);
}


void PMeshViewer::closeEvent(QCloseEvent *event)
{
    meshTable->hide();
    event->accept();
}


void PMeshViewer::setInput(vtkAlgorithmOutput *input)
{
}


void PMeshViewer::updateNow()
{
}


// Create window widgets

#define NumRow 0
#define NumColumn 4

void PMeshViewer::createWidgets()
{
    // Create vtk objects
    vtkWidget = new QVTKWidget(this);

    // QVTKWidget has already created a render window and interactor
    renderWindow = vtkWidget->GetRenderWindow();
    renderer = vtkRenderer::New();
    renderer->SetBackground(0.0, 0.0, 0.0);
    renderWindow->AddRenderer(renderer);
    
    interactor = renderWindow->GetInteractor();
    style = vtkInteractorStyleTrackballCamera::New();
    interactor->SetInteractorStyle(style);

    PMeshViewerCallback *callback = PMeshViewerCallback::New();
    callback->viewer = this;
    interactor->AddObserver(vtkCommand::RightButtonPressEvent, callback);
    callback->Delete();
    
    // Central widget
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(vtkWidget, 0, 0);
    QWidget *main = new QWidget;
    main->setLayout(grid);
    setCentralWidget(main); 
    
    // Overall
    setWindowTitle(appName);
    setWindowIcon(QIcon(":/images/panax-icon.png"));
    
    // Create mesh mode box
    meshModeBox = new QComboBox(this);
    QStringList choices;
    choices << "smooth surface" << "flat surface" << "flat + lines" 
        << "wire frame";
    meshModeBox->insertItems(0, choices);
    connect(meshModeBox, SIGNAL(activated(int)), this,
        SLOT(setMeshMode(int)));
    
    // Create mesh table
    meshTable = new QTableWidget(NumRow, NumColumn);
    QPalette palette = meshTable->palette();

    palette.setColor(QPalette::Base, QColor(248, 248, 248));
    meshTable->setPalette(palette);
    meshTable->verticalHeader()->hide();
    meshTable->setShowGrid(false);
    meshTable->setSelectionMode(QTableWidget::ContiguousSelection);
    
    QStringList labels;
    labels << "" << "Colour" << "Name";
    meshTable->setHorizontalHeaderLabels(labels);
    QTableWidgetItem *headerItem = new QTableWidgetItem(QString("Source"));
    headerItem->setTextAlignment(Qt::AlignLeft);
    meshTable->setHorizontalHeaderItem(3, headerItem);
    
    QList<QHeaderView::ResizeMode> modes;
    modes << QHeaderView::Fixed << QHeaderView::Fixed
          << QHeaderView::Interactive << QHeaderView::Interactive;
    QHeaderView *header = meshTable->horizontalHeader();
    for (int i = 0; i < NumColumn; ++i)
        header->setResizeMode(i, modes[i]);
    
    QList<int> widths;
    widths << 30 << 50 << 100 << 200; 
    int tableWidth = 0;
    for (int i = 0; i < NumColumn; ++i)
    {
        meshTable->setColumnWidth(i, widths[i]);
        tableWidth += widths[i];
    }
    meshTable->resize(tableWidth + 25, 200);
    connect(meshTable, SIGNAL(cellClicked(int, int)),
        this, SLOT(setVisibility(int, int)));
    connect(meshTable, SIGNAL(cellDoubleClicked(int, int)),
        this, SLOT(setColor(int, int)));
    connect(meshTable, SIGNAL(cellClicked(int, int)),
        this, SLOT(blink(int, int)));
    meshTable->installEventFilter(this);
    
    // Create dock widget for right dock area.
    rightDockWidget = new QDockWidget("Mesh List");
    rightDockWidget->setWidget(meshTable);
    rightDockWidget->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, rightDockWidget);
}


void PMeshViewer::createActions()
{
    newProjectAction = new QAction(tr("&New Project"), this);
    newProjectAction->setIcon(QIcon(":/images/new.png"));
    newProjectAction->setShortcut(tr("Ctrl+N"));
    newProjectAction->setStatusTip(tr("Create a new project"));
    connect(newProjectAction, SIGNAL(triggered()),
        this, SLOT(newProject()));
    
    openProjectAction = new QAction(tr("&Open Project"), this);
    openProjectAction->setIcon(QIcon(":/images/open.png"));
    openProjectAction->setShortcut(tr("Ctrl+O"));
    openProjectAction->setStatusTip(tr("Open a project file"));
    connect(openProjectAction, SIGNAL(triggered()),
        this, SLOT(openProject()));

    saveProjectAction = new QAction(tr("&Save Project"), this);
    saveProjectAction->setIcon(QIcon(":/images/save.png"));
    saveProjectAction->setShortcut(tr("Ctrl+S"));
    saveProjectAction->setStatusTip(tr("Save project to file"));
    connect(saveProjectAction, SIGNAL(triggered()),
        this, SLOT(saveProject()));

    saveProjectAsAction = new QAction(tr("Save Project &As"), this);
    saveProjectAsAction->setIcon(QIcon(":/images/saveas.png"));
    saveProjectAsAction->setShortcut(tr("Ctrl+A"));
    saveProjectAsAction->setStatusTip(tr("Save project to another file"));
    connect(saveProjectAsAction, SIGNAL(triggered()),
        this, SLOT(saveProjectAs()));
    
    loadDirAction = new QAction(tr("Load Mesh by &Directory"), this);
    loadDirAction->setIcon(QIcon(":/images/loaddir.png"));
    loadDirAction->setShortcut(tr("Ctrl+D"));
    loadDirAction->setStatusTip(tr("Load mesh files in a directory"));
    connect(loadDirAction, SIGNAL(triggered()), this, SLOT(loadDir()));
    
    loadMeshAction = new QAction(tr("Load Mesh by &File"), this);
    loadMeshAction->setIcon(QIcon(":/images/loadmesh.png"));
    loadMeshAction->setShortcut(tr("Ctrl+F"));
    loadMeshAction->setStatusTip(tr("Load meshes by file names"));
    connect(loadMeshAction, SIGNAL(triggered()), this, SLOT(loadMesh()));
    
    addMeshAction = new QAction(tr("Add Mesh by File"), this);
    addMeshAction->setIcon(QIcon(":/images/addmesh.png"));
    addMeshAction->setStatusTip(tr("Add meshes by file names"));
    connect(addMeshAction, SIGNAL(triggered()), this, SLOT(addMesh()));
    
    deleteMeshAction = new QAction(tr("Delete Mesh"), this);
    deleteMeshAction->setIcon(QIcon(":/images/deletemesh.png"));
    deleteMeshAction->setStatusTip(tr("Delete loaded meshes"));
    connect(deleteMeshAction, SIGNAL(triggered()), this, SLOT(deleteMesh()));
    
    saveDirPlyAction = new QAction(tr("in PLY format"), this);
    connect(saveDirPlyAction, SIGNAL(triggered()), this, SLOT(saveDirPly()));

    saveDirStlAction = new QAction(tr("in STL format"), this);
    connect(saveDirStlAction, SIGNAL(triggered()), this, SLOT(saveDirStl()));
    
    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit Mesh Viewer"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    
    saveViewAction = new QAction(tr("Save &View"), this);
    saveViewAction->setIcon(QIcon(":/images/saveview.png"));
    saveViewAction->setShortcut(tr("Shift+V"));
    saveViewAction->setStatusTip(tr("Save current view to a file"));
    connect(saveViewAction, SIGNAL(triggered()), this, SLOT(saveView()));
    
    frontFaceAction = new QAction(tr("&Hide Front Faces"), this);
    frontFaceAction->setIcon(QIcon(":/images/hide.png"));
    frontFaceAction->setShortcut(tr("Shift+H"));
    frontFaceAction->setStatusTip(tr("Hide/show front faces"));
    frontFaceAction->setCheckable(true);
    connect(frontFaceAction, SIGNAL(triggered()), this,
        SLOT(toggleFrontFace()));
        
    smoothSurfaceAction = new QAction(tr("&Smooth Surface"), this);
    smoothSurfaceAction->setShortcut(tr("Shift+S"));
    smoothSurfaceAction->setStatusTip(tr("Smooth surface"));
    smoothSurfaceAction->setCheckable(true);
    connect(smoothSurfaceAction, SIGNAL(triggered()), this,
        SLOT(setSmoothSurface()));
        
    flatSurfaceAction = new QAction(tr("&Flat Surface"), this);
    flatSurfaceAction->setShortcut(tr("Shift+F"));
    flatSurfaceAction->setStatusTip(tr("Flat surface"));
    flatSurfaceAction->setCheckable(true);
    connect(flatSurfaceAction, SIGNAL(triggered()), this,
        SLOT(setFlatSurface()));
        
    flatLinesAction = new QAction(tr("Flat &Lines"), this);
    flatLinesAction->setShortcut(tr("Shift+L"));
    flatLinesAction->setStatusTip(tr("Flat surface & lines"));
    flatLinesAction->setCheckable(true);
    connect(flatLinesAction, SIGNAL(triggered()), this,
        SLOT(setFlatLines()));
        
    wireFrameAction = new QAction(tr("&Wire Frame"), this);
    wireFrameAction->setShortcut(tr("Shift+W"));
    wireFrameAction->setStatusTip(tr("Wire frame"));
    wireFrameAction->setCheckable(true);
    connect(wireFrameAction, SIGNAL(triggered()), this,
        SLOT(setWireFrame()));
        
    QActionGroup *meshModeGroup = new QActionGroup(this);
    meshModeGroup->addAction(smoothSurfaceAction);
    meshModeGroup->addAction(flatSurfaceAction);
    meshModeGroup->addAction(flatLinesAction);
    meshModeGroup->addAction(wireFrameAction);
    smoothSurfaceAction->setChecked(true);
    
    toggleRightDockWidgetAction = rightDockWidget->toggleViewAction();
    toggleRightDockWidgetAction->setIcon(QIcon(":/images/list.png"));
       
    infoAction = new QAction(tr("&Info"), this);
    infoAction->setIcon(QIcon(":/images/info.png"));
    infoAction->setShortcut(tr("Ctrl+I"));
    infoAction->setStatusTip(tr("Data information"));
    connect(infoAction, SIGNAL(triggered()), this,
        SLOT(info()));

    helpAction = new QAction(tr("&Help"), this);
    helpAction->setIcon(QIcon(":/images/help.png"));
    helpAction->setShortcut(tr("Ctrl+H"));
    helpAction->setStatusTip(tr("Help information"));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));
        
    aboutAction = new QAction(tr("About"), this);
    aboutAction->setStatusTip(tr("About this software"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}


void PMeshViewer::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newProjectAction);
    fileMenu->addAction(openProjectAction);
    fileMenu->addAction(saveProjectAction);
    fileMenu->addAction(saveProjectAsAction);
    fileMenu->addSeparator();

    fileMenu->addAction(loadDirAction);
    fileMenu->addAction(loadMeshAction);
    fileMenu->addAction(addMeshAction);
    
    saveMeshMenu = fileMenu->addMenu(tr("&Save Visible Meshes"));
    saveMeshMenu->setIcon(QIcon("images/savedir.png"));
    saveMeshMenu->addAction(saveDirPlyAction);
    saveMeshMenu->addAction(saveDirStlAction);
    
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(saveViewAction);
    viewMenu->addAction(frontFaceAction);
    
    meshModeMenu = viewMenu->addMenu(tr("&Mesh Mode"));
    meshModeMenu->addAction(smoothSurfaceAction);
    meshModeMenu->addAction(flatSurfaceAction);
    meshModeMenu->addAction(flatLinesAction);
    meshModeMenu->addAction(wireFrameAction);

    viewMenu->addAction(toggleRightDockWidgetAction);
    viewMenu->addAction(infoAction);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
}


void PMeshViewer::createToolBars()
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addAction(newProjectAction);
    fileToolBar->addAction(openProjectAction);
    fileToolBar->addAction(saveProjectAction);
    fileToolBar->addAction(loadDirAction);
    fileToolBar->addAction(loadMeshAction);
    fileToolBar->addAction(addMeshAction);
    
    viewToolBar = addToolBar(tr("&View"));
    viewToolBar->addAction(saveViewAction);
    viewToolBar->addAction(frontFaceAction);
    viewToolBar->addWidget(meshModeBox);
    viewToolBar->addAction(toggleRightDockWidgetAction);
    viewToolBar->addAction(infoAction);

    helpToolBar = addToolBar(tr("&Help"));
    helpToolBar->addAction(helpAction);
}


void PMeshViewer::createStatusBar()
{
    statusBar()->showMessage(tr(""));
}


// Slot methods

void PMeshViewer::newProject()
{
    uninstallPipeline();
    renderer = vtkRenderer::New();
    renderer->SetBackground(0.0, 0.0, 0.0);
    renderWindow->AddRenderer(renderer);
    renderWindow->Render();
}


void PMeshViewer::openProject()
{
}


void PMeshViewer::saveProject()
{
}


void PMeshViewer::saveProjectAs()
{
}


void PMeshViewer::loadDir()
{
    QString dirName = QFileDialog::getExistingDirectory(this,
        tr("Load all mesh files in a directory"), ".");
        
    if (!dirName.isEmpty())
    {
        QDir dir(dirName);
        QStringList filters;
        filters << "*.obj" << "*.ply" << "*.stl";
        QStringList list = dir.entryList(filters);
        
        if (!list.isEmpty())
        {
            QStringList fileNames;
            for (int i = 0; i < list.size(); ++i)
                fileNames.append(dirName + "/" + list[i]);
            QApplication::setOverrideCursor(Qt::WaitCursor);
            loadMesh(fileNames);
            QApplication::restoreOverrideCursor();
        }
    }
}


void PMeshViewer::loadMesh()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
        tr("Load one or more mesh files"), ".",
        tr("3D mesh files: *.obj, *.ply, *.stl (*.obj *.ply *.stl)"));
        
    if (!fileNames.isEmpty())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        loadMesh(fileNames);
        QApplication::restoreOverrideCursor();
    }
}


void PMeshViewer::addMesh()
{
    QStringList fileNames = QFileDialog::getOpenFileNames(this,
        tr("Add one or more mesh files"), ".",
        tr("3D mesh files: *.obj, *.ply, *.stl (*.obj *.ply *.stl)"));
        
    if (!fileNames.isEmpty())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        addMesh(fileNames);
        QApplication::restoreOverrideCursor();
    }
}


void PMeshViewer::deleteMesh()
{
    QList<QTableWidgetSelectionRange> list = meshTable->selectedRanges();
    if (list.isEmpty())
        return;
        
    QTableWidgetSelectionRange range = list[0];  // Only 1 range.
    for (int i = range.bottomRow(); i >= range.topRow(); --i)
    {
        meshTable->removeRow(i);
        renderer->RemoveActor(meshList[i].actor);
        meshList[i].normals->Delete();
        meshList[i].mapper->Delete();
        meshList[i].actor->Delete();
        meshList.removeAt(i);
    }

    renderWindow->Render();
}


void PMeshViewer::saveDirPly()
{
    QString dirName = QFileDialog::getExistingDirectory(this,
        tr("Save visible meshes into a directory in PLY format"), ".");
        
    if (!dirName.isEmpty() && !meshList.isEmpty())
    {
        for (int i = 0; i < meshList.size(); ++i)
            if (meshList[i].actor->GetVisibility())
            {
                QString fileName = dirName + QString("/%1.ply").
                    arg(i, 8, 10, QChar('0'));
                vtkPLYWriter *writer = vtkPLYWriter::New();
                writer->SetFileName(fileName.toAscii().data());
                writer->SetInput(meshList[i].data);
                writer->Update();
                writer->Delete();
            }
    }
}


void PMeshViewer::saveDirStl()
{
    QString dirName = QFileDialog::getExistingDirectory(this,
        tr("Save visible meshes into a directory in STL format"), ".");
        
    if (!dirName.isEmpty() && !meshList.isEmpty())
    {
        for (int i = 0; i < meshList.size(); ++i)
            if (meshList[i].actor->GetVisibility())
            {
                QString fileName = dirName + QString("/%1.stl").
                    arg(i, 8, 10, QChar('0'));
                vtkSTLWriter *writer = vtkSTLWriter::New();
                writer->SetFileName(fileName.toAscii().data());
                writer->SetInput(meshList[i].data);
                writer->Update();
                writer->Delete();
            }
    }
}


void PMeshViewer::saveView()
{
    if (!loaded)  // Nothing to save.
        return;
        
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save current view to a file"), ".",
        tr("Image files (*.jpg *.png *.tif)"));
        
    if (fileName.isEmpty())
        return;

    saveImage(fileName);
}


void PMeshViewer::toggleFrontFace()
{
    if (meshList.isEmpty())
        return;
        
    hideFrontFace = !hideFrontFace;
    
    for (int i = 0; i < meshList.size(); ++i)
        meshList[i].actor->GetProperty()->
            SetFrontfaceCulling(hideFrontFace);
    renderWindow->Render();
}


void PMeshViewer::setMeshMode(int mode)
{
    int rep, interpol, edgeOn;
    
    switch (mode)
    {
        case 0: // smooth surface
            rep = VTK_SURFACE;
            interpol = VTK_GOURAUD;
            edgeOn = 0;
            break;
            
        case 1: // flat surface
            rep = VTK_SURFACE;
            interpol = VTK_FLAT;
            edgeOn = 0;
            break;
            
        case 2: // surface + lines
            rep = VTK_SURFACE;
            interpol = VTK_FLAT;
            edgeOn = 1;
            break;
            
        case 3: // wire frame
            rep = VTK_WIREFRAME;
            interpol = VTK_FLAT;
            edgeOn = 1;
            break;
            
        default: // smooth surface
            rep = VTK_SURFACE;
            interpol = VTK_GOURAUD;
            edgeOn = 0;
            break;
    }
    
    for (int i = 0; i < meshList.size(); ++i)
    {
        vtkProperty *property = meshList[i].actor->GetProperty();
        property->SetRepresentation(rep);
        property->SetInterpolation(interpol);
        property->SetEdgeVisibility(edgeOn);
    }
    
    renderWindow->Render();
}


void PMeshViewer::setSmoothSurface()
{
    meshModeBox->setCurrentIndex(0);
    setMeshMode(0);
}


void PMeshViewer::setFlatSurface()
{
    meshModeBox->setCurrentIndex(1);
    setMeshMode(1);
}


void PMeshViewer::setFlatLines()
{
    meshModeBox->setCurrentIndex(2);
    setMeshMode(2);
}


void PMeshViewer::setWireFrame()
{
    meshModeBox->setCurrentIndex(3);
    setMeshMode(3);
}


void PMeshViewer::info()
{
    QString msg;
    
    if (loaded)
    {
        msg = QString("Project name: %1<br><br>").arg(projectName);
        
        int numFaces = 0;
        long memSize = 0;
        
        for (int i = 0; i < meshList.size(); ++i)
        {
            vtkPolyData *data = meshList[i].data;
            numFaces += data->GetPolys()->GetNumberOfCells();
            memSize += data->GetPolys()->GetActualMemorySize();
        }
        
        msg += QString("%1 faces, %2 MB<br>").
           arg(numFaces).arg(memSize / 1000.0, 0, 'f', 2);
    }
    else
        msg = QString("No mesh loaded.");
        
           
    QMessageBox::about(this, tr("Data Information"),
        QString("<h2>%1</h2> <p></p>").arg(appName) + msg);
}


void PMeshViewer::help()
{
    QMessageBox::about(this, tr("Help information"),
       QString("<h2>%1</h2>").arg(appName) +
       "<p></p>" +
       "<p>Control keys:<br><br>" +
       "left mouse button: camera rotate<br>" +
       "SHIFT + left mouse button: camera pan<br>" +
       "CTRL + left mouse button: camera spin<br>" +
       "middle mouse wheel: zoom<br>" +
       "middle mouse button: camera pan<br>" +
       "right mouse button: camera zoom<br>" +
       "r: reset camera<br><br>" +
       "right mouse click: select mesh<br>" +
       "DEL: delete selected meshes<br>");
}


void PMeshViewer::about()
{
    QMessageBox::about(this, QString("About %1").arg(appName),
       QString("<h2>%1</h2>").arg(appName) +
       "<p>Copyright &copy; 2012, 2013<br>" +
       "Leow Wee Kheng<br>" +
       "Department of Computer Science<br>" +
       "National University of Singapore<p></p>" +
       "Implemented using Qt, VTK.");
}


void PMeshViewer::setAppName(const QString &name)
{
    appName = name;
}


void PMeshViewer::setVisibility(int row, int col)
{
    if (col != 0)
        return;
        
    int visibility;
    Qt::CheckState state;
    
    if (meshTable->item(row, col)->checkState() == Qt::Checked)
    {
        visibility = 1;
        state = Qt::Checked;
    }
    else
    {
        visibility = 0;
        state = Qt::Unchecked;
    }
        
    QList<QTableWidgetSelectionRange> list = meshTable->selectedRanges();
    
    if (list.isEmpty() || list[0].rowCount() == 1)
    {
        meshList[row].actor->SetVisibility(visibility);
    }
    else
    {
        int top = list[0].topRow();
        int bot = list[0].bottomRow();
        for (int r = top; r <= bot; ++r)
        {
            meshList[r].actor->SetVisibility(visibility);
            meshTable->item(r, col)->setCheckState(state);
        }
        meshList[row].actor->SetVisibility(visibility);
    }

    renderWindow->Render();
    
    if (!list.isEmpty())
        meshTable->setRangeSelected(list[0], false);
}


void PMeshViewer::setColor(int row, int col)
{
    if (col != 1)
        return;
        
    QColor color = QColorDialog::getColor();
    if (!color.isValid())
        return;
        
    QTableWidgetItem *item = meshTable->item(row, col);
    item->setBackground(QBrush(color));
    
    double red = color.red() / 255.0;
    double green = color.green() / 255.0;
    double blue = color.blue() / 255.0;
    meshList[row].actor->GetProperty()->SetColor(red, green, blue);
    renderWindow->Render();
}


void PMeshViewer::blink(int row, int col)
{
    if (col < 2)
        return;
        
    double red, green, blue, r, g, b;
    vtkProperty *property = meshList[row].actor->GetProperty();
    property->GetColor(red, green, blue);
    r = g = b = 0.5;
    
    for (int i = 0; i < 2; ++i)
    {
        property->SetColor(r, g, b);
        renderWindow->Render();
        usleep(250000);
        property->SetColor(red, green, blue);
        renderWindow->Render();
        usleep(250000);
    }
}


// Supporting methods

void PMeshViewer::initSize()
{   
    winWidth = 750;
    winHeight = 550;
    setMinimumSize(winWidth, winHeight);

    QRect rect = geometry();  // Get current window's geometry.
    setGeometry(rect.left(), rect.top(), winWidth, winHeight);
    vtkWidget->updateGeometry();
}


void PMeshViewer::installPipeline(int startIndex)
{
    if (!renderer)
    {
        renderer = vtkRenderer::New();  // Camera is also reset.
        renderer->SetBackground(0.0, 0.0, 0.0);
        renderWindow->AddRenderer(renderer);
    }
    
    for (int i = startIndex; i < meshList.size(); ++i)
        renderer->AddActor(meshList[i].actor);

    hideFrontFace = 0;
    frontFaceAction->setChecked(false);
    meshModeBox->setCurrentIndex(0);
    smoothSurfaceAction->setChecked(true);
    renderer->ResetCamera();
    loaded = true;
}


void PMeshViewer::uninstallPipeline()
{   
    if (renderer)
    {
        renderer->RemoveAllViewProps();
        renderWindow->RemoveRenderer(renderer);
        renderer->Delete();
        renderer = NULL;
    }
    
    for (int i = 0; i < meshList.size(); ++i)
    {
        meshList[i].normals->Delete();
        meshList[i].mapper->Delete();
        meshList[i].actor->Delete();
        meshTable->removeRow(0);  // Remove row 0 size times.
    }
    meshList.clear();
    projectName = QString("Untitled");
    loaded = false;
}


#define RowHeight 20
#define MaxHeight 600

void PMeshViewer::loadMesh(const QStringList &fileNames)
{
    uninstallPipeline();
    addMesh(fileNames);
}


void PMeshViewer::addMesh(const QStringList &fileNames)
{
    int startIndex = meshList.size();
    
    for (int i = 0; i < fileNames.size(); ++i)
    {
        QString suffix = QFileInfo(fileNames[i]).suffix();
       
        vtkPolyDataAlgorithm *reader = NULL;

        if (suffix == "obj")
        {
            vtkOBJReader *rd = vtkOBJReader::New();
            rd->SetFileName(fileNames[i].toAscii().data());
            reader = rd;
        }
        else if (suffix == "ply")
        {
            vtkPLYReader *rd = vtkPLYReader::New();
            rd->SetFileName(fileNames[i].toAscii().data());
            reader = rd;
        }
        else if (suffix == "stl")
        {
            vtkSTLReader *rd = vtkSTLReader::New();
            rd->SetFileName(fileNames[i].toAscii().data());
            reader = rd;
        }
        else // This should not happen, but just in case.
        {
            uninstallPipeline();
            cout << "Error in loadMesh: file type " << 
                suffix.toAscii().data() << " is unsupported.\n" << flush;
            return;
        }

        // Append to mesh list
        PMeshPart part;
        part.name = QFileInfo(fileNames[i]).baseName();
        part.source = fileNames[i];
        part.data = reader->GetOutput();
        reader->Update();
        
        part.normals = vtkPolyDataNormals::New();
        part.normals->SetInput(part.data);
        part.mapper = vtkPolyDataMapper::New();
        part.mapper->SetInput(part.normals->GetOutput());
        part.mapper->Update();
        part.actor = vtkActor::New();
        part.actor->SetMapper(part.mapper);
        meshList.append(part);
        reader->Delete();

        // Append to mesh table
        int j = meshTable->rowCount();
        meshTable->insertRow(j);
        meshTable->setRowHeight(j, RowHeight);
    
        QTableWidgetItem *item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);
        meshTable->setItem(j, 0, item);
               
        item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsEnabled);
        item->setBackground(QBrush(QColor(255, 255, 255)));
        meshTable->setItem(j, 1, item);

        item = new QTableWidgetItem(part.name);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable |
            Qt::ItemIsEditable);
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        meshTable->setItem(j, 2, item);
        
        item = new QTableWidgetItem(part.source);
        item->setFlags(Qt::ItemIsEnabled);
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        meshTable->setItem(j, 3, item);
    }

    int width = meshTable->width();
    int height = meshTable->height();
    int bestHeight = qMin(MaxHeight,
            (meshTable->rowCount() + 2) * RowHeight);
    if (height < bestHeight)
        meshTable->resize(width, bestHeight);
        
    installPipeline(startIndex);
        
    setWindowTitle(QString("%1 - %2").arg(appName).arg(projectName));
}


bool PMeshViewer::saveImage(const QString &fileName)
{
    vtkWindowToImageFilter *filter = vtkWindowToImageFilter::New();
    filter->SetInput(vtkWidget->GetRenderWindow());

    QString suffix = QFileInfo(fileName).suffix();
    vtkImageWriter *writer;

    if (suffix == "jpg")
        writer = vtkJPEGWriter::New();
    else if (suffix == "png")
        writer = vtkPNGWriter::New();
    else if (suffix == "tif")
        writer = vtkTIFFWriter::New();
    else
    {
        QMessageBox::critical(this, appName,
            QString("File type %1 is unsupported.").arg(suffix));
        return false;
    }

    writer->SetInput(filter->GetOutput());
    writer->SetFileName(fileName.toAscii().data());
    writer->Write();
    writer->Delete();
    filter->Delete();
    return true;
}


// For callback

vtkRenderWindowInteractor *PMeshViewer::getInteractor()
{
    return interactor;
}


vtkRenderer *PMeshViewer::getRenderer()
{
    return renderer;
}


void PMeshViewer::highlight(vtkActor *actor)
{
    if (!actor)
        meshTable->setCurrentCell(-1, -1);
    else
        for (int i = 0; i < meshList.size(); ++i)
            if (meshList[i].actor == actor)
            {
                meshTable->setCurrentCell(i, 2);
            }
}


// Callback

PMeshViewerCallback::PMeshViewerCallback()
{
}


PMeshViewerCallback::~PMeshViewerCallback()
{
}


void PMeshViewerCallback::Execute(vtkObject *caller,
    unsigned long eventId, void *callData)
{
    vtkRenderWindowInteractor *interactor = viewer->getInteractor();
    int *pos = interactor->GetEventPosition();
    vtkRenderer *renderer = viewer->getRenderer();
    
    // Pick at the mouse location provided by the interactor
    vtkPropPicker *picker = vtkPropPicker::New(); 
    int found = picker->PickProp(pos[0], pos[1], renderer);
    
    if (found)
    {
        vtkProp *prop = picker->GetViewProp();
        viewer->highlight(vtkActor::SafeDownCast(prop));
    }
    else
        viewer->highlight(NULL);
}

