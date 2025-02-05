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
#include "SofaViewer.h"
#include <sofa/helper/Factory.inl>
#include <SofaBaseVisual/VisualStyle.h>
#include <sofa/core/visual/DisplayFlags.h>

namespace sofa
{
namespace gui
{
namespace qt
{
namespace viewer
{

SofaViewer::SofaViewer()
    : sofa::gui::BaseViewer()
    , m_isControlPressed(false)
{
    colourPickingRenderCallBack = ColourPickingRenderCallBack(this);
}

SofaViewer::~SofaViewer()
{
}

void SofaViewer::redraw()
{
    getQWidget()->update();
}

void SofaViewer::keyPressEvent(QKeyEvent * e)
{

    if (currentCamera)
    {
        sofa::core::objectmodel::KeypressedEvent kpe(e->key());
        currentCamera->manageEvent(&kpe);
    }

    switch (e->key())
    {
    case Qt::Key_T:
    {
        if (!currentCamera) break;
        if (currentCamera->getCameraType() == core::visual::VisualParams::ORTHOGRAPHIC_TYPE)
            setCameraMode(core::visual::VisualParams::PERSPECTIVE_TYPE);
        else
            setCameraMode(core::visual::VisualParams::ORTHOGRAPHIC_TYPE);
        break;
    }
    case Qt::Key_Shift:
    {
        if (!getPickHandler()) break;
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT,viewport);
        getPickHandler()->activateRay(viewport[2],viewport[3], groot.get());
        break;
    }
    case Qt::Key_B:
        // --- change background
    {
        _background = (_background + 1) % 3;
        break;
    }
    case Qt::Key_R:
        // --- draw axis
    {
        _axis = !_axis;
        break;
    }
    case Qt::Key_S:
    {
        screenshot(capture.findFilename());
        break;
    }
    case Qt::Key_V:
        // --- save video
    {
        if(!_video)
        {
            switch (SofaVideoRecorderManager::getInstance()->getRecordingType())
            {
            case SofaVideoRecorderManager::SCREENSHOTS :
                break;
            case SofaVideoRecorderManager::MOVIE :
            {
#ifdef SOFA_HAVE_FFMPEG
                SofaVideoRecorderManager* videoManager = SofaVideoRecorderManager::getInstance();
                unsigned int bitrate = videoManager->getBitrate();
                unsigned int framerate = videoManager->getFramerate();
                std::string videoFilename = videoRecorder.findFilename(videoManager->getCodecExtension());
                videoRecorder.init( videoFilename, framerate, bitrate, videoManager->getCodecName());
#endif

                break;
            }
            default :
                break;
            }
            if (SofaVideoRecorderManager::getInstance()->realtime())
            {
                unsigned int framerate = SofaVideoRecorderManager::getInstance()->getFramerate();
                std::cout << "Starting capture timer ( " << framerate << " Hz )" << std::endl;
                unsigned int interv = (1000+framerate-1)/framerate;
                captureTimer.start(interv);
            }

        }
        else
        {
            if(captureTimer.isActive())
            {
                std::cout << "Stopping capture timer" << std::endl;
                captureTimer.stop();
            }
            switch (SofaVideoRecorderManager::getInstance()->getRecordingType())
            {
            case SofaVideoRecorderManager::SCREENSHOTS :
                break;
            case SofaVideoRecorderManager::MOVIE :
            {
#ifdef SOFA_HAVE_FFMPEG
                videoRecorder.finishVideo();
#endif //SOFA_HAVE_FFMPEG
                break;
            }
            default :
                break;
            }
        }

        _video = !_video;
        //capture.setCounter();

        break;
    }
    case Qt::Key_W:
        // --- save current view
    {
        if (!currentCamera) break;
        saveView();
        break;
    }
    case Qt::Key_F1:
        // --- enable stereo mode
    {
        _stereoEnabled = !_stereoEnabled;
        std::cout << "Stereoscopic View " << (_stereoEnabled ? "Enabled" : "Disabled") << std::endl;
        break;
    }
    case Qt::Key_F2:
        // --- reduce shift distance or eye offset (if Shift is pressed)
    {
        if (e->state() & Qt::ShiftButton)
        {
            _stereoEyeOffset -= 0.01;
            std::cout << "Stereo eye offset = " << _stereoEyeOffset << std::endl;
        }
        else
        {
            _stereoShift -= 0.1;
            std::cout << "Stereo separation = " << _stereoShift << std::endl;
        }
        break;
    }
    case Qt::Key_F3:
        // --- increase shift distance or eye offset (if Shift is pressed)
    {
        if (e->state() & Qt::ShiftButton)
        {
            _stereoEyeOffset += 0.01;
            std::cout << "Stereo eye offset = " << _stereoEyeOffset << std::endl;
        }
        else
        {
            _stereoShift += 0.1;
            std::cout << "Stereo separation = " << _stereoShift << std::endl;
        }
        break;
    }
    case Qt::Key_F4:
        // --- reset shift distance or eye offset (if Shift is pressed)
    {
        if (e->state() & Qt::ShiftButton)
        {
            _stereoEyeOffset = 0.0;
            std::cout << "Stereo eye offset = " << _stereoEyeOffset << std::endl;
        }
        else
        {
            _stereoShift = 1.0;
            std::cout << "Stereo separation = " << _stereoShift << std::endl;
        }
        break;
    }
    case Qt::Key_F5:
        // --- enable binocular mode
    {
        _stereoMode = (StereoMode)(((int)_stereoMode+1)%(int)NB_STEREO_MODES);
        switch (_stereoMode)
        {
        case STEREO_INTERLACED:
            std::cout << "Stereo mode: Interlaced" << std::endl;
            break;
        case STEREO_SIDE_BY_SIDE:
            std::cout << "Stereo mode: Side by Side" << std::endl; break;
        case STEREO_SIDE_BY_SIDE_HALF:
            std::cout << "Stereo mode: Side by Side Half" << std::endl; break;
        case STEREO_FRAME_PACKING:
            std::cout << "Stereo mode: Frame Packing" << std::endl; break;
        case STEREO_TOP_BOTTOM:
            std::cout << "Stereo mode: Top Bottom" << std::endl; break;
        case STEREO_TOP_BOTTOM_HALF:
            std::cout << "Stereo mode: Top Bottom Half" << std::endl; break;
        case STEREO_AUTO:
            std::cout << "Stereo mode: Automatic" << std::endl; break;
        case STEREO_NONE:
            std::cout << "Stereo mode: None" << std::endl; break;
        default:
            std::cout << "Stereo mode: INVALID" << std::endl; break;
        }
        break;
    }
    case Qt::Key_Control:
    {
        m_isControlPressed = true;
        //cerr<<"QtViewer::keyPressEvent, CONTROL pressed"<<endl;
        break;
    }
    default:
    {
        e->ignore();
    }
    }
}

void SofaViewer::keyReleaseEvent(QKeyEvent * e)
{
    sofa::core::objectmodel::KeyreleasedEvent kre(e->key());
    currentCamera->manageEvent(&kre);

    switch (e->key())
    {
    case Qt::Key_Shift:
    {
        if (getPickHandler())
            getPickHandler()->deactivateRay();

        break;
    }
    case Qt::Key_Control:
    {
        m_isControlPressed = false;

        // Send Control Release Info to a potential ArticulatedRigid Instrument
        sofa::core::objectmodel::MouseEvent mouseEvent(
            sofa::core::objectmodel::MouseEvent::Reset);
        if (groot)
            groot->propagateEvent(core::ExecParams::defaultInstance(), &mouseEvent);
        break;
    }
    default:
    {
        e->ignore();
    }
    }

    if (isControlPressed())
    {
        sofa::core::objectmodel::KeyreleasedEvent keyEvent(e->key());
        if (groot)
            groot->propagateEvent(core::ExecParams::defaultInstance(), &keyEvent);
    }

}

bool SofaViewer::isControlPressed() const
{
    return m_isControlPressed;
}

// ---------------------- Here are the Mouse controls   ----------------------
void SofaViewer::wheelEvent(QWheelEvent *e)
{
    if (!currentCamera) return;
    //<CAMERA API>
    sofa::core::objectmodel::MouseEvent me(sofa::core::objectmodel::MouseEvent::Wheel,e->delta());
    currentCamera->manageEvent(&me);

    getQWidget()->update();
#ifndef SOFA_GUI_INTERACTION
    if (groot)
        groot->propagateEvent(core::ExecParams::defaultInstance(), &me);
#endif
}

void SofaViewer::mouseMoveEvent ( QMouseEvent *e )
{
    if (!currentCamera) return;
    //<CAMERA API>
    sofa::core::objectmodel::MouseEvent me(sofa::core::objectmodel::MouseEvent::Move,e->x(), e->y());
    currentCamera->manageEvent(&me);

    getQWidget()->update();
#ifndef SOFA_GUI_INTERACTION
    if (groot)
        groot->propagateEvent(core::ExecParams::defaultInstance(), &me);
#endif
}

void SofaViewer::mousePressEvent ( QMouseEvent * e)
{
    if (!currentCamera) return;
    //<CAMERA API>
    sofa::core::objectmodel::MouseEvent::State mState;

    if (e->state() & Qt::ShiftButton)
    {
        switch (e->button())
        {
        case Qt::LeftButton:
            mState = sofa::core::objectmodel::MouseEvent::LeftPressedAction;
            break;
        case Qt::RightButton:
            mState = sofa::core::objectmodel::MouseEvent::RightPressedAction;
            break;
        case Qt::MidButton:
            mState = sofa::core::objectmodel::MouseEvent::MiddlePressedAction;
            break;
        default:
            mState = sofa::core::objectmodel::MouseEvent::AnyExtraButtonPressedAction;
            break;
        }
    }
    else
    {
        switch (e->button())
        {
        case Qt::LeftButton:
            mState = sofa::core::objectmodel::MouseEvent::LeftPressed;
            break;
        case Qt::RightButton:
            mState = sofa::core::objectmodel::MouseEvent::RightPressed;
            break;
        case Qt::MidButton:
            mState = sofa::core::objectmodel::MouseEvent::MiddlePressed;
            break;
        default:
            mState = sofa::core::objectmodel::MouseEvent::AnyExtraButtonPressed;
            break;
        }
    }
    sofa::core::objectmodel::MouseEvent* const mEvent = new sofa::core::objectmodel::MouseEvent(mState, e->x(), e->y());
    currentCamera->manageEvent(mEvent);

    getQWidget()->update();
#ifndef SOFA_GUI_INTERACTION
    if (groot)
        groot->propagateEvent(core::ExecParams::defaultInstance(), mEvent);
#endif
}

void SofaViewer::mouseReleaseEvent ( QMouseEvent * e)
{
    if (!currentCamera) return;
    //<CAMERA API>
    sofa::core::objectmodel::MouseEvent::State mState;
    if (e->state() & Qt::ShiftButton)
    {
        switch (e->button())
        {
        case Qt::LeftButton:
            mState = sofa::core::objectmodel::MouseEvent::LeftReleasedAction;
            break;
        case Qt::RightButton:
            mState = sofa::core::objectmodel::MouseEvent::RightReleasedAction;
            break;
        case Qt::MidButton:
            mState = sofa::core::objectmodel::MouseEvent::MiddleReleasedAction;
            break;
        default:
            mState = sofa::core::objectmodel::MouseEvent::AnyExtraButtonReleasedAction;
            break;
        }
    }
    else
    {
        switch (e->button())
        {
        case Qt::LeftButton:
            mState = sofa::core::objectmodel::MouseEvent::LeftReleased;
            break;
        case Qt::RightButton:
            mState = sofa::core::objectmodel::MouseEvent::RightReleased;
            break;
        case Qt::MidButton:
            mState = sofa::core::objectmodel::MouseEvent::MiddleReleased;
            break;
        default:
            mState = sofa::core::objectmodel::MouseEvent::AnyExtraButtonReleased;
            break;
        }
    }
    sofa::core::objectmodel::MouseEvent* const mEvent = new sofa::core::objectmodel::MouseEvent(mState, e->x(), e->y());
    currentCamera->manageEvent(mEvent);

    getQWidget()->update();
#ifndef SOFA_GUI_INTERACTION
    if (groot)
        groot->propagateEvent(core::ExecParams::defaultInstance(), mEvent);
#endif
}

bool SofaViewer::mouseEvent(QMouseEvent *e)
{
    if (!currentCamera) return true;
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT,viewport);

    MousePosition mousepos;
    mousepos.screenWidth  = viewport[2];
    mousepos.screenHeight = viewport[3];
    mousepos.x      = e->x();
    mousepos.y      = e->y();

    if (e->state() & Qt::ShiftButton)
    {

        getPickHandler()->activateRay(viewport[2],viewport[3], groot.get());
        getPickHandler()->updateMouse2D( mousepos );

        //_sceneTransform.ApplyInverse();
        switch (e->type())
        {
        case QEvent::MouseButtonPress:

            if (e->button() == Qt::LeftButton)
            {
                getPickHandler()->handleMouseEvent(PRESSED, LEFT);
            }
            else if (e->button() == Qt::RightButton) // Shift+Rightclick to remove triangles
            {
                getPickHandler()->handleMouseEvent(PRESSED, RIGHT);
            }
            else if (e->button() == Qt::MidButton) // Shift+Midclick (by 2 steps defining 2 input points) to cut from one point to another
            {
                getPickHandler()->handleMouseEvent(PRESSED, MIDDLE);
            }
            break;
        case QEvent::MouseButtonRelease:
            //if (e->button() == Qt::LeftButton)
        {

            if (e->button() == Qt::LeftButton)
            {
                getPickHandler()->handleMouseEvent(RELEASED, LEFT);
            }
            else if (e->button() == Qt::RightButton)
            {
                getPickHandler()->handleMouseEvent(RELEASED, RIGHT);
            }
            else if (e->button() == Qt::MidButton)
            {
                getPickHandler()->handleMouseEvent(RELEASED, MIDDLE);
            }
        }
        break;
        default:
            break;
        }
        moveRayPickInteractor(e->x(), e->y());
    }
    else
    {
        getPickHandler()->activateRay(viewport[2],viewport[3], groot.get());
    }
    return true;
}

void SofaViewer::captureEvent()
{
    if (!_videoPrefixLink.empty())
    {
        Node* root = this->getScene();
        sofa::core::objectmodel::BaseData* ptr = NULL;
        sofa::core::objectmodel::BaseLink* lptr = NULL;
        if (root->findDataLinkDest(ptr, _videoPrefixLink, lptr))
        {
            std::string value = ptr->getValueString();
            if (value.empty())
            {
                if (captureTimer.isActive())
                {
                    captureTimer.stop();
                    _video = false;
                }
            }
            else
            {
                if (capture.getPrefix() != value)
                {
                    capture.setPrefix(value);
                    capture.setCounter();
                    _video = true;
                    captureTimer.start();
                }
            }
        }
        else
        {
            std::cerr<<"Data link not found "<<std::endl;
            _videoPrefixLink.clear();
        }

    }
    if (_video)
    {
        bool skip = false;
        unsigned int frameskip = SofaVideoRecorderManager::getInstance()->getFrameskip();
        unsigned int frameskip0 = SofaVideoRecorderManager::getInstance()->getFrameskip0();
        if (frameskip0)
        {
            SofaVideoRecorderManager::getInstance()->setFrameskip0(frameskip0-1);
            skip = true;
        }
        else if (frameskip)
        {
            static unsigned int skipcounter = 0;
            if (skipcounter < frameskip)
            {
                skip = true;
                ++skipcounter;
            }
            else
            {
                skip = false;
                skipcounter = 0;
            }
        }
        if (!skip)
        {
            switch (SofaVideoRecorderManager::getInstance()->getRecordingType())
            {
            case SofaVideoRecorderManager::SCREENSHOTS :
                screenshot(capture.findFilename(), 1, false);
                break;
            case SofaVideoRecorderManager::MOVIE :
#ifdef SOFA_HAVE_FFMPEG
                videoRecorder.addFrame();
#endif //SOFA_HAVE_FFMPEG
                break;
            default :
                break;
            }
        }
    }
}


}
}
}
}
