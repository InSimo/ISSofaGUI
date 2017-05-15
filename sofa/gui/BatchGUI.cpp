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
#ifdef SOFA_SMP
#include <athapascan-1>
#endif
#include <iostream>
#include <iomanip>
#include <sstream>

namespace sofa
{

namespace gui
{

const unsigned int BatchGUI::DEFAULT_NUMBER_OF_ITERATIONS = 1000;
unsigned int BatchGUI::nbIter = BatchGUI::DEFAULT_NUMBER_OF_ITERATIONS;
bool BatchGUI::logStepDuration = false;

BatchGUI::BatchGUI()
    : groot(NULL)
{
}

BatchGUI::~BatchGUI()
{
}

int BatchGUI::mainLoop()
{
    if (groot)
    {
        typedef std::chrono::steady_clock::time_point time_point;
        sofa::simulation::getSimulation()->animate(groot.get());
        //As no visualization is done by the Batch GUI, this line is not necessary.
        sofa::simulation::getSimulation()->updateVisual(groot.get());
        std::cout << "Computing "<<nbIter<<" iterations." << std::endl;

        if (!logStepDuration)
        {
            const time_point startT = std::chrono::steady_clock::now();

            for (unsigned int i = 0; i < nbIter; i++)
            {
                sofa::simulation::getSimulation()->animate(groot.get());
                //As no visualization is done by the Batch GUI, this line is not necessary.
                sofa::simulation::getSimulation()->updateVisual(groot.get());
            }

            const std::chrono::duration<double> duration = std::chrono::steady_clock::now() - startT;

            std::cout << nbIter << " iterations done in " << duration.count() << " s ( " << nbIter / duration.count() << " FPS)." << std::endl;
        }
        else
        {
            sofa::helper::vector< std::chrono::duration<double, std::milli> > stepDurationVec;
            stepDurationVec.reserve(nbIter);
            time_point previousT = std::chrono::steady_clock::now();
            for (unsigned int i = 0; i < nbIter; i++)
            {
                sofa::simulation::getSimulation()->animate(groot.get());
                //As no visualization is done by the Batch GUI, this line is not necessary.
                sofa::simulation::getSimulation()->updateVisual(groot.get());
                const time_point currentT = std::chrono::steady_clock::now();
                stepDurationVec.push_back(currentT - previousT);
                previousT = currentT;
            }

            saveStepDurationLog(stepDurationVec);
        }
    }
    return 0;
}

void BatchGUI::redraw()
{
}

int BatchGUI::closeGUI()
{
    delete this;
    return 0;
}

void BatchGUI::setScene(sofa::simulation::Node::SPtr groot, const char* filename, bool )
{
    this->groot = groot;
    this->filename = (filename?filename:"");

    resetScene();
}


void BatchGUI::resetScene()
{
    sofa::simulation::Node* root = currentSimulation();

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
    sofa::simulation::Node* root = currentSimulation();
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

sofa::simulation::Node* BatchGUI::currentSimulation()
{
    return groot.get();
}


int BatchGUI::InitGUI(const char* /*name*/, const std::vector<std::string>& options)
{
    setNumIterations(DEFAULT_NUMBER_OF_ITERATIONS);

    //parse options
    for (unsigned int i=0 ; i<options.size() ; i++)
    {
        size_t cursor = 0;
        std::string opt = options[i];
        //Set number of iterations
        //(option = "nbIterations=N where N is the number of iterations)
        if ( (cursor = opt.find("nbIterations=")) != std::string::npos )
        {
            unsigned int nbIterations;
            std::istringstream iss;
            iss.str(opt.substr(cursor+std::string("nbIterations=").length(), std::string::npos));
            iss >> nbIterations;
            setNumIterations(nbIterations);
        }
        else if (opt.find("logStepDuration") != std::string::npos)
        {
            logStepDuration = true;
        }
    }
    return 0;
}

BaseGUI* BatchGUI::CreateGUI(const char* name, const std::vector<std::string>& /*options*/, sofa::simulation::Node::SPtr groot, const char* filename)
{
    BatchGUI::mGuiName = name;
    BatchGUI* gui = new BatchGUI();
    gui->setScene(groot, filename);
    return gui;
}

} // namespace gui

} // namespace sofa
