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
#include "QDataDescriptionWidget.h"

#include <sofa/core/ObjectFactory.h>
#include <sofa/helper/system/SetDirectory.h>

#ifdef SOFA_QT4
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <Q3GroupBox>
#include <QLabel>
#else
#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#endif



namespace sofa
{

namespace gui
{
namespace qt
{
QDataDescriptionWidget::QDataDescriptionWidget(QWidget* parent, core::objectmodel::Base* object):QWidget(parent)
{

    QVBoxLayout* tabLayout = new QVBoxLayout( this, 0, 1, QString("tabInfoLayout"));
    //Instance
    {
        Q3GroupBox *box = new Q3GroupBox(this, QString("Instance"));
        box->setColumns(2);
        box->setTitle(QString("Instance"));
        new QLabel(QString("Name"), box);
        new QLabel(QString(object->getName().c_str()), box);
        new QLabel(QString("Class"), box);
        new QLabel(QString(object->getClassName().c_str()), box);
        std::string namespacename = core::objectmodel::BaseClass::decodeNamespaceName(typeid(*object));
        if (!namespacename.empty())
        {
            new QLabel(QString("Namespace"), box);
            (new QLabel(QString(namespacename.c_str()), box))->setMinimumWidth(20);
        }
        if (!object->getTemplateName().empty())
        {
            new QLabel(QString("Template"), box);
            (new QLabel(QString(object->getTemplateName().c_str()), box))->setMinimumWidth(20);
        }
        if (object->getNbSourceFiles()>0)
        {
            for (std::size_t i = 0; i < object->getNbSourceFiles(); ++i)
            {
                new QLabel(QString("From"), box);
                std::string path = object->getSourceFileName(i);
                std::string abspath;
                std::string currentdir = sofa::helper::system::SetDirectory::GetCurrentDir();
                if (sofa::helper::system::SetDirectory::IsAbsolute(path))
                {
                    abspath = path;
                    if (path.substr(0,currentdir.length()) == currentdir)
                    {
                        path = path.substr(currentdir.length()+1);
                    }
                }
                else
                {
                    abspath = sofa::helper::system::SetDirectory::GetRelativeFromDir(path.c_str(), currentdir.c_str());
                }
                std::string suffix;
                std::pair<int,int> pos = object->getSourceFilePos(i);
                if (pos.first || pos.second)
                {
                    suffix += ':';
                    suffix += std::to_string(pos.first);
                    if (pos.second)
                    {
                        suffix += ':';
                        suffix += std::to_string(pos.second);
                    }
                }
                QLabel* txtLab = new QLabel(QString(""), box);
                // Doc on vscode file url with line numbers: https://code.visualstudio.com/docs/editor/command-line#_opening-vs-code-with-urls
                txtLab->setText("<a href=\"file:///" + QString(abspath.c_str()) + "\"> "+ QString(path.c_str())
                    + "</a> <a href=\"vscode://file/" + QString(abspath.c_str()) + QString(suffix.c_str()) + "\">"+ "L" + QString(suffix.c_str()) + " (vscode)</a>");
                txtLab->setOpenExternalLinks(true);
                txtLab->setMinimumWidth(20);
            }
        }

        tabLayout->addWidget( box );
    }


    //Class description
    core::ObjectFactory::ClassEntry entry = core::ObjectFactory::getInstance()->getEntry(object->getClassName());
    if (! entry.creatorMap.empty())
    {
        Q3GroupBox *box = new Q3GroupBox(this, QString("Class"));
        box->setColumns(2);
        box->setTitle(QString("Class"));
        if (!entry.description.empty() && entry.description != std::string("TODO"))
        {
            new QLabel(QString("Description"), box);
            (new QLabel(QString(entry.description.c_str()), box))->setMinimumWidth(20);
        }
        core::ObjectFactory::CreatorMap::iterator it = entry.creatorMap.find(object->getTemplateName());
        if (it != entry.creatorMap.end() && *it->second->getTarget())
        {
            new QLabel(QString("Provided by"), box);
            (new QLabel(QString(it->second->getTarget()), box))->setMinimumWidth(20);
        }

        if (!entry.authors.empty() && entry.authors != std::string("TODO"))
        {
            new QLabel(QString("Authors"), box);
            (new QLabel(QString(entry.authors.c_str()), box))->setMinimumWidth(20);
        }
        if (!entry.license.empty() && entry.license != std::string("TODO"))
        {
            new QLabel(QString("License"), box);
            (new QLabel(QString(entry.license.c_str()), box))->setMinimumWidth(20);
        }
        tabLayout->addWidget( box );
    }

    tabLayout->addStretch();
}



} // qt
} //gui
} //sofa

