/* PStrokeAnalyser.h

   Stroke CT Analyser.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#ifndef PSTROKEANALYSER_H
#define PSTROKEANALYSER_H

#include "PDicomSegmenter.h"


class PStrokeAnalyser: public PDicomViewer
{
    Q_OBJECT

public:
    PStrokeAnalyser();
    ~PStrokeAnalyser();
    
protected:
    void closeEvent(QCloseEvent *event);
    
protected slots:
    void setSegmenterInput();
    void getSegmenterOutput();

protected:
    void addWidgets();
    void addActions();
    void addMenus();
    void addToolBars();
    
    QMenu *segmentMenu;
    QToolBar *segmentToolBar;
    
    QAction *showSegmenterAction;
    QAction *setSegmenterInputAction;
    QAction *getSegmenterOutputAction;
    
    // Widgets and variables for stroke analyser
    PDicomSegmenter *segmenter;
};

#endif 