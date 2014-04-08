#include "qmylesson.h"

QMyLesson::QMyLesson(QWidget *parent) :
    QWidget(parent)
{
    createWidgets();
    createActions();
    createToolBars();
}

void QMyLesson::clear()
{
    lessonList.clear();
    lessonTable->clear();
}

// Protected methods

void QMyLesson::closeEvent(QCloseEvent *event)
{
    event->accept();
}

QSize QMyLesson::sizeHint() const
{
    return QSize(parentWidget()->width(), parentWidget()->height());
}

// Create widgets, actions and toolbars

void QMyLesson::createWidgets()
{
    // Create assessment table
    lessonTable = new QTableWidget(0, 1);
    QPalette palette = lessonTable->palette();
    palette.setColor(QPalette::Base, QColor(248, 248, 248));
    lessonTable->setPalette(palette);
    lessonTable->verticalHeader()->hide();
    lessonTable->horizontalHeader()->hide();
    lessonTable->setShowGrid(false);
    lessonTable->setColumnWidth(0, 350);
    lessonTable->setSelectionMode(QTableWidget::SingleSelection);
    connect(lessonTable, SIGNAL(cellDoubleClicked(int, int)),
        this, SLOT(open(int, int)));

    lessonTable->installEventFilter(this);

    QList<Slide> sL;
    lessonWidget = new LessonWidget(sL, this);
    connect(lessonWidget, SIGNAL(updateSlides(QList<Slide>)), this, SLOT(updateLessonSlides(QList<Slide>)));
}

void QMyLesson::createActions()
{
    openAction = new QAction(tr("&Open"), this);
    openAction->setIcon(QIcon(":/images/open.png"));
    openAction->setStatusTip(tr("Open the selected Lesson"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    editAction = new QAction(tr("&Edit"), this);
    editAction->setIcon(QIcon(":/images/equipment.png"));
    editAction->setStatusTip(tr("Open the selected Lesson"));
    connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));

    addAction = new QAction(tr("&Add"), this);
    addAction->setIcon(QIcon(":/images/addmesh.png"));
    addAction->setStatusTip(tr("Add a new Lesson"));
    connect(addAction, SIGNAL(triggered()), this, SLOT(add()));

    delAction = new QAction(tr("&Delete"), this);
    delAction->setIcon(QIcon(":/images/deletemesh.png"));
    delAction->setStatusTip(tr("Delete an Lesson"));
    connect(delAction, SIGNAL(triggered()), this, SLOT(del()));
}

void QMyLesson::createToolBars()
{
    toolBar = new QToolBar("Tools", this);
    toolBar->addAction(openAction);
    //toolBar->addAction(editAction);
    toolBar->addAction(addAction);
    toolBar->addAction(delAction);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(toolBar);
    vbox->addWidget(lessonTable);
    vbox->addWidget(lessonWidget);
    lessonWidget->hide();

    setLayout(vbox);
    //show();
}

// Slot functions
void QMyLesson::open(int row, int col)
{
    qDebug() << "open row-" << row << " col-" << col;

    if (lessonTable->rowCount() == 0)
    {
        QMessageBox msgBox;
        msgBox.setText("Please add a lesson first!");
        msgBox.setWindowTitle("Warning");
        msgBox.exec();

        return;
    }

    Lesson curLsn = lessonList.value(row);
    lessonWidget->setSlidesList(curLsn.slides);
    connect(lessonWidget, SIGNAL(backtoLectureList()), this, SLOT(showLessonList()));

    lessonWidget->show();
    toolBar->hide();
    lessonTable->hide();
}

void QMyLesson::open()
{
    int row = lessonTable->currentRow();
    open(row, 0);
}

void QMyLesson::edit()
{
    // Create edit lesson dialog
    editLesson = new QEditLesson(this);
    connect(editLesson, SIGNAL(updateLessonSlides(QList<Slide>)), this, SLOT(updateLessonSlides(QList<Slide>)));

    int row = lessonTable->currentRow();

    if (row >= 0)
    {
        Lesson curLsn = lessonList.value(row);
        QList<Slide> lsnL = curLsn.slides;
        editLesson->setLessonSlides(lsnL);
        editLesson->show();
    }
}

void QMyLesson::add()
{
    Lesson newLsn;

    lessonList.append(newLsn);
    qDebug() << lessonList.length();

    // Append to assessment table
    int j = lessonTable->rowCount();
    lessonTable->insertRow(j);

    QString title = "Lesson ";
    title.append(QString::number(j+1));
    QTableWidgetItem *item = new QTableWidgetItem(title);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    lessonTable->setItem(j, 0, item);
}

void QMyLesson::del()
{
    int row = lessonTable->currentRow();
    //qDebug() << "row: " << row;

    // Remind the user when no lesson is selected
    if (row < 0)
    {
        QMessageBox msgBox;
        msgBox.setText("Please select an lesson first!");
        msgBox.setWindowTitle("Warning");
        msgBox.exec();
    }

    lessonTable->removeRow(row);
    lessonList.removeAt(row);
}

void QMyLesson::showLessonList()
{
    lessonWidget->hide();
    toolBar->show();
    lessonTable->show();
}

void QMyLesson::updateLessonSlides(QList<Slide> lsnS)
{
    int j = lessonTable->currentRow();

    if (j >= 0)
    {
        Lesson newLsn = lessonList.value(j);
        newLsn.slides = lsnS;
        lessonList.replace(j, newLsn);
    }
}
