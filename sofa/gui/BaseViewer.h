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
#ifndef SOFA_GUI_BASEVIEWER_H
#define SOFA_GUI_BASEVIEWER_H

#include "SofaGUI.h"

#include "ColourPickingVisitor.h"

#include <sofa/helper/Factory.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/core/CollisionModel.h>


#include <sofa/helper/system/FileRepository.h>
#include <sofa/helper/system/SetDirectory.h>

#include <sofa/helper/gl/Capture.h>
#include <sofa/helper/gl/Texture.h>
#ifdef SOFA_HAVE_FFMPEG
#include <sofa/helper/gl/VideoRecorder.h>
#endif //SOFA_HAVE_FFMPEG

#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/core/objectmodel/KeyreleasedEvent.h>
#include <sofa/core/objectmodel/MouseEvent.h>
#include <sofa/core/collision/Pipeline.h>

#include <SofaGraphComponent/ViewerSetting.h>

//instruments handling
#include <SofaUserInteraction/Controller.h>
#include <sofa/defaulttype/LaparoscopicRigidTypes.h>
//#include <sofa/simulation/common/GrabVisitor.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/simulation/common/UpdateMappingVisitor.h>
#include <sofa/simulation/common/Simulation.h>
#include <sofa/simulation/common/Node.h>
#include <SofaBaseVisual/InteractiveCamera.h>
#include <sofa/simulation/common/BaseGUI.h>

#include <sofa/helper/io/Image.h>

#include <string>

namespace sofa
{

namespace gui
{
class PickHandler;

enum
{
    BTLEFT_MODE = 101, BTRIGHT_MODE = 102, BTMIDDLE_MODE = 103,
};

class SOFA_SOFAGUI_API BaseViewer
{

public:
    BaseViewer();
    virtual ~BaseViewer();

    virtual void drawColourPicking (ColourPickingVisitor::ColourCode /*code*/) {}

    virtual sofa::simulation::Node* getScene();
    virtual const std::string& getSceneFileName();
    virtual void setSceneFileName(const std::string &f);
    virtual void setScene(sofa::simulation::Node::SPtr scene, const char* filename = NULL, bool /*keepParams*/= false);
    virtual void setCameraMode(core::visual::VisualParams::CameraType);
    virtual void setSelectedComponent(sofa::core::objectmodel::Base* selected);
    virtual sofa::core::objectmodel::Base* getSelectedComponent() const;
    virtual void setGUIMode(int);

    using CopyScreenInfo = simulation::gui::BaseGUI::CopyScreenInfo;
    virtual bool getCopyScreenRequest(CopyScreenInfo* info);
    virtual void useCopyScreen(CopyScreenInfo* info);

    /// true when the viewer keep the hand on the render
    /// false when it's not in activity
    virtual bool ready();

    /// ask the viewer to resume its activity
    virtual void wait();

    /// Load the viewer. It's the initialisation
    virtual bool load(void);

    /// unload the viewer without delete
    virtual bool unload(void);

    /// Recompute viewer's home position so it encompass the whole scene and apply it
    virtual void viewAll(void) = 0;

    //Allow to configure your viewer using the Sofa Component, ViewerSetting
    virtual void configure(sofa::component::configurationsetting::ViewerSetting* viewerConf);

    //Fonctions needed to take a screenshot
    virtual const std::string screenshotName();
    virtual void setPrefix(const std::string& filename);
    virtual void screenshot(const std::string& filename, int compression_level =-1, bool front = true);

    virtual void getView(sofa::defaulttype::Vec3d& pos, sofa::defaulttype::Quat& ori) const;
    virtual void setView(const sofa::defaulttype::Vec3d& pos, const sofa::defaulttype::Quat &ori);
    virtual void moveView(const sofa::defaulttype::Vec3d& pos, const sofa::defaulttype::Quat &ori);
    virtual void newView();
    virtual void resetView();

    virtual void setVideoRecording(bool enable);
    bool isVideoRecording() const;
    virtual void setVideoPrefix(const std::string& prefix);
    virtual void setVideoLinkPrefix(const std::string& link);

    virtual void setBackgroundColour(float r, float g, float b);
    virtual void setBackgroundImage(std::string imageFileName = std::string("textures/SOFA_logo.bmp"));
    std::string getBackgroundImage();

    virtual void saveView()=0;
    virtual void setSizeW(int)=0;
    virtual void setSizeH(int)=0;
    virtual void captureEvent() {}
    virtual void fitObjectBBox(sofa::core::objectmodel::BaseObject* );
    virtual void fitNodeBBox(sofa::core::objectmodel::BaseNode*);

    virtual void setFullScreen(bool /*enable*/) {}


    /// RayCasting PickHandler
    virtual void moveRayPickInteractor(int, int) {}

    PickHandler* getPickHandler();

    /// the rendering pass is done here (have to be called in a loop)
    virtual void drawScene(void) = 0;

    virtual void recordFrame(void) = 0;
    virtual void updateVisualBuffer(int bufferSize) = 0;

protected:
    /// internally called while the actual viewer needs a redraw (ie the camera changed)
    virtual void redraw() = 0;

    /// the sofa root note of the current scene
    sofa::simulation::Node::SPtr groot;

    sofa::component::visualmodel::BaseCamera::SPtr currentCamera;

    sofa::core::objectmodel::Base::SPtr currentSelectedComponent;

    std::string sceneFileName;

#ifndef SOFA_NO_OPENGL
    sofa::helper::gl::Capture capture;
    sofa::helper::gl::Texture* texLogo;
#endif

#ifdef SOFA_HAVE_FFMPEG
    sofa::helper::gl::VideoRecorder videoRecorder;
#endif

    bool _video;
    std::string _videoPrefix;
    std::string _videoPrefixLink;
    bool _axis;
    bool _fullScreen;
    int _background;
    bool initTexturesDone;

    sofa::defaulttype::Vector3 backgroundColour;
    std::string backgroundImageFile;

    sofa::defaulttype::Vector3 ambientColour;

    PickHandler *pick;

    //instruments handling
    int _navigationMode;
    bool _mouseInteractorMoving;
    int _mouseInteractorSavedPosX;
    int _mouseInteractorSavedPosY;

    //Stereo parameters
    bool _stereoEnabled;
    enum StereoMode
    {
        STEREO_AUTO = 0,
        STEREO_INTERLACED,
        STEREO_FRAME_PACKING,
        STEREO_SIDE_BY_SIDE,
        STEREO_TOP_BOTTOM,
        STEREO_SIDE_BY_SIDE_HALF,
        STEREO_TOP_BOTTOM_HALF,
        STEREO_NONE,
        NB_STEREO_MODES
    };
    StereoMode _stereoMode;
    double _stereoShift;
    double _stereoEyeOffset;
    int _currentGUIMode;

};

}
}

#endif
