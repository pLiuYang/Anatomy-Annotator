#ifndef QEDITASSESSMENT_H
#define QEDITASSESSMENT_H

#include <QDialog>
#include <QtGui>
#include <QWidget>
#include <QTableWidget>
#include "question.h"

class QLabel;
class QPushButton;
class Question;
class QToolBar;
class QTableWidget;

class QEditAssessment : public QDialog
{
    Q_OBJECT
public:
    QEditAssessment(QWidget *parent = 0);
    void setQuestionList(QList<Question> qL);

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void updateQuestionList(QList<Question> qL);

public slots:
    void save();
    void add();
    void del();
    void updateQuestionL(int, int);

private:
    void createWidgets();
    void createActions();
    void createToolBars();

    QAction *saveAction;
    QAction *addAction;
    QAction *delAction;

    QToolBar *toolBar;
    QTableWidget *questionTable;
    QList<Question> questionList;

    void refreshIndices();
    void updateTable(QList<Question> qL);
    void add(Question q);
};

#endif // QEDITASSESSMENT_H
