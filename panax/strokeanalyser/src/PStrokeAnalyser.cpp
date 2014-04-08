/* PStrokeAnalyser.cpp

   Stroke CT Analyser.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#include "PStrokeAnalyser.h"
#include <QtGui>


PStrokeAnalyser::PStrokeAnalyser()
{
    segmenter = NULL;
    loaded = false;
    
    addWidgets();
    addActions();
    addMenus();
    addToolBars();

    appName = QString("Stroke Analyser");
    setWindowTitle(appName);
}


PStrokeAnalyser::~PStrokeAnalyser()
{
    delete segmenter;
}


// Qt event handlers

void PStrokeAnalyser::closeEvent(QCloseEvent *event)
{
    segmenter->hide();
    event->accept();
}


// Create widgets

void PStrokeAnalyser::addWidgets()
{
    segmenter = new PDicomSegmenter;
}


void PStrokeAnalyser::addActions()
{    
    showSegmenterAction = new QAction(tr("Show &Segmenter"), this);
    showSegmenterAction->setIcon(QIcon(":/images/segment.png"));
    showSegmenterAction->setShortcut(tr("Ctrl+S"));
    showSegmenterAction->setStatusTip(tr("Show DICOM segmenter."));
    connect(showSegmenterAction, SIGNAL(triggered()),
        segmenter, SLOT(show()));
        
    setSegmenterInputAction = new QAction(tr("Set Segmenter &Input"), this);
    setSegmenterInputAction->setIcon(QIcon(":/images/input.png"));
    setSegmenterInputAction->setShortcut(tr("Ctrl+I"));
    setSegmenterInputAction->setStatusTip(tr("Set input of segmenter."));
    connect(setSegmenterInputAction, SIGNAL(triggered()),
        this, SLOT(setSegmenterInput()));
        
    getSegmenterOutputAction = new QAction(tr("Get Segmenter &Output"), this);
    getSegmenterOutputAction->setIcon(QIcon(":/images/output.png"));
    getSegmenterOutputAction->setShortcut(tr("Ctrl+O"));
    getSegmenterOutputAction->setStatusTip(tr("Get output of segmenter."));
    connect(getSegmenterOutputAction, SIGNAL(triggered()),
        this, SLOT(getSegmenterOutput()));
}


void PStrokeAnalyser::addMenus()
{
    segmentMenu = new QMenu(tr("Segmenter"));
    menuBar()->insertMenu(helpMenu->menuAction(), segmentMenu);
    segmentMenu->addAction(showSegmenterAction);
    segmentMenu->addAction(setSegmenterInputAction);
    segmentMenu->addAction(getSegmenterOutputAction);
}


void PStrokeAnalyser::addToolBars()
{
    segmentToolBar = new QToolBar(tr("Segmenter"));
    insertToolBar(helpToolBar, segmentToolBar);
    segmentToolBar->addAction(showSegmenterAction);
    segmentToolBar->addAction(setSegmenterInputAction);
    segmentToolBar->addAction(getSegmenterOutputAction);
}


// Slot functions

void PStrokeAnalyser::setSegmenterInput()
{
    if (!loaded)
    {
        QMessageBox::critical(this, appName,
            "Please load a volume image.");
        return;
    }
    
    segmenter->setReader(reader);
}


void PStrokeAnalyser::getSegmenterOutput()
{
    setInput(segmenter->getOutputImage());
}
