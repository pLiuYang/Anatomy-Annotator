#ifndef QEDITLESSON_H
#define QEDITLESSON_H

#include <QDialog>
#include <QtGui>
#include <QWidget>
#include <QTableWidget>
#include "slide.h"

class QLabel;
class QPushButton;
class Question;
class QToolBar;
class QTableWidget;

class QEditLesson : public QDialog
{
    Q_OBJECT
public:
    QEditLesson(QWidget *parent = 0);
    void setLessonSlides(QList<Slide> lsnS);

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void updateLessonSlides(QList<Slide> lsnS);

public slots:
    void save();
    void add();
    void del();
    void updateSlides(int, int);
    void enterSlideContent(int, int);
    void storeCameraPosition(int, int);

private:
    void createWidgets();
    void createActions();
    void createToolBars();

    QAction *saveAction;
    QAction *addAction;
    QAction *delAction;

    QToolBar *toolBar;
    QTableWidget *lessonTable;
    QList<Slide> lessonSlides;

    void refreshIndices();
    void updateTable(QList<Slide> lsnS);
    void add(Slide s);
};

#endif // QEDITLESSON_H
