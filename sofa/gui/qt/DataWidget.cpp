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
#define SOFA_GUI_QT_DATAWIDGET_CPP

#include "DataWidget.h"
#include "ModifyObject.h"
#include <sofa/helper/Factory.inl>

#ifdef SOFA_QT4
#include <QToolTip>
#else
#include <qtooltip.h>
#endif

#define SIZE_TEXT     60
namespace sofa
{
namespace helper
{
template class SOFA_SOFAGUIQT_API Factory<std::string, sofa::gui::qt::DataWidget, sofa::gui::qt::DataWidget::CreatorArgument>;
}

namespace gui
{
namespace qt
{

using namespace core::objectmodel;

DataWidget::DataWidget(QWidget* parent,const char* name, MyData* d) 
:QWidget(parent,name), baseData(d), readOnly(false), dirty(false), counter(-1)
{
      if (d) readOnly = d->isReadOnly();
}

DataWidget::~DataWidget()
{
}

void 
DataWidget::setData( MyData* d)
{
    baseData = d;
    if (d) readOnly = d->isReadOnly();
    readFromData();
}

void 
DataWidget::setReadOnly( bool b )
{
    readOnly = b;
    setDataReadOnly(b);
}

void
DataWidget::updateVisibility()
{
    parentWidget()->setShown(baseData->isDisplayed());
}

void
DataWidget::updateDataValue()
{
    if (!isDirty())
    {
        dirty = false;
    }
    else
    {
        const bool hasOwner = baseData->getOwner();
        std::string previousName;
        if ( hasOwner ) previousName = baseData->getOwner()->getName();
        writeToData();

        if (hasOwner)
        {
            std::string path;
            BaseNode* ownerAsNode = BaseNode::DynamicCast(baseData->getOwner() );
            BaseObject* ownerAsObject = BaseObject::DynamicCast(baseData->getOwner() );

            if (ownerAsNode)
            {
                path = ownerAsNode->getPathName() + "." + baseData->getName();
            }
            else if (ownerAsObject)
            {
                std::string objectPath = ownerAsObject->getName();
                sofa::core::objectmodel::BaseObject* master = ownerAsObject->getMaster();
                while (master)
                {
                    objectPath = master->getName() + "/" + objectPath;
                    master = master->getMaster();
                }
                BaseNode* n = BaseNode::DynamicCast(ownerAsObject->getContext());
                if (n)
                {
                    path = n->getPathName() + std::string("/") + objectPath + std::string(".") + baseData->getName(); // TODO: compute relative path
                }
                else
                {
                    if (ownerAsObject->getContext())
                    {
                        std::cout << __FUNCTION__ << ": " << ownerAsObject->getContext()->getName() << std::endl; 
                    }
                    else
                    {
                        std::cerr << __FUNCTION__ << ": NULL context for data " << baseData->getName() << std::endl;  
                    }
                    path = objectPath + "." + baseData->getName();
                }
            }
            else
            {
                std::cerr << __FUNCTION__ << " " << __LINE__ << " something went awfully wrong..." << std::endl;
            }
            std::string dataString = (path + " = " + baseData->getValueString());
            baseData->getOwner()->sout << dataString << baseData->getOwner()->sendl;
            const QString dataQString = dataString.c_str();
            Q_EMIT dataValueChanged(dataQString);
        }

        updateVisibility();
        
        dirty = false;
        counter = baseData->getCounter();

        if(hasOwner && baseData->getOwner()->getName() != previousName)
        {
            Q_EMIT DataOwnerDirty(true);
        }
    }

    dirty = false;
    counter = baseData->getCounter();

}

void
DataWidget::updateWidgetValue()
{
    if(!isDirty())
    {
        if(counter != baseData->getCounter())
        {
            readFromData();
            this->update();
        }
    }
}

void
DataWidget::setWidgetDirty(bool b)
{
    dirty = b;
    Q_EMIT WidgetDirty(b);
}
/*QDisplayDataInfoWidget definitions */

QDisplayDataInfoWidget::QDisplayDataInfoWidget(QWidget* parent, const std::string& helper,
        core::objectmodel::BaseData* d, bool modifiable):QWidget(parent), data(d), numLines_(1)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
#ifdef SOFA_QT4
    layout->setContentsMargins(0,0,0,0);
#endif
    std::string final_str;
    formatHelperString(helper,final_str);
    std::string ownerClass=data->getOwnerClass();
    if (modifiable)
    {
        QPushButton *helper_button = new QPushButton(QString(final_str.c_str()),this);
        // helper_button ->setFlat(true);
        helper_button ->setAutoDefault(false);
        layout->addWidget(helper_button);
        connect(helper_button, SIGNAL( clicked() ), this, SLOT( linkModification()));
        if (!ownerClass.empty()) QToolTip::add(helper_button, ("Data from "+ownerClass).c_str());
    }
    else
    {
#ifndef SOFA_GUI_QT_NO_DATA_HELP
        QLabel* helper_label = new QLabel(this);
        helper_label->setText(QString(final_str.c_str()));
        helper_label->setMinimumWidth(20);
        layout->addWidget(helper_label);
        if (!ownerClass.empty()) QToolTip::add(helper_label, ("Data from "+ownerClass).c_str());
#else
        numLines_ = 0;
        if (!final_str.empty() || !ownerClass.empty())
        {
            if (!final_str.empty()) final_str += '\n';
            final_str += "Data from ";
            final_str += ownerClass;
            QToolTip::add(parent, final_str.c_str());
        }
#endif
    }
    if(modifiable || !data->getLinkPath().empty())
    {
        linkpath_edit = new QLineEdit(this);
        linkpath_edit->setText(QString(data->getLinkPath().c_str()));
        linkpath_edit->setReadOnly(!modifiable);
        layout->addWidget(linkpath_edit);
        linkpath_edit->setShown(!data->getLinkPath().empty());
        connect(linkpath_edit, SIGNAL( lostFocus()), this, SLOT( linkEdited()));
    }
    else
    {
        linkpath_edit=NULL;
    }
}

void QDisplayDataInfoWidget::linkModification()
{
    if (linkpath_edit->isShown() && linkpath_edit->text().isEmpty())
        linkpath_edit->setShown(false);
    else
    {
        linkpath_edit->setShown(true);
        //Open a dialog window to let the user select the data he wants to link
    }
}
void QDisplayDataInfoWidget::linkEdited()
{
    std::cerr << "linkEdited " << linkpath_edit->text().ascii() << std::endl;
    data->setParent(linkpath_edit->text().ascii() );
}

void QDisplayDataInfoWidget::formatHelperString(const std::string& helper, std::string& final_text)
{
    std::string label_text=helper;
    numLines_ = 0;
    while (!label_text.empty())
    {
        std::string::size_type pos = label_text.find('\n');
        std::string current_sentence;
        if (pos != std::string::npos)
            current_sentence  = label_text.substr(0,pos+1);
        else
            current_sentence = label_text;
        if (current_sentence.size() > SIZE_TEXT)
        {
            unsigned int index_cut;
            unsigned int cut = current_sentence.size()/SIZE_TEXT;
            for (index_cut=1; index_cut<=cut; index_cut++)
            {
                std::string::size_type numero_char=current_sentence.rfind(' ',SIZE_TEXT*index_cut);
                current_sentence = current_sentence.insert(numero_char+1,1,'\n');
                numLines_++;
            }
        }
        if (pos != std::string::npos) label_text = label_text.substr(pos+1);
        else label_text = "";
        final_text += current_sentence;
        numLines_++;
    }
}

unsigned int QDisplayDataInfoWidget::numLines(const std::string& str)
{
    std::string::size_type newline_pos;
    unsigned int numlines = 1;
    newline_pos = str.find('\n',0);
    while( newline_pos != std::string::npos )
    {
        numlines++;
        newline_pos = str.find('\n',newline_pos+1);
    }
    return numlines;
}


/* QPushButtonUpdater definitions */

void QPushButtonUpdater::setDisplayed(bool b)
{

    if (b)
    {
        this->setText(QString("Hide the values"));
    }
    else
    {
        this->setText(QString("Display the values"));
    }

}






}//qt
}//gui
}//sofa
