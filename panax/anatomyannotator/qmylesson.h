#ifndef QMYLESSON_H
#define QMYLESSON_H

#include <QtGui>
#include <QWidget>
#include <QTableWidget>
#include "qeditlesson.h"
#include "Lesson.h"
#include "slide.h"
#include "lessonwidget.h"

class QMyLesson : public QWidget
{
    Q_OBJECT
public:
    explicit QMyLesson(QWidget *parent = 0);
    QSize sizeHint() const;
    void clear();
    
protected:
    void closeEvent(QCloseEvent *event);

signals:
    
public slots:
    void open(int row, int col);
    void open();
    void edit();
    void add();
    void del();
    void showLessonList();
    void updateLessonSlides(QList<Slide> lsnS);

private:
    void createWidgets();
    void createActions();
    void createToolBars();

    QAction *openAction;
    QAction *editAction;
    QAction *addAction;
    QAction *delAction;

    QToolBar *toolBar;
    QTableWidget *lessonTable;
    QList<Lesson> lessonList;

    QEditLesson *editLesson;
    LessonWidget *lessonWidget;
};

#endif // QMYLESSON_H
