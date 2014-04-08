/* PAnatomyAnnotator.h

   Annotator of 3D mesh models of human anatomy.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#ifndef PANATOMYANNOTATOR_H
#define PANATOMYANNOTATOR_H

#include "PMeshViewer.h"
#include <QListWidget>
#include <QTabWidget>
#include <QStringList>
#include <QList>
#include "qmyassessment.h"
#include "qmylesson.h"

class vtkTextWidget;
class vtkCaptionWidget;

class PAnatomyAnnotator: public PMeshViewer
{
    Q_OBJECT

public:
    PAnatomyAnnotator();
    ~PAnatomyAnnotator();

    void updateLabelAnchor(double *pos);
    
protected slots:
    void showAnnotator();
    void showLesson();
    void hideAnatomy();
    void setLabelAnchor(QTableWidgetItem *item);
    void insertAnatomyInfo(const QModelIndex &parent, int row, int col);
    void removeAnatomyInfo(const QModelIndex &parent, int row, int col);

    void newProject();
    void openProject();
    void saveProject();
    void saveProjectAs();

protected:
    void addWidgets();
    void addActions();
    void addMenus();
    void addToolBars();



    // Annotator
    QAction *annotationAction;
    QAction *lessonAction;
    
    // Widgets and variables for annotator in right dock widget
    QMyAssessment *assessment;
    QMyLesson *lesson;
    QTabWidget *tabWidget;

    // for lesson and assessment
    QDockWidget *rightDockWidget2;

    // Annotation information
    QList<vtkCaptionWidget *> labelsWidgets;
    QStringList anatomyLabels;
    QStringList anatomyAnnotations;
    QList<double> anchorPosList;
    QList<double> anchorPos2DList;

    QString fileName;

    bool largeRightDock;
};

#endif
