#include "lessonwidget.h"

LessonWidget::LessonWidget(QList<Slide> sl, QWidget *parent)
    : QWidget(parent)
{
    sL = sl;

    setWindowTitle(tr("Assessment"));

    animTime=200;
    createGuiControlComponents();
    createSlidingStackedWidget();
    createActions();
    createToolBars();

    currIndex = 0;
}

void LessonWidget::createGuiControlComponents() {
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

void LessonWidget::createActions()
{
    backAction = new QAction(tr("&Back"), this);
    backAction->setIcon(QIcon(":/images/home.png"));
    backAction->setStatusTip(tr("Back to the lecture list"));
    connect(backAction, SIGNAL(triggered()), this, SLOT(back()));

    addAction = new QAction(tr("&Add"), this);
    addAction->setIcon(QIcon(":/images/addmesh.png"));
    addAction->setStatusTip(tr("Add a new slide"));
    connect(addAction, SIGNAL(triggered()), this, SLOT(add()));

    delAction = new QAction(tr("&Delete"), this);
    delAction->setIcon(QIcon(":/images/deletemesh.png"));
    delAction->setStatusTip(tr("Delete current slide"));
    connect(delAction, SIGNAL(triggered()), this, SLOT(del()));

    nextAction = new QAction(tr("&Delete"), this);
    nextAction->setIcon(QIcon(":/images/play.png"));
    nextAction->setStatusTip(tr("Go to next slide"));
    connect(nextAction, SIGNAL(triggered()), slidingStacked, SLOT(slideInNext()));
    connect(nextAction, SIGNAL(triggered()), this, SLOT(slideInNext()));

    prevAction = new QAction(tr("&Delete"), this);
    prevAction->setIcon(QIcon(":/images/playback.png"));
    prevAction->setStatusTip(tr("Go to previous slide"));
    connect(prevAction, SIGNAL(triggered()), slidingStacked, SLOT(slideInPrev()));
    connect(prevAction, SIGNAL(triggered()), this, SLOT(slideInPrev()));

    editAction = new QAction(tr("&Edit"), this);
    editAction->setIcon(QIcon(":/images/modify.png"));
    editAction->setStatusTip(tr("Edit the slide"));
    connect(editAction, SIGNAL(triggered()), this, SLOT(edit()));
}

void LessonWidget::createToolBars()
{
    toolBar = new QToolBar("Tools", this);
    toolBar->addAction(backAction);
    toolBar->addSeparator();
    toolBar->addAction(addAction);
    toolBar->addAction(delAction);
    //toolBar->addAction(editAction);
    toolBar->addSeparator();
    toolBar->addWidget(listAll);
    toolBar->addAction(prevAction);
    toolBar->addAction(nextAction);

    QVBoxLayout *vbox = new QVBoxLayout(this);
    vbox->addWidget(toolBar);
    vbox->addWidget(slidingStacked);

    setLayout(vbox);
    //show();
}

void LessonWidget::createSlidingStackedWidget() {
    slidingStacked = new SlidingStackedWidget(this);

    listAll = new QComboBox();
    listAll->setMinimumWidth(20);
    connect(listAll, SIGNAL(activated(int)), this, SLOT(slideInIdx(int)));

    // Add slides pages here
    {
        for (int i = 0; i<sL.length() ;i++)
        {
            Slide q = sL.at(i);
            slides.append( new LessonSlide(q, this) );
            slidingStacked->addWidget(slides[i]);

            listAll->addItem("Slide "+QString::number(i+1));
        }
    }

    slidingStacked->setSpeed(animTime);

}

void LessonWidget::setSlidesList(QList<Slide> sl)
{
    listAll->clear();

    for (int i = 0; i<sl.length() ;i++)
    {
        slidingStacked->removeWidget(slides[i]);
    }
    slides.clear();

    sL = sl;

    for (int i = 0; i<sL.length() ;i++)
    {
        Slide q = sL.at(i);
        //q.content = QString::number(i+1)+">??";
        slides.append(new LessonSlide(q, this));
        slidingStacked->addWidget(slides[i]);

        listAll->addItem("Slide "+QString::number(i+1));
    }

    update();
}

void LessonWidget::back()
{
    emit backtoLectureList();
}

void LessonWidget::add()
{
    if (sL.length() == 0)
    {
        Slide q = Slide();
        q.content = "New slide.";

        slides.append(new LessonSlide(q, this));
        slidingStacked->addWidget(slides[0]);
        sL.append(q);
        listAll->addItem("Slide 1");

        emit updateSlides(sL);

        return;
    }

    Slide q = sL.at(currIndex);
    q.content = "New slide";
    slides.insert(currIndex+1, new LessonSlide(q, this));
    sL.insert(currIndex+1, q);
    slidingStacked->insertWidget(currIndex+1, slides[currIndex+1]);

    listAll->clear();
    for (int i = 0; i<sL.length() ;i++)
    {
        listAll->addItem("Slide "+QString::number(i+1));
    }

    emit updateSlides(sL);

    slidingStacked->slideInNext();
    slideInNext();
}

void LessonWidget::del()
{
    qDebug() << "Del: " << currIndex;

    int temp = currIndex;

    sL.removeAt(temp);

    listAll->clear();
    for (int i = 0; i<sL.length() ;i++)
    {
        listAll->addItem("Slide "+QString::number(i+1));
    }
    slideInPrev();
    //slidingStacked->slideInPrev();

    slidingStacked->removeWidget(slides[temp]);
    slides.removeAt(temp);

    emit updateSlides(sL);
}

void LessonWidget::slideInNext()
{
    if (currIndex == sL.length()-1) return;

    currIndex ++;
    listAll->setCurrentIndex(currIndex);
    qDebug() << "click next: " << currIndex;
}

void LessonWidget::slideInPrev()
{
    if (currIndex == 0) return;

    currIndex --;
    listAll->setCurrentIndex(currIndex);
    qDebug() << "click prev: " << currIndex;
}

void LessonWidget::slideInIdx(int index)
{
    qDebug() << "slide to: " << index;
    slidingStacked->slideInIdx(index);
    currIndex = index;
}

void LessonWidget::edit()
{
    if (sL.length() < 1)    return;

    Slide s = sL.at(currIndex);qDebug() << "1";
    EditLesson *editLesson = new EditLesson(this);qDebug() << "2";
    editLesson->setText(s.content);qDebug() << "3";
}

void LessonWidget::updateSlideContent(QString ss)
{
    Slide s = sL.at(currIndex);
    s.content = ss;
    sL.replace(currIndex, s);

    emit updateSlides(sL);
}

LessonSlide::LessonSlide(Slide s, QWidget *parent)
    : QWidget(parent)
{
    curS = s;
    slideContent = new QTextEdit(this);
    slideContent->setText(curS.content);
    //slideContent->setReadOnly(true);
    useStoredCameraPosition = new QPushButton("Use stored camera view angle", this);
    useStoredCameraPosition->hide();

    QVBoxLayout *layout = new QVBoxLayout;
    //layout->addWidget(useStoredCameraPosition);
    layout->addWidget(slideContent);
    setLayout(layout);

    connect(slideContent, SIGNAL(textChanged()), this, SLOT(updateSlideContent()));
    connect(this, SIGNAL(updateSlideContent(QString)), parentWidget(), SLOT(updateSlideContent(QString)));
    //connect(useStoredCameraPosition, SIGNAL(clicked()), this, SLOT(updateCameraAngle()));
}

void LessonSlide::updateCameraAngle()
{
    emit updateCameraAngleSignal();
}

void LessonSlide::updateSlideContent()
{
    emit updateSlideContent(slideContent->toHtml());
}
