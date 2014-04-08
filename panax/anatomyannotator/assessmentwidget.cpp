#include "assessmentwidget.h"

AssessmentWidget::AssessmentWidget(QList<Question> ql, QWidget *parent)
    : QWidget(parent)
{
    qL = ql;

    setWindowTitle(tr("Assessment"));

    animTime=500;
    createGuiControlComponents();
    createSlidingStackedWidget();
    createActions();
    createToolBars();

    currIndex = 0;
}

void AssessmentWidget::createGuiControlComponents() {
    int _min=500;
    int _max=1500;
    animTime=(_min+_max)>>1;

    listAll = new QComboBox();
    //listAll->setMinimumHeight ( 40 );

    slideSpeed = new QSlider(Qt::Horizontal);
    slideSpeed->setMinimum(_min);
    slideSpeed->setMaximum(_max);

    //initialize slider and its display
    slideSpeed->setValue(animTime);
}

void AssessmentWidget::createActions()
{
    backAction = new QAction(tr("&Back"), this);
    backAction->setIcon(QIcon(":/images/home.png"));
    backAction->setStatusTip(tr("Back to the assessment list"));
    connect(backAction, SIGNAL(triggered()), this, SLOT(back()));

    addQuestionAction = new QAction(tr("&Add"), this);
    addQuestionAction->setIcon(QIcon(":/images/addmesh.png"));
    addQuestionAction->setStatusTip(tr("Add a new question after current one"));
    connect(addQuestionAction, SIGNAL(triggered()), this, SLOT(add()));

    delQuestionAction = new QAction(tr("&Delete"), this);
    delQuestionAction->setIcon(QIcon(":/images/deletemesh.png"));
    delQuestionAction->setStatusTip(tr("Delete current question"));
    connect(delQuestionAction, SIGNAL(triggered()), this, SLOT(del()));

    nextAction = new QAction(tr("&Delete"), this);
    nextAction->setIcon(QIcon(":/images/play.png"));
    nextAction->setStatusTip(tr("Go to next question"));
    connect(nextAction, SIGNAL(triggered()), slidingStacked, SLOT(slideInNext()));
    connect(nextAction, SIGNAL(triggered()), this, SLOT(slideInNext()));

    prevAction = new QAction(tr("&Delete"), this);
    prevAction->setIcon(QIcon(":/images/playback.png"));
    prevAction->setStatusTip(tr("Go to previous question"));
    connect(prevAction, SIGNAL(triggered()), slidingStacked, SLOT(slideInPrev()));
    connect(prevAction, SIGNAL(triggered()), this, SLOT(slideInPrev()));

    editQuestionAction = new QAction(tr("&Edit"), this);
    editQuestionAction->setIcon(QIcon(":/images/modify.png"));
    editQuestionAction->setStatusTip(tr("Edit current question"));
    connect(editQuestionAction, SIGNAL(triggered()), this, SLOT(editQuestion()));
}

void AssessmentWidget::createToolBars()
{
    toolBar = new QToolBar("Tools", this);
    toolBar->addAction(backAction);
    //toolBar->addAction(addQuestionAction);
    //toolBar->addAction(delQuestionAction);
    toolBar->addWidget(listAll);
    toolBar->addAction(prevAction);
    toolBar->addAction(nextAction);
    //toolBar->addAction(editQuestionAction);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(toolBar);
    vbox->addWidget(slidingStacked);

    setLayout(vbox);
    //show();
}

void AssessmentWidget::createSlidingStackedWidget() {
    slidingStacked = new SlidingStackedWidget(this);

    listAll = new QComboBox();
    listAll->setMinimumWidth(20);

    // Add question pages and feedback page here
    if (qL.length() == 0)
    {
        // Add a dummy question and a feedback page
        Question q;
        q.content = "Question content";
        q.choiceA = "Choice A";
        q.choiceB = "Choice B";
        q.choiceC = "Choice C";
        q.choiceD = "Choice D";
        q.answer = "Answer: B";

        qL.append(q);

        QuestionPage *qp = new QuestionPage(q, this);
        slidingStacked->addWidget(qp);

        listAll->addItem("Question 1");
    }
    else
    {
        qList.clear();

        for (int i = 0; i<qL.length() ;i++)
        {
            Question q = qL.at(i);
            //QuestionPage *qp = new QuestionPage(q, this);
            qList[i] = new QuestionPage(q, this);
            slidingStacked->addWidget(qList[i]);

            listAll->addItem("Question "+QString::number(i+1));
        }
    }

    FeedbackPage *fbp = new FeedbackPage(this);
    slidingStacked->addWidget(fbp);
    listAll->addItem("Feedback");

    slidingStacked->setSpeed(animTime);

}

void AssessmentWidget::setQuestionList(QList<Question> ql)
{
    //slidingStacked->clearList();
    listAll->clear();

    for (int i = 0; i<qL.length() ;i++)
    {
        slidingStacked->removeWidget(qList[i]);
    }

    qL = ql;

    for (int i = 0; i<qL.length() ;i++)
    {
        Question q = qL.at(i);
        QuestionPage *qp = new QuestionPage(q, this);
        slidingStacked->addWidget(qp);

        listAll->addItem("Question "+QString::number(i+1));
    }

    FeedbackPage *fbp = new FeedbackPage(this);
    slidingStacked->addWidget(fbp);

    update();
}

void AssessmentWidget::back()
{
    emit backtoAssessmentList();
}

void AssessmentWidget::add()
{
    Question q;
    q.content = "Dummy question content";
    q.choiceA = "Choice A";
    q.choiceB = "Choice B";
    q.choiceC = "Choice C";
    q.choiceD = "Choice D";
    q.answer = "Answer: B";

    qL.insert(currIndex+1, q);

    //createSlidingStackedWidget();
    qDebug() << "Adding question, list length: " << qL.length();
}

void AssessmentWidget::del()
{
    if (qL.length() < 2)
        return;

    qL.removeAt(currIndex);

    createSlidingStackedWidget();
}

void AssessmentWidget::editQuestion()
{

}

void AssessmentWidget::slideInNext()
{
    currIndex ++;
}

void AssessmentWidget::slideInPrev()
{
    currIndex --;
}

QuestionPage::QuestionPage(Question q, QWidget *parent)
    : QWidget(parent)
{
    curQ = q;
    questionTitle = new QLabel(q.content, this);
    choiceA = new QPushButton(QString("A. ")+q.choiceA, this);
    choiceB = new QPushButton(QString("B. ")+q.choiceB, this);
    choiceC = new QPushButton(QString("C. ")+q.choiceC, this);
    choiceD = new QPushButton(QString("D. ")+q.choiceD, this);
    answer = new QLabel(q.answer, this);
    answer->hide();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(questionTitle);
    if (!q.choiceA.isEmpty())   layout->addWidget(choiceA);
    if (!q.choiceB.isEmpty())   layout->addWidget(choiceB);
    if (!q.choiceC.isEmpty())   layout->addWidget(choiceC);
    if (!q.choiceD.isEmpty())   layout->addWidget(choiceD);
    layout->addWidget(answer);
    setLayout(layout);

    connect(choiceA, SIGNAL(clicked()), this, SLOT(showAnswer()));
    connect(choiceB, SIGNAL(clicked()), this, SLOT(showAnswer()));
    connect(choiceC, SIGNAL(clicked()), this, SLOT(showAnswer()));
    connect(choiceD, SIGNAL(clicked()), this, SLOT(showAnswer()));
}

void QuestionPage::showAnswer()
{
    answer->show();
}

FeedbackPage::FeedbackPage(QWidget *parent)
    : QWidget(parent)
{
    label = new QLabel("Write your comments here: ", this);
    feedbackBox = new QTextEdit;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    layout->addWidget(feedbackBox);
    setLayout(layout);
}
