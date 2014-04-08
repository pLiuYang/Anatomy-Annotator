/* PDicomSegmenter.cpp

   DICOM image segmentation tool.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#include "PDicomSegmenter.h"
#include <QtGui>
#include "vtkPLYWriter.h"
#include "vtkSTLWriter.h"
#include "vtkCellArray.h"
#include "vtkProperty.h"
#include "vtkCommand.h"
#include "vtkInteractorStyleImage.h"
#include "vtkWindowToImageFilter.h"
#include "vtkJPEGWriter.h"
#include "vtkPNGWriter.h"
#include "vtkTIFFWriter.h"

#include <unistd.h>
using namespace std;

// #define USE_SMART_MAPPER


PDicomSegmenter::PDicomSegmenter()
{
    outputMesh = NULL;
    outputVolume = NULL;
    
    loaded = false;
    voiDone = false;
    thresholdDone = false;
    removeSkullDone = false;
    segmented = false;
    anisoDiffuseDone = false;
    hasVolumeActor = false;
    hasMeshActor = false;
    
    addWidgets();
    addActions();
    addMenus();
    addToolBars();

    appName = QString("DICOM Segmenter");
    setWindowTitle(appName);
}


PDicomSegmenter::~PDicomSegmenter()
{
    delete transVoi;
    delete coronalVoi;
    delete sagittalVoi;
    delete thresholdDialog;
    delete skullRemover;
    delete volumeRenderDialog;
    delete genMeshDialog;

    extractVoi->Delete();     
    outputVoi->Delete(); 

    anisoDiffuser->Delete();
    
    colorFn->Delete();
    opacityFn->Delete();
    property->Delete();
#ifdef USE_SMART_MAPPER
    smartMapper->Delete();
#endif
    rayCastMapper->Delete();
    volumeActor->Delete();
    volumeRenderer->RemoveAllViewProps();
    volumeWidget->GetRenderWindow()->RemoveRenderer(volumeRenderer);
    volumeRenderer->Delete();
    volumeStyle->Delete();
    
    mcubes->Delete();
    decimate->Delete();
    smooth->Delete();
    normals->Delete();
    meshMapper->Delete();
    meshActor->Delete();
    meshRenderer->RemoveAllViewProps();
    meshWidget->GetRenderWindow()->RemoveRenderer(meshRenderer);
    meshRenderer->Delete();
    meshStyle->Delete();

    if (outputVolume)
        outputVolume->Delete();
    if (outputMesh)
        outputMesh->Delete();
}


void PDicomSegmenter::setReader(vtkDICOMImageReader *rd)
{
    PDicomViewer::setReader(rd);
    resetInput();
    resetVoi();
}


void PDicomSegmenter::setInput(vtkImageData *in)
{
    cout << "PDicomSegmenter::setInput() is not implemented.\n" << flush;
}


vtkImageData *PDicomSegmenter::getOutput()
{
    cout << "PDicomSegmenter::getOutput() is not implemented.\n" <<
        "Use getOutputImage() instead.\n" << flush;
    return NULL;
}


vtkImageData *PDicomSegmenter::getOutputImage()
{
    if (!loaded)
        return NULL;
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    computeOutputVolume();
    QApplication::restoreOverrideCursor();
    return outputVolume;
}


vtkPolyData *PDicomSegmenter::getOutputMesh()
{
    return outputMesh;
}


// Qt event handlers

void PDicomSegmenter::closeEvent(QCloseEvent *event)
{
    anisoDiffuseDialog->hide(); // Hide this one first
    thresholdDialog->hide();
    skullRemover->hide();
    volumeRenderDialog->hide();
    genMeshDialog->hide();
    event->accept();
}


void PDicomSegmenter::hideEvent(QHideEvent *event)
{
    anisoDiffuseDialog->hide(); // Hide this one first
    thresholdDialog->hide();
    skullRemover->hide();
    volumeRenderDialog->hide();
    genMeshDialog->hide();
    event->accept();
}


// Create additional widgets and objects

void PDicomSegmenter::addWidgets()
{
    createVoiObjects();
    
    // Create tools    
    thresholdDialog = new PThresholdDialog;
    connect(thresholdDialog, SIGNAL(updated()), this, SLOT(updateViewers()));
    
    skullRemover = new PSkullRemover;
    connect(skullRemover, SIGNAL(updated()), this, SLOT(updateViewers()));
    
    createAnisoDiffuser();
    createVolumeRenderer();
    createVolumeDialog();
    createMeshObjects();
    createMeshViewer();
    createMeshDialog();
    
    // Set pane01
    box01 = new QHBoxLayout;
    box01->addWidget(volumeWidget);
    box01->addWidget(meshWidget);
    meshWidget->hide();
    pane01->setLayout(box01);
}


void PDicomSegmenter::createVoiObjects()
{
    transVoi = new PVoiWidget;
    transVoi->alignXYPlane();
    coronalVoi = new PVoiWidget;
    coronalVoi->alignXZPlane();
    sagittalVoi = new PVoiWidget;
    sagittalVoi->alignYZPlane();
            
    connect(transVoi, SIGNAL(topLineChanged(int)),
        sagittalVoi, SLOT(setLeftLine(int)));
    connect(transVoi, SIGNAL(bottomLineChanged(int)),
        sagittalVoi, SLOT(setRightLine(int)));
    connect(transVoi, SIGNAL(leftLineChanged(int)),
        coronalVoi, SLOT(setLeftLine(int)));
    connect(transVoi, SIGNAL(rightLineChanged(int)),
        coronalVoi, SLOT(setRightLine(int)));
    
    connect(coronalVoi, SIGNAL(topLineChanged(int)),
        sagittalVoi, SLOT(setTopLine(int)));
    connect(coronalVoi, SIGNAL(bottomLineChanged(int)),
        sagittalVoi, SLOT(setBottomLine(int)));
    connect(coronalVoi, SIGNAL(leftLineChanged(int)),
        transVoi, SLOT(setLeftLine(int)));
    connect(coronalVoi, SIGNAL(rightLineChanged(int)),
        transVoi, SLOT(setRightLine(int)));

    connect(sagittalVoi, SIGNAL(topLineChanged(int)),
        coronalVoi, SLOT(setTopLine(int)));
    connect(sagittalVoi, SIGNAL(bottomLineChanged(int)),
        coronalVoi, SLOT(setBottomLine(int)));
    connect(sagittalVoi, SIGNAL(leftLineChanged(int)),
        transVoi, SLOT(setTopLine(int)));
    connect(sagittalVoi, SIGNAL(rightLineChanged(int)),
        transVoi, SLOT(setBottomLine(int)));
        
    extractVoi = vtkExtractVOI::New();
    extractVoi->SetSampleRate(1, 1, 1);
    // extractVoi's input is not yet set.
    
    outputVoi = vtkExtractVOI::New();
    outputVoi->SetSampleRate(1, 1, 1);
    // outputVoi's input is not yet set.
}


void PDicomSegmenter::createAnisoDiffuser()
{
    // Create anisotropic diffuser
    anisoDiffuser = vtkImageAnisotropicDiffusion3D::New();
    anisoDiffuser->FacesOn();
    anisoDiffuser->EdgesOn();
    anisoDiffuser->CornersOn();
    anisoDiffuser->GradientMagnitudeThresholdOn();

    // Create dialog
    anisoDiffuseDialog = new QWidget;
    anisoDiffuseDialog->setWindowTitle("Anisotropic Diffuser");
    anisoDiffuseDialog->setFixedSize(370, 150);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    anisoDiffuseDialog->setLayout(mainLayout);
    
    // Diffusion threshold box
    diffThresholdBox = new QSpinBox;
    diffThresholdBox->setRange(0, 200);
    diffThresholdSlider = new QSlider(Qt::Horizontal);
    diffThresholdSlider->setRange(0, 200);
    connect(diffThresholdBox, SIGNAL(valueChanged(int)),
        diffThresholdSlider, SLOT(setValue(int)));
    connect(diffThresholdSlider, SIGNAL(valueChanged(int)),
        diffThresholdBox, SLOT(setValue(int)));
    diffThresholdBox->setValue(20);
    connect(diffThresholdBox, SIGNAL(valueChanged(int)),
        this, SLOT(setDiffThreshold(int)));
    
    diffFactorBox = new QDoubleSpinBox;
    diffFactorBox->setDecimals(2);
    diffFactorBox->setRange(0.0, 1.0);
    diffFactorBox->setSingleStep(0.1);
    diffFactorBox->setValue(0.5);
    connect(diffFactorBox, SIGNAL(valueChanged(double)),
        this, SLOT(setDiffFactor(double)));
    
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(new QLabel("threshold"), 0, 0);
    grid->addWidget(diffThresholdBox, 0, 1);
    grid->addWidget(diffThresholdSlider, 0, 2);
    grid->addWidget(new QLabel("distance"), 1, 0);
    grid->addWidget(diffFactorBox, 1, 1);
    QGroupBox *gbox = new QGroupBox("Diffusion Parameters");
    gbox->setLayout(grid);
    mainLayout->addWidget(gbox);
    
    // Close button
    closeDiffuseButton = new QPushButton("close");
    connect(closeDiffuseButton, SIGNAL(clicked()),
        anisoDiffuseDialog, SLOT(hide()));
    
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(closeDiffuseButton);
    hbox->addStretch();
    mainLayout->addLayout(hbox);
}


void PDicomSegmenter::createVolumeRenderer()
{
    // Create volume renderer widget
    volumeWidget = new QVTKWidget;
        
    // Create transfer functions
    opacityFn = vtkPiecewiseFunction::New();
    colorFn = vtkColorTransferFunction::New();
  
    // Create volume property and attach transfer functions
    property = vtkVolumeProperty::New();
    property->SetIndependentComponents(true);
    property->SetScalarOpacity(opacityFn);
    property->SetColor(colorFn);
    property->SetInterpolationTypeToLinear();
    
    // Create mapper and actor
    rayCastMapper = vtkFixedPointVolumeRayCastMapper::New();
    volumeMapper = rayCastMapper;
    
#ifdef USE_SMART_MAPPER
    smartMapper = vtkSmartVolumeMapper::New();
    volumeMapper = smartMapper;  // Default
#endif

    // mapper's input is not yet connected
    volumeActor = vtkVolume::New();
    volumeActor->SetProperty(property);
    // actor's mapper is not yet set
    
    // Create renderer
    volumeRenderer = vtkRenderer::New();
    volumeRenderer->SetBackground(0.0, 0.0, 0.0);
    vtkRenderWindow *renderWindow = volumeWidget->GetRenderWindow();
    renderWindow->AddRenderer(volumeRenderer);
    // actor is not yet added to renderer
    
    // Set interactor style
    vtkRenderWindowInteractor *interactor = renderWindow->GetInteractor();
    volumeStyle = vtkInteractorStyleTrackballCamera::New();
    interactor->SetInteractorStyle(volumeStyle);
}


void PDicomSegmenter::createVolumeDialog()
{
    // Volume render dialog
    volumeRenderDialog = new QWidget;
    volumeRenderDialog->setWindowTitle("Volume Renderer");
    volumeRenderDialog->setFixedSize(250, 250);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    volumeRenderDialog->setLayout(mainLayout);
    
    // Viewing mode setting
    QGroupBox *gbox = new QGroupBox("Viewing Mode");
    viewTypeBox = new QComboBox;
    QStringList choices;
    choices << "CT Skin" << "CT Muscle" << "CT Bone" << "CTA Vessels";
    viewTypeBox->insertItems(0, choices);
    
    sampleDistanceBox = new QComboBox;
    choices.clear();
    choices << "1" << "1/2" << "1/4" << "1/8" << "1/16";
    sampleDistanceBox->insertItems(0, choices);
    connect(sampleDistanceBox, SIGNAL(activated(int)),
        this, SLOT(setSampleDistance(int)));
        
    ambientBox = new QDoubleSpinBox;
    ambientBox->setDecimals(2);
    ambientBox->setRange(0.0, 1.0);
    ambientBox->setSingleStep(0.1);
    ambientBox->setValue(0.1);
    
    specularBox = new QDoubleSpinBox;
    specularBox->setDecimals(2);
    specularBox->setRange(0.0, 1.0);
    specularBox->setSingleStep(0.1);
    specularBox->setValue(0.2);
        
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(new QLabel("mode"), 0, 0);
    grid->addWidget(viewTypeBox, 0, 1);
    grid->addWidget(new QLabel("sampling"), 1, 0);
    grid->addWidget(sampleDistanceBox, 1, 1);
    grid->addWidget(new QLabel("ambient"), 2, 0);
    grid->addWidget(ambientBox, 2, 1);
    grid->addWidget(new QLabel("specular"), 3, 0);
    grid->addWidget(specularBox, 3, 1);
    gbox->setLayout(grid);
    mainLayout->addWidget(gbox);
    
    // Apply and close buttons
    QPushButton *abutton = new QPushButton("apply");
    connect(abutton, SIGNAL(clicked()), this, SLOT(volumeRender()));
    closeVolumeButton = new QPushButton("close");
    connect(closeVolumeButton, SIGNAL(clicked()),
        volumeRenderDialog, SLOT(hide()));
    
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(abutton);
    hbox->addStretch();
    hbox->addWidget(closeVolumeButton);
    mainLayout->addLayout(hbox);
    
    setBlendType();
}


void PDicomSegmenter::createMeshObjects()
{
    // Marching cubes and post-processors
    mcubes = vtkMarchingCubes::New();
    mcubes->ComputeScalarsOff();   // Speed up processing.
    mcubes->ComputeNormalsOff();   // Speed up processing.
    mcubes->ComputeGradientsOff(); // Speed up processing.
    // input to mcubes is not yet set.

    double featureAngle = 60.0;
    decimate = vtkDecimatePro::New();
    decimate->SetFeatureAngle(featureAngle);
    decimate->PreserveTopologyOn();
    // mcubes to decimate is not yet connected.
    
    smooth = vtkSmoothPolyDataFilter::New();
    smooth->SetFeatureAngle(featureAngle);
    smooth->FeatureEdgeSmoothingOff();
    smooth->BoundarySmoothingOff();
    smooth->SetConvergence(0);
    smooth->SetInputConnection(decimate->GetOutputPort());

    normals = vtkPolyDataNormals::New();
    normals->SetFeatureAngle(featureAngle);
    // mcubes or smooth to normals is not yet connected.
}


void PDicomSegmenter::createMeshViewer()
{
    // Create mesh viewer widget
    meshWidget = new QVTKWidget;

    // Create mapper and actor
    meshMapper = vtkPolyDataMapper::New();
    // mapper's input is not yet connected
    meshActor = vtkActor::New();
    // actor's mapper is not yet set
    
    // Create renderer
    meshRenderer = vtkRenderer::New();
    meshRenderer->SetBackground(0.0, 0.0, 0.0);
    vtkRenderWindow *renderWindow = meshWidget->GetRenderWindow();
    renderWindow->AddRenderer(meshRenderer);
    // actor is not yet added to renderer
    
    // Set interaction style
    vtkRenderWindowInteractor *interactor = renderWindow->GetInteractor();
    meshStyle = vtkInteractorStyleTrackballCamera::New();
    interactor->SetInteractorStyle(meshStyle);
}


void PDicomSegmenter::createMeshDialog()
{
    // Mesh generation dialog
    genMeshDialog = new QWidget;
    genMeshDialog->setWindowTitle("Mesh Generation");
    genMeshDialog->setFixedSize(300, 350);
    QVBoxLayout *mainLayout = new QVBoxLayout;
    genMeshDialog->setLayout(mainLayout);
    
    // Intensity setting
    QGroupBox *gbox = new QGroupBox("Surface Intensity");
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(-200, 500);
    intensityBox = new QSpinBox;
    intensityBox->setRange(-200, 500);
    intensityBox->setValue(0);
    connect(slider, SIGNAL(valueChanged(int)),
        intensityBox, SLOT(setValue(int)));
    connect(intensityBox, SIGNAL(valueChanged(int)),
        slider, SLOT(setValue(int)));

    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(new QLabel("value"));
    hbox->addWidget(intensityBox);
    hbox->addWidget(slider);
    gbox->setLayout(hbox);
    
    // Apply button
    QPushButton *abutton = new QPushButton("apply");
    connect(abutton, SIGNAL(clicked()), this, SLOT(generateMesh()));
    hbox = new QHBoxLayout;
    hbox->addWidget(abutton);
    hbox->addStretch();
    
    mainLayout->addWidget(gbox);
    mainLayout->addLayout(hbox);
    mainLayout->addWidget(
        new QLabel("_____________________________________"));
    mainLayout->addSpacing(7);
    
    // Reduce resolution
    gbox = new QGroupBox("Reduce Resolution");
    ratioBox = new QDoubleSpinBox;
    ratioBox->setDecimals(2);
    ratioBox->setRange(0.0, 1.0);
    ratioBox->setSingleStep(0.1);
    ratioBox->setValue(0.3);
    
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(new QLabel("ratio        "), 0, 0);
    grid->addWidget(ratioBox, 0, 1);
    
    hbox = new QHBoxLayout;
    hbox->addLayout(grid);
    hbox->addStretch();
    gbox->setLayout(hbox);
    mainLayout->addWidget(gbox);
    
    // Surface Smoothing
    gbox = new QGroupBox("Surface Smoothing");
    factorBox = new QDoubleSpinBox;
    factorBox->setDecimals(2);
    factorBox->setRange(0.0, 1.0);
    factorBox->setSingleStep(0.1);
    factorBox->setValue(0.1);
    
    smoothIterBox = new QSpinBox;
    smoothIterBox->setRange(1, 30);
    smoothIterBox->setValue(1);
    
    grid = new QGridLayout;
    grid->addWidget(new QLabel("factor"), 0, 0);
    grid->addWidget(factorBox, 0, 1);
    grid->addWidget(new QLabel("iteration"), 1, 0);
    grid->addWidget(smoothIterBox, 1, 1);
    
    hbox = new QHBoxLayout;
    hbox->addLayout(grid);
    hbox->addStretch();
    gbox->setLayout(hbox);
    mainLayout->addWidget(gbox);
    
    // Apply and close buttons
    abutton = new QPushButton("apply");
    connect(abutton, SIGNAL(clicked()), this, SLOT(smoothing()));
    closeMeshButton = new QPushButton("close");
    connect(closeMeshButton, SIGNAL(clicked()),
        genMeshDialog, SLOT(hide()));
    
    hbox = new QHBoxLayout;
    hbox->addWidget(abutton);
    hbox->addStretch();
    hbox->addWidget(closeMeshButton);
    mainLayout->addLayout(hbox);
}


void PDicomSegmenter::addActions()
{
    connect(loadDirAction, SIGNAL(triggered()), this, SLOT(resetInput()));
    connect(loadDirAction, SIGNAL(triggered()), this, SLOT(resetVoi()));

    disconnect(infoAction, 0, 0, 0);  // Override
    connect(infoAction, SIGNAL(triggered()), this, SLOT(info()));
    
    disconnect(helpAction, 0, 0, 0);  // Override
    connect(helpAction, SIGNAL(triggered()), this, SLOT(help()));
    
    saveMeshPlyAction = new QAction(tr("in PLY format"), this);
    connect(saveMeshPlyAction, SIGNAL(triggered()),
        this, SLOT(saveMeshPly()));

    saveMeshStlAction = new QAction(tr("in STL format"), this);
    connect(saveMeshStlAction, SIGNAL(triggered()),
        this, SLOT(saveMeshStl()));
        
    saveVolumeViewAction = new QAction(tr("&Volume view"), this);
    saveVolumeViewAction->setStatusTip(tr("Save volume view to a file"));
    connect(saveVolumeViewAction, SIGNAL(triggered()),
        this, SLOT(saveVolumeView()));
        
    saveMeshViewAction = new QAction(tr("&Mesh view"), this);
    saveMeshViewAction->setStatusTip(tr("Save mesh view to a file"));
    connect(saveMeshViewAction, SIGNAL(triggered()),
        this, SLOT(saveMeshView()));
    
    resetInputAction = new QAction(tr("Reset Input"), this);
    resetInputAction->setIcon(QIcon(":/images/reset.png"));
    resetInputAction->setStatusTip(tr("Reset to original input."));
    connect(resetInputAction, SIGNAL(triggered()),
        this, SLOT(resetInput()));

    setVoiAction = new QAction(tr("Set &VOI"), this);
    setVoiAction->setIcon(QIcon(":/images/roi.png"));
    setVoiAction->setStatusTip(tr("Set volume of interest."));
    setVoiAction->setCheckable(true);
    connect(setVoiAction, SIGNAL(triggered()), this, SLOT(setVoi()));
    
    cropVoiAction = new QAction(tr("Crop VOI"), this);
    cropVoiAction->setIcon(QIcon(":/images/cut.png"));
    cropVoiAction->setStatusTip(tr("Crop volume of interest."));
    connect(cropVoiAction, SIGNAL(triggered()), this, SLOT(cropVoi()));
    
    thresholdAction = new QAction(tr("&Thresholding"), this);
    thresholdAction->setIcon(QIcon(":/images/halfwhite.png"));
    thresholdAction->setShortcut(tr("Ctrl+T"));
    thresholdAction->setStatusTip(tr("Apply thresholding to image."));
    thresholdAction->setCheckable(true);
    connect(thresholdAction, SIGNAL(triggered()),
        this, SLOT(showThresholdDialog()));
    connect(thresholdDialog, SIGNAL(closed()),
        thresholdAction, SLOT(toggle()));
    
    removeSkullAction = new QAction(tr("Remove Skull"), this);
    removeSkullAction->setIcon(QIcon(":/images/fullwhite.png"));
    removeSkullAction->setStatusTip(tr("Remove skull in image."));
    removeSkullAction->setCheckable(true);
    connect(removeSkullAction, SIGNAL(triggered()),
        this, SLOT(showSkullRemover()));
    connect(skullRemover, SIGNAL(closed()),
        removeSkullAction, SLOT(toggle()));

    anisoDiffuseAction = new QAction(tr("&Anisotropic Diffusion"), this);
    anisoDiffuseAction->setIcon(QIcon(":/images/smooth.png"));
    anisoDiffuseAction->setShortcut(tr("Ctrl+A"));
    anisoDiffuseAction->setStatusTip(tr("Apply anisotropic diffusion."));
    anisoDiffuseAction->setCheckable(true);
    connect(anisoDiffuseAction, SIGNAL(triggered()),
        this, SLOT(showDiffuserDialog()));
    connect(closeDiffuseButton, SIGNAL(clicked()),
        anisoDiffuseAction, SLOT(toggle()));
        
    volumeRenderAction = new QAction(tr("Volume &Render"), this);
    volumeRenderAction->setIcon(QIcon(":/images/volume.png"));
    volumeRenderAction->setStatusTip(tr("Volume render segmented volumes."));
    volumeRenderAction->setCheckable(true);
    connect(volumeRenderAction, SIGNAL(triggered()),
        this, SLOT(showVolumeRenderDialog()));
    connect(closeVolumeButton, SIGNAL(clicked()),
        volumeRenderAction, SLOT(toggle()));
        
    genMeshAction = new QAction(tr("Generate &Mesh"), this);
    genMeshAction->setIcon(QIcon(":/images/mesh.png"));
    genMeshAction->setShortcut(tr("Ctrl+G"));
    genMeshAction->setStatusTip(tr("Generate mesh model."));
    genMeshAction->setCheckable(true);
    connect(genMeshAction, SIGNAL(triggered()),
        this, SLOT(showGenMeshDialog()));
    connect(closeMeshButton, SIGNAL(clicked()),
        genMeshAction, SLOT(toggle()));
    
    // Mutually exclusive actions
    connect(volumeRenderAction, SIGNAL(toggled(bool)),
        genMeshAction, SLOT(setDisabled(bool)));
    connect(genMeshAction, SIGNAL(toggled(bool)),
        volumeRenderAction, SLOT(setDisabled(bool)));
}


void PDicomSegmenter::addMenus()
{
    saveMeshMenu = new QMenu(tr("Save &Meshes"));
    saveMeshMenu->setIcon(QIcon("images/save.png"));
    fileMenu->insertMenu(exitAction, saveMeshMenu);
    saveMeshMenu->addAction(saveMeshPlyAction);
    saveMeshMenu->addAction(saveMeshStlAction);
    
    saveViewMenu->addAction(saveVolumeViewAction);
    saveViewMenu->addAction(saveMeshViewAction);
    
    toolMenu = new QMenu(tr("Tool"));
    menuBar()->insertMenu(helpMenu->menuAction(), toolMenu);
    toolMenu->addAction(resetInputAction);
    toolMenu->addAction(setVoiAction);
    toolMenu->addAction(cropVoiAction);
    toolMenu->addAction(thresholdAction);
    toolMenu->addAction(removeSkullAction);
    toolMenu->addAction(anisoDiffuseAction);

    outputMenu = new QMenu(tr("Output"));
    menuBar()->insertMenu(helpMenu->menuAction(), outputMenu);
    outputMenu->addAction(volumeRenderAction);
    outputMenu->addAction(genMeshAction);
}


void PDicomSegmenter::addToolBars()
{
    toolToolBar = new QToolBar(tr("Tool"));
    insertToolBar(helpToolBar, toolToolBar);
    toolToolBar->addAction(resetInputAction);
    toolToolBar->addAction(setVoiAction);
    toolToolBar->addAction(cropVoiAction);
    toolToolBar->addAction(thresholdAction);
    toolToolBar->addAction(removeSkullAction);
    toolToolBar->addAction(anisoDiffuseAction);
    
    outputToolBar = new QToolBar(tr("Output"));
    insertToolBar(helpToolBar, outputToolBar);
    outputToolBar->addAction(volumeRenderAction);
    outputToolBar->addAction(genMeshAction);
}


// Slot functions

void PDicomSegmenter::info()
{
    QString msg1, msg2;
    
    if (!loaded)
        msg1 = QString("No volume loaded.");
    else
    {
        double *sp = reader->GetPixelSpacing();
        
        msg1 = QString("DICOM directory: %1<br><br>").arg(fullDirName) +
            QString("Patient name: %1<br>").arg(reader->GetPatientName()) +
            QString("Description: %1<br>").arg(reader->GetDescriptiveName()) +
            QString("Study ID: %1<br>").arg(reader->GetStudyID()) +
            QString("Study UID: %1<br>").arg(reader->GetStudyUID()) +
            QString("Size: %1 x %2 x %3 pixels<br>").arg(imageWidth).
                arg(imageHeight).arg(imageDepth) +
            QString("Pixel spacing: %1, %2, %3<br>").arg(sp[0]).
                arg(sp[1]).arg(sp[2]);
    }
        
    if (outputMesh)
    {
        vtkCellArray *mesh = outputMesh->GetPolys();
        msg2 = QString("<br>Mesh has %1 faces, %2 MB<br>").
           arg(mesh->GetNumberOfCells()).
           arg(mesh->GetActualMemorySize() / 1000.0, 0, 'f', 2);
    }
    else
        msg2 = QString();
           
    QMessageBox::about(this, tr("Data Information"),
        QString("<h2>%1</h2> <p></p>").arg(appName) + msg1 + msg2);
}


void PDicomSegmenter::saveMeshPly()
{
    if (!outputMesh)
    {
        QMessageBox::critical(this, appName,
            "No segmented mesh model to save.");
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save segmented mesh model into a PLY file"), ".",
        tr("PLY file (*.ply)"));
        
    vtkPLYWriter *writer = vtkPLYWriter::New();
    writer->SetFileName(fileName.toAscii().data());
    writer->SetInput(outputMesh);
    writer->Update();
    writer->Delete();
}


void PDicomSegmenter::saveMeshStl()
{
    if (!outputMesh)
    {
        QMessageBox::critical(this, appName,
            "No segmented mesh model to save.");
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save segmented mesh model into a STL file"), ".",
        tr("STL file (*.stl)"));
        
    vtkSTLWriter *writer = vtkSTLWriter::New();
    writer->SetFileName(fileName.toAscii().data());
    writer->SetInput(outputMesh);
    writer->Update();
    writer->Delete();
}


void PDicomSegmenter::saveVolumeView()
{
    saveView(3);
}


void PDicomSegmenter::saveMeshView()
{
    saveView(4);
}


void PDicomSegmenter::resetPipeline()
{
    // Reset in reverse order
    
    if (meshRenderer)
    {
        // Remove renderer
        meshRenderer->RemoveAllViewProps();
        meshWidget->GetRenderWindow()->RemoveRenderer(meshRenderer);
        meshRenderer->Delete();
        
        // Re-create renderer
        meshRenderer = vtkRenderer::New();  // Camera is also reset.
        meshRenderer->SetBackground(0.0, 0.0, 0.0);
        meshWidget->GetRenderWindow()->AddRenderer(meshRenderer);
        hasMeshActor = false;
    }
    
    genMeshDialog->hide();
    genMeshAction->setChecked(false);
    mcubes->SetInputConnection(NULL);
    decimate->SetInputConnection(NULL);
    normals->SetInputConnection(NULL);
    if (outputMesh)
        outputMesh->Delete();
    outputMesh = NULL;
    
    if (volumeRenderer)
    {
        // Remove renderer
        volumeRenderer->RemoveAllViewProps();
        volumeWidget->GetRenderWindow()->RemoveRenderer(volumeRenderer);
        volumeRenderer->Delete();
        
        // Re-create renderer
        volumeRenderer = vtkRenderer::New();  // Camera is also reset.
        volumeRenderer->SetBackground(0.0, 0.0, 0.0);
        volumeWidget->GetRenderWindow()->AddRenderer(volumeRenderer);
        hasVolumeActor = false;
    }
    
    volumeRenderDialog->hide();
    volumeRenderAction->setChecked(false);
    if (outputVolume)
        outputVolume->Delete();
    outputVolume = NULL;
    
    if (setVoiAction->isChecked())  // Reset this one first
    {
        transVoi->hide();
        coronalVoi->hide();
        sagittalVoi->hide();

        setVoiAction->setChecked(false);
        extractVoi->SetInputConnection(NULL);
        voiDone = false;
    }
    
    anisoDiffuseDialog->hide();
    anisoDiffuseAction->setChecked(false);
    anisoDiffuser->SetInputConnection(NULL);
    anisoDiffuseDone = false;
    
    skullRemover->hide();
    removeSkullAction->setChecked(false);
    skullRemover->setInputConnection(NULL);
    removeSkullDone = false;
    segmented = false;
    
    thresholdDialog->hide();
    thresholdAction->setChecked(false);
    thresholdDialog->setInputConnection(NULL);
    thresholdDone = false;
    segmented = false;
}


void PDicomSegmenter::resetInput()
{
    resetPipeline();
    
    if (volumeWidget->isVisible())
        volumeWidget->GetRenderWindow()->Render();
    else
        meshWidget->GetRenderWindow()->Render();
    
    if (loaded)
    {
        transViewer->SetInputConnection(reader->GetOutputPort());
        coronalViewer->SetInputConnection(reader->GetOutputPort());
        sagittalViewer->SetInputConnection(reader->GetOutputPort());
        
        transVoi->setInteractor(transViewer->GetRenderWindow()->
            GetInteractor());
        coronalVoi->setInteractor(coronalViewer->GetRenderWindow()->
            GetInteractor());
        sagittalVoi->setInteractor(sagittalViewer->GetRenderWindow()->
            GetInteractor());
        
        updateViewers();
        
        // Set initial VOI
        int *bound = reader->GetDataExtent();
        double *spacing = reader->GetPixelSpacing();
        selectedVoi[0] = bound[0] / spacing[0];
        selectedVoi[1] = bound[1] / spacing[0];
        selectedVoi[2] = bound[2] / spacing[1];
        selectedVoi[3] = bound[3] / spacing[1];
        selectedVoi[4] = bound[4] / spacing[2];
        selectedVoi[5] = bound[5] / spacing[2];
    } 
}


void PDicomSegmenter::updateViewers()
{
    transViewer->Render();
    coronalViewer->Render();
    sagittalViewer->Render();
}


void PDicomSegmenter::help()
{
    QMessageBox::about(this, tr("Help information"),
       QString("<h2>%1</h2>").arg(appName) +
       "<p></p>" +
       "<p>Tool Sequence:<br>" +
       "The tools, if used, are chained in the following order:<br>" +
       "1. Volume of interest (VOI) selection<br>" +
       "2. Thresholding<br>" +
       "3. Skull removal<br>" +
       "4. Anisotropic diffusion (noise reduction)<br>" +
       "5. Outupt generation: volume rendering or mesh<br>" +
       "Volume rendering is usually faster than mesh generation.<br>" +
       
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


void PDicomSegmenter::resetVoi()
{
    if (loaded)
    {
        transVoi->initPosition(transViewer->GetImageActor(),
            transViewer->GetRenderer()->GetActiveCamera());
        coronalVoi->initPosition(coronalViewer->GetImageActor(),
            coronalViewer->GetRenderer()->GetActiveCamera());
        sagittalVoi->initPosition(sagittalViewer->GetImageActor(),
            sagittalViewer->GetRenderer()->GetActiveCamera());
        
        setVoiAction->setChecked(false);
        voiDone = false;
    }
}


void PDicomSegmenter::setVoi()
{
    if (!loaded)
    {
        QMessageBox::critical(this, appName,
            "No volume image to work on.<br>Please load a volume image.");
        setVoiAction->setChecked(false);
        return;
    }
    
    bool isVisible = setVoiAction->isChecked();
    transVoi->show(isVisible);
    transVoi->update();
    coronalVoi->show(isVisible);
    coronalVoi->update();
    sagittalVoi->show(isVisible);
    sagittalVoi->update();
    
    if (isVisible)
        wakeVoi();
}


void PDicomSegmenter::cropVoi()
{    
    double *spacing = reader->GetPixelSpacing();
    int *bound = computeBound();
    selectedVoi[0] = bound[0] / spacing[0];
    selectedVoi[1] = bound[1] / spacing[0];
    selectedVoi[2] = bound[2] / spacing[1];
    selectedVoi[3] = bound[3] / spacing[1];
    selectedVoi[4] = bound[4] / spacing[2];
    selectedVoi[5] = bound[5] / spacing[2];
    
    extractVoi->SetVOI(selectedVoi);
    extractVoi->SetInputConnection(reader->GetOutputPort());
    transViewer->SetInputConnection(extractVoi->GetOutputPort());
    coronalViewer->SetInputConnection(extractVoi->GetOutputPort());
    sagittalViewer->SetInputConnection(extractVoi->GetOutputPort());
    updateViewers();
    voiDone = true;
    
    // Reset in reverse order
    genMeshDialog->hide();
    genMeshAction->setChecked(false);
    
    volumeRenderDialog->hide();
    volumeRenderAction->setChecked(false);
    
    anisoDiffuseDialog->hide();
    anisoDiffuseAction->setChecked(false);
    anisoDiffuseDone = false;
    
    skullRemover->hide();
    removeSkullAction->setChecked(false);
    removeSkullDone = false;
    segmented = false;
    
    thresholdDialog->hide();
    thresholdAction->setChecked(false);
    thresholdDone = false;
    segmented = false;
}


void PDicomSegmenter::showThresholdDialog()
{    
    if (!loaded)
    {
        QMessageBox::critical(this, appName,
            "No volume image to work on.<br>Please load a volume image.");
        thresholdAction->setChecked(false);
        return;
    }
    
    bool visible = thresholdAction->isChecked();
    thresholdDialog->setVisible(visible);

    if (visible)
    {   
        skullRemover->hide();  // Reset
        removeSkullAction->setChecked(false);
        removeSkullDone = false;
        
        anisoDiffuseDialog->hide();  // Reset
        anisoDiffuseAction->setChecked(false);
        anisoDiffuseDone = false;
        
        if (voiDone)
            thresholdDialog->setInputConnection(
                extractVoi->GetOutputPort());
        else
            thresholdDialog->setInputConnection(
                reader->GetOutputPort());
            
        transViewer->SetInputConnection(
            thresholdDialog->getOutputPort());
        coronalViewer->SetInputConnection(
            thresholdDialog->getOutputPort());
        sagittalViewer->SetInputConnection(
            thresholdDialog->getOutputPort());
        thresholdDone = true;
        segmented = true;
    }
}


void PDicomSegmenter::showSkullRemover()
{
    if (!loaded)
    {
        QMessageBox::critical(this, appName,
            "No volume image to work on.<br>Please load a volume image.");
        removeSkullAction->setChecked(false);
        return;
    }
    
    bool visible = removeSkullAction->isChecked();
    skullRemover->setVisible(visible);
    
    if (visible)
    {
        anisoDiffuseDialog->hide();  // Reset
        anisoDiffuseAction->setChecked(false);
        anisoDiffuseDone = false;
        
        if (thresholdDone)
            skullRemover->setInputConnection(
                thresholdDialog->getOutputPort());
        else if (voiDone)
            skullRemover->setInputConnection(
                extractVoi->GetOutputPort());
        else
            skullRemover->setInputConnection(
                reader->GetOutputPort());

        transViewer->SetInputConnection(
            skullRemover->getOutputPort());
        coronalViewer->SetInputConnection(
            skullRemover->getOutputPort());
        sagittalViewer->SetInputConnection(
            skullRemover->getOutputPort());
        removeSkullDone = true;
        segmented = true;
    }
}


void PDicomSegmenter::showDiffuserDialog()
{
    if (!loaded)
    {
        QMessageBox::critical(this, appName,
            "No volume image to work on.<br>Please load a volume image.");
        anisoDiffuseAction->setChecked(false);
        return;
    }

    bool visible = anisoDiffuseAction->isChecked();
    anisoDiffuseDialog->setVisible(visible);
    
    if (visible)
    {
        if (removeSkullDone)
            anisoDiffuser->SetInputConnection(
                skullRemover->getOutputPort());
        else if (thresholdDone)
            anisoDiffuser->SetInputConnection(
                thresholdDialog->getOutputPort());
        else if (voiDone)
            anisoDiffuser->SetInputConnection(
                extractVoi->GetOutputPort());
        else
            anisoDiffuser->SetInputConnection(
                reader->GetOutputPort());
    
        transViewer->SetInputConnection(
            anisoDiffuser->GetOutputPort());
        coronalViewer->SetInputConnection(
            anisoDiffuser->GetOutputPort());
        sagittalViewer->SetInputConnection(
            anisoDiffuser->GetOutputPort());
            
        anisoDiffuseDone = true;
    }
}


void PDicomSegmenter::setDiffThreshold(int threshold)
{
    anisoDiffuser->SetDiffusionThreshold((double) threshold);
    updateViewers();
}


void PDicomSegmenter::setDiffFactor(double factor)
{
    anisoDiffuser->SetDiffusionFactor((double) factor);
    updateViewers();
}


void PDicomSegmenter::anisoDiffuse()
{
    anisoDiffuser->SetDiffusionThreshold((double) diffThresholdBox->value());
    anisoDiffuser->SetDiffusionFactor((double) diffFactorBox->value());
    updateViewers();
}


void PDicomSegmenter::showVolumeRenderDialog()
{
    if (!loaded)
    {
        QMessageBox::critical(this, appName,
            "No volume image to work on.<br>Please load a volume image.");
        volumeRenderAction->setChecked(false);
        return;
    }
    
    if (meshWidget->isVisible())  // Changing from mesh view
    {
        meshWidget->hide();
        volumeWidget->show();
    }
    
    bool visible = volumeRenderAction->isChecked();
    volumeRenderDialog->setVisible(visible);
}


void PDicomSegmenter::volumeRender()
{
    if (!loaded)
    {
        QMessageBox::critical(this, appName,
            "No volume image to work on.<br>Please load a volume image.");
        volumeRenderAction->setChecked(false);
        return;
    }
    
    if (!hasVolumeActor)
    {
        volumeRenderer->AddActor(volumeActor);
        hasVolumeActor = true;
    }
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    computeOutputVolume();
    setBlendType();
#ifdef USE_SMART_MAPPER
    if (sampleDistanceBox->currentIndex() == 0)
    {
        smartMapper->SetInput(outputVolume);
        volumeMapper = smartMapper;
    }
    else
    {
        rayCastMapper->SetInput(outputVolume);
        volumeMapper = rayCastMapper;
    }
#else
    rayCastMapper->SetInput(outputVolume);
    volumeMapper = rayCastMapper;
#endif
    volumeActor->SetMapper(volumeMapper);
    volumeRenderer->ResetCamera();
    volumeWidget->GetRenderWindow()->Render();
    QApplication::restoreOverrideCursor();
}


void PDicomSegmenter::setSampleDistance(int option)
{
    if (!loaded)
        return;
       
    float distance = 1.0 / (1 << option);
    rayCastMapper->SetSampleDistance(distance);
}


void PDicomSegmenter::showGenMeshDialog()
{    
    if (!loaded)
    {
        QMessageBox::critical(this, appName,
            "No volume image to work on.<br>Please load a volume image.");
        genMeshAction->setChecked(false);
        return;
    }
    
    if (!segmented)
    {
        QMessageBox::critical(this, appName,
            "Please apply segmentation tools first.");
        genMeshAction->setChecked(false);
        return;
    }
    
    if (volumeWidget->isVisible())  // Changing from volume rendering view
    {
        volumeWidget->hide();
        meshWidget->show();
    }
    
    bool visible = genMeshAction->isChecked();
    genMeshDialog->setVisible(visible);
}


void PDicomSegmenter::generateMesh()
{
    if (!loaded)
    {
        QMessageBox::critical(this, appName,
            "No volume image to work on.<br>Please load a volume image.");
        return;
    }
    
    if (!segmented)
    {
        QMessageBox::critical(this, appName,
            "Please apply segmentation first.");
        return;
    }
    
    if (!hasMeshActor)
    {
        meshRenderer->AddActor(meshActor);
        hasMeshActor = true;
    }
    
    if (anisoDiffuseDone)
        mcubes->SetInputConnection(anisoDiffuser->GetOutputPort());
    else if (removeSkullDone)
        mcubes->SetInputConnection(skullRemover->getOutputPort());
    else if (thresholdDone)
        mcubes->SetInputConnection(thresholdDialog->getOutputPort());
    else
    {
        QMessageBox::critical(this, appName,
            "Please apply segmentation first.");
        return;
    }
    
    mcubes->SetValue(0, intensityBox->value());
    normals->SetInputConnection(mcubes->GetOutputPort());
        
    QApplication::setOverrideCursor(Qt::WaitCursor);
    normals->Update();
    if (outputMesh)
        outputMesh->Delete();
    outputMesh = vtkPolyData::New();
    outputMesh->DeepCopy(normals->GetOutput());
    
    meshMapper->SetInput(outputMesh);  // Cut connection from normals.
    meshActor->SetMapper(meshMapper);
    meshRenderer->ResetCamera();
    meshWidget->GetRenderWindow()->Render();
    QApplication::restoreOverrideCursor();
}


// Mesh decimation and smoothing

void PDicomSegmenter::smoothing()
{
    if (!outputMesh)
    {
        QMessageBox::critical(this, appName,
            "Please generate the segmented mesh first.");
        return;
    }
    
    decimate->SetInputConnection(mcubes->GetOutputPort());
    decimate->SetTargetReduction(1.0 - ratioBox->value());
    smooth->SetRelaxationFactor(factorBox->value());
    smooth->SetNumberOfIterations(smoothIterBox->value());
    // decimate to smooth is already connected.
    normals->SetInputConnection(smooth->GetOutputPort());
    
    QApplication::setOverrideCursor(Qt::WaitCursor);
    normals->Update();
    if (outputMesh)
        outputMesh->Delete();
    outputMesh = vtkPolyData::New();
    outputMesh->DeepCopy(normals->GetOutput());
    
    meshMapper->SetInput(outputMesh);  // Cut connection from normals.
    meshWidget->GetRenderWindow()->Render();
    QApplication::restoreOverrideCursor();
}


// Supporting functions

void PDicomSegmenter::saveView(int type)
{
    if (!loaded)
        return;
    
    if (type == 3 && !hasVolumeActor)
    {
        QMessageBox::critical(this, appName,
            "Please apply volume rendering first.");
        return;
    }
    else if (type == 4 && !hasMeshActor)
    {
        QMessageBox::critical(this, appName,
            "Please apply mesh generation first.");
        return;
    }  
        
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save current view to a file"), ".",
        tr("Image files (*.jpg *.png *.tif)"));
        
    if (fileName.isEmpty())
        return;

    saveView(fileName, type);
}


bool PDicomSegmenter::saveView(const QString &fileName, int type)
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
            
        case 3:
            filter->SetInput(volumeWidget->GetRenderWindow());
            break;
            
        case 4:
            filter->SetInput(meshWidget->GetRenderWindow());
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


// Trick image viewer to update Voi

void PDicomSegmenter::wakeVoi()
{
    int v = transSlider->value();
    transSlider->setValue(v-1);
    transSlider->setValue(v);
    
    v = coronalSlider->value();
    coronalSlider->setValue(v-1);
    coronalSlider->setValue(v);
    
    v = sagittalSlider->value();
    sagittalSlider->setValue(v-1);
    sagittalSlider->setValue(v);
}


int *PDicomSegmenter::computeBound()
{
    static int bound[6];  // min x, max x, min y, max y, min z, max z
    int *b;
    
    b = transVoi->getBound();  // XY plane
    bound[0] = b[0];
    bound[1] = b[1];
    bound[2] = b[2];
    bound[3] = b[3];
    
    b = coronalVoi->getBound();  // XZ plane
    bound[0] = qMin(bound[0], b[0]);
    bound[1] = qMax(bound[1], b[1]);
    bound[4] = b[2];
    bound[5] = b[3];
    
    b = sagittalVoi->getBound();  // YZ plane
    bound[2] = qMin(bound[2], b[0]);
    bound[3] = qMax(bound[3], b[1]);
    bound[4] = qMin(bound[4], b[2]);
    bound[5] = qMax(bound[5], b[3]);
    
    return bound;
}


void PDicomSegmenter::computeOutputVolume()
{
    vtkImageAlgorithm *output;
    if (anisoDiffuseDone)
        output = anisoDiffuser;
    else if (removeSkullDone)
        output = skullRemover->getOutputFilter();
    else if (thresholdDone)
        output = thresholdDialog->getOutputFilter();
    else if (voiDone)
        output = extractVoi;
    else
        output = reader;

    output->UpdateWholeExtent();
    if (outputVolume)
        outputVolume->Delete();
    outputVolume = vtkImageData::New();
    outputVolume->DeepCopy(output->GetOutput());
    outputVolume->Update();
}


void PDicomSegmenter::setBlendType()
{       
    // Init
    opacityFn->RemoveAllPoints();
    colorFn->RemoveAllPoints();
    
    int blendType = viewTypeBox->currentIndex();
    double ambient = ambientBox->value();
    double specular = specularBox->value();
    
    // Add function points
    switch (blendType)
    {
        // CT Skin
        // Use compositing and functions set to highlight skin in CT data.
        case 0:     
            opacityFn->AddPoint(-2000, 0.0, 0.5, 0.0);
            opacityFn->AddPoint( -300, 0.0, 0.5, 0.5);
            opacityFn->AddPoint( -100, 1.0, 0.5, 0.0);
            opacityFn->AddPoint( 2000, 1.0, 0.5, 0.0);
                  
            colorFn->AddRGBPoint(-2000, 0.0, 0.0, 0.0, 0.5, 0.0);
            colorFn->AddRGBPoint( -300, 0.6, 0.4, 0.2, 0.5, 0.5);
            colorFn->AddRGBPoint( -100, 0.9, 0.8, 0.5, 0.5, 0.0);
            colorFn->AddRGBPoint( 2000, 1.0, 1.0, 1.0, 0.5, 0.0);
  
            volumeMapper->SetBlendModeToComposite();
            property->ShadeOn();
            property->SetAmbient(ambient);
            property->SetDiffuse(0.9);
            property->SetSpecular(specular);
            property->SetSpecularPower(10.0);
            break;
  
        // CT Muscle
        // Use compositing and functions set to highlight muscle in CT data.
        case 1:     
            opacityFn->AddPoint(-2000, 0.0, 0.5, 0.0);
            opacityFn->AddPoint(  -70, 0.0, 0.5, 0.5);
            opacityFn->AddPoint(  200, 1.0, 0.5, 0.0);
            opacityFn->AddPoint( 2000, 1.0, 0.5, 0.0);
                  
            colorFn->AddRGBPoint(-2000, 0.0, 0.0, 0.0, 0.5, 0.0);
            colorFn->AddRGBPoint(  -70, 1.0, 0.6, 0.6, 0.5, 0.5);
            colorFn->AddRGBPoint(  200, 1.0, 1.0, 1.0, 0.5, 0.0);
            colorFn->AddRGBPoint( 2000, 1.0, 1.0, 1.0, 0.5, 0.0);

            volumeMapper->SetBlendModeToComposite();
            property->ShadeOn();
            property->SetAmbient(ambient);
            property->SetDiffuse(0.9);
            property->SetSpecular(specular);
            property->SetSpecularPower(10.0);
            break;
            
        // CT Bone
        // Use compositing and functions set to highlight bone in CT data.
        case 2:
            opacityFn->AddPoint(-2000, 0.0, 0.5, 0.0);
            opacityFn->AddPoint(  -20, 0.0, 0.5, 0.5);
            opacityFn->AddPoint(  600, 1.0, 0.5, 0.0);
            opacityFn->AddPoint( 2000, 1.0, 0.5, 0.0);

            colorFn->AddRGBPoint(-2000, 0.0, 0.0, 0.0, 0.5, 0.0);
            colorFn->AddRGBPoint(  -20, 1.0, 0.3, 0.3, 0.5, 0.5);
            colorFn->AddRGBPoint(  600, 1.0, 1.0, 1.0, 0.5, 0.0);
            colorFn->AddRGBPoint( 2000, 1.0, 1.0, 1.0, 0.5, 0.0);
  
            volumeMapper->SetBlendModeToComposite();
            property->ShadeOn();
            property->SetAmbient(ambient);
            property->SetDiffuse(0.9);
            property->SetSpecular(specular);
            property->SetSpecularPower(10.0);
            break;
            
        // CTA Vessels
        // Use compositing and functions set to highlight bone in CT data.
        case 3:
            opacityFn->AddPoint(-2000, 0.0, 0.5, 0.0);
            opacityFn->AddPoint(  -20, 0.0, 0.5, 0.5);
            opacityFn->AddPoint(   50, 0.0, 0.5, 0.5);
            opacityFn->AddPoint(  100, 1.0, 0.5, 0.0);
            opacityFn->AddPoint( 2000, 1.0, 0.5, 0.0);

            colorFn->AddRGBPoint(-2000, 0.0, 0.0, 0.0, 0.5, 0.0);
            colorFn->AddRGBPoint(  -20, 0.3, 0.3, 0.3, 0.5, 0.5);
            colorFn->AddRGBPoint(   50, 0.3, 0.1, 0.1, 0.5, 0.5);
            colorFn->AddRGBPoint(  100, 1.0, 0.6, 0.6, 0.5, 0.0);
            colorFn->AddRGBPoint( 2000, 1.0, 0.6, 0.6, 0.5, 0.0);
  
            volumeMapper->SetBlendModeToComposite();
            property->ShadeOn();
            property->SetAmbient(ambient);
            property->SetDiffuse(0.9);
            property->SetSpecular(specular);
            property->SetSpecularPower(10.0);
            break;
  
        default: // Case 0
            opacityFn->AddPoint(-2000, 0.0, 0.5, 0.0);
            opacityFn->AddPoint( -300, 0.0, 0.5, 0.5);
            opacityFn->AddPoint( -100, 1.0, 0.5, 0.0);
            opacityFn->AddPoint( 2000, 1.0, 0.5, 0.0);
                  
            colorFn->AddRGBPoint(-2000, 0.0, 0.0, 0.0, 0.5, 0.0);
            colorFn->AddRGBPoint( -300, 0.6, 0.4, 0.2, 0.5, 0.5);
            colorFn->AddRGBPoint( -100, 0.9, 0.7, 0.3, 0.5, 0.0);
            colorFn->AddRGBPoint( 2000, 1.0, 1.0, 1.0, 0.5, 0.0);
  
            volumeMapper->SetBlendModeToComposite();
            property->ShadeOn();
            property->SetAmbient(ambient);
            property->SetDiffuse(0.9);
            property->SetSpecular(specular);
            property->SetSpecularPower(10.0);
            break;
    }
}
