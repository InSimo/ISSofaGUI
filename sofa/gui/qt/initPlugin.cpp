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
#include "initPlugin.h"
#include "RealGUI.h"

namespace sofa
{
namespace gui
{
namespace qt
{

    //Here are just several convenient functions to help user to know what contains the plugin


    void initExternalModule()
    {
        static bool first = true;
        if (first)
        {
            first = false;
            RealGUI::InitGUI();
        }

    }

    const char* getModuleName()
    {
        return "SofaGUIQt";
    }

    const char* getModuleVersion()
    {
        return "0.0";
    }

    const char* getModuleLicense()
    {
        return "GPL";
    }


    const char* getModuleDescription()
    {
        return "GUI Plugin: Qt";
    }

    const char* getModuleComponentList()
    {
        return "";
    }

} // namespace qt

} // namespace gui

} // namespace sofa
