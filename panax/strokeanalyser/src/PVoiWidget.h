/* PVoiWidget.h

   Volume of interest widget.

   Copyright 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#ifndef PVOIWIDGET_H
#define PVOIWIDGET_H

#include <QObject>
#include "vtkLineWidget.h"
#include "vtkImageActor.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"


class PVoiWidget: public QObject
{
    Q_OBJECT
    
    friend class PVoiWidgetCallback;

public:
    PVoiWidget();
    ~PVoiWidget();
    enum PlaneType {XY, YZ, XZ};
    void alignXYPlane();
    void alignYZPlane();
    void alignXZPlane();
    void initPosition(vtkImageActor *actor, vtkCamera *camera);
    void setInteractor(vtkRenderWindowInteractor *interactor);
    int *getBound();
    
signals:
    void topLineChanged(int pos);
    void bottomLineChanged(int pos);
    void leftLineChanged(int pos);
    void rightLineChanged(int pos);
    
public slots:
    void show(bool option);
    void show();
    void hide();
    void setTopLine(int pos);
    void setBottomLine(int pos);
    void setLeftLine(int pos);
    void setRightLine(int pos);
    void update();
    
protected:
    PlaneType plane; 
    vtkLineWidget *topLine;
    vtkLineWidget *botLine;
    vtkLineWidget *leftLine;
    vtkLineWidget *rightLine;
    vtkRenderWindowInteractor *interactor;
};

#endif