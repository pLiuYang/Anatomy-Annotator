#ifndef LESSONWIDGET_H
#define LESSONWIDGET_H

#include <QWidget>
#include "question.h"
#include <QLabel>
#include <QTextEdit>
#include <QList>
#include <QPushButton>
#include <QtGui>
#include "slidingstackedwidget.h"
#include "slide.h"
#include "editlesson.h"

class LessonSlide;

class LessonWidget : public QWidget
{
    Q_OBJECT

public:
    LessonWidget(QList<Slide> sl, QWidget *parent = 0);

    void setSlidesList(QList<Slide> sl);

signals:
    void backtoLectureList();
    void updateSlides(QList<Slide> sl);

public slots:
    void back();
    void add();
    void del();
    void slideInNext();
    void slideInPrev();
    void slideInIdx(int index);
    void edit();
    void updateSlideContent(QString s);

protected:
    void createGuiControlComponents();
    void createSlidingStackedWidget();
    void createActions();
    void createToolBars();

    QAction *backAction;
    QAction *addAction;
    QAction *delAction;
    QAction *nextAction;
    QAction *prevAction;
    QAction *editAction;

    QToolBar *toolBar;

    QSlider *slideSpeed;
    QComboBox *listAll;

    QList<LessonSlide *> slides;
    SlidingStackedWidget *slidingStacked;

    int animTime;

private:
    QList<Slide> sL;
    int currIndex;
};

class LessonSlide : public QWidget
{
    Q_OBJECT

public:
    LessonSlide(Slide s, QWidget *parent = 0);

public slots:
    void updateCameraAngle();
    void updateSlideContent();

signals:
    void updateCameraAngleSignal();
    void updateSlideContent(QString s);

public:
    Slide curS;
    QTextEdit *slideContent;
    QPushButton *useStoredCameraPosition;
};

#endif // LESSONWIDGET_H
