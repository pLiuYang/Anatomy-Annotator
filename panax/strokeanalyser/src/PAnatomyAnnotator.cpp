/* PAnatomyAnnotator.cpp

   Annotator of 3D mesh models of human anatomy.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#include "PAnatomyAnnotator.h"
#include <QMainWindow>
#include <QtGui>


PAnatomyAnnotator::PAnatomyAnnotator()
{
    addWidgets();
    addActions();
    addMenus();
    addToolBars();

    appName = QString("Anatomy Annotator");
    setWindowTitle(appName);
}


PAnatomyAnnotator::~PAnatomyAnnotator()
{
}


void PAnatomyAnnotator::addWidgets()
{
    // Example of how to replace widget in right dock area.
    annotator = new QListWidget;
    meshTable->hide();  // Hide meshTable in right dock area.
    rightDockWidget->setWidget(annotator);  // Change widget.
    rightDockWidget->setWindowTitle("Annotator");
}


void PAnatomyAnnotator::addActions()
{
}


void PAnatomyAnnotator::addMenus()
{
}


void PAnatomyAnnotator::addToolBars()
{
}

