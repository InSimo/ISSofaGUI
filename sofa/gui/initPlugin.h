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
#ifndef SOFA_GUI_BATCH_INITPLUGIN_H
#define SOFA_GUI_BATCH_INITPLUGIN_H

#include "BatchGUI.h"

namespace sofa
{
namespace gui
{
namespace batch
{

extern "C" {
    SOFA_SOFAGUIBATCH_API void initExternalModule();
    SOFA_SOFAGUIBATCH_API const char* getModuleName();
    SOFA_SOFAGUIBATCH_API const char* getModuleVersion();
    SOFA_SOFAGUIBATCH_API const char* getModuleLicense();
    SOFA_SOFAGUIBATCH_API const char* getModuleDescription();
    SOFA_SOFAGUIBATCH_API const char* getModuleComponentList();
}


}

}

}


#endif // SOFA_GUI_BATCH_INITPLUGIN_H
