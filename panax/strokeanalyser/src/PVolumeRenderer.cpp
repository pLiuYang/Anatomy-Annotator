/* PVolumeRenderer.h

   Volume renderer for DICOM volume image.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/


#include "PVolumeRenderer.h"
#include <QtGui>
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

using namespace std;


// Callback for box widget

class vtkBoxWidgetCallback: public vtkCommand
{
public:
    static vtkBoxWidgetCallback *New()
    { return new vtkBoxWidgetCallback; }
        
    virtual void Execute(vtkObject *caller, unsigned long, void*)
    {
        vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
        if (this->mapper1 && this->mapper2)
        {
            vtkPlanes *planes = vtkPlanes::New();
            widget->GetPlanes(planes);
            this->mapper1->SetClippingPlanes(planes);
            this->mapper2->SetClippingPlanes(planes);
            planes->Delete();
        }
    }
    
    void SetMapper(vtkVolumeMapper *m1, vtkVolumeMapper *m2)
    { this->mapper1 = m1;    this->mapper2 = m2; }

protected:
    vtkBoxWidgetCallback() 
    { this->mapper1 = 0;    this->mapper2 = 0; }

    vtkVolumeMapper *mapper1, *mapper2;
};




// PVolumeRenderer class

PVolumeRenderer::PVolumeRenderer()
{
    // Initialisation
    reader = NULL;
    mapper = NULL;
    rcmapper = NULL;
    volumeRenderer = NULL;
    boxWidget = NULL;
    boxCallback = NULL;
    appName = QString("Volume Renderer");
    loaded = false;
    
    // Create GUI
    createWidgets();
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    initSize();
}


PVolumeRenderer::~PVolumeRenderer()
{
    colorFn->Delete();
    opacityFn->Delete();
    property->Delete();
    actor->Delete();
    boxWidget->Delete();
    boxCallback->Delete();
    uninstallPipeline();
}


// Qt event handlers

void PVolumeRenderer::closeEvent(QCloseEvent *event)
{
    lightDialog->hide();
    event->accept();
}


// Create window widgets

void PVolumeRenderer::createWidgets()
{
    // Create volume renderer widget with default render window
    volumeWidget = new QVTKWidget(this);
    vtkRenderWindow *renderWindow = volumeWidget->GetRenderWindow();
    vtkRenderWindowInteractor *interactor = renderWindow->GetInteractor();
    vtkInteractorStyleTrackballCamera *style =
        vtkInteractorStyleTrackballCamera::New();
    interactor->SetInteractorStyle(style);
    
    boxWidget = vtkBoxWidget::New();
    boxWidget->SetInteractor(interactor);
    boxWidget->SetPlaceFactor(1.01);
    boxWidget->InsideOutOn();
    boxWidget->RotationEnabledOff();
    boxCallback = vtkBoxWidgetCallback::New();
    boxWidget->AddObserver(vtkCommand::InteractionEvent, boxCallback);
        
    // Create transfer functions
    opacityFn = vtkPiecewiseFunction::New();
    colorFn = vtkColorTransferFunction::New();
  
    // Create volume property and attach transfer functions
    property = vtkVolumeProperty::New();
    property->SetIndependentComponents(true);
    property->SetScalarOpacity(opacityFn);
    property->SetColor(colorFn);
    property->SetInterpolationTypeToLinear();
    
    // Create actor
    actor = vtkVolume::New();
    actor->SetProperty(property);

    // Layout
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(volumeWidget, 0, 0);
    QWidget *main = new QWidget;
    main->setLayout(grid);
    setCentralWidget(main); 
    
    // Create combo boxes
    viewTypeBox = new QComboBox(this);
    QStringList choices;
    choices << "MIP" << "CT All" << "CT Skin" << "CT Muscle" << "CT Organ" 
        << "CT Bone";
    viewTypeBox->insertItems(0, choices);
    connect(viewTypeBox, SIGNAL(activated(int)),
        this, SLOT(apply()));
            
    sampleDistanceBox = new QComboBox(this);
    choices.clear();
    choices << "1" << "1/2" << "1/4" << "1/8" << "1/16";
    sampleDistanceBox->insertItems(0, choices);
    connect(sampleDistanceBox, SIGNAL(activated(int)),
        this, SLOT(setSampleDistance(int)));
        
    // Overall
    setWindowTitle(appName);
    setWindowIcon(QIcon(":/images/panax-icon.png"));
    
    createLightDialog();
}


void PVolumeRenderer::createLightDialog()
{
    lightDialog = new QWidget;
    QGroupBox *gbox = new QGroupBox("Lighting Dialog");
    ambientBox = new QDoubleSpinBox;
    ambientBox->setDecimals(2);
    ambientBox->setRange(0.0, 1.0);
    ambientBox->setSingleStep(0.01);
    ambientBox->setValue(0.1);
    
    specularBox = new QDoubleSpinBox;
    specularBox->setDecimals(2);
    specularBox->setRange(0.0, 1.0);
    specularBox->setSingleStep(0.01);
    specularBox->setValue(0.2);
    
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(new QLabel("ambient"), 0, 0);
    grid->addWidget(ambientBox, 0, 1);
    grid->addWidget(new QLabel("specular"), 1, 0);
    grid->addWidget(specularBox, 1, 1);
    gbox->setLayout(grid);
    
    // Apply and close buttons
    QPushButton *abutton = new QPushButton("apply");
    connect(abutton, SIGNAL(clicked()), this, SLOT(apply()));
    closeButton = new QPushButton("close");
    connect(closeButton, SIGNAL(clicked()), lightDialog, SLOT(hide()));
    
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(abutton);
    hbox->addStretch();
    hbox->addWidget(closeButton);
    
    // Overall
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(gbox);
    mainLayout->addLayout(hbox);
    lightDialog->setLayout(mainLayout);
}


void PVolumeRenderer::initSize()
{
    winWidth = 550;
    winHeight = 550;
    setMinimumSize(winWidth, winHeight);
    
    imageWidth = imageHeight = imageDepth = 0;

    QRect rect = geometry();  // Get current window's geometry.
    setGeometry(rect.left(), rect.top(), winWidth, winHeight);    
    volumeWidget->updateGeometry();
}


void PVolumeRenderer::createActions()
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
    
    saveViewAction = new QAction(tr("Save &view"), this);
    saveViewAction->setIcon(QIcon(":/images/saveview.png"));
    saveViewAction->setShortcut(tr("Ctrl+V"));
    saveViewAction->setStatusTip(tr("Save view to a file"));
    connect(saveViewAction, SIGNAL(triggered()),
        this, SLOT(saveView()));
        
    setVOIAction = new QAction(tr("Volume of interest"), this);
    setVOIAction->setIcon(QIcon(":/images/roi.png"));
    setVOIAction->setStatusTip(tr("Toggle volume of interest"));
    connect(setVOIAction, SIGNAL(triggered()),
        this, SLOT(setVOI()));
        
    setLightAction = new QAction(tr("Set &Lighting"), this);
    setLightAction->setIcon(QIcon(":/images/brightness.png"));
    setLightAction->setStatusTip(tr("Set ambient and specular lighting"));
    setLightAction->setCheckable(true);
    connect(setLightAction, SIGNAL(triggered()),
        this, SLOT(showLightDialog()));
    connect(closeButton, SIGNAL(clicked()),
        setLightAction, SLOT(toggle()));
       
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


void PVolumeRenderer::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(loadDirAction);
    fileMenu->addAction(saveDirAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(saveViewAction);
    viewMenu->addAction(setVOIAction);
    viewMenu->addAction(setLightAction);
    viewMenu->addAction(infoAction);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
}


void PVolumeRenderer::createToolBars()
{
    fileToolBar = addToolBar(tr("&File"));
    fileToolBar->addAction(loadDirAction);
    fileToolBar->addAction(saveDirAction);
    
    viewToolBar = addToolBar(tr("&View"));
    viewToolBar->addAction(saveViewAction);
    viewToolBar->addAction(setVOIAction);
    viewToolBar->addWidget(viewTypeBox);
    viewToolBar->addAction(setLightAction);
    viewToolBar->addWidget(new QLabel("  Step ", this));
    viewToolBar->addWidget(sampleDistanceBox);
    viewToolBar->addAction(infoAction);
    
    helpToolBar = addToolBar(tr("&Help"));
    helpToolBar->addAction(helpAction);
}


void PVolumeRenderer::createStatusBar()
{
    statusBar()->showMessage(tr(""));
}


// Slot methods

void PVolumeRenderer::loadDir()
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


void PVolumeRenderer::saveDir()
{
}


void PVolumeRenderer::saveView()
{
    if (!loaded)
        return;
        
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save current view to a file"), ".",
        tr("Image files (*.jpg *.png *.tif)"));
        
    if (fileName.isEmpty())
        return;

    saveView(fileName);
}


void PVolumeRenderer::showLightDialog()
{
    bool visible = setLightAction->isChecked();
    lightDialog->setVisible(visible);
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    volumeWidget->GetRenderWindow()->Render();
    QApplication::restoreOverrideCursor();
}


void PVolumeRenderer::setVOI()
{
    if (!loaded)
        return;
        
    boxWidget->SetEnabled(1 - boxWidget->GetEnabled());
}


void PVolumeRenderer::setSampleDistance(int option)
{
    if (!loaded)
        return;

    if (option == 0)
        actor->SetMapper(mapper);
    else
        actor->SetMapper(rcmapper);
        
    float distance = 1.0 / (1 << option);
    rcmapper->SetSampleDistance(distance);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    volumeWidget->GetRenderWindow()->Render();
    QApplication::restoreOverrideCursor();
}


void PVolumeRenderer::apply()
{
    if (!loaded)
        return;
    
    setBlendType();
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    volumeWidget->GetRenderWindow()->Render();
    QApplication::restoreOverrideCursor();
}


void PVolumeRenderer::info()
{
    QString msg;
    
    if (loaded)
        msg = QString("Data directory: %1<br><br>").arg(fullDirName) +
            QString("Size: %1 x %2 x %3 <br>").arg(imageWidth).
            arg(imageHeight).arg(imageDepth);
    else
        msg = QString("No volume loaded.");
           
    QMessageBox::about(this, tr("Data Information"),
        QString("<h2>%1</h2> <p></p>").arg(appName) + msg);
}


void PVolumeRenderer::help()
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


void PVolumeRenderer::about()
{
    QMessageBox::about(this, QString("About %1").arg(appName),
       QString("<h2>%1</h2>").arg(appName) +
       "<p>Copyright &copy; 2012, 2013<br>" +
       "Leow Wee Kheng<br>" +
       "Department of Computer Science<br>" +
       "National University of Singapore<p></p>" +
       "Implemented using Qt, VTK.");
}


void PVolumeRenderer::setAppName(const QString &name)
{
    appName = name;
}


// Supporting methods

void PVolumeRenderer::installPipeline()
{   
    // Create volume renderer
    volumeRenderer = vtkRenderer::New();
    volumeRenderer->AddActor(actor);
    volumeRenderer->SetBackground(0.0, 0.0, 0.0);

    // QVTKWidget has already created a render window and interactor
    vtkRenderWindow *renderWindow = volumeWidget->GetRenderWindow();
    renderWindow->AddRenderer(volumeRenderer);
    
    // Connect box widget
    boxWidget->SetInput(reader->GetOutput());
    boxWidget->SetDefaultRenderer(volumeRenderer);
    boxWidget->PlaceWidget();
    boxCallback->SetMapper(mapper, rcmapper);
    // boxWidget->EnabledOn();
    
    loaded = true;
}


void PVolumeRenderer::uninstallPipeline()
{
    if (reader)
    {
        reader->Delete();
        reader = NULL;
    }
    
    if (mapper)
    {
        mapper->Delete();
        mapper = NULL;
    }
    
    if (rcmapper)
    {
        rcmapper->Delete();
        rcmapper = NULL;
    }
    
    if (volumeRenderer)
    {
        volumeRenderer->Delete();
        volumeRenderer = NULL;
    }
        
    loaded = false;
}


void PVolumeRenderer::loadVolume(const QString &dirName)
{
    // Reset
    uninstallPipeline();
    viewTypeBox->setCurrentIndex(0);
    sampleDistanceBox->setCurrentIndex(0);
    
    // Create reader to read DICOM volume
    reader = vtkDICOMImageReader::New();
    reader->SetDirectoryName(dirName.toAscii().data());
    reader->Update();
    
    // Get volume size
    int *ip = reader->GetDataExtent();
    imageWidth = ip[1] - ip[0] + 1;
    imageHeight = ip[3] - ip[2] + 1;
    imageDepth = ip[5] - ip[4] + 1;
    
    if (imageWidth == 0 || imageHeight == 0 || imageDepth == 0)
    {
        QString msg = QString("Directory %1 does not contain DICOM volume").
            arg(dirName);
        QMessageBox::critical(this, appName, msg);
        return;
    }
    
    fullDirName = dirName;
    
    // Create mapper
    mapper = vtkSmartVolumeMapper::New();
    rcmapper = vtkFixedPointVolumeRayCastMapper::New();
    viewTypeBox->setCurrentIndex(0);
    setBlendType();
    rcmapper->SetSampleDistance(1.0);
    mapper->SetInputConnection(reader->GetOutputPort());
    rcmapper->SetInputConnection(reader->GetOutputPort());
    actor->SetMapper(mapper);  // Default mapper.

    installPipeline();
    volumeRenderer->ResetCamera();

    vtkRenderWindow *renderWindow = volumeWidget->GetRenderWindow();
    renderWindow->Render();
    
    // Update window title
    setWindowTitle(QString("%1 - %2").arg(appName).arg(dirName));
}


bool PVolumeRenderer::saveView(const QString &fileName)
{
    vtkWindowToImageFilter *filter = vtkWindowToImageFilter::New();
    filter->SetInput(volumeWidget->GetRenderWindow());

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


void PVolumeRenderer::setBlendType()
{       
    // Init
    opacityFn->RemoveAllPoints();
    colorFn->RemoveAllPoints();
    double opacityLevel = 1048;
    double opacityWindow = 2048;
    
    int blendType = viewTypeBox->currentIndex();
    double ambient = ambientBox->value();
    double specular = specularBox->value();
    
    // Add function points
    switch (blendType)
    {      
        // Maximum Intensity Projection
        // Create an opacity ramp from the window and level values.
        // Color is white. Blending is MIP.
        case 0:
            opacityFn->AddSegment(opacityLevel - 0.5*opacityWindow, 0.0, 
                                  opacityLevel + 0.5*opacityWindow, 1.0 );
            colorFn->AddRGBSegment(0, 1.0, 1.0, 1.0, 500, 1.0, 1.0, 1.0);
            mapper->SetBlendModeToMaximumIntensity();
            rcmapper->SetBlendModeToMaximumIntensity();
            break;

        // CT All
        // Use compositing and functions set to highlight skin in CT data.
        case 1:     
            opacityFn->AddPoint(-2000, 0.0, 0.5, 0.0);
            opacityFn->AddPoint(-1000, 0.0, 0.5, 0.5);
            opacityFn->AddPoint( -500, 1.0, 0.5, 0.0);
            opacityFn->AddPoint( 2000, 1.0, 0.5, 0.0);
                  
            colorFn->AddRGBPoint(-2000, 0.0, 0.0, 0.0, 0.5, 0.0);
            colorFn->AddRGBPoint(-1000, 0.6, 0.6, 0.6, 0.5, 0.5);
            colorFn->AddRGBPoint( -500, 0.8, 0.8, 0.8, 0.5, 0.0);
            colorFn->AddRGBPoint( 2000, 1.0, 1.0, 1.0, 0.5, 0.0);
  
            mapper->SetBlendModeToComposite();
            rcmapper->SetBlendModeToComposite();
            property->ShadeOn();
            property->SetAmbient(ambient);
            property->SetDiffuse(0.9);
            property->SetSpecular(specular);
            property->SetSpecularPower(10.0);
            break;
            
        // CT Skin
        // Use compositing and functions set to highlight skin in CT data.
        case 2:     
            opacityFn->AddPoint(-2000, 0.0, 0.5, 0.0);
            opacityFn->AddPoint( -300, 0.0, 0.5, 0.5);
            opacityFn->AddPoint( -100, 1.0, 0.5, 0.0);
            opacityFn->AddPoint( 2000, 1.0, 0.5, 0.0);
                  
            colorFn->AddRGBPoint(-2000, 0.0, 0.0, 0.0, 0.5, 0.0);
            colorFn->AddRGBPoint( -300, 0.6, 0.4, 0.2, 0.5, 0.5);
            colorFn->AddRGBPoint( -100, 0.9, 0.8, 0.5, 0.5, 0.0);
            colorFn->AddRGBPoint( 2000, 1.0, 1.0, 1.0, 0.5, 0.0);
  
            mapper->SetBlendModeToComposite();
            rcmapper->SetBlendModeToComposite();
            property->ShadeOn();
            property->SetAmbient(ambient);
            property->SetDiffuse(0.9);
            property->SetSpecular(specular);
            property->SetSpecularPower(10.0);
            break;
  
        // CT Muscle
        // Use compositing and functions set to highlight muscle in CT data.
        case 3:     
            opacityFn->AddPoint(-2000, 0.0, 0.5, 0.0);
            opacityFn->AddPoint(  -70, 0.0, 0.5, 0.5);
            opacityFn->AddPoint(  200, 1.0, 0.5, 0.0);
            opacityFn->AddPoint( 2000, 1.0, 0.5, 0.0);
                  
            colorFn->AddRGBPoint(-2000, 0.0, 0.0, 0.0, 0.5, 0.0);
            colorFn->AddRGBPoint(  -70, 1.0, 0.3, 0.3, 0.5, 0.5);
            colorFn->AddRGBPoint(  200, 1.0, 1.0, 1.0, 0.5, 0.0);
            colorFn->AddRGBPoint( 2000, 1.0, 1.0, 1.0, 0.5, 0.0);

            mapper->SetBlendModeToComposite();
            rcmapper->SetBlendModeToComposite();
            property->ShadeOn();
            property->SetAmbient(ambient);
            property->SetDiffuse(0.9);
            property->SetSpecular(specular);
            property->SetSpecularPower(10.0);
            break;
            
        // CT Organ
        // Use compositing and functions set to highlight muscle in CT data.
        case 4:
            opacityFn->AddPoint(-2000, 0.0, 0.5, 0.0);
            opacityFn->AddPoint(    0, 0.0, 0.5, 0.5);
            opacityFn->AddPoint(  300, 1.0, 0.5, 0.0);
            opacityFn->AddPoint( 2000, 1.0, 0.5, 0.0);
            
            colorFn->AddRGBPoint(-2000, 0.0, 0.0, 0.0, 0.5, 0.0);
            colorFn->AddRGBPoint(    0, 1.0, 0.3, 0.3, 0.5, 0.5);
            colorFn->AddRGBPoint(  300, 1.0, 1.0, 1.0, 0.5, 0.0);
            colorFn->AddRGBPoint( 2000, 1.0, 1.0, 1.0, 0.5, 0.0);

            mapper->SetBlendModeToComposite();
            rcmapper->SetBlendModeToComposite();
            property->ShadeOn();
            property->SetAmbient(ambient);
            property->SetDiffuse(0.9);
            property->SetSpecular(specular);
            property->SetSpecularPower(10.0);
            break;
            
        // CT Bone
        // Use compositing and functions set to highlight bone in CT data.
        case 5:
            opacityFn->AddPoint(-2000, 0.0, 0.5, 0.0);
            opacityFn->AddPoint(  -20, 0.0, 0.5, 0.5);
            opacityFn->AddPoint(  600, 1.0, 0.5, 0.0);
            opacityFn->AddPoint( 2000, 1.0, 0.5, 0.0);

            colorFn->AddRGBPoint(-2000, 0.0, 0.0, 0.0, 0.5, 0.0);
            colorFn->AddRGBPoint(  -20, 1.0, 0.3, 0.3, 0.5, 0.5);
            colorFn->AddRGBPoint(  600, 1.0, 1.0, 1.0, 0.5, 0.0);
            colorFn->AddRGBPoint( 2000, 1.0, 1.0, 1.0, 0.5, 0.0);
  
            mapper->SetBlendModeToComposite();
            rcmapper->SetBlendModeToComposite();
            property->ShadeOn();
            property->SetAmbient(ambient);
            property->SetDiffuse(0.9);
            property->SetSpecular(specular);
            property->SetSpecularPower(10.0);
            break;
  
        default: // Case 0
            opacityFn->AddSegment(opacityLevel - 0.5*opacityWindow, 0.0, 
                                  opacityLevel + 0.5*opacityWindow, 1.0 );
            colorFn->AddRGBSegment(0, 1.0, 1.0, 1.0, 500, 1.0, 1.0, 1.0);
            mapper->SetBlendModeToMaximumIntensity();
            rcmapper->SetBlendModeToMaximumIntensity();
            break;
    }
}
