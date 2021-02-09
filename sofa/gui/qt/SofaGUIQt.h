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
#ifndef SOFA_GUI_QT_SOFAGUIQT_H
#define SOFA_GUI_QT_SOFAGUIQT_H

#include <sofa/helper/system/config.h>

#ifdef SOFA_BUILD_SOFAGUIQT
#	define SOFA_SOFAGUIQT_API SOFA_EXPORT_DYNAMIC_LIBRARY
#else
#	define SOFA_SOFAGUIQT_API SOFA_IMPORT_DYNAMIC_LIBRARY
#endif

// uncomment to enable scrollbars for data widget dialogs (fix issues with unresizeable very long window, but add other issues with initial sizes)
//#define SOFAGUIQT_SCROLL

// uncomment to activate more compact layout of data widget dialogs (help displayed as tooltip by default, smaller margins)
#define SOFAGUIQT_COMPACT

#endif
