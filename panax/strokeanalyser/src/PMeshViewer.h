/* PMeshViewer.h

   3D mesh viewer.

   Copyright 2012, 2103, National University of Singapore
   Author: Leow Wee Kheng
*/

#ifndef PMESHVIEWER_H
#define PMESHVIEWER_H

#include <QMainWindow>
#include <QTableWidget>

class QAction;
class QComboBox;

#include "QVTKWidget.h"
#include "vtkPolyDataAlgorithm.h"
#include "vtkPolyDataNormals.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkPropPicker.h"


class PMeshPart
{
    friend class PMeshViewer;
    QString name;
    QString source;
    vtkPolyData *data;
    vtkPolyDataNormals *normals;
    vtkPolyDataMapper *mapper;
    vtkActor *actor;
    QColor color;
};


class PMeshViewer: public QMainWindow
{
    Q_OBJECT

public:
    PMeshViewer();
    ~PMeshViewer();
    void setAppName(const QString &name);
    bool eventFilter(QObject *target, QEvent *event);
    
    void setInput(vtkAlgorithmOutput *input);
    void updateNow();
   
    // For callback
    vtkRenderWindowInteractor *getInteractor();
    vtkRenderer *getRenderer();
    void highlight(vtkActor *actor);
    
protected:
    void closeEvent(QCloseEvent *event);
    
protected slots:
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectAs();
    void loadDir();
    void loadMesh();
    void addMesh();
    void deleteMesh();
    void saveDirPly();
    void saveDirStl();
    void saveView();
    void toggleFrontFace();
    void setMeshMode(int);
    void setSmoothSurface();
    void setFlatSurface();
    void setFlatLines();
    void setWireFrame();
    void info();
    void help();
    void about();

    void setVisibility(int row, int col);
    void setColor(int row, int col);
    void blink(int row, int col);

protected:
    void createWidgets();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();

    QAction *newProjectAction;
    QAction *openProjectAction;
    QAction *saveProjectAction;
    QAction *saveProjectAsAction;
    QAction *loadDirAction;
    QAction *loadMeshAction;
    QAction *addMeshAction;
    QAction *deleteMeshAction;
    QAction *saveDirPlyAction;
    QAction *saveDirStlAction;
    QAction *exitAction;
    QAction *saveViewAction;
    QAction *frontFaceAction;
    QAction *smoothSurfaceAction;
    QAction *flatSurfaceAction;
    QAction *flatLinesAction;
    QAction *wireFrameAction;
    QAction *toggleRightDockWidgetAction;
    QAction *infoAction;
    QAction *helpAction;
    QAction *aboutAction;
    
    QMenu *fileMenu;
    QMenu *saveMeshMenu;
    QMenu *viewMenu;
    QMenu *meshModeMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    
    QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QToolBar *windowToolBar;
    QToolBar *helpToolBar;

    // Widgets and VTK objects.
    QVTKWidget *vtkWidget;
    QComboBox *meshModeBox;
    QDockWidget *rightDockWidget;
    QTableWidget *meshTable;
    QList<PMeshPart> meshList;

    vtkRenderer *renderer;
    vtkRenderWindow *renderWindow;
    vtkRenderWindowInteractor *interactor;
    vtkInteractorStyleTrackballCamera *style;
    
    // Internal variables.
    QString appName;
    QString projectName;
    bool loaded;
    int winWidth, winHeight;
    int hideFrontFace;
    
    // Supporting methods
    void initSize();
    void installPipeline(int startIndex);
    void uninstallPipeline();
    void loadMesh(const QStringList &fileNames);
    void addMesh(const QStringList &fileNames);
    bool saveImage(const QString &fileName);
};

#endif