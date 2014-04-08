#ifndef ASSESSMENTWIDGET_H
#define ASSESSMENTWIDGET_H

#include <QWidget>
#include "question.h"
#include <QLabel>
#include <QTextEdit>
#include <QList>
#include <QPushButton>
#include <QtGui>
#include "slidingstackedwidget.h"

class QuestionPage;

class AssessmentWidget : public QWidget
{
    Q_OBJECT

public:
    AssessmentWidget(QList<Question> ql, QWidget *parent = 0);

    void setQuestionList(QList<Question> ql);

signals:
    void updateAssessmentStatus();
    void backtoAssessmentList();

public slots:
    void back();
    void add();
    void del();
    void editQuestion();
    void slideInNext();
    void slideInPrev();

private:
    QList<Question> qL;
    int currIndex;

protected:
    void createGuiControlComponents();
    void createSlidingStackedWidget();
    void createActions();
    void createToolBars();

    QAction *backAction;
    QAction *addQuestionAction;
    QAction *delQuestionAction;
    QAction *nextAction;
    QAction *prevAction;
    QAction *editQuestionAction;

    QToolBar *toolBar;

    QSlider *slideSpeed;
    QComboBox *listAll;

    SlidingStackedWidget *slidingStacked;
    QList<QuestionPage *> qList;

    int animTime;
};

class QuestionPage : public QWidget
{
    Q_OBJECT

public:
    QuestionPage(Question q, QWidget *parent = 0);

public slots:
    void showAnswer();

private:
    Question curQ;
    QLabel *questionTitle;
    QPushButton *choiceA;
    QPushButton *choiceB;
    QPushButton *choiceC;
    QPushButton *choiceD;
    QLabel *answer;
};

class FeedbackPage : public QWidget
{
    Q_OBJECT

public:
    FeedbackPage(QWidget *parent = 0);

private:
    QLabel *label;
    QTextEdit *feedbackBox;
};

#endif // ASSESSMENTWIDGET_H
