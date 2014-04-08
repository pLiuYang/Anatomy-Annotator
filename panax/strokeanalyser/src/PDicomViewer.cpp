/* PDicomViewer.h

   3-plane DICOM volume viewer.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/


#include "PDicomViewer.h"
#include <QtGui>
#include "vtkCommand.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkWindowToImageFilter.h"
#include "vtkJPEGWriter.h"
#include "vtkPNGWriter.h"
#include "vtkTIFFWriter.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkPlanes.h"
#include "vtkProperty.h"
#include "vtkPointData.h"
#include "vtkImageActor.h"
#include "vtkInteractorStyleImage.h"
#include "vtkAssemblyPath.h"
#include "vtkCell.h"
#include "vtkExtractVOI.h"

using namespace std;

// #define DEBUG


// Callback class

class PDicomViewerCallback: public vtkCommand
{
public:
    static PDicomViewerCallback *New() { return new PDicomViewerCallback; }
    void Execute(vtkObject *caller, unsigned long eventId, void *callData);
    PDicomViewer *dview;
    vtkImageViewer2 *viewer;

protected:
    PDicomViewerCallback();
    ~PDicomViewerCallback();
    
private:
    vtkPointData *pointData;
};


// PDicomViewer class

PDicomViewer::PDicomViewer()
{
    // Initialisation
    reader = NULL;
    intReader = NULL;
    input = NULL;
    output = NULL;
    transViewer = NULL;
    transWidget = NULL;
    coronalViewer = NULL;
    coronalWidget = NULL;
    sagittalViewer = NULL;
    sagittalWidget = NULL;
    picker = NULL;
    
    appName = QString("DICOM Viewer");
    numPane = 4;
    loaded = false;
    
    // Create GUI
    createWidgets();
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
}


PDicomViewer::~PDicomViewer()
{
    uninstallPipeline();
    if (output)
        output->Delete();
}


void PDicomViewer::setAppName(const QString &name)
{
    appName = name;
}


void PDicomViewer::setReader(vtkDICOMImageReader *rd)
{
    if (!rd)
    {
        cout << "Error in PDicomViewer::setReader(): Input reader is NULL.\n"
            << flush;
        return;
    }
    
    uninstallPipeline();  // Reset
    reader = rd;
    fullDirName = QString();
    setWindowTitle(QString("%1 - external input").arg(appName));
    setupWidgets();
}


vtkDICOMImageReader *PDicomViewer::getReader()
{
    return reader;
}


void PDicomViewer::setInput(vtkImageData *in)
{
    if (!in)
    {
        cout << "Error: In PDicomViewer::setInput(): Input is NULL.\n" 
            << flush;
        return;
    }
    
    uninstallPipeline();  // Reset
    input = in;
    fullDirName = QString();
    setWindowTitle(QString("%1 - %2").arg(appName).arg("external input"));
    setupWidgets();
}


vtkImageData *PDicomViewer::getOutput()
{
    if (!loaded)
        return NULL;
        
    if (!output)
        output = vtkImageData::New();
    
    reader->UpdateWholeExtent();
    output->DeepCopy(reader->GetOutput());
    return output;
}


// Qt event handlers

void PDicomViewer::closeEvent(QCloseEvent *event)
{
    event->accept();
}


// Create window widgets

void PDicomViewer::createWidgets()
{    
    // Create volume viewer widgets for transverse view
    transWidget = new QVTKWidget;
    transWidget->GetRenderWindow();
    transSlider = new QSlider(Qt::Vertical);
    transSlider->setRange(0, 1);
    transSlider->setValue(0);
    transSlider->setInvertedAppearance(true);
    transSlider->setInvertedControls(true);
    connect(transSlider, SIGNAL(valueChanged(int)), this,
        SLOT(transViewSetSlice(int)));
    QHBoxLayout *transBox = new QHBoxLayout;
    transBox->addWidget(transWidget);
    transBox->addWidget(transSlider);
    pane00 = new QWidget;
    pane00->setLayout(transBox);
    
    // Unused pane
    pane01 = new QWidget;
    
    // Create volume viewer widgets for coronal view
    coronalWidget = new QVTKWidget;
    coronalWidget->GetRenderWindow();
    coronalSlider = new QSlider(Qt::Vertical);
    coronalSlider->setRange(0, 1);
    coronalSlider->setValue(0);
    coronalSlider->setInvertedAppearance(true);
    coronalSlider->setInvertedControls(true);
    connect(coronalSlider, SIGNAL(valueChanged(int)), this,
        SLOT(coronalViewSetSlice(int)));
    QHBoxLayout *coronalBox = new QHBoxLayout;
    coronalBox->addWidget(coronalWidget);
    coronalBox->addWidget(coronalSlider);
    pane10 = new QWidget;
    pane10->setLayout(coronalBox);
    
    // Create volume viewer widget for sagittal view
    sagittalWidget = new QVTKWidget;
    sagittalWidget->GetRenderWindow();
    sagittalSlider = new QSlider(Qt::Vertical);
    sagittalSlider->setRange(0, 1);
    sagittalSlider->setValue(0);
    sagittalSlider->setInvertedAppearance(true);
    sagittalSlider->setInvertedControls(true);
    connect(sagittalSlider, SIGNAL(valueChanged(int)), this,
        SLOT(sagittalViewSetSlice(int)));
    QHBoxLayout *sagittalBox = new QHBoxLayout;
    sagittalBox->addWidget(sagittalWidget);
    sagittalBox->addWidget(sagittalSlider);
    pane11 = new QWidget;
    pane11->setLayout(sagittalBox);

    // Layout
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(pane00, 0, 0);
    grid->addWidget(pane01, 0, 1);
    grid->addWidget(pane10, 1, 0);
    grid->addWidget(pane11, 1, 1);
    QWidget *main = new QWidget;
    main->setLayout(grid);
    setCentralWidget(main); 
    
    // Create combo boxes        
    windowLevelBox = new QComboBox(this);
    QStringList choices;
    choices.clear();
    choices << "Default" << "CT Abdomen" << "CT Brain" << "CT Bone" 
        << "CT Bone details" << "CT Head" << "CT Skin";
    windowLevelBox->insertItems(0, choices);
    connect(windowLevelBox, SIGNAL(activated(int)), this,
        SLOT(setWindowLevel(int)));
        
    // Overall
    setWindowTitle(appName);
    setWindowIcon(QIcon(":/images/panax-icon.png"));
    
    initSize();
}


void PDicomViewer::initSize()
{
    winWidth = 750;
    winHeight = 710;
    setMinimumSize(winWidth, winHeight);
    
    imageWidth = imageHeight = imageDepth = 0;

    QRect rect = geometry();  // Get current window's geometry.
    setGeometry(rect.left(), rect.top(), winWidth, winHeight);    
    transWidget->updateGeometry();
    coronalWidget->updateGeometry();
    sagittalWidget->updateGeometry();
}


void PDicomViewer::createActions()
{
    loadDirAction = new QAction(tr("&Load from Directory"), this);
    loadDirAction->setIcon(QIcon(":/images/loaddir.png"));
    loadDirAction->setShortcut(tr("Ctrl+L"));
    loadDirAction->setStatusTip(tr("Load DICOM images in directory."));
    connect(loadDirAction, SIGNAL(triggered()), this, SLOT(loadDir()));
    
    saveDirAction = new QAction(tr("&Save into Directory"), this);
    saveDirAction->setIcon(QIcon(":/images/savedir.png"));
    saveDirAction->setShortcut(tr("Ctrl+S"));
    saveDirAction->setStatusTip(tr("Save DICOM images into directory."));
    connect(saveDirAction, SIGNAL(triggered()), this, SLOT(saveDir()));
        
    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcut(tr("Ctrl+Q"));
    exitAction->setStatusTip(tr("Exit Mesh Viewer"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
    
    setViewPaneAction = new QAction(tr("Set viewing &panes"), this);
    setViewPaneAction->setIcon(QIcon(":/images/viewpane.png"));
    setViewPaneAction->setShortcut(tr("Ctrl+P"));
    setViewPaneAction->setStatusTip(tr("Set number of viewing panes"));
    connect(setViewPaneAction, SIGNAL(triggered()),
        this, SLOT(setViewPane()));
    
    resetWindowLevelAction = new QAction(tr("Reset &Window Level"), this);
    resetWindowLevelAction->setIcon(QIcon(":/images/contrast.png"));
    resetWindowLevelAction->setShortcut(tr("Ctrl+W"));
    resetWindowLevelAction->setStatusTip(tr("Reset window and level"));
    connect(resetWindowLevelAction, SIGNAL(triggered()),
        this, SLOT(resetWindowLevel()));
               
    saveTransViewAction = new QAction(tr("&Transverse view"), this);
    saveTransViewAction->setStatusTip(tr("Save transverse view to a file"));
    connect(saveTransViewAction, SIGNAL(triggered()),
        this, SLOT(saveTransView()));
        
    saveCoronalViewAction = new QAction(tr("&Coronal view"), this);
    saveCoronalViewAction->setStatusTip(tr("Save coronal view to a file"));
    connect(saveCoronalViewAction, SIGNAL(triggered()),
        this, SLOT(saveCoronalView()));
        
    saveSagittalViewAction = new QAction(tr("&Sagittal view"), this);
    saveSagittalViewAction->setStatusTip(tr("Save sagittal view to a file"));
    connect(saveSagittalViewAction, SIGNAL(triggered()),
        this, SLOT(saveSagittalView()));
        
    infoAction = new QAction(tr("&Info"), this);
    infoAction->setIcon(QIcon(":/images/info.png"));
    infoAction->setShortcut(tr("Ctrl+I"));
    infoAction->setStatusTip(tr("Data information"));
    connect(infoAction, SIGNAL(triggered()), this, SLOT(info()));
        
    helpAction = new QAction(tr("&Help"), this);
    helpAction->setIcon(QIcon(":/images/help.png"));
    helpAction->setShortcut(tr("Ctrl+H"));
    helpAction->setStatusTip(tr("Help information"));
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));
        
    aboutAction = new QAction(tr("&About"), this);
    aboutAction->setShortcut(tr("Ctrl+A"));
    aboutAction->setStatusTip(tr("About this viewer"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}


void PDicomViewer::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(loadDirAction);
    fileMenu->addAction(saveDirAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(setViewPaneAction);
    viewMenu->addAction(resetWindowLevelAction);
    
    saveViewMenu = viewMenu->addMenu(tr("&Save"));
    saveViewMenu->setIcon(QIcon(":/images/saveview.png"));
    saveViewMenu->addAction(saveTransViewAction);
    saveViewMenu->addAction(saveCoronalViewAction);
    saveViewMenu->addAction(saveSagittalViewAction);
    viewMenu->addAction(infoAction);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
}


void PDicomViewer::createToolBars()
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addAction(loadDirAction);
    fileToolBar->addAction(saveDirAction);
    
    viewToolBar = addToolBar(tr("&View"));
    viewToolBar->addAction(setViewPaneAction);
    viewToolBar->addAction(resetWindowLevelAction);
    viewToolBar->addWidget(windowLevelBox);
    viewToolBar->addAction(infoAction);
    
    helpToolBar = addToolBar(tr("&Help"));
    helpToolBar->addAction(helpAction);
}


void PDicomViewer::createStatusBar()
{
    statusBar()->showMessage(tr(""));
}


// Slot methods

void PDicomViewer::loadDir()
{
    QString	dirName = QFileDialog::getExistingDirectory(this, 
        tr("Load DICOM volume by directory"), ".");
        
    if (!dirName.isEmpty())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);
        loadVolume(dirName);
        QApplication::restoreOverrideCursor();
    }
}


void PDicomViewer::saveDir()
{
}


void PDicomViewer::setViewPane()
{
    if (numPane == 1)
    {
        pane01->show();
        pane10->hide();
        pane11->hide();
        numPane = 2;
    }
    else if (numPane == 2)
    {
        pane10->show();
        pane11->show();
        numPane = 4;
    }
    else if (numPane == 4)
    {
        pane01->hide();
        pane10->hide();
        pane11->hide();
        numPane = 1;
    }
}


void PDicomViewer::resetWindowLevel()
{
    if (!loaded)
        return;

    setWindowLevel(0);
}


void PDicomViewer::setWindowLevel(int index)
{
    int level, window;
    
    if (!loaded)
        return;

    switch (index)
    {
        case 0:  // Default
            level = 128;
            window = 256;
            break;

        case 1:  // Abdomen
            level = 90;
            window = 400;
            break;

        case 2:  // Brain
            level = 45;
            window = 90;
            break;

        case 3:  // Bone
            level = 128;
            window = 200;
            break;

        case 4:  // Bone details
            level = 0;
            window = 2500;
            break;
            
        case 5:  // Head
            level = 128;
            window = 256;
            break;

        case 6:  // Skin
            level = 0;
            window = 400;
            break;

        default:
            level = 128;
            window = 256;
            break;
    }

    setWindowLevel(window, level);
}


void PDicomViewer::setWindowLevel(int window, int level)
{
    transViewer->SetColorLevel(level);
    transViewer->SetColorWindow(window);
    transViewer->Render();
    coronalViewer->SetColorLevel(level);
    coronalViewer->SetColorWindow(window);
    coronalViewer->Render();
    sagittalViewer->SetColorLevel(level);
    sagittalViewer->SetColorWindow(window);
    sagittalViewer->Render();
    QString msg = QString("(%1, %2, %3) (L:%4, W:%5)").
        arg(sagittalSlice).arg(coronalSlice).arg(transSlice).
        arg(level).arg(window);
    statusBar()->showMessage(QString(msg));
}


void PDicomViewer::saveTransView()
{
    saveView(0);
}


void PDicomViewer::saveCoronalView()
{
    saveView(1);
}


void PDicomViewer::saveSagittalView()
{
    saveView(2);
}


void PDicomViewer::info()
{
    QString msg;
    
    if (!loaded)
        msg = QString("No volume loaded.");
    else
    {
        double *sp = reader->GetPixelSpacing();
        
        msg = QString("DICOM directory: %1<br><br>").arg(fullDirName) +
            QString("Patient name: %1<br>").arg(reader->GetPatientName()) +
            QString("Description: %1<br>").arg(reader->GetDescriptiveName()) +
            QString("Study ID: %1<br>").arg(reader->GetStudyID()) +
            QString("Study UID: %1<br>").arg(reader->GetStudyUID()) +
            QString("Size: %1 x %2 x %3 voxels<br>").arg(imageWidth).
                arg(imageHeight).arg(imageDepth) +
            QString("Voxel spacing: %1, %2, %3<br>").arg(sp[0]).
                arg(sp[1]).arg(sp[2]);
                
#ifdef DEBUG
        msg += QString("<br>Bits allocated: %1<br>").
                arg(reader->GetBitsAllocated()) +
            QString("Number of components: %1<br>").
                arg(reader->GetNumberOfComponents()) +
            QString("Value: %1<br>").arg((reader->GetPixelRepresentation()
                ? "signed" : "unsigned")) +
            QString("Rescale slope: %1<br>").
                arg(reader->GetRescaleSlope()) +
            QString("Rescale offset: %1<br>").
                arg(reader->GetRescaleOffset());
#endif
    }
           
    QMessageBox::about(this, tr("Data Information"),
        QString("<h2>%1</h2> <p></p>").arg(appName) + msg);
}


void PDicomViewer::help()
{
    QMessageBox::about(this, tr("Help information"),
       QString("<h2>%1</h2>").arg(appName) +
       "<p></p>" +
       "<p>Control keys:<br>" +
       "left mouse button: camera rotate<br>" +
       "SHIFT + left mouse button: camera pan<br>" +
       "CTRL + left mouse button: camera spin<br>" +
       "CTRL + SHIFT + left mouse button: camera zoom<br>" +
       "middle mouse wheel: zoom<br>" +
       "middle mouse button: camera pan<br>" +
       "right mouse button: camera zoom<br>" +
       "r: reset camera<br>" +
       "i: toggle clipping box<br>");
}


void PDicomViewer::about()
{
    QMessageBox::about(this, QString("About %1").arg(appName),
       QString("<h2>%1</h2>").arg(appName) +
       "<p>Copyright &copy; 2012, 2013<br>" +
       "Leow Wee Kheng<br>" +
       "Department of Computer Science<br>" +
       "National University of Singapore<p></p>" +
       "Implemented using Qt, VTK.");
}


void PDicomViewer::transViewSetSlice(int slice)
{
    if (!loaded)
        return;
        
    transSlice = slice;
    transViewer->SetSlice(slice);
    transViewer->Render();
    statusBar()->showMessage(QString("(%1, %2, %3)").
        arg(sagittalSlice).arg(coronalSlice).arg(transSlice));
}


void PDicomViewer::coronalViewSetSlice(int slice)
{
    if (!loaded)
        return;
        
    coronalSlice = slice;
    coronalViewer->SetSlice(slice);
    coronalViewer->Render();
    statusBar()->showMessage(QString("(%1, %2, %3)").
        arg(sagittalSlice).arg(coronalSlice).arg(transSlice));
}


void PDicomViewer::sagittalViewSetSlice(int slice)
{
    if (!loaded)
        return;
        
    sagittalSlice = slice;
    sagittalViewer->SetSlice(slice);
    sagittalViewer->Render();
    statusBar()->showMessage(QString("(%1, %2, %3)").
        arg(sagittalSlice).arg(coronalSlice).arg(transSlice));
}


// Supporting methods

void PDicomViewer::installPipeline()
{
    vtkRenderWindow *renderWindow;
    
    // Create transverse image viewer
    transViewer = vtkImageViewer2::New();
    if (reader)
        transViewer->SetInputConnection(reader->GetOutputPort());
    else if (input)
        transViewer->SetInput(input);
    transViewer->GetRenderer();  // Init renderer
    renderWindow = transViewer->GetRenderWindow();
    transWidget->SetRenderWindow(renderWindow);
    transViewer->SetupInteractor(renderWindow->GetInteractor());
    
    // Create coronal image viewer
    coronalViewer = vtkImageViewer2::New();
    coronalViewer->SetSliceOrientationToXZ();
    if (reader)
        coronalViewer->SetInputConnection(reader->GetOutputPort());
    else if (input)
        coronalViewer->SetInput(input);
    coronalViewer->GetRenderer();
    renderWindow = coronalViewer->GetRenderWindow();
    coronalWidget->SetRenderWindow(renderWindow);
    coronalViewer->SetupInteractor(renderWindow->GetInteractor());
    
    // Create sgaittal image viewer
    sagittalViewer = vtkImageViewer2::New();
    sagittalViewer->SetSliceOrientationToYZ();
    if (reader)
        sagittalViewer->SetInputConnection(reader->GetOutputPort());
    else if (input)
        sagittalViewer->SetInput(input);
    sagittalViewer->GetRenderer();
    renderWindow = sagittalViewer->GetRenderWindow();
    sagittalWidget->SetRenderWindow(renderWindow);
    sagittalViewer->SetupInteractor(renderWindow->GetInteractor());
    
    loaded = true;
    
    // Sets up callback
    picker = vtkPropPicker::New();
    picker->PickFromListOn();
    picker->AddPickList(transViewer->GetImageActor());
    picker->AddPickList(coronalViewer->GetImageActor());
    picker->AddPickList(sagittalViewer->GetImageActor());
    
    PDicomViewerCallback *callback = PDicomViewerCallback::New();
    callback->dview = this;
    callback->viewer = transViewer;
    vtkInteractorStyleImage* imageStyle = transViewer->GetInteractorStyle();
    imageStyle->AddObserver(vtkCommand::MouseMoveEvent, callback);
    callback->Delete();
    
    callback = PDicomViewerCallback::New();
    callback->dview = this;
    callback->viewer = coronalViewer;
    imageStyle = coronalViewer->GetInteractorStyle();
    imageStyle->AddObserver(vtkCommand::MouseMoveEvent, callback);
    callback->Delete();
    
    callback = PDicomViewerCallback::New();
    callback->dview = this;
    callback->viewer = sagittalViewer;
    imageStyle = sagittalViewer->GetInteractorStyle();
    imageStyle->AddObserver(vtkCommand::MouseMoveEvent, callback);
    callback->Delete();
}


void PDicomViewer::uninstallPipeline()
{
    reader = NULL;
    
    if (intReader)
    {
        intReader->Delete();
        intReader = NULL;
    }
    
    input = NULL;
    
    if (transViewer)
    {
        transViewer->Delete();
        transViewer = NULL;
    }
    
    if (coronalViewer)
    {
        coronalViewer->Delete();
        coronalViewer = NULL;
    }
    
    if (sagittalViewer)
    {
        sagittalViewer->Delete();
        sagittalViewer = NULL;
    }
    
    if (picker)
    {
        picker->Delete();
        picker = NULL;
    }

    loaded = false;
    transSlider->setValue(0);  // Must be after loaded is set to false.
    coronalSlider->setValue(0);
    sagittalSlider->setValue(0);
}


void PDicomViewer::loadVolume(const QString &dirName)
{
    uninstallPipeline();  // Reset
    intReader = vtkDICOMImageReader::New();
    intReader->SetDirectoryName(dirName.toAscii().data());
    reader = intReader;
    reader->Update();
    
    long errcode = reader->GetErrorCode();
    if (errcode != 0)
    {
        QString msg = QString("Directory %1 does not contain DICOM image").
            arg(dirName);
        QMessageBox::critical(this, appName, msg);
        return;
    }

    fullDirName = dirName;
    setWindowTitle(QString("%1 - ").arg(appName) + dirName);
    setupWidgets();
}


void PDicomViewer::setupWidgets()
{
    // Get volume size
    int *ip;
    if (reader)
        ip = reader->GetDataExtent();
    else if (input)
        ip = input->GetExtent();
    else
    {
        cout << "Error: In PDicomViewer::setupWidgets(): " <<
            "Both reader and input are NULL.\n" << flush;
        return;
    }
    
//     imageWidth = ip[1] - ip[0] + 1;
//     imageHeight = ip[3] - ip[2] + 1;
//     imageDepth = ip[5] - ip[4] + 1;
    
    if (ip[0] < 0 || ip[2] < 0 || ip[4] < 0)
        cout << "Warning: In PDicomViewer::setupWidgets(): " <<
            "Lower bound of extent is negative.\n" << flush;
    
    imageWidth = ip[1] + 1;
    imageHeight = ip[3] + 1;
    imageDepth = ip[5] + 1;
    
    if (imageWidth == 0 || imageHeight == 0 || imageDepth == 0)
    {
        cout << "Error: In PDicomViewer::setupWidgets(): "
            "Image is not 3D.\n" <<  flush;
        return;
    }
    
    transSlider->setRange(0, imageDepth - 1);
    coronalSlider->setRange(0, imageHeight - 1);
    sagittalSlider->setRange(0, imageWidth - 1);

    installPipeline();
    resetCameras();
    
    // Set to slice in middle of volume and reset window level
    transSlider->setValue(imageDepth / 2);
    coronalSlider->setValue(imageHeight / 2);
    sagittalSlider->setValue(imageWidth / 2);
    resetWindowLevel();
}


void PDicomViewer::resetCameras()
{   
    // Reset transverse viewer's camera
    vtkRenderer *renderer = transViewer->GetRenderer();
    vtkCamera *camera = renderer->GetActiveCamera();
    renderer->ResetCamera();

    // Reset coronal viewer's camera
    renderer = coronalViewer->GetRenderer();
    camera = renderer->GetActiveCamera();
    camera->SetViewUp(0.0, 0.0, -1.0);
    renderer->ResetCamera();
    
    // Flip coronal view
    double posx, posy, posz;
    double fpx, fpy, fpz;
    double dx, dy, dz;
    camera->GetPosition(posx, posy, posz);
    camera->GetFocalPoint(fpx, fpy, fpz); 
    dx = posx - fpx;
    dy = posy - fpy;
    dz = posz - fpz;
    posx = fpx - dx;
    posy = fpz - dy;
    posz = fpz - dz;
    camera->SetPosition(posx, posy, posz);

    // Reset sagittal viewer's camera
    renderer = sagittalViewer->GetRenderer();
    camera = renderer->GetActiveCamera();
    camera->SetViewUp(0.0, 0.0, -1.0);
    renderer->ResetCamera();
}


void PDicomViewer::saveView(int type)
{
    if (!loaded)
        return;
        
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save current view to a file"), ".",
        tr("Image files (*.jpg *.png *.tif)"));
        
    if (fileName.isEmpty())
        return;

    saveView(fileName, type);
}


bool PDicomViewer::saveView(const QString &fileName, int type)
{
    vtkWindowToImageFilter *filter = vtkWindowToImageFilter::New();
    switch (type)
    {            
        case 0:
            filter->SetInput(transWidget->GetRenderWindow());
            break;
            
        case 1:
            filter->SetInput(coronalWidget->GetRenderWindow());
            break;
            
        case 2:
            filter->SetInput(sagittalWidget->GetRenderWindow());
            break;
            
        default:
            return false;
    }

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


//--- PDicomViewerCallback functions

PDicomViewerCallback::PDicomViewerCallback()
{
    pointData = vtkPointData::New();
}


PDicomViewerCallback::~PDicomViewerCallback()
{
    pointData->Delete();
}


void PDicomViewerCallback::Execute(vtkObject *caller,
    unsigned long eventId, void *callData)
{       
    if (!dview->loaded)
        return;
           
    // vtkImageViewer2 *viewer = dview->transViewer;
    vtkImageData *data = viewer->GetInput();
    vtkImageActor *actor = viewer->GetImageActor();
    
    vtkRenderWindowInteractor *interactor = viewer->
        GetRenderWindow()->GetInteractor();
    vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(
        interactor->GetInteractorStyle());

    // Pick at the mouse location provided by the interactor
    vtkPropPicker *picker = dview->picker;
    vtkRenderer *renderer = viewer->GetRenderer();
    picker->Pick(interactor->GetEventPosition()[0],
        interactor->GetEventPosition()[1], 0.0, renderer);

    // There could be other props assigned to this picker, so 
    // make sure we picked the image actor
    vtkAssemblyPath* path = picker->GetPath();
    bool validPick = false;

    if (path)
    {
        vtkCollectionSimpleIterator sit;
        path->InitTraversal(sit);
        vtkAssemblyNode *node;
    
        for (int i = 0; i < path->GetNumberOfItems() && !validPick; ++i)
        {
            node = path->GetNextNode(sit);
            if (actor == vtkImageActor::SafeDownCast(node->GetViewProp()))
                validPick = true;
        }
    }

    if (!validPick)
    {
        // dview->statusBar()->showMessage(QString("Off image"));
        dview->statusBar()->showMessage(QString());
    
        // Pass the event further on
        style->OnMouseMove();
        return;
    }

    // Get the world coordinates of the pick
    double pos[3];
    picker->GetPickPosition(pos);

    // Fixes some numerical problems with the picking
    double *bounds = actor->GetDisplayBounds();
    int axis = viewer->GetSliceOrientation();
    pos[axis] = bounds[2*axis];

    vtkPointData* pd = data->GetPointData();
    if (!pd)
        return;
        
    pointData->InterpolateAllocate(pd, 1, 1);

    // Use tolerance as a function of size of source data
    double tol2 = data->GetLength();
    tol2 = tol2 ? tol2*tol2 / 1000.0 : 0.001;

    // Find the cell that contains pos
    int subId;
    double pcoords[3], weights[8];
    vtkCell* cell = data->FindAndGetCell(pos, NULL, -1, tol2, subId, 
        pcoords, weights);
    
    if (cell)
    {
        // Interpolate the point data
        pointData->InterpolatePoint(pd, 0, cell->PointIds, weights);
        int components = pointData->GetScalars()->GetNumberOfComponents();
        double* tuple = pointData->GetScalars()->GetTuple(0);

        QString msg =
            QString("(x:%1, y:%2, z:%3), slice:(%4, %5, %6) V:(").
                arg((int) pos[0]).arg((int) pos[1]).arg((int) pos[2]).
                arg(dview->sagittalSlice).arg(dview->coronalSlice).
                arg(dview->transSlice);
        
        for (int c = 0; c < components; ++c)
        {
           msg += QString("%1").arg((int) tuple[c]);
           if (c != components - 1)
              msg += ", ";
        }
        msg += ") ";
    
        int level = viewer->GetColorLevel();
        int window = viewer->GetColorWindow();
        msg += QString("(L:%1, W:%2)").arg(level).arg(window);

        if (viewer != dview->transViewer)
        {
            dview->transViewer->SetColorLevel(level);
            dview->transViewer->SetColorWindow(window);
            dview->transViewer->Render();
        }
        if (viewer != dview->coronalViewer)
        {
            dview->coronalViewer->SetColorLevel(level);
            dview->coronalViewer->SetColorWindow(window);
            dview->coronalViewer->Render();
        }
        if (viewer != dview->sagittalViewer)
        {
            dview->sagittalViewer->SetColorLevel(level);
            dview->sagittalViewer->SetColorWindow(window);
            dview->sagittalViewer->Render();
        }
    
        dview->statusBar()->showMessage(msg);
    
        // Pass the event further on
        style->OnMouseMove();
    }
}