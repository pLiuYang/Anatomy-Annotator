#include "qeditassessment.h"

QEditAssessment::QEditAssessment(QWidget *parent) : QDialog(parent)
{
    createWidgets();
    createActions();
    createToolBars();
    resize(570, 400);
    setWindowTitle(tr("Edit questions"));
}

void QEditAssessment::setQuestionList(QList<Question> qL)
{
    updateTable(qL);
}

// Protected methods

void QEditAssessment::closeEvent(QCloseEvent *event)
{
    close();
}

// Create widgets, actions and toolbars

void QEditAssessment::createWidgets()
{
    // Create question table
    questionTable = new QTableWidget(0, 7);
    QPalette palette = questionTable->palette();
    palette.setColor(QPalette::Base, QColor(248, 248, 248));
    questionTable->setPalette(palette);
    questionTable->verticalHeader()->hide();
    questionTable->setShowGrid(false);
    questionTable->setSelectionMode(QTableWidget::SingleSelection);

    QStringList labels;
    labels << "Index" << "Question" << "Choice A" << "Choice B" << "Choice C" << "Choice D" << "Answer" ;
    questionTable->setHorizontalHeaderLabels(labels);

    QList<QHeaderView::ResizeMode> modes;
    modes << QHeaderView::Fixed << QHeaderView::Interactive
          << QHeaderView::Interactive << QHeaderView::Interactive << QHeaderView::Interactive << QHeaderView::Interactive << QHeaderView::Interactive;
    QHeaderView *header = questionTable->horizontalHeader();
    for (int i = 0; i < 7; ++i)
        header->setResizeMode(i, modes[i]);

    QList<int> widths;
    widths << 50 << 150 << 70 << 70 << 70 << 70 << 60;
    int tableWidth = 0;
    for (int i = 0; i < 7; ++i)
    {
        questionTable->setColumnWidth(i, widths[i]);
        tableWidth += widths[i];
    }

    // Connect question table to question list
    connect(questionTable, SIGNAL(cellChanged(int,int)), this, SLOT(updateQuestionL(int,int)));

    questionTable->installEventFilter(this);
}

void QEditAssessment::createActions()
{
    saveAction = new QAction(tr("&Save"), this);
    saveAction->setIcon(QIcon(":/images/save.png"));
    saveAction->setStatusTip(tr("Save the assessment"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    addAction = new QAction(tr("&Add"), this);
    addAction->setIcon(QIcon(":/images/addmesh.png"));
    addAction->setStatusTip(tr("Add a new question"));
    connect(addAction, SIGNAL(triggered()), this, SLOT(add()));

    delAction = new QAction(tr("&Delete"), this);
    delAction->setIcon(QIcon(":/images/deletemesh.png"));
    delAction->setStatusTip(tr("Delete an question"));
    connect(delAction, SIGNAL(triggered()), this, SLOT(del()));
}

void QEditAssessment::createToolBars()
{
    toolBar = new QToolBar("Tools", this);
    toolBar->addAction(saveAction);
    toolBar->addAction(addAction);
    toolBar->addAction(delAction);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(toolBar);
    vbox->addWidget(questionTable);

    setLayout(vbox);
    //show();
}

void QEditAssessment::save()
{
    //qDebug() << "here 1";
    emit updateQuestionList(questionList);
    //qDebug() << "here 2";
}

void QEditAssessment::add()
{
    Question q;

    questionList.append(q);

    // Append to question table
    int j = questionTable->rowCount();
    questionTable->insertRow(j);
    //questionTable->setRowHeight(j, 90);

    QTableWidgetItem *item = new QTableWidgetItem(QString::number(j+1));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignHCenter);
    questionTable->setItem(j, 0, item);

    for (int i=1; i<7 ; i++)
    {
        item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        item->setTextAlignment(Qt::AlignHCenter);
        item->setBackground(QBrush(QColor(255, 255, 255)));
        questionTable->setItem(j, i, item);
    }
}

void QEditAssessment::del()
{
    int row = questionTable->currentRow();
    //qDebug() << "row: " << row;

    // Remind the user when no assessment is selected
    if (row < 0)
    {
        QMessageBox msgBox;
        msgBox.setText("Please select an question first!");
        msgBox.setWindowTitle("Warning");
        msgBox.exec();
    }

    questionTable->removeRow(row);

    // Refresh the indices
    refreshIndices();
}

void QEditAssessment::updateQuestionL(int row, int col)
{
    QString content = questionTable->item(row, col)->text();
    Question tempQ = questionList.value(row);
    qDebug() << "Row: "<<row<<" Col: "<<col<<" was editted with content: "<<content;
    switch(col)
    {
    case 1:
        tempQ.content = content;
        break;
    case 2:
        tempQ.choiceA = content;
        break;
    case 3:
        tempQ.choiceB = content;
        break;
    case 4:
        tempQ.choiceC = content;
        break;
    case 5:
        tempQ.choiceD = content;
        break;
    case 6:
        tempQ.answer = content;
        break;
    }

    questionList.replace(row, tempQ);
}

// Helper functions
void QEditAssessment::refreshIndices()
{
    int j = questionTable->rowCount();

    for (int i=0; i<j ; i++)
    {
        questionTable->item(i, 0)->setText(QString::number(i+1));
    }
}

void QEditAssessment::updateTable(QList<Question> qL)
{
    for (int i=0; i<qL.length() ; i++)
    {
        add(qL.value(i));
    }

    refreshIndices();
    update();
}

void QEditAssessment::add(Question q)
{
    questionList.append(q);

    // Append to question table
    int j = questionTable->rowCount();
    questionTable->insertRow(j);
    //questionTable->setRowHeight(j, 90);

    QTableWidgetItem *item = new QTableWidgetItem(QString::number(j+1));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignHCenter);
    questionTable->setItem(j, 0, item);

    for (int i=1; i<7 ; i++)
    {
        item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        item->setTextAlignment(Qt::AlignHCenter);
        item->setBackground(QBrush(QColor(255, 255, 255)));
        switch(i)
        {
        case 1:
            item->setText(q.content);
            break;
        case 2:
            item->setText(q.choiceA);
            break;
        case 3:
            item->setText(q.choiceB);
            break;
        case 4:
            item->setText(q.choiceC);
            break;
        case 5:
            item->setText(q.choiceD);
            break;
        case 6:
            item->setText(q.answer);
            break;
        }
        questionTable->setItem(j, i, item);
    }
}

