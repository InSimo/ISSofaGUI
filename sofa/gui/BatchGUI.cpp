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
#include "BatchGUI.h"
#include <sofa/simulation/common/Simulation.h>
#include <sofa/simulation/common/UpdateContextVisitor.h>
#include <sofa/simulation/common/GUIFactory.h>
#include <sofa/core/objectmodel/IdleEvent.h>
#ifdef SOFA_SMP
#include <athapascan-1>
#endif
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>

namespace sofa
{

namespace gui
{

sofa::helper::Creator<sofa::simulation::gui::GUIFactory,BatchGUI> creatorBatchGUI("batch", false, -1, "SofaGUI Batch GUI", {"SofaGuiBatch"});

const unsigned int BatchGUI::DEFAULT_NUMBER_OF_ITERATIONS = 0;
unsigned int BatchGUI::m_nbIter = BatchGUI::DEFAULT_NUMBER_OF_ITERATIONS;
bool BatchGUI::m_exitWhenPaused = false;
double BatchGUI::m_idleEventFreq = 100; //Hz
bool BatchGUI::m_logStepDuration = false;

BatchGUI::BatchGUI(const sofa::simulation::gui::BaseGUIArgument* a)
: BaseGUI(a)
, m_groot(NULL)
{
}

BatchGUI::~BatchGUI()
{
}

BatchGUI* BatchGUI::CreateGUI(const sofa::simulation::gui::BaseGUIArgument* a)
{
    return new BatchGUI(a);
}

int BatchGUI::mainLoop()
{
    if (m_groot)
    {
        // It makes no sense to start without animating in this GUI, so animate by default
        m_groot->setAnimate(true);

        // Note: As no visualization is done by the Batch GUI, calling updateVisual() is not necessary if nothing else needs the VisualModels to be updated.
        if (m_nbIter != 0) // benchmark mode
        {
            typedef std::chrono::steady_clock::time_point time_point;
            sofa::simulation::getSimulation()->animate(m_groot.get());
            sofa::simulation::getSimulation()->updateVisual(m_groot.get());

            std::cout << "Computing " << m_nbIter << " iterations." << std::endl;

            if (!m_logStepDuration)
            {
                const time_point startT = std::chrono::steady_clock::now();

                for (unsigned int i = 0; i < m_nbIter; ++i)
                {
                    if (!step())
                    {
                        break;
                    }
                }
                const std::chrono::duration<double> duration = std::chrono::steady_clock::now() - startT;

                std::cout << m_nbIter << " iterations done in " << duration.count() << " s ( " << m_nbIter / duration.count() << " FPS)." << std::endl;
            }
            else
            {
                sofa::helper::vector< std::chrono::duration<double, std::milli> > stepDurationVec;
                stepDurationVec.reserve(m_nbIter);
                time_point previousT = std::chrono::steady_clock::now();

                for (unsigned int i = 0; i < m_nbIter; ++i)
                {
                    if (!step())
                    {
                        break;
                    }
                    const time_point currentT = std::chrono::steady_clock::now();
                    stepDurationVec.push_back(currentT - previousT);
                    previousT = currentT;
                }

                saveStepDurationLog(stepDurationVec);
            }
        }
        else // daemon-like mode
        {
            while (step()) {}
        }
    }
    return 0;
}

bool BatchGUI::step()
{
    if (m_groot->getAnimate())
    {
        sofa::simulation::getSimulation()->animate(m_groot.get());
        sofa::simulation::getSimulation()->updateVisual(m_groot.get());
    }
    else
    {
        if (m_exitWhenPaused)
        {
            return false;
        }

        // Yield to avoid unnecessary cpu work
        if (std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - m_lastIdleEventTime).count() < 1. / m_idleEventFreq)
        {
            std::this_thread::yield(); // must yield instead of sleep in case the frequency is very low as not to block unpausing the simulation
            return true;
        }

        // Propagate idle event
        m_lastIdleEventTime = std::chrono::high_resolution_clock::now();
        sofa::core::objectmodel::IdleEvent ie;
        getCurrentSimulation()->propagateEvent(sofa::core::ExecParams::defaultInstance(), &ie);
    }

    return true;
}

void BatchGUI::redraw()
{
}

void BatchGUI::initialize()
{
    initGUI();
}

void BatchGUI::setScene(sofa::simulation::Node::SPtr groot, const char* filename, bool )
{
    this->m_groot = groot;
    this->m_filename = (filename?filename:"");

    resetScene();
}


void BatchGUI::resetScene()
{
    sofa::simulation::Node* root = getCurrentSimulation();

    if ( root )
    {
        root->setTime(0.);
        simulation::getSimulation()->reset ( root );

        sofa::simulation::UpdateSimulationContextVisitor(sofa::core::ExecParams::defaultInstance()).execute(root);
    }
}

void BatchGUI::startDumpVisitor()
{
#ifdef SOFA_DUMP_VISITOR_INFO
    sofa::simulation::Node* root = getCurrentSimulation();
    if (root)
    {
        m_dumpVisitorStream.str("");
        Visitor::startDumpVisitor(&m_dumpVisitorStream, root->getTime());
    }
#endif
}

void BatchGUI::stopDumpVisitor()
{
#ifdef SOFA_DUMP_VISITOR_INFO
    Visitor::stopDumpVisitor();
    m_dumpVisitorStream.flush();
    m_dumpVisitorStream.str("");
#endif
}

void BatchGUI::saveStepDurationLog(const sofa::helper::vector<std::chrono::duration<double, std::milli>>& stepDurationVec) const
{
    std::ofstream stepDurationFile;
    const time_t timeNow = time(0);
    struct tm * timeInfo = localtime(&timeNow);
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << (1900 + timeInfo->tm_year) << "_"
        << std::setw(2) << (timeInfo->tm_mon + 1) << "_"  // Month
        << std::setw(2) << timeInfo->tm_mday << "__" // Day
        << std::setw(2) << timeInfo->tm_hour << "_" // Hours
        << std::setw(2) << timeInfo->tm_min << "_"  // Minutes
        << std::setw(2) << timeInfo->tm_sec;        // Seconds
    const std::string fileName = "StepDurationLog_" + oss.str() + ".txt";
    stepDurationFile.open(fileName);
    if (stepDurationFile.is_open())
    {
        stepDurationFile.clear();
        for (const auto& stepDuration : stepDurationVec)
        {
            stepDurationFile << stepDuration.count() << std::endl;
        }
        stepDurationFile.close();
        std::cout << "File " << fileName << " generated.\n";
    }
    else
    {
        std::cerr << "Can't create " << fileName << " file.\n";
    }
}

sofa::simulation::Node* BatchGUI::getCurrentSimulation()
{
    return m_groot.get();
}


int BatchGUI::initGUI()
{
    auto& guiOptions = this->getGUIOptions();
    //parse options
    for (unsigned int i=0 ; i<guiOptions.size() ; i++)
    {
        size_t cursor = 0;
        std::string opt = guiOptions[i];
        std::istringstream iss;
        if ((cursor = opt.find("nbIterations=")) != std::string::npos)
        {
            //Set number of iterations
            //(option = "nbIterations=N where N is the number of iterations)
            unsigned int nbIterations;
            std::istringstream iss;
            iss.str(opt.substr(cursor+std::string("nbIterations=").length(), std::string::npos));
            iss >> nbIterations;
            setNumIterations(nbIterations);
        }
        else if (opt.find("logStepDuration") != std::string::npos)
        {
            m_logStepDuration = true;
        }
        else if ((cursor = opt.find("exitWhenPaused")) != std::string::npos)
        {
            m_exitWhenPaused = true;
        }
        else if ((cursor = opt.find("idleEventFreq=")) != std::string::npos)
        {
            iss.str(opt.substr(cursor + std::string("idleEventFreq=").length(), std::string::npos));
            iss >> m_idleEventFreq;
        }

    }
    return 0;
}

} // namespace gui

} // namespace sofa
