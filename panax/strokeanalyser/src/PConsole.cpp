/* PConsole.cpp

   Panax main GUI.

   Copyright 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#include "PConsole.h"
#include <QtGui>


PConsole::PConsole()
{
    meshViewer = NULL;
    dicomViewer = NULL;
    volumeRenderer = NULL;
    dicomSegmenter = NULL;
    anatomyAnnotator = NULL;
    strokeAnalyser = NULL;
    skullAnalyser = NULL;
    createWidgets();
}


PConsole::~PConsole()
{
}


// Event handling

bool PConsole::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() == QEvent::Close)
    {
        if (target == meshViewer)
            meshViewerButton->setChecked(false);
        else if (target == dicomViewer)
            dicomViewerButton->setChecked(false);
        else if (target == volumeRenderer)
            volumeRendererButton->setChecked(false);
            
        else if (target == dicomSegmenter)
            dicomSegmenterButton->setChecked(false);
        else if (target == anatomyAnnotator)
            anatomyAnnotatorButton->setChecked(false);
        else if (target == strokeAnalyser)
            strokeAnalyserButton->setChecked(false);
        else if (target == skullAnalyser)
            skullAnalyserButton->setChecked(false);
    }
    return QWidget::eventFilter(target, event);
}


void PConsole::closeEvent(QCloseEvent *event)
{
    if (meshViewer)
        delete meshViewer;
    if (dicomViewer)
        delete dicomViewer;
    if (volumeRenderer)
        delete volumeRenderer;
        
    if (dicomSegmenter)
        delete dicomSegmenter;
    if (anatomyAnnotator)
        delete anatomyAnnotator;
    if (strokeAnalyser)
        delete strokeAnalyser;
    if (skullAnalyser)
        delete skullAnalyser;
        
    event->accept();
}


void PConsole::createWidgets()
{    
    // Viewers
    meshViewerButton = new QPushButton("Mesh Viewer");
    meshViewerButton->setCheckable(true);
    connect(meshViewerButton, SIGNAL(toggled(bool)),
        this, SLOT(toggleMeshViewer(bool)));
    
    dicomViewerButton = new QPushButton("DICOM Viewer");
    dicomViewerButton->setCheckable(true);
    connect(dicomViewerButton, SIGNAL(toggled(bool)),
        this, SLOT(toggleDicomViewer(bool)));

    volumeRendererButton = new QPushButton("Volume Renderer");
    volumeRendererButton->setCheckable(true);
    connect(volumeRendererButton, SIGNAL(toggled(bool)),
        this, SLOT(toggleVolumeRenderer(bool)));
        
    // Tools
    dicomSegmenterButton = new QPushButton("DICOM Segmenter");
    dicomSegmenterButton->setCheckable(true);
    connect(dicomSegmenterButton, SIGNAL(toggled(bool)),
        this, SLOT(toggleDicomSegmenter(bool)));
        
    anatomyAnnotatorButton = new QPushButton("Anatomy Annotator");
    anatomyAnnotatorButton->setCheckable(true);
    connect(anatomyAnnotatorButton, SIGNAL(toggled(bool)),
        this, SLOT(toggleAnatomyAnnotator(bool)));
    
    strokeAnalyserButton = new QPushButton("Stroke CT Analyser");
    strokeAnalyserButton->setCheckable(true);
    connect(strokeAnalyserButton, SIGNAL(toggled(bool)),
        this, SLOT(toggleStrokeAnalyser(bool)));
    
    skullAnalyserButton = new QPushButton("Skull Analyser");
    skullAnalyserButton->setCheckable(true);
    connect(skullAnalyserButton, SIGNAL(toggled(bool)),
        this, SLOT(toggleSkullAnalyser(bool)));
    
    // Buttons
    QVBoxLayout *col1 = new QVBoxLayout;    
    col1->addWidget(new QLabel("<b>Viewers</b>"), 0, Qt::AlignHCenter);
    col1->addWidget(meshViewerButton);
    col1->addWidget(dicomViewerButton);
    col1->addWidget(volumeRendererButton);
    col1->addStretch();
    
    QVBoxLayout *col2 = new QVBoxLayout;
    col2->addWidget(new QLabel("<b>Tools</b>"), 0, Qt::AlignHCenter);
    col2->addWidget(dicomSegmenterButton);
    col2->addWidget(anatomyAnnotatorButton);
    col2->addWidget(strokeAnalyserButton);
    col2->addWidget(skullAnalyserButton);
    col2->addStretch();
    
    QHBoxLayout *hbox1 = new QHBoxLayout;
    hbox1->addLayout(col1);
    hbox1->addSpacing(10);
    hbox1->addLayout(col2);
    hbox1->addStretch();

    // Title + buttons
    QVBoxLayout *vbox1 = new QVBoxLayout;
    vbox1->addWidget(new QLabel(
        QString("<h2>Panax</h2>\n") + 
        "An integrated platform for medical image &<br>" +
        "3D model analysis<br>"));
    vbox1->addLayout(hbox1);

    // Picture
    QPixmap *pix = new QPixmap(":/images/panax.jpg");
    QLabel *pixLabel = new QLabel;
    pixLabel->setPixmap(*pix);
    
    QHBoxLayout *hbox2 = new QHBoxLayout;
    hbox2->addWidget(pixLabel, 0, Qt::AlignTop);
    hbox2->addSpacing(10);
    hbox2->addLayout(vbox1);
    
    // Quit button   
    quitButton = new QPushButton("Quit");
    connect(quitButton, SIGNAL(clicked()), this, SLOT(close()));
    
    QHBoxLayout *hbox3 = new QHBoxLayout;
    hbox3->addWidget(quitButton);
    hbox3->addStretch();
    
    // Overall layout
    QVBoxLayout *vbox2 = new QVBoxLayout;
    vbox2->addLayout(hbox2);
    vbox2->addSpacing(5);
    vbox2->addLayout(hbox3);
    
    setLayout(vbox2);
    setFixedSize(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setWindowTitle("Panax");
    setWindowIcon(QIcon(":/images/panax-icon.png"));
}


// Slot functions

void PConsole::toggleMeshViewer(bool visible)
{
    if (!meshViewer)
    {
        meshViewer = new PMeshViewer;
        meshViewer->installEventFilter(this);
    }
    
    meshViewer->setVisible(visible);
}


void PConsole::toggleDicomViewer(bool visible)
{
    if (!dicomViewer)
    {
        dicomViewer = new PDicomViewer;
        dicomViewer->installEventFilter(this);
    }
    
    dicomViewer->setVisible(visible);
}


void PConsole::toggleVolumeRenderer(bool visible)
{
    if (!volumeRenderer)
    {
        volumeRenderer = new PVolumeRenderer;
        volumeRenderer->installEventFilter(this);
    }
    
    volumeRenderer->setVisible(visible);
}


void PConsole::toggleDicomSegmenter(bool visible)
{
    if (!dicomSegmenter)
    {
        dicomSegmenter = new PDicomSegmenter;
        dicomSegmenter->installEventFilter(this);
    }
    
    dicomSegmenter->setVisible(visible);
}


void PConsole::toggleAnatomyAnnotator(bool visible)
{
    if (!anatomyAnnotator)
    {
        anatomyAnnotator = new PAnatomyAnnotator;
        anatomyAnnotator->installEventFilter(this);
    }
    
    anatomyAnnotator->setVisible(visible);
}


void PConsole::toggleStrokeAnalyser(bool visible)
{
    if (!strokeAnalyser)
    {
        strokeAnalyser = new PStrokeAnalyser;
        strokeAnalyser->installEventFilter(this);
    }
    
    strokeAnalyser->setVisible(visible);
}


void PConsole::toggleSkullAnalyser(bool visible)
{
    if (!skullAnalyser)
    {
        skullAnalyser = new PSkullAnalyser;
        skullAnalyser->installEventFilter(this);
    }
    
    skullAnalyser->setVisible(visible);
}