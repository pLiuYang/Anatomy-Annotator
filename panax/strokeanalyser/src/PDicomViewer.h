/* PDicomViewer.h

   3-plane DICOM volume viewer.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/


#ifndef PDICOMVIEWER_H
#define PDICOMVIEWER_H

#include <QMainWindow>

class QAction;
class QComboBox;
class QSlider;

#include "QVTKWidget.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageViewer2.h"
#include "vtkPropPicker.h"


class PDicomViewer: public QMainWindow
{
    Q_OBJECT
    
    friend class PDicomViewerCallback;

public:
    PDicomViewer();
    ~PDicomViewer();
    void setAppName(const QString &name);
    void setReader(vtkDICOMImageReader *reader);
    vtkDICOMImageReader *getReader();
    void setInput(vtkImageData *input);
    vtkImageData *getOutput();
        
protected:
    void closeEvent(QCloseEvent *event);
    
protected slots:
    void loadDir();
    void saveDir();
    void setViewPane();
    void resetWindowLevel();
    void setWindowLevel(int type);
    void saveTransView();
    void saveCoronalView();
    void saveSagittalView();
    void info();
    void help();
    void about();

    void transViewSetSlice(int slice);
    void coronalViewSetSlice(int slice);
    void sagittalViewSetSlice(int slice);

protected:
    void createWidgets();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    QAction *loadDirAction;
    QAction *saveDirAction;
    QAction *exitAction;
    QAction *infoAction;
    QAction *setViewPaneAction;
    QAction *resetWindowLevelAction;
    QAction *saveTransViewAction;
    QAction *saveCoronalViewAction;
    QAction *saveSagittalViewAction;
    QAction *helpAction;
    QAction *aboutAction;
    
    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *setViewModeMenu;
    QMenu *saveViewMenu;
    QMenu *helpMenu;
    
    QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QToolBar *helpToolBar;
    
    QComboBox *windowLevelBox;
    
    // DICOM reader
    vtkDICOMImageReader *reader;
    vtkDICOMImageReader *intReader;  // Internal reader
    vtkImageData *input;   // External input volume
    vtkImageData *output;  // Output volume
   
    // Widgets and VTK objects for volume viewer (transverse view)
    QVTKWidget *transWidget;
    vtkImageViewer2 *transViewer;
    QSlider *transSlider;
    QWidget *pane00;
    
    // Free (unused pane)
    QWidget *pane01;
    
    // Widgets and VTK objects for volume viewer (coronal view)
    QVTKWidget *coronalWidget;
    vtkImageViewer2 *coronalViewer;
    QSlider *coronalSlider;
    QWidget *pane10;
    
    // Widgets and VTK objects for volume viewer (sagittal view)
    QVTKWidget *sagittalWidget;
    vtkImageViewer2 *sagittalViewer;
    QSlider *sagittalSlider;
    QWidget *pane11;
    
    // Picker
    vtkPropPicker *picker;
    
    // Internal variables.
    QString appName;
    QString fullDirName;
    int numPane;
    bool loaded;
    int winWidth, winHeight;
    int imageWidth, imageHeight, imageDepth;
    int transSlice, coronalSlice, sagittalSlice;
    
    // Supporting methods
    void initSize();
    void installPipeline();
    void uninstallPipeline();
    void resetCameras();
    void loadVolume(const QString &dirName);
    void setupWidgets();
    void saveView(int);
    bool saveView(const QString &fileName, int type);
    void setWindowLevel(int window, int level);
};

#endif