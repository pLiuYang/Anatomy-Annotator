/* PConsole.h

   Panax main GUI.

   Copyright 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#ifndef PCONSOLE_H
#define PCONSOLE_H

#include <QMainWindow>
#include "PMeshViewer.h"
#include "PDicomViewer.h"
#include "PVolumeRenderer.h"
#include "PDicomSegmenter.h"
#include "PAnatomyAnnotator.h"
#include "PStrokeAnalyser.h"
#include "PSkullAnalyser.h"

class QPushButton;


class PConsole: public QWidget
{
    Q_OBJECT

public:
    PConsole();
    ~PConsole();
    bool eventFilter(QObject *target, QEvent *event);
    
private:
    void closeEvent(QCloseEvent *event);
    
private slots:
    void toggleMeshViewer(bool visible);
    void toggleDicomViewer(bool visible);
    void toggleVolumeRenderer(bool visible);
    
    void toggleDicomSegmenter(bool visible);
    void toggleAnatomyAnnotator(bool visible);
    void toggleStrokeAnalyser(bool visible);
    void toggleSkullAnalyser(bool visible);

private:
    void createWidgets();
    
    QPushButton *quitButton;
    
    QPushButton *meshViewerButton;
    QPushButton *dicomViewerButton;
    QPushButton *volumeRendererButton;

    QPushButton *dicomSegmenterButton;
    QPushButton *anatomyAnnotatorButton;
    QPushButton *strokeAnalyserButton;
    QPushButton *skullAnalyserButton;
    
    // Private variables
    PMeshViewer *meshViewer;
    PDicomViewer *dicomViewer;
    PVolumeRenderer *volumeRenderer;
    PDicomSegmenter *dicomSegmenter;
    PAnatomyAnnotator *anatomyAnnotator;
    PStrokeAnalyser *strokeAnalyser;
    PSkullAnalyser *skullAnalyser;
};

#endif
