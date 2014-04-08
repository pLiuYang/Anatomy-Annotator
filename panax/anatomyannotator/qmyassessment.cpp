#include "qmyassessment.h"

QMyAssessment::QMyAssessment(QWidget *parent) :
    QWidget(parent)
{
    createWidgets();
    createActions();
    createToolBars();
}

// Protected methods

void QMyAssessment::closeEvent(QCloseEvent *event)
{
    event->accept();
}

// Create widgets, actions and toolbars

void QMyAssessment::createWidgets()
{
    // Create assessment table
    assessmentTable = new QTableWidget(0, 2);
    QPalette palette = assessmentTable->palette();
    palette.setColor(QPalette::Base, QColor(248, 248, 248));
    assessmentTable->setPalette(palette);
    assessmentTable->verticalHeader()->hide();
    assessmentTable->horizontalHeader()->hide();
    assessmentTable->setShowGrid(false);
    assessmentTable->setColumnWidth(0, 150);
    assessmentTable->setColumnWidth(1, 100);
    assessmentTable->setSelectionMode(QTableWidget::SingleSelection);
    connect(assessmentTable, SIGNAL(cellDoubleClicked(int, int)),
        this, SLOT(open(int, int)));

    assessmentTable->installEventFilter(this);

    QList<Question> qL;
    assessmentWidget = new AssessmentWidget(qL, this);
}

void QMyAssessment::createActions()
{
    openAction = new QAction(tr("&Open"), this);
    openAction->setIcon(QIcon(":/images/open.png"));
    openAction->setStatusTip(tr("Open the selected Assessment"));
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    editAction = new QAction(tr("&Edit"), this);
    editAction->setIcon(QIcon(":/images/equipment.png"));
    editAction->setStatusTip(tr("Open the selected Assessment"));
    connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));

    addAction = new QAction(tr("&Add"), this);
    addAction->setIcon(QIcon(":/images/addmesh.png"));
    addAction->setStatusTip(tr("Add a new assessment"));
    connect(addAction, SIGNAL(triggered()), this, SLOT(add()));

    delAction = new QAction(tr("&Delete"), this);
    delAction->setIcon(QIcon(":/images/deletemesh.png"));
    delAction->setStatusTip(tr("Delete an assessment"));
    connect(delAction, SIGNAL(triggered()), this, SLOT(del()));
}

void QMyAssessment::createToolBars()
{
    toolBar = new QToolBar("Tools", this);
    toolBar->addAction(openAction);
    toolBar->addAction(editAction);
    toolBar->addAction(addAction);
    toolBar->addAction(delAction);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(toolBar);
    vbox->addWidget(assessmentTable);
    vbox->addWidget(assessmentWidget);
    assessmentWidget->hide();

    setLayout(vbox);
    //show();
}

// Slot functions
void QMyAssessment::open(int row, int col)
{
    qDebug() << "open row-" << row << " col-" << col;

    if (assessmentTable->rowCount() == 0)
    {
        QMessageBox msgBox;
        msgBox.setText("Please add an assessment first!");
        msgBox.setWindowTitle("Warning");
        msgBox.exec();

        return;
    }

    Assessment curAsm = assessmentList.value(row);
    assessmentWidget->setQuestionList(curAsm.questionList);
    connect(assessmentWidget, SIGNAL(backtoAssessmentList()), this, SLOT(showAssessmentList()));

    assessmentWidget->show();
    toolBar->hide();
    assessmentTable->hide();
}

void QMyAssessment::showAssessmentList()
{
    assessmentWidget->hide();
    toolBar->show();
    assessmentTable->show();

}

void QMyAssessment::open()
{
    int row = assessmentTable->currentRow();
    open(row, 0);
}

void QMyAssessment::edit()
{
    // Create edit assessment dialog
    editAssessment = new QEditAssessment(this);
    connect(editAssessment, SIGNAL(updateQuestionList(QList<Question>)), this, SLOT(updateQuestionList(QList<Question>)));

    int row = assessmentTable->currentRow();

    if (row >= 0)
    {
        Assessment curAsm = assessmentList.value(row);
        QList<Question> qL = curAsm.questionList;
        editAssessment->setQuestionList(qL);
        editAssessment->show();
    }
}

void QMyAssessment::add()
{
    Assessment newAsm;
    newAsm.isDone = false;

    assessmentList.append(newAsm);
    qDebug() << assessmentList.length();

    // Append to assessment table
    int j = assessmentTable->rowCount();
    assessmentTable->insertRow(j);

    QString title = "Assessment ";
    title.append(QString::number(j+1));
    QTableWidgetItem *item = new QTableWidgetItem(title);
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignCenter);
    assessmentTable->setItem(j, 0, item);

    item = new QTableWidgetItem("Not done");
    item->setFlags(Qt::ItemIsEnabled);
    item->setTextAlignment(Qt::AlignCenter);
    item->setBackground(QBrush(QColor(255, 255, 255)));
    assessmentTable->setItem(j, 1, item);
}

void QMyAssessment::del()
{
    int row = assessmentTable->currentRow();
    //qDebug() << "row: " << row;

    // Remind the user when no assessment is selected
    if (row < 0)
    {
        QMessageBox msgBox;
        msgBox.setText("Please select an assessment first!");
        msgBox.setWindowTitle("Warning");
        msgBox.exec();
    }

    assessmentTable->removeRow(row);
    assessmentList.removeAt(row);

    // Rename the assessments
    //renameAssessments();
}

void QMyAssessment::updateQuestionList(QList<Question> qL)
{
    //qDebug() << "here 3";
    int j = assessmentTable->currentRow();

    if (j >= 0)
    {//qDebug() << "here 4, current row: " << j;
        //qDebug() << j;
        //qDebug() << assessmentList.length();
        Assessment newAsm = assessmentList.value(j);
        newAsm.questionList = qL;//qDebug() << "here 5";
        assessmentList.replace(j, newAsm);//qDebug() << "here 6";
    }

    assessmentWidget->setQuestionList(qL);
}

void QMyAssessment::updateAsmStatus()
{
    int j = assessmentTable->currentRow();
    if (j >= 0)
    {
        Assessment newAsm = assessmentList.value(j);
        newAsm.isDone = true;
        assessmentList.replace(j, newAsm);

        // Update table
        assessmentTable->item(j, 1)->setText("Done");

    }
}

// Helper functions
void QMyAssessment::renameAssessments()
{
    int j = assessmentTable->rowCount();

    for (int i=0; i<j ; i++)
    {
        QString title = "Assessment ";
        title.append(QString::number(i+1));
        assessmentTable->item(i, 0)->setText(title);
    }
}
