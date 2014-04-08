/* PVoiWidget.cpp

   Volume of interest widget.

   Copyright 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#include "PVoiWidget.h"
#include "vtkProperty.h"
#include "vtkCommand.h"

#include <unistd.h>
using namespace std;


// Callback class

class PVoiWidgetCallback: public vtkCommand
{
public:
    static PVoiWidgetCallback *New() { return new PVoiWidgetCallback; }
    void Execute(vtkObject *caller, unsigned long eventId, void *callData);
    PVoiWidget *host;

protected:
    PVoiWidgetCallback();
    ~PVoiWidgetCallback();
};


#define LowBound -2000
#define HighBound 2000


PVoiWidget::PVoiWidget()
{
    // Create line widgets
    topLine = vtkLineWidget::New();
    topLine->ClampToBoundsOn();
    vtkProperty *prop = topLine->GetLineProperty();
    prop->SetLineWidth(1.0);
    prop->SetColor(1.0, 1.0, 0.0);
    topLine->GetHandleProperty()->SetOpacity(0.0);
    topLine->PlaceWidget(LowBound, HighBound, LowBound, HighBound,
        LowBound, HighBound);
        
    botLine = vtkLineWidget::New();
    botLine->ClampToBoundsOn();
    prop = botLine->GetLineProperty();
    prop->SetLineWidth(1.0);
    prop->SetColor(0.0, 1.0, 1.0);
    botLine->GetHandleProperty()->SetOpacity(0.0);
    botLine->PlaceWidget(LowBound, HighBound, LowBound, HighBound,
        LowBound, HighBound);

    leftLine = vtkLineWidget::New();
    leftLine->ClampToBoundsOn();
    prop = leftLine->GetLineProperty();
    prop->SetLineWidth(1.0);
    prop->SetColor(1.0, 1.0, 0.0);
    leftLine->GetHandleProperty()->SetOpacity(0.0);
    leftLine->PlaceWidget(LowBound, HighBound, LowBound, HighBound,
        LowBound, HighBound);
        
    rightLine = vtkLineWidget::New();
    rightLine->ClampToBoundsOn();
    prop = rightLine->GetLineProperty();
    prop->SetLineWidth(1.0);
    prop->SetColor(0.0, 1.0, 1.0);
    rightLine->GetHandleProperty()->SetOpacity(0.0);
    rightLine->PlaceWidget(LowBound, HighBound, LowBound, HighBound,
        LowBound, HighBound);
    
    // Create callbacks
    PVoiWidgetCallback *callback = PVoiWidgetCallback::New();
    callback->host = this;
    topLine->AddObserver(vtkCommand::InteractionEvent, callback);
    callback->Delete();
    
    callback = PVoiWidgetCallback::New();
    callback->host = this;
    botLine->AddObserver(vtkCommand::InteractionEvent, callback);
    callback->Delete();
    
    callback = PVoiWidgetCallback::New();
    callback->host = this;
    leftLine->AddObserver(vtkCommand::InteractionEvent, callback);
    callback->Delete();
    
    callback = PVoiWidgetCallback::New();
    callback->host = this;
    rightLine->AddObserver(vtkCommand::InteractionEvent, callback);
    callback->Delete();
}


PVoiWidget::~PVoiWidget()
{
    topLine->Delete();
    botLine->Delete();
    leftLine->Delete();
    rightLine->Delete();
}


void PVoiWidget::alignXYPlane()
{
    plane = XY;
    topLine->SetAlignToXAxis();
    botLine->SetAlignToXAxis();
    leftLine->SetAlignToYAxis();
    rightLine->SetAlignToYAxis();
}


void PVoiWidget::alignYZPlane()
{
    plane = YZ;
    topLine->SetAlignToZAxis();
    botLine->SetAlignToZAxis();
    leftLine->SetAlignToYAxis();
    rightLine->SetAlignToYAxis();
}


void PVoiWidget::alignXZPlane()
{
    plane = XZ;
    topLine->SetAlignToZAxis();
    botLine->SetAlignToZAxis();
    leftLine->SetAlignToXAxis();
    rightLine->SetAlignToXAxis();
}


void PVoiWidget::initPosition(vtkImageActor *actor, vtkCamera *camera)
{
    double *bound = actor->GetBounds();
    double *focal = camera->GetFocalPoint();
    double *pos = camera->GetPosition();
    
//     cout << bound[0] << " " << bound[1] << " " << bound[2] << " " <<
//             bound[3] << " " << bound[4] << " " << bound[5] << "\n" <<
//             focal[0] << " " << focal[1] << " " << focal[2] << "\n" <<
//             pos[0] << " " << pos[1] << " " << pos[2] << "\n\n" << flush;
    
    int top, high, low, left, right;
    int margin = 10;
    
    switch(plane)
    {
        case XY:
            top = (focal[2] + pos[2]) / 2;
            high = bound[3] - margin;
            low = bound[2] + margin;
            left = bound[0] + margin;
            right = bound[1] - margin;
            
            topLine->SetPoint1(LowBound, high, top);
            topLine->SetPoint2(HighBound, high, top);
            botLine->SetPoint1(LowBound, low, top);
            botLine->SetPoint2(HighBound, low, top);
            leftLine->SetPoint1(left, LowBound, top);
            leftLine->SetPoint2(left, HighBound, top);
            rightLine->SetPoint1(right, LowBound, top);
            rightLine->SetPoint2(right, HighBound, top);
            break;
        
        case YZ:
            top = (focal[0] + pos[0]) / 2;
            low = bound[5] - margin;
            high = bound[4] + margin;
            right = bound[2] + margin;
            left = bound[3] - margin;
            
            topLine->SetPoint1(top, LowBound, high);
            topLine->SetPoint2(top, HighBound, high);
            botLine->SetPoint1(top, LowBound, low);
            botLine->SetPoint2(top, HighBound, low);
            leftLine->SetPoint1(top, left, LowBound);
            leftLine->SetPoint2(top, left, HighBound);
            rightLine->SetPoint1(top, right, LowBound);
            rightLine->SetPoint2(top, right, HighBound);
            break;
            
        case XZ:
            top = (focal[1] + pos[1]) / 2;
            low = bound[5] - margin;
            high = bound[4] + margin;
            left = bound[0] + margin;
            right = bound[1] - margin;
            
            topLine->SetPoint1(LowBound, top, high);
            topLine->SetPoint2(HighBound, top, high);
            botLine->SetPoint1(LowBound, top, low);
            botLine->SetPoint2(HighBound, top, low);
            leftLine->SetPoint1(left, top, LowBound);
            leftLine->SetPoint2(left, top, HighBound);
            rightLine->SetPoint1(right, top, LowBound);
            rightLine->SetPoint2(right, top, HighBound);
            break;
            
        default:
            break;
    }
}


void PVoiWidget::setInteractor(vtkRenderWindowInteractor *inter)
{
    interactor = inter;
    topLine->SetInteractor(inter);
    botLine->SetInteractor(inter);
    leftLine->SetInteractor(inter);
    rightLine->SetInteractor(inter);
}


// getBound: Get coordinates of bounding box
// bound[0] = min 1st coord, bound[1] = max 1st coord,
// bound[2] = min 2nd coord, bound[3] = max 2nd coord.
// XY plane: 1st coord = x, 2nd coord = y
// YZ plane: 1st coord = y, 2nd coord = z
// XZ plane: 1st coord = x, 2nd coord = z

int *PVoiWidget::getBound()
{
    static int bound[4];
    
    if (plane == XY)
    {
        bound[0] = leftLine->GetPoint1()[0];
        bound[1] = rightLine->GetPoint1()[0];
        bound[2] = topLine->GetPoint1()[1];
        bound[3] = botLine->GetPoint1()[1];
    }
    else if (plane == YZ)
    {
        bound[0] = rightLine->GetPoint1()[1];
        bound[1] = leftLine->GetPoint1()[1];
        bound[2] = topLine->GetPoint1()[2];
        bound[3] = botLine->GetPoint1()[2];
    }
    else if (plane == XZ)
    {
        bound[0] = leftLine->GetPoint1()[0];
        bound[1] = rightLine->GetPoint1()[0];
        bound[2] = topLine->GetPoint1()[2];
        bound[3] = botLine->GetPoint1()[2];
    }
    
    return bound;
}


void PVoiWidget::setTopLine(int pos)
{
    double *p;
    int i = 0;
    
    if (plane == XY)
        i = 1;
    else if (plane == YZ)
        i = 2;
    else if (plane == XZ)
        i = 2;
    
    p = topLine->GetPoint1();
    p[i] = pos;
    topLine->SetPoint1(p);
    p = topLine->GetPoint2();
    p[i] = pos;
    topLine->SetPoint2(p);
    update();
}


void PVoiWidget::setBottomLine(int pos)
{
    double *p;
    int i = 0;
    
    if (plane == XY)
        i = 1;
    else if (plane == YZ)
        i = 2;
    else if (plane == XZ)
        i = 2;
    
    p = botLine->GetPoint1();
    p[i] = pos;
    botLine->SetPoint1(p);
    p = botLine->GetPoint2();
    p[i] = pos;
    botLine->SetPoint2(p);
    update();
}


void PVoiWidget::setLeftLine(int pos)
{
    double *p;
    int i = 0;
    
    if (plane == XY)
        i = 0;
    else if (plane == YZ)
        i = 1;
    else if (plane == XZ)
        i = 0;
    
    p = leftLine->GetPoint1();
    p[i] = pos;
    leftLine->SetPoint1(p);
    p = leftLine->GetPoint2();
    p[i] = pos;
    leftLine->SetPoint2(p);
    update();
}


void PVoiWidget::setRightLine(int pos)
{
    double *p;
    int i = 0;
    
    if (plane == XY)
        i = 0;
    else if (plane == YZ)
        i = 1;
    else if (plane == XZ)
        i = 0;
    
    p = rightLine->GetPoint1();
    p[i] = pos;
    rightLine->SetPoint1(p);
    p = rightLine->GetPoint2();
    p[i] = pos;
    rightLine->SetPoint2(p);
    update();
}


void PVoiWidget::update()
{
    interactor->Render();
}


// Slot functions

void PVoiWidget::show(bool option)
{
    int flag;
    
    if (option)
        flag = 1;
    else
        flag = 0;
        
    topLine->SetEnabled(flag);
    botLine->SetEnabled(flag);
    leftLine->SetEnabled(flag);
    rightLine->SetEnabled(flag);
}


void PVoiWidget::show()
{
    show(true);
}


void PVoiWidget::hide()
{
    show(false);
}


//--- PVoiWidgetCallback functions

PVoiWidgetCallback::PVoiWidgetCallback()
{
}


PVoiWidgetCallback::~PVoiWidgetCallback()
{
}


void PVoiWidgetCallback::Execute(vtkObject *caller,
    unsigned long eventId, void *callData)
{   
    switch(host->plane)
    {
        case PVoiWidget::XY:
            if (caller == host->topLine)
                emit host->topLineChanged(host->topLine->GetPoint1()[1]);
            else if (caller == host->botLine)
                emit host->bottomLineChanged(host->botLine->GetPoint1()[1]);
            else if (caller == host->leftLine)
                emit host->leftLineChanged(host->leftLine->GetPoint1()[0]);
            else if (caller == host->rightLine)
                emit host->rightLineChanged(host->rightLine->GetPoint1()[0]);
            break;
            
        case PVoiWidget::YZ:
            if (caller == host->topLine)
                emit host->topLineChanged(host->topLine->GetPoint1()[2]);
            else if (caller == host->botLine)
                emit host->bottomLineChanged(host->botLine->GetPoint1()[2]);
            else if (caller == host->leftLine)
                emit host->leftLineChanged(host->leftLine->GetPoint1()[1]);
            else if (caller == host->rightLine)
                emit host->rightLineChanged(host->rightLine->GetPoint1()[1]);
            break;
            
        case PVoiWidget::XZ:
            if (caller == host->topLine)
                emit host->topLineChanged(host->topLine->GetPoint1()[2]);
            else if (caller == host->botLine)
                emit host->bottomLineChanged(host->botLine->GetPoint1()[2]);
            else if (caller == host->leftLine)
                emit host->leftLineChanged(host->leftLine->GetPoint1()[0]);
            else if (caller == host->rightLine)
                emit host->rightLineChanged(host->rightLine->GetPoint1()[0]);
            break;
            
        default:
            break;
    }
}
