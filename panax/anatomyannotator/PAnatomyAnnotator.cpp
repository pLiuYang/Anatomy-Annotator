/* PAnatomyAnnotator.cpp

   Annotator of 3D mesh models of human anatomy.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#include "PAnatomyAnnotator.h"
#include <QMainWindow>
#include <QtGui>
#include <QComboBox>
#include <vtkTextWidget.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkTextRepresentation.h>
#include <vtkCoordinate.h>

#include <vtkCaptionWidget.h>
#include <vtkCaptionRepresentation.h>
#include <vtkCaptionActor2D.h>


// Callback class

class AnatomyLabelsCallback: public vtkCommand
{
public:
    static AnatomyLabelsCallback *New() { return new AnatomyLabelsCallback; }
    void Execute(vtkObject *caller, unsigned long eventId, void *callData);
    PAnatomyAnnotator *viewer;

protected:
    AnatomyLabelsCallback();
    ~AnatomyLabelsCallback();

    // private:
    //     vtkPointData *pointData;
};


PAnatomyAnnotator::PAnatomyAnnotator()
{
    addWidgets();
    addActions();
    addMenus();
    addToolBars();

    appName = QString("Anatomy Annotator");
    setWindowTitle(appName);

    largeRightDock = false;
}


PAnatomyAnnotator::~PAnatomyAnnotator()
{
}


void PAnatomyAnnotator::addWidgets()
{
    // Example of how to replace widget in right dock area.
    tabWidget = new QTabWidget(this);
    lesson = new QMyLesson(this);
    assessment = new QMyAssessment(this);

    //tabWidget->addTab(meshTable, "Anatomy");
    tabWidget->addTab(lesson, "Lesson");
    tabWidget->addTab(assessment, "Assessment");

    //meshTable->hide();  // Hide meshTable in right dock area.
    rightDockWidget->setMinimumWidth(275);
    rightDockWidget->setWidget(meshTable);  // Change widget.
    rightDockWidget->setWindowTitle("Anatomy");

    // Create dock widget for right dock area.
    rightDockWidget2 = new QDockWidget("");
    rightDockWidget2->setWidget(tabWidget);
    rightDockWidget2->setAllowedAreas(Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, rightDockWidget2);
    rightDockWidget2->setMinimumWidth(width() * 0.5);
    rightDockWidget2->hide();

    // Connect mesh table to update anatomy info
    connect(meshTable->model(), SIGNAL(rowsAboutToBeInserted(const QModelIndex &,int,int)), this, SLOT(insertAnatomyInfo(const QModelIndex &,int,int)));
    connect(meshTable->model(), SIGNAL(rowsAboutToBeRemoved(const QModelIndex &,int,int)), this, SLOT(removeAnatomyInfo(const QModelIndex &,int,int)));

    // Connect mesh table to put on labels
    connect(meshTable, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(setLabelAnchor(QTableWidgetItem*)));

    // Add event handler to label clicks
    AnatomyLabelsCallback *callback = AnatomyLabelsCallback::New();
    callback->viewer = this;
    interactor->AddObserver(vtkCommand::RightButtonPressEvent, callback);
    callback->Delete();
}


void PAnatomyAnnotator::addActions()
{
    annotationAction = new QAction(tr("&Annotator"), this);
    annotationAction->setIcon(QIcon(":/images/pin.png"));
    //annotationAction->setShortcut(tr("Ctrl+N"));
    annotationAction->setStatusTip(tr("Click to show labels and annotations."));
    annotationAction->setCheckable(true);
    connect(annotationAction, SIGNAL(triggered()),
            this, SLOT(showAnnotator()));

    lessonAction = new QAction(tr("&Lesson"), this);
    lessonAction->setIcon(QIcon(":/images/annotator.png"));
    lessonAction->setStatusTip(tr("Click to show lesson part"));
    lessonAction->setCheckable(true);
    connect(lessonAction, SIGNAL(triggered()),
            this, SLOT(showLesson()));

    connect(toggleRightDockWidgetAction, SIGNAL(triggered()), this, SLOT(hideAnatomy()));

    connect(newProjectAction, SIGNAL(triggered()),
            this, SLOT(newProject()));
    connect(openProjectAction, SIGNAL(triggered()),
            this, SLOT(openProject()));
    connect(saveProjectAction, SIGNAL(triggered()),
            this, SLOT(saveProject()));
    connect(saveProjectAsAction, SIGNAL(triggered()),
            this, SLOT(saveProjectAs()));
}

void PAnatomyAnnotator::addMenus()
{
    viewMenu->addAction(lessonAction);
    viewMenu->addAction(annotationAction);
}

void PAnatomyAnnotator::addToolBars()
{
    viewToolBar->addAction(lessonAction);
    viewToolBar->addAction(annotationAction);
}

void PAnatomyAnnotator::showAnnotator()
{
    // Store the labels position in screen
    for (int i=0; i<labelsWidgets.length(); i++)
    {
        vtkCaptionWidget *capWidget = labelsWidgets.at(i);
        qDebug() << "here 1";
        vtkCaptionRepresentation *rep = (vtkCaptionRepresentation *)capWidget->GetBorderRepresentation();
        anchorPos2DList.replace(i*4, rep->GetPositionCoordinate()->GetValue()[0]);
        anchorPos2DList.replace(i*4+1, rep->GetPositionCoordinate()->GetValue()[1]);
        anchorPos2DList.replace(i*4+2, rep->GetPosition2Coordinate()->GetValue()[0]);
        anchorPos2DList.replace(i*4+3, rep->GetPosition2Coordinate()->GetValue()[1]);
    }

    if (!annotationAction->isChecked())
    {
        qDebug() << "annotation off";
        for (int i=0; i<labelsWidgets.length(); i++)
        {
            vtkCaptionWidget *capWidget = labelsWidgets.at(i);

            capWidget->Off();
        }
        renderWindow->Render();
        return;
    }

    // Display the anatomy info
    for (int i=0; i<labelsWidgets.length(); i++)
    {
        vtkCaptionRepresentation *capRep = vtkCaptionRepresentation::New();
        capRep->GetCaptionActor2D()->SetCaption(anatomyLabels.at(i).toAscii());
        capRep->GetPositionCoordinate()->SetValue(anchorPos2DList[i*4], anchorPos2DList[i*4+1]);
        capRep->GetPosition2Coordinate()->SetValue(anchorPos2DList[i*4+2], anchorPos2DList[i*4+3]);

        capRep->GetCaptionActor2D()->GetCaptionTextProperty()->BoldOff();
        capRep->GetCaptionActor2D()->GetCaptionTextProperty()->ItalicOff();
        capRep->GetCaptionActor2D()->GetCaptionTextProperty()->SetColor(1.0,1.0,0.0);

        double pos[3] = {anchorPosList[3*i], anchorPosList[3*i+1], anchorPosList[3*i+2]};
        capRep->SetAnchorPosition(pos);

        vtkCaptionWidget *capWidget = labelsWidgets.at(i);
        capWidget->SetInteractor(interactor);
        capWidget->SetRepresentation(capRep);

        capWidget->On();
    }

    renderWindow->Render();
}

void PAnatomyAnnotator::showLesson()
{
    if (!lessonAction->isChecked())
    {
        rightDockWidget2->hide();
        return;
    }

    if (toggleRightDockWidgetAction->isChecked())
    {
        toggleRightDockWidgetAction->setChecked(false);
        rightDockWidget->hide();

    }

    rightDockWidget2->show();
}

void PAnatomyAnnotator::hideAnatomy()
{
    if (lessonAction->isChecked())
    {
        rightDockWidget2->hide();
        lessonAction->setChecked(false);
        return;
    }
}

void PAnatomyAnnotator::insertAnatomyInfo(const QModelIndex &parent, int row, int col)
{
    qDebug() << "Insert Row: " << row;

    anatomyLabels.insert(row, "Label");
    anatomyAnnotations.insert(row, "Annotation!");
    labelsWidgets.insert(row, vtkCaptionWidget::New());
    anchorPosList.insert(row*3, 0.0);
    anchorPosList.insert(row*3+1, 0.0);
    anchorPosList.insert(row*3+2, 0.0);

    anchorPos2DList.insert(row*4, .05);
    anchorPos2DList.insert(row*4+1, .15+.15*row);
    anchorPos2DList.insert(row*4+2, .3);
    anchorPos2DList.insert(row*4+3, .05);

    vtkCaptionRepresentation *capRep = vtkCaptionRepresentation::New();
    capRep->GetCaptionActor2D()->SetCaption(anatomyLabels.at(row).toAscii());
    capRep->GetPositionCoordinate()->SetValue(anchorPos2DList[row*4],anchorPos2DList[row*4+1]);
    capRep->GetPosition2Coordinate()->SetValue(anchorPos2DList[row*4+2],anchorPos2DList[row*4+3]);

    double pos[3] = {anchorPosList[3*row], anchorPosList[3*row+1], anchorPosList[3*row+2]};
    capRep->SetAnchorPosition(pos);

    vtkCaptionWidget *capWidget = labelsWidgets.at(row);
    capWidget->SetInteractor(interactor);
    capWidget->SetRepresentation(capRep);
}

void PAnatomyAnnotator::removeAnatomyInfo(const QModelIndex &parent, int row, int col)
{
    qDebug() << "Remove Row: " << row;

    anatomyLabels.removeAt(row);
    anatomyAnnotations.removeAt(row);
    anchorPosList.removeAt(row*3);
    anchorPosList.removeAt(row*3+1);
    anchorPosList.removeAt(row*3+2);
    anchorPos2DList.removeAt(row*4);
    anchorPos2DList.removeAt(row*4+1);
    anchorPos2DList.removeAt(row*4+2);
    anchorPos2DList.removeAt(row*4+3);
}

void PAnatomyAnnotator::setLabelAnchor(QTableWidgetItem *item)
{
    if (!(item->column() == 3) || !annotationAction->isChecked())
        return;

    /*if (!annotationAction->isChecked())
    {
        annotationAction->setChecked(true);
        showAnnotator();
    }*/
    qDebug() << "text in cell: " << item->text();
    anatomyLabels.replace(item->row(), item->text());

    showAnnotator();
}

void PAnatomyAnnotator::updateLabelAnchor(double *pos)
{
    int row = meshTable->currentRow();

    anchorPosList.replace(row*3, pos[0]);
    anchorPosList.replace(row*3+1, pos[1]);
    anchorPosList.replace(row*3+2, pos[2]);

    showAnnotator();
}

void PAnatomyAnnotator::newProject()
{


    QMessageBox msgBox;
    msgBox.setText("Do you want to save before proceed?");
    //msgBox.setInformativeText("Do you want to save your changes?");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();

    switch (ret) {
    case QMessageBox::Save:
        // Save was clicked
        saveProject();
        break;
    case QMessageBox::Cancel:
        // Cancel was clicked
        return;
        break;
    default:
        // should never be reached
        break;
    }

    lesson->clear();
    labelsWidgets.clear();
    anatomyLabels.clear();
    anatomyAnnotations.clear();
    anchorPosList.clear();
    anchorPos2DList.clear();
    fileName.clear();
    annotationAction->setChecked(false);

    PMeshViewer::newProject();

    update();
}

void PAnatomyAnnotator::openProject()
{
    QXmlStreamReader Rxml;
    fileName = QFileDialog::getOpenFileName(this,
                                            tr("Open Lesson"), ".",
                                            tr("Xml files (*.xml)"));

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        std::cerr << "Error: Cannot read file " << qPrintable(fileName)
                  << ": " << qPrintable(file.errorString())
                  << std::endl;
    }
    Rxml.setDevice(&file);
    Rxml.readNext();
    qDebug() << "1";
    while(!Rxml.atEnd())
    {   qDebug() << "2 " << QString::number(Rxml.isStartElement());
        if(Rxml.isStartElement())
        {qDebug() << "3 "<<Rxml.name();
            if(Rxml.name() == "Mesh")
            {
                Rxml.readNext();
            }
            else if(Rxml.name() == "MeshListLength")
            {
                qDebug() << "l: "<<Rxml.readElementText();
                Rxml.readNext();
                QStringList src;
                while(!Rxml.isEndElement())
                {
                    QString s = Rxml.readElementText();
                    qDebug() << "content: " << s;

                    if (s.length() > 0)
                        src.append(s);
                    Rxml.readNext();
                }
                addMesh(src);
                qDebug() <<src;
            } else
                Rxml.readNext();
        } else
            Rxml.readNext();
    }

    file.close();
    if (Rxml.hasError())
    {
        std::cerr << "Error: Failed to parse file "
                  << qPrintable(fileName) << ": "
                  << qPrintable(Rxml.errorString()) << std::endl;
    }
    else if (file.error() != QFile::NoError)
    {
        std::cerr << "Error: Cannot read file " << qPrintable(fileName)
                  << ": " << qPrintable(file.errorString())
                  << std::endl;
    }

    statusBar()->showMessage(tr("Lesson Opened"));
}

void PAnatomyAnnotator::saveProject()
{
    if (fileName.isEmpty())
        fileName = QFileDialog::getSaveFileName(this, tr("Save Lesson Plan"), ".", tr("Xml files (*.xml)"));

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("Mesh");
    xmlWriter.writeTextElement("MeshListLength", QString::number(meshList.length()));

    for (int i = 0; i<meshList.length(); i++)
    {
        xmlWriter.writeTextElement("Mesh"+QString::number(i), meshList[i].source);
    }
    xmlWriter.writeEndElement();

    xmlWriter.writeEndDocument();

    file.close();
    statusBar()->showMessage(tr("Lesson saved"));
}

void PAnatomyAnnotator::saveProjectAs()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Save as Lesson Plan"), "", tr("Lesson plan (*.ann);;All Files (*)"));

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();

    xmlWriter.writeStartElement("LAMPS");

    xmlWriter.writeStartElement("LIGHT1");
    xmlWriter.writeTextElement("State", "On" );

    xmlWriter.writeEndDocument();

    file.close();
    statusBar()->showMessage(tr("Lesson saved"));
}


// Callback

AnatomyLabelsCallback::AnatomyLabelsCallback()
{
}


AnatomyLabelsCallback::~AnatomyLabelsCallback()
{
}


void AnatomyLabelsCallback::Execute(vtkObject *caller,
                                    unsigned long eventId, void *callData)
{
    vtkRenderWindowInteractor *interactor = viewer->getInteractor();
    int *pos = interactor->GetEventPosition();
    vtkRenderer *renderer = viewer->getRenderer();

    // Pick at the mouse location provided by the interactor
    vtkPropPicker *picker = vtkPropPicker::New();
    int found = picker->PickProp(pos[0], pos[1], renderer);

    if (found)
    {
        //qDebug() << "...";
        //qDebug() << picker->GetActor()->GetClassName();

        double* pos = picker->GetPickPosition();
        //qDebug() << sizeof(pos);
        qDebug() << "Get actor " <<pos[0]<<" "<<pos[1]<<" "<<pos[2];

        viewer->updateLabelAnchor(pos);
    }
}


