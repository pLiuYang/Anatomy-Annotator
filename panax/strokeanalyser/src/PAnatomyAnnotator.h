/* PAnatomyAnnotator.h

   Annotator of 3D mesh models of human anatomy.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#ifndef PANATOMYANNOTATOR_H
#define PANATOMYANNOTATOR_H

#include "PMeshViewer.h"
#include <QListWidget>


class PAnatomyAnnotator: public PMeshViewer
{
    Q_OBJECT

public:
    PAnatomyAnnotator();
    ~PAnatomyAnnotator();
    
protected:
    void addWidgets();
    void addActions();
    void addMenus();
    void addToolBars();
    
    // Widgets and variables for annotator in right dock widget
    QListWidget *annotator;
};

#endif