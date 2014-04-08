/* PVolumeRenderer.h

   Volume renderer for DICOM volume image.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/


#ifndef PVolumeRenderer_H
#define PVolumeRenderer_H

#include <QMainWindow>

class QAction;
class QComboBox;
class QSlider;
class QDoubleSpinBox;
class QPushButton;

#include "QVTKWidget.h"
#include "vtkDICOMImageReader.h"
#include "vtkFixedPointVolumeRayCastMapper.h"
#include "vtkSmartVolumeMapper.h"
#include "vtkGPUVolumeRayCastMapper.h"
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkVolumeProperty.h"
#include "vtkVolume.h"
#include "vtkRenderer.h"
#include "vtkBoxWidget.h"

class vtkBoxWidgetCallback;

class PVolumeRenderer: public QMainWindow
{
    Q_OBJECT

public:
    PVolumeRenderer();
    ~PVolumeRenderer();
    
    void setAppName(const QString &name);
        
protected:
    void closeEvent(QCloseEvent *event);
    
protected slots:
    void loadDir();
    void saveDir();
    void saveView();
    void setVOI();
    void showLightDialog();
    void setSampleDistance(int option);
    void apply();
    void info();
    void help();
    void about();

protected:
    void createWidgets();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    QAction *loadDirAction;
    QAction *saveDirAction;
    QAction *exitAction;
    QAction *saveViewAction;
    QAction *setVOIAction;
    QAction *setLightAction;
    QAction *infoAction;
    QAction *helpAction;
    QAction *aboutAction;
    
    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
    
    QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QToolBar *helpToolBar;
    
    // Widgets and VTK objects for volume renderer.
    QVTKWidget *volumeWidget;
    vtkDICOMImageReader *reader;
    vtkSmartVolumeMapper *mapper;
    vtkFixedPointVolumeRayCastMapper *rcmapper;
    vtkPiecewiseFunction *opacityFn;
    vtkColorTransferFunction *colorFn;
    vtkVolumeProperty *property;
    vtkVolume *actor;
    vtkRenderer* volumeRenderer;
    QComboBox *viewTypeBox;
    QComboBox *sampleDistanceBox;
    // QComboBox *saveViewBox;
    vtkBoxWidget *boxWidget;
    vtkBoxWidgetCallback *boxCallback;
    
    // Lighting dialog
    QWidget *lightDialog;
    QDoubleSpinBox *ambientBox;
    QDoubleSpinBox *specularBox;
    QPushButton *closeButton;
    
    // Private variables.
    QString appName;
    QString fullDirName;
    bool loaded;
    int winWidth, winHeight;
    int imageWidth, imageHeight, imageDepth;
    
    // Supporting methods
    void createLightDialog();
    void initSize();
    void installPipeline();
    void uninstallPipeline();
    void resetCameras();
    void loadVolume(const QString &dirName);
    bool saveView(const QString &fileName);
    void setBlendType();
};

#endif