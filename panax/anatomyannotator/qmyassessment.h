#ifndef QMYASSESSMENT_H
#define QMYASSESSMENT_H

#include <QtGui>
#include <QWidget>
#include <QTableWidget>
#include "qeditassessment.h"
#include "question.h"
#include "Assessment.h"
#include "assessmentwidget.h"

class QEditAssessment;

class QMyAssessment : public QWidget
{
    Q_OBJECT
public:
    explicit QMyAssessment(QWidget *parent = 0);
    
protected:
    void closeEvent(QCloseEvent *event);

signals:
    
public slots:
    void open(int row, int col);
    void open();
    void edit();
    void add();
    void del();
    void showAssessmentList();
    void updateQuestionList(QList<Question> qL);
    void updateAsmStatus();

private:
    void createWidgets();
    void createActions();
    void createToolBars();

    QAction *openAction;
    QAction *editAction;
    QAction *addAction;
    QAction *delAction;

    QToolBar *toolBar;
    QTableWidget *assessmentTable;
    QList<Assessment> assessmentList;

    AssessmentWidget *assessmentWidget;

    QEditAssessment *editAssessment;

    void renameAssessments();
};

#endif // QMYASSESSMENT_H
