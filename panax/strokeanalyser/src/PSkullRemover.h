/* PSkullRemover.h

   Interactive thresholding tool suite.

   Copyright 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#ifndef PSKULLREMOVER_H
#define PSKULLREMOVER_H

#include "PThresholdDialog.h"
#include <QObject>
#include <QSpinBox>
#include <QSlider>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>

#include "vtkImageThreshold.h"
#include "vtkImageMathematics.h"
#include "vtkImageDilateErode3D.h"
#include "vtkImageContinuousDilate3D.h"
#include "vtkImageStencil.h"
#include "vtkImageToImageStencil.h"
#include "vtkImageCacheFilter.h"
#include "vtkAlgorithmOutput.h"
#include "vtkImageData.h"

#include "itkVTKImageToImageFilter.h"
#include "itkImageToVTKImageFilter.h"

typedef itk::Image<short, 3> ItkImageType;
typedef itk::VTKImageToImageFilter<ItkImageType> Vtk2ItkType;
typedef itk::ImageToVTKImageFilter<ItkImageType> Itk2VtkType;


class PSkullRemover: public QWidget
{
    Q_OBJECT
    
public:
    PSkullRemover();
    ~PSkullRemover();

    void setInputConnection(vtkAlgorithmOutput *input);
    vtkAlgorithmOutput *getOutputPort();
    vtkImageData *getOutput();
    vtkImageAlgorithm *getOutputFilter();
    
protected:
    void closeEvent(QCloseEvent *event);
        
signals:
    void updated();
    void closed();
    
public slots:
    void changeOutput(QAbstractButton *box);
    void setMinThreshold(const QString &text);
    void setMaxThreshold(const QString &text);
    void setThreshold(int value);
    void setFillValue(const QString &text);
    void setDilateSize(int size);
    void apply();

protected:   
    // Thresholding
    vtkImageThreshold *thresholder;
    QLabel *typeLabel;
    double minThreshold, maxThreshold;
    double threshold, fillValue;
    QLineEdit *minThresholdBox, *maxThresholdBox;
    QLineEdit *fillValueBox;
    QSpinBox *thresholdBox;
    QSlider *thresholdSlider;
    QCheckBox *showThresholdBox;
    
    // SubtrpolyDatatoaction
    vtkImageMathematics *subtractor;
    QCheckBox *showSubtractBox;
    
    // Dilation
    vtkImageContinuousDilate3D *dilater;
    QSpinBox *dilateSizeBox;
    QCheckBox *showDilateBox;
    
    // Masking
    vtkImageStencil *stencil;
    vtkImageToImageStencil *makeStencil;
    QCheckBox *showMaskBox;
    
    // Flood fill
    Vtk2ItkType::Pointer vtk2itk;
    Itk2VtkType::Pointer itk2vtk;
    QCheckBox *showFillBox;
    
    // Buttons
    QPushButton *closeButton;
    
    // Output connector
    vtkImageCacheFilter *output;
    
    // Internal variables;
    bool hasInput;
    
    // Supporting functions
    void createWidgets();
};

#endif

