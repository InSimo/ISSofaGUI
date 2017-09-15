/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 RC 1        *
*                (c) 2006-2011 INRIA, USTL, UJF, CNRS, MGH                    *
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
#ifndef SOFA_GUI_BATCHGUI_H
#define SOFA_GUI_BATCHGUI_H

#include "BaseGUI.h"
#include <sofa/simulation/common/Node.h>
#include <chrono>


#ifdef SOFA_BUILD_SOFAGUIBATCH
#	define SOFA_SOFAGUIBATCH_API SOFA_EXPORT_DYNAMIC_LIBRARY
#else
#	define SOFA_SOFAGUIBATCH_API SOFA_IMPORT_DYNAMIC_LIBRARY
#endif

namespace sofa
{

namespace gui
{

class SOFA_SOFAGUIBATCH_API BatchGUI : public BaseGUI
{

public:

    /// @name methods each GUI must implement
    /// @{

    explicit BatchGUI(const sofa::simulation::gui::BaseGUIArgument* a);

    void setScene(sofa::simulation::Node::SPtr groot, const char* filename="", bool temporaryFile=false) override;

    void resetScene();

    int mainLoop() override;
    bool step();
    void redraw();

    static void setNumIterations(unsigned int n) {m_nbIter=n;};
    sofa::simulation::Node* getCurrentSimulation() override;
    
    void initialize() override;


    /// @}

    /// @name registration of each GUI
    /// @{
    int initGUI();
    static BatchGUI* CreateGUI(const sofa::simulation::gui::BaseGUIArgument* a);

    static const unsigned int DEFAULT_NUMBER_OF_ITERATIONS;
    /// @}

protected:
    /// The destructor should not be called directly. Use the closeGUI() method instead.
    ~BatchGUI();

    void startDumpVisitor();
    void stopDumpVisitor();

    void saveStepDurationLog(const sofa::helper::vector<std::chrono::duration<double, std::milli>>& stepDurationVec) const;

    std::ostringstream m_dumpVisitorStream;

    sofa::simulation::Node::SPtr m_groot;
    std::string m_filename;
    static unsigned int m_nbIter;
    static bool m_exitWhenPaused;
    static double m_idleEventFreq;

    std::chrono::high_resolution_clock::time_point m_lastIdleEventTime;
    static bool m_logStepDuration;
};

} // namespace gui

} // namespace sofa

#endif
