/* PDicomSegmenter.h

   DICOM image segmentation tool.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#ifndef PDICOMSEGMENTER_H
#define PDICOMSEGMENTER_H

class QPushButton;
class QHBoxLayout;

#include "PDicomViewer.h"
#include "PVoiWidget.h"
#include "PThresholdDialog.h"
#include "PSkullRemover.h"
#include "vtkImageAnisotropicDiffusion3D.h"
    
#include "vtkExtractVOI.h"
#include "vtkMarchingCubes.h"
#include "vtkDecimatePro.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataMapper.h"

#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"

#include "vtkDICOMImageReader.h"
#include "vtkSmartVolumeMapper.h"
#include "vtkFixedPointVolumeRayCastMapper.h"
#include "vtkPiecewiseFunction.h"
#include "vtkColorTransferFunction.h"
#include "vtkVolumeProperty.h"


class PDicomSegmenter: public PDicomViewer
{
    Q_OBJECT

public:
    PDicomSegmenter();
    ~PDicomSegmenter();
    void setReader(vtkDICOMImageReader *reader);  // Override
    void setInput(vtkImageData *in);  // Override
    vtkImageData *getOutput();  // Override
    vtkImageData *getOutputImage();
    vtkPolyData *getOutputMesh();
    
protected:
    void closeEvent(QCloseEvent *event);
    void hideEvent(QHideEvent *event);
    
protected slots:
    void info();
    void saveMeshPly();
    void saveMeshStl();
    void saveVolumeView();
    void saveMeshView();    
    void resetInput();
    void updateViewers();
    void help();  // Redefine
    
    // VOI
    void resetVoi();
    void setVoi();
    void cropVoi();
    
    // Tools
    void showThresholdDialog();
    void showSkullRemover();
    
    void showDiffuserDialog();
    void setDiffThreshold(int threshold);
    void setDiffFactor(double factor);
    void anisoDiffuse();

    // Volume rendering
    void showVolumeRenderDialog();
    void volumeRender();
    void setSampleDistance(int dist);
    
    // Mesh generation
    void showGenMeshDialog();
    void generateMesh();
    void smoothing();

protected:
    void addWidgets();
    void addActions();
    void addMenus();
    void addToolBars();

    QAction *saveMeshPlyAction;
    QAction *saveMeshStlAction;
    QAction *saveVolumeViewAction;
    QAction *saveMeshViewAction;
    QAction *resetInputAction;
    QAction *setVoiAction;
    QAction *cropVoiAction;
    QAction *thresholdAction;
    QAction *removeSkullAction;
    QAction *anisoDiffuseAction;
    QAction *volumeRenderAction;
    QAction *genMeshAction;
    
    QMenu *saveMeshMenu;
    QMenu *toolMenu;
    QMenu *outputMenu;
    
    QToolBar *toolToolBar;
    QToolBar *outputToolBar;
    
    // VOI
    PVoiWidget *transVoi;
    PVoiWidget *coronalVoi;
    PVoiWidget *sagittalVoi;
    vtkExtractVOI *extractVoi;
   
    // Tools
    PThresholdDialog *thresholdDialog;
    PSkullRemover *skullRemover;
    vtkImageAnisotropicDiffusion3D *anisoDiffuser;
    
    // Anisotropic diffuser dialog
    QWidget *anisoDiffuseDialog;
    QSpinBox *diffThresholdBox;
    QSlider *diffThresholdSlider;
    QDoubleSpinBox *diffFactorBox;
    QPushButton *closeDiffuseButton;
    
    // Output connector
    vtkExtractVOI *outputVoi;
    
    // Volume renderer dialog
    QWidget *volumeRenderDialog;
    QComboBox *viewTypeBox;
    QComboBox *sampleDistanceBox;
    QDoubleSpinBox *ambientBox;
    QDoubleSpinBox *specularBox;
    QPushButton *closeVolumeButton;
    
    // Volume renderer: faster visualisation than mesh generation
    QVTKWidget *volumeWidget;
    vtkVolumeMapper *volumeMapper;
    vtkSmartVolumeMapper *smartMapper;
    vtkFixedPointVolumeRayCastMapper *rayCastMapper;
    vtkPiecewiseFunction *opacityFn;
    vtkColorTransferFunction *colorFn;
    vtkVolumeProperty *property;
    vtkVolume *volumeActor;
    vtkRenderer* volumeRenderer;
    vtkInteractorStyleTrackballCamera *volumeStyle;
    vtkImageData *outputVolume;

    // Mesh objects
    vtkMarchingCubes *mcubes;
    vtkDecimatePro *decimate;
    vtkSmoothPolyDataFilter *smooth;
    vtkPolyDataNormals *normals;
    vtkPolyData *outputMesh;
    
    // Mesh generation dialog
    QWidget *genMeshDialog;
    QSpinBox *intensityBox;
    QDoubleSpinBox *ratioBox;
    QDoubleSpinBox *factorBox;
    QSpinBox *smoothIterBox;
    QPushButton *closeMeshButton;
    
    // Mesh viewer
    QHBoxLayout *box01;
    QVTKWidget *meshWidget;
    vtkPolyDataMapper *meshMapper;
    vtkActor *meshActor;
    vtkRenderer *meshRenderer;
    vtkInteractorStyleTrackballCamera *meshStyle;
    
    // Internal variables
    bool voiDone;
    bool thresholdDone;
    bool removeSkullDone;
    bool segmented;
    bool anisoDiffuseDone;
    bool hasVolumeActor;  // Actor added
    bool hasMeshActor;  // Actor added
    
    int selectedVoi[6];

    // Supporting functions
    void createVoiObjects();
    void createAnisoDiffuser();
    void createVolumeRenderer();
    void createVolumeDialog();
    void createMeshObjects();
    void createMeshViewer();
    void createMeshDialog();
    void resetPipeline();
    
    void saveView(int type);
    bool saveView(const QString &fileName, int type);
    void wakeVoi();
    int *computeBound();
    void computeOutputVolume();
    void setBlendType();
};

#endif 