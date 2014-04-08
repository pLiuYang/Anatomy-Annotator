/* PThresholdDialog.h

   Threshold dialog box and functions.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#ifndef PTHRESHOLDDIALOG_H
#define PTHRESHOLDDIALOG_H

#include <QObject>
#include <QVariant>
#include <QSpinBox>
#include <QSlider>
#include <QLineEdit>
#include "vtkImageThreshold.h"


class PThresholdDialog: public QWidget
{
    Q_OBJECT

public:
    PThresholdDialog();
    ~PThresholdDialog();

    void setInputConnection(vtkAlgorithmOutput *input);
    vtkAlgorithmOutput *getOutputPort();
    vtkImageData *getOutput();
    vtkImageAlgorithm *getOutputFilter();
    void apply();
    
signals:
    void updated();
    void closed();

protected slots:
    void thresholdLower();
    void thresholdBetween();
    void thresholdUpper();
    void setUpperMin(const QString &text);
    void setUpperMax(const QString &text);
    void setUpper(int value);
    void setLowerMin(const QString &text);
    void setLowerMax(const QString &text);
    void setLower(int value);
    void setFill(const QString &text);

protected:
    vtkImageThreshold *threshold;
    bool hasInput;
    double upperMin, upperMax;
    double lowerMin, lowerMax;
    double upper;
    double lower;
    double fill;
    enum Type {Lower, Between, Upper};
    Type type;

    QLineEdit *upperMinBox;
    QLineEdit *upperMaxBox;
    QSpinBox  *upperBox;
    QLineEdit *lowerMinBox;
    QLineEdit *lowerMaxBox;
    QSpinBox  *lowerBox;
    QLineEdit *fillBox;
    QSlider *upperSlider;
    QSlider *lowerSlider;

    void createWidgets();
};

#endif
