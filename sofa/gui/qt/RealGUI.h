/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*            (c) 2006-2021 INRIA, USTL, UJF, CNRS, MGH, InSimo                *
*                                                                             *
* This program is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU General Public License as published by the Free  *
* Software Foundation; either version 2 of the License, or (at your option)   *
* any later version.                                                          *
*                                                                             *
* This program is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
* more details.                                                               *
*                                                                             *
* You should have received a copy of the GNU General Public License along     *
* with this program; if not, write to the Free Software Foundation, Inc., 51  *
* Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.                   *
*******************************************************************************
*                            SOFA :: Applications                             *
*                                                                             *
* Authors: The SOFA Team and external contributors (see Authors.txt)          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_GUI_VIEWER_REALGUI_H
#define SOFA_GUI_VIEWER_REALGUI_H

#include <sofa/SofaGui.h>
#include <ui_GUI.h>
#include "SofaGUIQt.h"
#include "GraphListenerQListView.h"
#include "QMenuFilesRecentlyOpened.h"
#include "PickHandlerCallBacks.h"

#include "../BaseGUI.h"
#include "../ViewerFactory.h"

#include <set>
#include <string>

#ifdef SOFA_QT4
#   include <Q3ListViewItem>
typedef Q3ListViewItem QListViewItem;
#   include <QStackedWidget>
typedef QStackedWidget QWidgetStack;
#   include <QUrl>
#   include <QSignalMapper>
#else
#   include <qurl.h>
#   include <qwidgetstack.h>
#   include <qlistview.h>
#endif

#include <time.h>

// Recorder GUI is not used (broken in most scenes)
#define SOFA_GUI_QT_NO_RECORDER

#ifdef SOFA_QT4
class QTimer;
class QTextBrowser;
#endif

class WDoubleLineEdit;
class QDragEnterEvent;

namespace sofa
{

namespace gui
{
class CallBackPicker;
class BaseViewer;

namespace qt
{

#ifndef SOFA_GUI_QT_NO_RECORDER
class QSofaRecorder;
#endif

enum SCRIPT_TYPE { PHP, PERL };

class QSofaListView;
class QSofaStatWidget;
class GraphListenerQListView;
class DisplayFlagsDataWidget;
class SofaPluginManager;
#ifdef SOFA_DUMP_VISITOR_INFO
class WindowVisitor;
class GraphVisitor;
#endif

namespace viewer
{
class SofaViewer;
}


class SOFA_SOFAGUIQT_API RealGUI :public Q3MainWindow, public Ui::GUI, public sofa::gui::BaseGUI
{
    Q_OBJECT

//-----------------STATIC METHODS------------------------{
public:
    using BaseGUI::create;
    static int InitGUI();
    static RealGUI* CreateGUI(const sofa::simulation::gui::BaseGUIArgument* a);

    static void SetPixmap(std::string pixmap_filename, QPushButton* b);

protected:
    static void CreateApplication(const sofa::simulation::gui::BaseGUIArgument* a);
    static void InitApplication( RealGUI* _gui);
//-----------------STATIC METHODS------------------------}



//-----------------CONSTRUCTOR - DESTRUCTOR ------------------------{
public:
    explicit RealGUI(const sofa::simulation::gui::BaseGUIArgument* a);

    ~RealGUI();
//-----------------CONSTRUCTOR - DESTRUCTOR ------------------------}



//-----------------OPTIONS DEFINITIONS------------------------{
public:
#ifndef SOFA_QT4
    void setWindowFilePath(const QString &filePath)
    {
        filePath_=filePath;
    }

    QString windowFilePath() const
    {
        QString filePath = filePath_; return filePath;
    }
#endif

#ifdef SOFA_GUI_INTERACTION
    QPushButton *interactionButton;
#endif

#ifdef SOFA_DUMP_VISITOR_INFO
    virtual void setTraceVisitors(bool);
#endif

    virtual void showFPS(double fps);

public slots:

	virtual void changeHtmlPage( const QUrl&);
	virtual void setGUIMode( int );

    virtual void updateVisualBufferSize(int bufferNewSize);
    virtual void setFrameDisplay(int);

protected:
#ifdef SOFA_GUI_INTERACTION
    void mouseMoveEvent( QMouseEvent * e);
    void wheelEvent( QWheelEvent * event );
    void mousePressEvent(QMouseEvent * e);
    void mouseReleaseEvent(QMouseEvent * e);
    void keyReleaseEvent(QKeyEvent * e);
    bool eventFilter(QObject *obj, QEvent *event);
#endif
    void startIdle();
    void stopIdle();

#ifndef SOFA_GUI_QT_NO_RECORDER
    QSofaRecorder* recorder;
#else
    QLabel* fpsLabel;
    QLabel* timeLabel;
    QLabel* stepLabel;
#endif
    QSignalMapper* guimodeSignalMapper;
    int currentGUIMode; ///< If -1, switch to mode 2 automatically if the application provides external image

    QSignalMapper* frameSignalMapper; //map ability to view previous frame
    int m_frame;

private:
#ifndef SOFA_QT4
    QString filePath_;
#endif

#ifdef SOFA_GUI_INTERACTION
    bool m_interactionActived;
#endif

#ifdef SOFA_DUMP_VISITOR_INFO
    WindowVisitor* windowTraceVisitor;
    GraphVisitor* handleTraceVisitor;
#endif
//-----------------OPTIONS DEFINITIONS------------------------}



//-----------------DATAS MEMBER------------------------{
public:
    //TODO: make a protected data with an accessor
    QSofaListView* simulationGraph;

protected:
    /// create a viewer by default, otherwise you have to manage your own viewer
    bool mCreateViewersOpt;
    bool mIsEmbeddedViewer;
    bool m_dumpState;
    std::ofstream* m_dumpStateStream;
    std::ostringstream m_dumpVisitorStream;
    bool m_exportGnuplot;
    bool _animationOBJ;
    int _animationOBJcounter;// save a succession of .obj indexed by _animationOBJcounter
    bool m_displayComputationTime;
    bool m_fullScreen;
    BaseViewer* mViewer;

    /// list of all viewer key name (for creation) mapped to its QAction in the GUI
    std::map< helper::SofaViewerFactory::Key, QAction* > viewerMap;
    InformationOnPickCallBack informationOnPickCallBack;

    QWidget* currentTab;
    QSofaStatWidget* statWidget;
    QTimer* timerStep;
    QTimer* timerIdle;
    WDoubleLineEdit *background[3];
    QLineEdit *backgroundImage;
    /// Stack viewer widget
    QWidgetStack* left_stack;
    SofaPluginManager* pluginManager_dialog;
    QMenuFilesRecentlyOpened recentlyOpenedFilesManager;

    std::string simulation_name;
    std::string gnuplot_directory;
    std::string pathDumpVisitor;
    
    /// Keep track of log files that have been modified since the GUI started
    std::set<std::string>   m_modifiedLogFiles;

    double maxFPS;
    sofa::helper::system::thread::ctime_t throttle_lastframe; // only used if maxFPS > 0
    double idleFrequency = 0.0;

    /// Will be set to true if the simulation is being step externally, i.e. not by the GUI
    bool externalStepping;

private:
    //currently unused: scale is experimental
    float object_Scale[2];
    bool saveReloadFile;
    DisplayFlagsDataWidget *displayFlag;
    QDialog* descriptionScene;
    QTextBrowser* htmlPage;
    bool animationState;
    int frameCounter;
    int stopAfterStep;
    int animateLockCounter;
    void* viewerShareRenderingContext;
//-----------------DATAS MEMBER------------------------}



//-----------------METHODS------------------------{
public :
    bool stepMainLoop ();

    int mainLoop() override;
    void initialize() override;
    virtual sofa::simulation::Node* getCurrentSimulation() override;
    virtual void fileOpen(std::string filename, bool temporaryFile=false);
    
    virtual void fileOpenSimu(std::string filename);
    virtual void setScene(Node::SPtr groot, const char* filename=NULL, bool temporaryFile=false) override;
    virtual void unloadScene(bool _withViewer = true);

    virtual void setTitle( std::string windowTitle );

    virtual void fileSaveAs(Node* node,const char* filename);

    virtual void saveXML();


    virtual void setViewerResolution(int w, int h);
    virtual void setFullScreen(bool enable = true);
    virtual void setBackgroundColor(const defaulttype::Vector3& c);
    virtual void setBackgroundImage(const std::string& i);
    virtual void setViewerConfiguration(sofa::component::configurationsetting::ViewerSetting* viewerConf);
    virtual void setMouseButtonConfiguration(sofa::component::configurationsetting::MouseButtonSetting *button);

    //Configuration methods
    virtual void setDumpState(bool);
    virtual void setLogTime(bool);
    virtual void setExportState(bool);
    virtual void setRecordPath(const std::string & path);
    virtual void setGnuplotPath(const std::string & path);

    /// create a viewer according to the argument key
    /// \note the viewerMap have to be initialize at least once before
    /// \arg _updateViewerList is used only if you want to reactualise the viewerMap in the GUI
    /// TODO: find a better way to propagate the argument when we construct the viewer
    virtual void createViewer(const char* _viewerName, bool _updateViewerList=false);

    /// Used to directly replace the current viewer
    virtual void registerViewer(BaseViewer* _viewer);

    virtual BaseViewer* getViewer();

    /// A way to know if our viewer is embedded or not... (see initViewer)
    /// TODO: Find a better way to do this
    sofa::gui::qt::viewer::SofaViewer* getQtViewer();

    /// Our viewer is a QObject SofaViewer
    bool isEmbeddedViewer();

    virtual void removeViewer();

    void dragEnterEvent( QDragEnterEvent* event);

    void dropEvent(QDropEvent* event);

    void initQt(const char* name);

    using CopyScreenInfo = sofa::simulation::gui::BaseGUI::CopyScreenInfo;
    bool getCopyScreenRequest(CopyScreenInfo& info) override;
    void useCopyScreen(CopyScreenInfo& info) override;

protected:
    /// init data member from RealGUI for the viewer initialisation in the GUI
    void init();
    void createDisplayFlags(Node::SPtr root);
    void loadHtmlDescription(const char* filename);
    void loadSimulation(bool one_step=false);//? where is the implementation ?
    void eventNewStep();
    void eventNewTime();
    void keyPressEvent ( QKeyEvent * e );
    void startDumpVisitor();
    void stopDumpVisitor();

    /// init the viewer for the GUI (embeded or not we have to connect some info about viewer in the GUI)
    virtual void initViewer(BaseViewer* _viewer);

    /// Our viewer is a QObject SofaViewer
    void isEmbeddedViewer(bool _onOff)
    {
        mIsEmbeddedViewer = _onOff;
    }

    virtual int exitApplication(unsigned int _retcode = 0)
    {
        return _retcode;
    }

    void sleep(float seconds, float init_time)
    {
        unsigned int t = 0;
        clock_t goal = (clock_t) (seconds + init_time);
        while (goal > clock()/(float)CLOCKS_PER_SEC) t++;
    }

private:
    void addViewer();//? where is the implementation ?

    /// Parse options from the RealGUI constructor
    void parseOptionsPreInit(const std::vector<std::string>& options);
    void parseOptionsPostInit(const std::vector<std::string>& options);

    void createPluginManager();

    /// configure Recently Opened Menu
    void createRecentFilesMenu();

    void createBackgroundGUIInfos();
    void createSimulationGraph();
    void createWindowVisitor();
    void createSceneDescription();
//----------------- METHODS------------------------}



//-----------------SIGNALS-SLOTS------------------------{
public slots:

	//-----TEST
	virtual void fileOpen();
	virtual void fileNew();
    virtual void fileSave();
    virtual void fileSaveAs()
    {
        fileSaveAs((Node *)NULL);
    }

    virtual void fileReload();
    virtual void fileExit();
	virtual void editUndo();
    virtual void editRedo();
    virtual void editCut();
    virtual void editCopy();
    virtual void editPaste();
    virtual void editFind();
    virtual void helpIndex();
    virtual void helpContents();
    virtual void helpAbout();
	virtual void editRecordDirectory();
    virtual void editGnuplotDirectory();
    virtual void showPluginManager();
    virtual void showMouseManager();
    virtual void showVideoRecorderManager();
	virtual void GUI_usesTextLabelChanged( bool );
	//----TEST

	virtual void NewRootNode(sofa::simulation::Node* root, const char* path);
    virtual void ActivateNode(sofa::simulation::Node* , bool );
    virtual void fileSaveAs(sofa::simulation::Node *node);
    virtual void setDrawStatus(sofa::simulation::Node *node, bool status);
    virtual void LockAnimation(bool);
    virtual void fileRecentlyOpened(int id);
    virtual void playpauseGUI(bool value);
    virtual void interactionGUI(bool value);
    virtual void step();
    virtual void idle();
    virtual void setDt(double);
    virtual void setDt(const QString&);
    virtual void setMaxFPS(double);
    virtual void setMaxFPS(const QString&);
    virtual void resetScene();
    virtual void screenshot();
    virtual void showhideElements();
    virtual void Update();
    virtual void updateBackgroundColour();
    virtual void updateBackgroundImage();

    // Propagate signal to call viewer method in case of it is not a widget
    virtual void resetView()            {if(getViewer())getViewer()->resetView();       }
    virtual void saveView()             {if(getViewer())getViewer()->saveView();        }
    virtual void setSizeW ( int _valW ) {if(getViewer())getViewer()->setSizeW(_valW);   }
    virtual void setSizeH ( int _valH ) {if(getViewer())getViewer()->setSizeH(_valH);   }

    virtual void clear();
    /// refresh the visualization window
    virtual void redraw();
    virtual void exportOBJ(sofa::simulation::Node* node, bool exportMTL=true);
    virtual void dumpState(bool);
    virtual void displayComputationTime(bool);
    virtual void setExportGnuplot(bool);
    virtual void setExportVisitor(bool);
    virtual void currentTabChanged(QWidget*);
    virtual void setSelectedComponent(sofa::core::objectmodel::Base*);

protected slots:
    /// Allow to dynamicly change viewer. Called when click on another viewer in GUI Qt viewer list (see viewerMap).
    /// TODO: find a better way to propagate the argument when we construct the viewer
    virtual void changeViewer();

    /// Update the viewerMap and create viewer if we haven't yet one (the first of the list)
    /// TODO: find a better way to propagate the argument when we construct the viewer
    virtual void updateViewerList();

    void appendToDataLogFile(QString);

signals:
    void reload();
    void newScene();
    void newStep();
    void quit();
//-----------------SIGNALS-SLOTS------------------------}

};





struct ActivationFunctor
{
    ActivationFunctor(bool act, GraphListenerQListView* l)
    :pixmap_filename("textures/media-record.png")
    , active(act)
    , listener(l)
    {
        if ( sofa::helper::system::DataRepository.findFile ( pixmap_filename ) )
            pixmap_filename = sofa::helper::system::DataRepository.getFile ( pixmap_filename );
    }
    void operator()(core::objectmodel::BaseNode* n)
    {
        if (active)
        {
            //Find the corresponding node in the Qt Graph
            QListViewItem *item=listener->items[n];
            //Remove the text
            QString desact_text = item->text(0);
            desact_text.remove(QString("Deactivated "), true);
            item->setText(0,desact_text);
            //Remove the icon
            QPixmap *p = getPixmap(n);
            item->setPixmap(0,*p);
            item->setOpen(true);
        }
        else
        {
            //Find the corresponding node in the Qt Graph
            QListViewItem *item=listener->items[n];
            //Remove the text
            item->setText(0, QString("Deactivated ") + item->text(0));
#ifdef SOFA_QT4
            item->setPixmap(0,QPixmap::fromImage(QImage(pixmap_filename.c_str())));
#else
            item->setPixmap(0,QPixmap(QImage(pixmap_filename.c_str())));
#endif
            item->setOpen(false);
        }
    }
protected:
    std::string pixmap_filename;
    bool active;
    GraphListenerQListView* listener;
};

} // namespace qt

} // namespace gui

} // namespace sofa

#endif // SOFA_GUI_VIEWER_REALGUI_H
