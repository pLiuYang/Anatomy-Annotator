#include "qeditlesson.h"

QEditLesson::QEditLesson(QWidget *parent) : QDialog(parent)
{
    createWidgets();
    createActions();
    createToolBars();
    resize(295, 400);
    setWindowTitle(tr("Edit lesson slides"));
}

void QEditLesson::setLessonSlides(QList<Slide> lsnS)
{
    updateTable(lsnS);
}

// Protected methods

void QEditLesson::closeEvent(QCloseEvent *event)
{
    close();
}

// Create widgets, actions and toolbars

void QEditLesson::createWidgets()
{
    // Create question table
    lessonTable = new QTableWidget(0, 3);
    QPalette palette = lessonTable->palette();
    palette.setColor(QPalette::Base, QColor(248, 248, 248));
    lessonTable->setPalette(palette);
    lessonTable->verticalHeader()->hide();
    lessonTable->setShowGrid(false);
    lessonTable->setSelectionMode(QTableWidget::SingleSelection);

    QStringList labels;
    labels << "Slide" << "Store camera angle" << "Content" ;
    lessonTable->setHorizontalHeaderLabels(labels);

    QList<QHeaderView::ResizeMode> modes;
    modes << QHeaderView::Fixed << QHeaderView::Interactive
          << QHeaderView::Interactive;
    QHeaderView *header = lessonTable->horizontalHeader();
    for (int i = 0; i < 3; ++i)
        header->setResizeMode(i, modes[i]);

    QList<int> widths;
    widths << 50 << 150 << 70;
    int tableWidth = 0;
    for (int i = 0; i < 3; ++i)
    {
        lessonTable->setColumnWidth(i, widths[i]);
        tableWidth += widths[i];
    }

    // Connect question table to question list
    //connect(lessonTable, SIGNAL(cellChanged(int,int)), this, SLOT(updateSlides(int,int)));
    connect(lessonTable, SIGNAL(cellClicked(int,int)), this, SLOT(enterSlideContent(int,int)));
    connect(lessonTable, SIGNAL(cellClicked(int,int)), this, SLOT(storeCameraPosition(int,int)));

    lessonTable->installEventFilter(this);
}

void QEditLesson::createActions()
{
    saveAction = new QAction(tr("&Save"), this);
    saveAction->setIcon(QIcon(":/images/save.png"));
    saveAction->setStatusTip(tr("Save the lesson"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    addAction = new QAction(tr("&Add"), this);
    addAction->setIcon(QIcon(":/images/addmesh.png"));
    addAction->setStatusTip(tr("Add a new slide"));
    connect(addAction, SIGNAL(triggered()), this, SLOT(add()));

    delAction = new QAction(tr("&Delete"), this);
    delAction->setIcon(QIcon(":/images/deletemesh.png"));
    delAction->setStatusTip(tr("Delete a slide"));
    connect(delAction, SIGNAL(triggered()), this, SLOT(del()));
}

void QEditLesson::createToolBars()
{
    toolBar = new QToolBar("Tools", this);
    toolBar->addAction(saveAction);
    toolBar->addAction(addAction);
    toolBar->addAction(delAction);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(toolBar);
    vbox->addWidget(lessonTable);

    setLayout(vbox);
    //show();
}

void QEditLesson::save()
{
    //qDebug() << "here 1";
    emit updateLessonSlides(lessonSlides);
    //qDebug() << "here 2";
}

void QEditLesson::add()
{
    Slide s;

    lessonSlides.append(s);

    // Append to question table
    int j = lessonTable->rowCount();
    lessonTable->insertRow(j);
    //lessonTable->setRowHeight(j, 90);

    QTableWidgetItem *item = new QTableWidgetItem(QString::number(j+1));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignHCenter);
    lessonTable->setItem(j, 0, item);

    for (int i=1; i<3 ; i++)
    {
        item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        item->setTextAlignment(Qt::AlignHCenter);
        item->setBackground(QBrush(QColor(255, 255, 255)));
        lessonTable->setItem(j, i, item);
    }
}

void QEditLesson::del()
{
    int row = lessonTable->currentRow();
    //qDebug() << "row: " << row;

    // Remind the user when no slide is selected
    if (row < 0)
    {
        QMessageBox msgBox;
        msgBox.setText("Please select a slide first!");
        msgBox.setWindowTitle("Warning");
        msgBox.exec();
    }

    lessonTable->removeRow(row);

    // Refresh the indices
    refreshIndices();
}

void QEditLesson::updateSlides(int row, int col)
{
    QString content = lessonTable->item(row, col)->text();
    Slide tempQ = lessonSlides.value(row);
    qDebug() << "Row: "<<row<<" Col: "<<col<<" was editted with content: "<<content;
    switch(col)
    {
    case 2:
        tempQ.content = content;
        break;
    }

    lessonSlides.replace(row, tempQ);
}

void QEditLesson::enterSlideContent(int row, int col)
{
    if (col == 2)
    {
        bool ok;
        QString text = QInputDialog::getText(this, tr("Slide content"), tr("Enter here:"), QLineEdit::Normal, "", &ok);
        if (ok && !text.isEmpty())
            lessonTable->item(row, col)->setText(text);

        updateSlides(row, col);
    }
}

void QEditLesson::storeCameraPosition(int row, int col)
{
    if (col == 1)
    {
        QMessageBox msgBox;
        msgBox.setText("Do you want to store the current camera position and view angle?");
        msgBox.setWindowTitle("Save camera position");
        msgBox.addButton(QMessageBox::Save);
        msgBox.addButton(QMessageBox::Cancel);
        msgBox.exec();
    }
}

// Helper functions
void QEditLesson::refreshIndices()
{
    int j = lessonTable->rowCount();

    for (int i=0; i<j ; i++)
    {
        lessonTable->item(i, 0)->setText(QString::number(i+1));
    }
}

void QEditLesson::updateTable(QList<Slide> lsnS)
{
    for (int i=0; i<lsnS.length() ; i++)
    {
        add(lsnS.value(i));
    }

    refreshIndices();
    update();
}

void QEditLesson::add(Slide s)
{
    lessonSlides.append(s);

    // Append to question table
    int j = lessonTable->rowCount();
    lessonTable->insertRow(j);
    //lessonTable->setRowHeight(j, 90);

    QTableWidgetItem *item = new QTableWidgetItem(QString::number(j+1));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(Qt::AlignHCenter);
    lessonTable->setItem(j, 0, item);

    for (int i=1; i<3 ; i++)
    {
        item = new QTableWidgetItem;
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsEditable);
        item->setTextAlignment(Qt::AlignHCenter);
        item->setBackground(QBrush(QColor(255, 255, 255)));
        switch(i)
        {
        case 1:

            break;
        case 2:
            item->setText(s.content);
            break;
        }
        lessonTable->setItem(j, i, item);
    }
}
