/* PSkullAnalyser.h

   3D skull model analyser

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#ifndef PSKULLANALYSER_H
#define PSKULLANALYSER_H

#include "PMeshViewer.h"


class PSkullAnalyser: public PMeshViewer
{
    Q_OBJECT

public:
    PSkullAnalyser();
    ~PSkullAnalyser();
    
protected:
    void addWidgets();
    void addActions();
    void addMenus();
    void addToolBars();
    
    // Widgets and variables for skull analyser

};

#endif