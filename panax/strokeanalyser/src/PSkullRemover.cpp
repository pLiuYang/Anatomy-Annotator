/* PSkullRemover.cpp

   Interactive thresholding tool suite.

   Copyright 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#include "PSkullRemover.h"
#include <QtGui>

#include "itkImage.h"


#include <unistd.h>
using namespace std;

// #define DEBUG


PSkullRemover::PSkullRemover()
{
    hasInput = false;
    
    createWidgets();
}


PSkullRemover::~PSkullRemover()
{
    thresholder->Delete();
    subtractor->Delete();
    dilater->Delete();
    stencil->Delete();
    makeStencil->Delete();
    // No need to explicitly delete vtk2itk and itk2vtk
    output->Delete();
}


// Qt event handlers

void PSkullRemover::closeEvent(QCloseEvent *event)
{
    event->accept();
    emit closed();
}


// Create widgets and objects

void PSkullRemover::createWidgets()
{   
    // Overall
    QVBoxLayout *mainLayout = new QVBoxLayout;
    setWindowTitle("Skull Remover");
    setLayout(mainLayout);

#ifdef DEBUG
    setFixedSize(370, 550);
#else
    setFixedSize(370, 420);
#endif
    
    // Thresholding
    minThreshold = 0;
    maxThreshold = 500;
    threshold = 500;
    fillValue = 0;
    thresholder = vtkImageThreshold::New();
    thresholder->ReplaceInOn();
    thresholder->SetInValue(fillValue);
    thresholder->ThresholdByUpper(threshold);

    minThresholdBox = new QLineEdit;
    minThresholdBox->setMaxLength(5);
    minThresholdBox->setFixedWidth(50);
    minThresholdBox->setText(QString("%1").arg(minThreshold));
    connect(minThresholdBox, SIGNAL(textEdited(const QString &)),
        this, SLOT(setMinThreshold(const QString &)));
        
    maxThresholdBox = new QLineEdit;
    maxThresholdBox->setMaxLength(5);
    maxThresholdBox->setFixedWidth(50);
    maxThresholdBox->setText(QString("%1").arg(maxThreshold));
    connect(maxThresholdBox, SIGNAL(textEdited(const QString &)),
        this, SLOT(setMaxThreshold(const QString &)));  
        
    QHBoxLayout *rangeBox = new QHBoxLayout;
    rangeBox->addWidget(minThresholdBox);
    rangeBox->addStretch();
    rangeBox->addWidget(maxThresholdBox);

    thresholdBox = new QSpinBox;
    thresholdBox->setRange(minThreshold, maxThreshold);
    thresholdSlider = new QSlider(Qt::Horizontal);
    thresholdSlider->setRange(minThreshold, maxThreshold);
    connect(thresholdBox, SIGNAL(valueChanged(int)),
        thresholdSlider, SLOT(setValue(int)));
    connect(thresholdSlider, SIGNAL(valueChanged(int)),
        thresholdBox, SLOT(setValue(int)));
    connect(thresholdBox, SIGNAL(valueChanged(int)),
        this, SLOT(setThreshold(int)));
    connect(thresholdSlider, SIGNAL(valueChanged(int)),
        this, SLOT(setThreshold(int)));
    // thresholdBox value can only be set after makeStencil is createed.

    fillValueBox = new QLineEdit;
    fillValueBox->setMaxLength(5);
    fillValueBox->setFixedWidth(50);
    connect(fillValueBox, SIGNAL(textEdited(const QString &)),
        this, SLOT(setFillValue(const QString &)));
    fillValueBox->setText("0");
    showThresholdBox = new QCheckBox;
    
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(new QLabel("range"), 0, 0);
    grid->addLayout(rangeBox, 0, 2);
    grid->addWidget(new QLabel("threshold"), 1, 0);
    grid->addWidget(thresholdBox, 1, 1);
    grid->addWidget(thresholdSlider, 1, 2);
    grid->addWidget(new QLabel("fill in"), 2, 0);
    grid->addWidget(fillValueBox);
    grid->addWidget(new QLabel("show         "), 3, 0);
    grid->addWidget(showThresholdBox, 3, 1);
    QGroupBox *gbox = new QGroupBox("Threshold to remove skull");
    gbox->setLayout(grid);
    mainLayout->addWidget(gbox);

    // Subtraction
    subtractor = vtkImageMathematics::New();
    subtractor->SetOperationToSubtract();

#ifdef DEBUG
    showSubtractBox = new QCheckBox;
    grid = new QGridLayout;
    grid->addWidget(new QLabel("show         "), 0, 0);
    grid->addWidget(showSubtractBox, 0, 1);
    grid->addWidget(new QLabel
        ("                                                     "), 0, 2);
    gbox = new QGroupBox("Subtract to obtain skull");
    gbox->setLayout(grid);
    mainLayout->addWidget(gbox);
#endif
    
    // Dilation
    dilater = vtkImageContinuousDilate3D::New();
    dilateSizeBox = new QSpinBox;
    dilateSizeBox->setRange(1, 20);
    connect(dilateSizeBox, SIGNAL(valueChanged(int)),
        this, SLOT(setDilateSize(int)));
    dilateSizeBox->setValue(1);
    showDilateBox = new QCheckBox;
    
    grid = new QGridLayout;
    grid->addWidget(new QLabel("size"), 0, 0);
    grid->addWidget(dilateSizeBox, 0, 1);
    grid->addWidget(new QLabel("show         "), 1, 0);
    grid->addWidget(showDilateBox, 1, 1);
    grid->addWidget(new QLabel
        ("                                                    "), 0, 2);
    gbox = new QGroupBox("Dilate to expand mask");
    gbox->setLayout(grid);
    mainLayout->addWidget(gbox);
    
    // Masking
    stencil = vtkImageStencil::New();
    stencil->ReverseStencilOn();
    stencil->SetBackgroundValue(0);
    makeStencil = vtkImageToImageStencil::New();
    thresholdBox->setValue(threshold); // Set after makeStencil is createed.

#ifdef DEBUG
    showMaskBox = new QCheckBox;
    grid = new QGridLayout;
    grid->addWidget(new QLabel("show         "), 0, 0);
    grid->addWidget(showMaskBox, 0, 1);
    grid->addWidget(new QLabel
        ("                                                     "), 0, 2);
    gbox = new QGroupBox("Mask out skull");
    gbox->setLayout(grid);
    mainLayout->addWidget(gbox);
#endif
    
    mainLayout->addWidget(
        new QLabel("____________________________________________"));
    mainLayout->addSpacing(5);
    
    // Flood fill    
    vtk2itk = Vtk2ItkType::New();
    itk2vtk = Itk2VtkType::New();
    
    showFillBox = new QCheckBox;
    grid = new QGridLayout;
    grid->addWidget(new QLabel("show         "), 0, 0);
    grid->addWidget(showFillBox, 0, 1);
    grid->addWidget(new QLabel
        ("                                                     "), 0, 2);
    gbox = new QGroupBox("Mask out skin and others");
    gbox->setLayout(grid);
    mainLayout->addWidget(gbox);
    
    // Mutually exclusive show boxes
    QButtonGroup *bgroup = new QButtonGroup;
    bgroup->addButton(showThresholdBox);
    bgroup->addButton(showDilateBox);
#ifdef DEBUG
    bgroup->addButton(showSubtractBox);
    bgroup->addButton(showMaskBox);
#endif
    bgroup->addButton(showFillBox);
    connect(bgroup, SIGNAL(buttonClicked(QAbstractButton *)),
        this, SLOT(changeOutput(QAbstractButton *)));
    showThresholdBox->setChecked(true);
        
    // Apply and Close button
    QPushButton *abutton = new QPushButton("apply");
    connect(abutton, SIGNAL(clicked()), this, SLOT(apply()));
    closeButton = new QPushButton("close");
    connect(closeButton, SIGNAL(clicked()), this, SLOT(hide()));
    connect(closeButton, SIGNAL(clicked()), this, SIGNAL(closed()));
    
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addWidget(abutton);
    hbox->addStretch();
    hbox->addWidget(closeButton);
    mainLayout->addLayout(hbox);
    
    // Output connector
    output = vtkImageCacheFilter::New();
    output->SetCacheSize(1);
}


void PSkullRemover::setInputConnection(vtkAlgorithmOutput *input)
{
    if (!input)
        return;
        
    thresholder->SetInputConnection(input);
    vtkImageAlgorithm *algo = 
        vtkImageAlgorithm::SafeDownCast(input->GetProducer());
    subtractor->SetInput1(algo->GetOutput());
    subtractor->SetInput2(thresholder->GetOutput());
    dilater->SetInputConnection(subtractor->GetOutputPort());
    makeStencil->SetInput(dilater->GetOutput());
    stencil->SetStencil(makeStencil->GetOutput());
    stencil->SetInputConnection(input);
    output->SetInputConnection(thresholder->GetOutputPort());
    hasInput = true;
}



vtkAlgorithmOutput* PSkullRemover::getOutputPort()
{
    return output->GetOutputPort();
}


vtkImageData *PSkullRemover::getOutput()
{
    return output->GetOutput();
}


vtkImageAlgorithm *PSkullRemover::getOutputFilter()
{
    return output;
}


void PSkullRemover::changeOutput(QAbstractButton *box)
{
#ifdef DEBUG
    if (box == showThresholdBox)
        output->SetInputConnection(thresholder->GetOutputPort());
    else if (box == showSubtractBox)
        output->SetInputConnection(subtractor->GetOutputPort());
    else if (box == showDilateBox)
        output->SetInputConnection(dilater->GetOutputPort());
    else if (box === showMaskBox)
        output->SetInputConnection(stencil->GetOutputPort());
#else
    if (box == showThresholdBox)
        output->SetInputConnection(thresholder->GetOutputPort());
    else if (box == showDilateBox)
        output->SetInputConnection(stencil->GetOutputPort());
#endif
    
    emit updated();
}


void PSkullRemover::setMinThreshold(const QString &text)
{
    bool ok;   
    int value = text.trimmed().toDouble(&ok);
    
    if (ok)
    {
        minThreshold = value;
        minThresholdBox->setText(QString("%1").arg(value));
        thresholdBox->setRange(minThreshold, maxThreshold);
        thresholdSlider->setRange(minThreshold, maxThreshold);

        if (threshold < minThreshold)
        {
            threshold = minThreshold;
            thresholdBox->setValue(threshold);
            apply();
        }
        
        makeStencil->ThresholdByUpper(threshold);
    }
    else
    {
        minThresholdBox->setText(QString("%1").arg((int) minThreshold));
    }
}


void PSkullRemover::setMaxThreshold(const QString &text)
{
    bool ok;   
    int value = text.trimmed().toDouble(&ok);
    
    if (ok)
    {
        maxThreshold = value;
        maxThresholdBox->setText(QString("%1").arg(value));
        thresholdBox->setRange(minThreshold, maxThreshold);
        thresholdSlider->setRange(minThreshold, maxThreshold);

        if (threshold > maxThreshold)
        {
            threshold = maxThreshold;
            thresholdBox->setValue(threshold);
            apply();
        }
        
        makeStencil->ThresholdByUpper(threshold);
    }
    else
    {
        maxThresholdBox->setText(QString("%1").arg((int) maxThreshold));
    }
}


void PSkullRemover::setThreshold(int value)
{
    threshold = value;
    makeStencil->ThresholdByUpper(threshold);
    apply();
}


void PSkullRemover::setFillValue(const QString &text)
{
    bool ok;   
    int value = text.trimmed().toDouble(&ok);
    
    if (ok)
    {
        fillValue = value;
        fillValueBox->setText(QString("%1").arg(value));
        thresholder->SetInValue(fillValue);
        apply();
    }
    else
    {
        fillValueBox->setText(QString("%1").arg((int) fillValue));
    }
}


void PSkullRemover::setDilateSize(int size)
{
    dilater->SetKernelSize(size, size, size);
    apply();
}


void PSkullRemover::apply()
{
    if (!hasInput)
        return;
    
    thresholder->ThresholdByUpper(threshold);
    
    vtk2itk->SetInput(thresholder->GetOutput());
    itk2vtk->SetInput(vtk2itk->GetOutput());
    emit updated();
}
