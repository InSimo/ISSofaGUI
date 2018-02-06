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

#include "ModifyObject.h"
#include "DataWidget.h"
#include "QDisplayDataWidget.h"
#include "QDataDescriptionWidget.h"
#include "QTabulationModifyObject.h"

#include "QTransformationWidget.h"
#ifdef SOFA_HAVE_QWT
#include <sofa/gui/qt/QEnergyStatWidget.h>
#include <sofa/gui/qt/QMomentumStatWidget.h>
#endif


#include <iostream>
#ifdef SOFA_QT4
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <Q3ListView>
#else
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qtextedit.h>
#endif
#include <QScrollArea>

// uncomment to show traces of GUI operations in this file
//#define DEBUG_GUI

namespace sofa
{

namespace gui
{

namespace qt
{


ModifyObject::ModifyObject(
    void *Id,
    Q3ListViewItem* item_clicked,
    QWidget* parent,
    const ModifyObjectFlags& dialogFlags,
    const std::map<core::objectmodel::Base*, Q3ListViewItem* >& items,
    const char* name,
    bool modal, Qt::WFlags f )
    :QDialog(parent, name, modal, f),
     Id_(Id),
     item_(item_clicked),
     items_(items),
     node(NULL),
     data_(NULL),
     dialogFlags_(dialogFlags),
     outputTab(NULL),
     logOutputEdit(NULL),
     warningTab(NULL),
     logWarningEdit(NULL),
     transformation(NULL)
#ifdef SOFA_HAVE_QWT
     ,energy(NULL)
     ,momentum(NULL)
#endif
{
    setCaption(name);
    listview = dynamic_cast<QSofaListView*>(parent);
}

void ModifyObject::createDialog(core::objectmodel::Base* base)
{
    if(base == NULL)
    {
        return;
    }
#ifdef DEBUG_GUI
    std::cout << "GUI: createDialog(" << base->getClassName() << " " << base->getName() << ")" << std::endl;
#endif
#ifdef DEBUG_GUI
    std::cout << "GUI>emit beginObjectModification(" << base->getName() << ")" << std::endl;
#endif
    emit beginObjectModification(base);
#ifdef DEBUG_GUI
    std::cout << "GUI<emit beginObjectModification(" << base->getName() << ")" << std::endl;
#endif
    node = base;
    data_ = NULL;

    //Layout to organize the whole window
    QVBoxLayout *generalLayout = new QVBoxLayout(this, 0, 1, "generalLayout");
    //generalLayout->setDirection(QVBoxLayout::BottomToTop);

    //Tabulation widget
    dialogTab = new QTabWidget(this);
    generalLayout->addWidget(dialogTab);
    connect(dialogTab, SIGNAL( currentChanged( QWidget*)), this, SLOT( updateTables()));

    //bool isNode = (simulation::Node::DynamicCast(node) != NULL);

    buttonUpdate = new QPushButton( this, "buttonUpdate" );
    buttonUpdate->setText("&Update");
    buttonUpdate->setEnabled(false);
    showHelp = new QCheckBox( this, "showHelp" );
    showHelp->setText("&Help");
    dataFilter = new QLineEdit(this, "data filter");

#ifndef SOFAGUIQT_COMPACT
    showHelp->setChecked(true);
#else
    showHelp->setChecked(false);
#endif
    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( tr( "&OK" ) );

    QPushButton *buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setText( tr( "&Cancel" ) );

    // displayWidget
    if (node)
    {
        connect(dataFilter, SIGNAL(returnPressed()), this, SLOT(updateFullDisplay()));

        const sofa::core::objectmodel::Base::VecData& fields = node->getDataFields();
        const sofa::core::objectmodel::Base::VecLink& links = node->getLinks();
        
        std::map< std::string, std::vector<QTabulationModifyObject* > > groupTabulation;

        //If we operate on a Node, we have to ...
#if 0
        if(isNode)
        {
            if (dialogFlags_.REINIT_FLAG)
            {
                //add the widgets to apply some basic transformations
                
                m_tabs.push_back(new QTabulationModifyObject(this,node, item_,1));
                groupTabulation[std::string("Property")].push_back(m_tabs.back());
                connect(m_tabs.back(), SIGNAL(nodeNameModification(simulation::Node *)), this, SIGNAL(nodeNameModification(simulation::Node *)));

                transformation = new QTransformationWidget(m_tabs.back(), QString("Transformation"));
                m_tabs.back()->layout()->add(transformation);
                m_tabs.back()->externalWidgetAddition(transformation->getNumWidgets());
                connect( transformation, SIGNAL(TransformationDirty(bool)), buttonUpdate, SLOT( setEnabled(bool) ) );
                connect( transformation, SIGNAL(TransformationDirty(bool)), this, SIGNAL( componentDirty(bool) ) );
            }
        }
#endif

        std::vector<std::string> tabNames;
        //Put first the Property Tab
        //tabNames.push_back("Property");

        for( sofa::core::objectmodel::Base::VecData::const_iterator it = fields.begin(); it!=fields.end(); ++it)
        {
            core::objectmodel::BaseData* data = *it;
            Q3ListViewItem* componentReference = nullptr;
            if (!data)
            {
                std::cerr << "ERROR: NULL Data in " << node->getName() << std::endl;
                continue;
            }

            if (data->getName().empty()) continue; // ignore unnamed data
            auto links = data->getLinkPath();
            auto parent = data->getParent();
            if (parent)
            {
                for (std::map<core::objectmodel::Base*, Q3ListViewItem* >::const_iterator it2 = items_.begin(); it2 != items_.end(); ++it2)
                {
                    if ((*it2).first == parent->getOwner())
                    {
                        componentReference = (*it2).second;
                    }
                }
            }
            if (!data->getGroup())
            {
                std::cerr << "ERROR: NULL group for Data " << data->getName() << " in " << node->getName() << std::endl;
                continue;
            }

            //For each Data of the current Object
            //We determine where it belongs:
            std::string currentGroup=data->getGroup();

            if (currentGroup.empty()) currentGroup="Property";

#ifdef DEBUG_GUI
            std::cout << "GUI: add Data " << data->getName() << " in " << currentGroup << std::endl;
#endif
            QTabulationModifyObject* currentTab=NULL;
            bool useScroll = (currentGroup[currentGroup.length()-1] == '_');
            std::vector<QTabulationModifyObject* > &tabs=groupTabulation[currentGroup];
            bool newTab = false;
            if (tabs.empty()) tabNames.push_back(currentGroup);
            if (tabs.empty() || (!useScroll && tabs.back()->isFull()))
            {
                newTab = true;
                m_tabs.push_back(new QTabulationModifyObject(this,node, item_,tabs.size()+1));
                tabs.push_back(m_tabs.back());
            }
            currentTab = tabs.back();
            currentTab->addData(data, getFlags(), componentReference, listview);
            if (newTab)
            {
                connect(buttonUpdate,   SIGNAL(clicked() ),          currentTab, SLOT( updateDataValue() ) );
                connect(buttonOk,       SIGNAL(clicked() ),          currentTab, SLOT( updateDataValue() ) );
                connect(this,           SIGNAL(updateDataWidgets()), currentTab, SLOT( updateWidgetValue()) );
                connect(showHelp,       SIGNAL(toggled(bool) ),      currentTab, SLOT( showHelp(bool) ) );

                connect(currentTab, SIGNAL( TabDirty(bool) ), buttonUpdate, SLOT( setEnabled(bool) ) );
                connect(currentTab, SIGNAL( TabDirty(bool) ), this, SIGNAL( componentDirty(bool) ) );
            }

#ifdef DEBUG_GUI
            std::cout << "GUI: added Data " << data->getName() << " in " << currentGroup << std::endl;
#endif
        }
#ifdef DEBUG_GUI
        std::cout << "GUI: end Data" << std::endl;
#endif

        for( sofa::core::objectmodel::Base::VecLink::const_iterator it = links.begin(); it!=links.end(); ++it)
        {
            core::objectmodel::BaseLink* link=*it;
            Q3ListViewItem* componentReference = nullptr;
            for (std::map<core::objectmodel::Base*, Q3ListViewItem* >::const_iterator it2 = items_.begin(); it2 != items_.end(); ++it2)
            {
                if ((*it2).first == link->getLinkedBase()  )
                {
                    componentReference = (*it2).second;
                }
            }
            if (link->getName().empty()) continue; // ignore unnamed links
            if (!link->storePath() && link->getSize() == 0) continue; // ignore empty links

            //For each Link of the current Object
            //We determine where it belongs:
            std::string currentGroup="Links";

#ifdef DEBUG_GUI
            std::cout << "GUI: add Link " << link->getName() << " in " << currentGroup << std::endl;
#endif

            QTabulationModifyObject* currentTab=NULL;

            std::vector<QTabulationModifyObject* > &tabs=groupTabulation[currentGroup];
            if (tabs.empty()) tabNames.push_back(currentGroup);
            if (tabs.empty() || tabs.back()->isFull())
            {
                m_tabs.push_back(new QTabulationModifyObject(this,node, item_,tabs.size()+1));
                tabs.push_back(m_tabs.back() );
            }
            currentTab = tabs.back();

            currentTab->addLink(link, getFlags(), componentReference, listview);
            connect(buttonUpdate,   SIGNAL(clicked() ),          currentTab, SLOT( updateDataValue() ) );
            connect(buttonOk,       SIGNAL(clicked() ),          currentTab, SLOT( updateDataValue() ) );
            connect(this,           SIGNAL(updateDataWidgets()), currentTab, SLOT( updateWidgetValue()) );
            connect(showHelp,       SIGNAL(toggled(bool) ),      currentTab, SLOT( showHelp(bool) ) );

            connect(currentTab, SIGNAL( TabDirty(bool) ), buttonUpdate, SLOT( setEnabled(bool) ) );
            connect(currentTab, SIGNAL( TabDirty(bool) ), this, SIGNAL( componentDirty(bool) ) );
        }
#ifdef DEBUG_GUI
        std::cout << "GUI: end Link" << std::endl;
#endif

        //std::map< std::string, std::vector<QTabulationModifyObject* > >::iterator it;
        //for (it=groupTabulation.begin();it!=groupTabulation.end();++it)
        //{
        //    const std::string &groupName=it->first;
        //    std::vector<QTabulationModifyObject* > &tabs=it->second;
        for (std::vector<std::string>::const_iterator it = tabNames.begin(), itend = tabNames.end(); it != itend; ++it)
        {
            std::string groupName = *it;
            std::vector<QTabulationModifyObject* > &tabs=groupTabulation[groupName];

#ifdef SOFAGUIQT_SCROLL
            bool useScroll = true;
#else
            bool useScroll = false;
#endif
            if (groupName[groupName.length()-1] == '_')
            {
                groupName = std::string(groupName, 0, groupName.length()-1);
                useScroll = true;
            }
            
            for (unsigned int i=0; i<tabs.size(); ++i)
            {
                QString nameTab;
                if (tabs.size() == 1) nameTab=groupName.c_str();
                else                  nameTab=QString(groupName.c_str())+ " " + QString::number(tabs[i]->getIndex()) + "/" + QString::number(tabs.size());
#ifdef DEBUG_GUI
                std::cout << "GUI: add Tab " << nameTab.ascii() << std::endl;
#endif

                if (!useScroll)
                {
                    dialogTab->addTab(tabs[i],nameTab);
                }
                else
                {
                    QScrollArea* scroll = new QScrollArea(this);
                    tabs[i]->addStretch();
                    scroll->setWidget(tabs[i]);
                    scroll->setWidgetResizable(true);
                    dialogTab->addTab(scroll,nameTab);
                }
            }
        }

#if 0 //def SOFA_HAVE_QWT
        //Energy Widget
        if (simulation::Node* real_node = simulation::Node::DynamicCast(node))
        {
            if (dialogFlags_.REINIT_FLAG /*&& (!real_node->mass.empty() || !real_node->forceField.empty() )*/ )
            {
                energy = new QEnergyStatWidget(dialogTab, real_node);
                dialogTab->addTab(energy,QString("Energy Stats"));
            }
        }

        //Momentum Widget
        if (simulation::Node* real_node = simulation::Node::DynamicCast(node))
        {
            if (dialogFlags_.REINIT_FLAG && (!real_node->mass.empty() ) )
            {
                momentum = new QMomentumStatWidget(dialogTab, real_node);
                dialogTab->addTab(momentum,QString("Momentum Stats"));
            }
        }
#endif
        // Info Widget
        {
#ifdef DEBUG_GUI
            std::cout << "GUI: add Tab Infos" << std::endl;
#endif
            QDataDescriptionWidget* description=new QDataDescriptionWidget(dialogTab, node);
            dialogTab->addTab(description, QString("Infos"));
        }

        //Console
        {
            updateConsole();
            if (outputTab)
            {
#ifdef DEBUG_GUI
                std::cout << "GUI: add Tab Logs" << std::endl;
#endif
                dialogTab->addTab(outputTab,  QString("Logs"));
            }
            if (warningTab)
            {
#ifdef DEBUG_GUI
                std::cout << "GUI: add Tab Warnings" << std::endl;
#endif
                dialogTab->addTab(warningTab, QString("Warnings"));
            }
        }


        //Adding buttons at the bottom of the dialog
        QHBoxLayout *lineLayout = new QHBoxLayout( 0, 0, 6, "Button Layout");
        lineLayout->addWidget(buttonUpdate);
        lineLayout->addWidget(showHelp);
        lineLayout->addWidget(dataFilter);
        QSpacerItem *Horizontal_Spacing = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
        lineLayout->addItem( Horizontal_Spacing );

        lineLayout->addWidget(buttonOk);
        lineLayout->addWidget(buttonCancel);
        generalLayout->addLayout( lineLayout );
        //Signals and slots connections
        connect( buttonUpdate,   SIGNAL( clicked() ), this, SLOT( updateValues() ) );
        connect( buttonOk,       SIGNAL( clicked() ), this, SLOT( accept() ) );
        connect( buttonCancel,   SIGNAL( clicked() ), this, SLOT( reject() ) );
        resize( QSize(450, 130).expandedTo(minimumSizeHint()) );
    }
}

void ModifyObject::createDialog(core::objectmodel::BaseData* data)
{
    data_ = data;
    node = NULL;

#ifdef DEBUG_GUI
    std::cout << "GUI>emit beginDataModification("<<data->getName()<<")" << std::endl;
#endif
    emit beginDataModification(data);
#ifdef DEBUG_GUI
    std::cout << "GUI<emit beginDataModification("<<data->getName()<<")" << std::endl;
#endif

#ifdef DEBUG_GUI
    std::cout << "GUI: createDialog( Data<" << data->getValueTypeString() << "> " << data->getName() << ")" << std::endl;
#endif

    QVBoxLayout *generalLayout = new QVBoxLayout(this, 0, 1, "generalLayout");
    QHBoxLayout *lineLayout = new QHBoxLayout( 0, 0, 6, "Button Layout");
    buttonUpdate = new QPushButton( this, "buttonUpdate" );
    buttonUpdate->setText("&Update");
    buttonUpdate->setEnabled(false);
    QPushButton *buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setText( tr( "&OK" ) );
    QPushButton *buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setText( tr( "&Cancel" ) );

    QDisplayDataWidget* displaydatawidget = new QDisplayDataWidget(this,data,getFlags());
    generalLayout->addWidget(displaydatawidget);
    lineLayout->addWidget(buttonUpdate);


    QSpacerItem *Horizontal_Spacing = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    lineLayout->addItem( Horizontal_Spacing );


    lineLayout->addWidget(buttonOk);
    lineLayout->addWidget(buttonCancel);
    generalLayout->addLayout( lineLayout );
    connect(buttonUpdate,   SIGNAL( clicked() ), displaydatawidget, SLOT( UpdateData() ) );
    connect(displaydatawidget, SIGNAL( WidgetDirty(bool) ), buttonUpdate, SLOT( setEnabled(bool) ) );
    connect(displaydatawidget, SIGNAL( WidgetDirty(bool) ), this, SIGNAL( componentDirty(bool) ) );
    connect(buttonOk, SIGNAL(clicked() ), displaydatawidget, SLOT( UpdateData() ) );
    connect(displaydatawidget, SIGNAL(DataOwnerDirty(bool)), this, SLOT( updateListViewItem() ) );
    connect( buttonOk,       SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel,   SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect(this, SIGNAL(updateDataWidgets()), displaydatawidget, SLOT(UpdateWidgets()) );
}

//******************************************************************************************
void ModifyObject::updateConsole()
{
    //Console Warnings
    if ( !node->getWarnings().empty())
    {
        if (!logWarningEdit)
        {
            warningTab = new QWidget();
            QVBoxLayout* tabLayout = new QVBoxLayout( warningTab, 0, 1, QString("tabWarningLayout"));

            QPushButton *buttonClearWarnings = new QPushButton(warningTab, "buttonClearWarnings");
            tabLayout->addWidget(buttonClearWarnings);
            buttonClearWarnings->setText( tr("&Clear"));
            connect( buttonClearWarnings, SIGNAL( clicked()), this, SLOT( clearWarnings()));

            logWarningEdit = new Q3TextEdit( warningTab, QString("WarningEdit"));
            tabLayout->addWidget( logWarningEdit );

            logWarningEdit->setReadOnly(true);
        }

        if (dialogTab->currentPage() == warningTab)
        {
            logWarningEdit->setText(QString(node->getWarnings().c_str()));
            logWarningEdit->moveCursor(Q3TextEdit::MoveEnd, false);
            logWarningEdit->ensureCursorVisible();
        }
    }
    //Console Outputs
    if ( !node->getOutputs().empty())
    {
        if (!logOutputEdit)
        {
            outputTab = new QWidget();
            QVBoxLayout* tabLayout = new QVBoxLayout( outputTab, 0, 1, QString("tabOutputLayout"));

            QPushButton *buttonClearOutputs = new QPushButton(outputTab, "buttonClearOutputs");
            tabLayout->addWidget(buttonClearOutputs);
            buttonClearOutputs->setText( tr("&Clear"));
            connect( buttonClearOutputs, SIGNAL( clicked()), this, SLOT( clearOutputs()));

            logOutputEdit = new Q3TextEdit( outputTab, QString("OutputEdit"));
            tabLayout->addWidget( logOutputEdit );

            logOutputEdit->setReadOnly(true);
        }

        if (dialogTab->currentPage() == outputTab)
        {
            logOutputEdit->setText(QString(node->getOutputs().c_str()));
            logOutputEdit->moveCursor(Q3TextEdit::MoveEnd, false);
            logOutputEdit->ensureCursorVisible();
        }
    }
}

//*******************************************************************************************************************
void ModifyObject::updateValues()
{
    if (buttonUpdate == NULL // || !buttonUpdate->isEnabled()
       ) return;

    //Make the update of all the values
    if (node)
    {
        bool isNode =( simulation::Node::DynamicCast(node) != 0);
        //If the current element is a node of the graph, we first apply the transformations
        if (dialogFlags_.REINIT_FLAG && isNode)
        {
            simulation::Node* current_node = simulation::Node::DynamicCast(node);
            if (!transformation->isDefaultValues())
                transformation->applyTransformation(current_node);
            transformation->setDefaultValues();
        }



        if (dialogFlags_.REINIT_FLAG)
        {
            if (sofa::core::objectmodel::BaseObject *obj = sofa::core::objectmodel::BaseObject::DynamicCast(node))
            {
                obj->reinit();
            }
            else if (simulation::Node *n = simulation::Node::DynamicCast(node)) n->reinit(sofa::core::ExecParams::defaultInstance());
        }

    }

#ifdef DEBUG_GUI
    std::cout << "GUI>emit objectUpdated()" << std::endl;
#endif
    emit (objectUpdated());
#ifdef DEBUG_GUI
    std::cout << "GUI<emit objectUpdated()" << std::endl;
#endif

    if (node)
    {
#ifdef DEBUG_GUI
        std::cout << "GUI>emit endObjectModification("<<node->getName()<<")" << std::endl;
#endif
        emit endObjectModification(node);
#ifdef DEBUG_GUI
        std::cout << "GUI<emit endObjectModification("<<node->getName()<<")" << std::endl;
#endif
#ifdef DEBUG_GUI
        std::cout << "GUI>emit beginObjectModification("<<node->getName()<<")" << std::endl;
#endif
        emit beginObjectModification(node);
#ifdef DEBUG_GUI
        std::cout << "GUI<emit beginObjectModification("<<node->getName()<<")" << std::endl;
#endif
    }

    buttonUpdate->setEnabled(false);
}


//*******************************************************************************************************************

void ModifyObject::updateListViewItem()
{
    Q3ListViewItem* parent = item_->parent();
    QString currentName =parent->text(0);
    std::string name = parent->text(0).ascii();
    std::string::size_type pos = name.find(' ');
    if (pos != std::string::npos)
        name.resize(pos);
    name += "  ";
    name += data_->getOwner()->getName();
    QString newName(name.c_str());
    if (newName != currentName) parent->setText(0,newName);
}

//*******************************************************************************************************************

void ModifyObject::updateFullDisplay()
{
    dialogTab->clear();
    m_tabs.clear();

    // displayWidget
    if (node)
    {
        const sofa::core::objectmodel::Base::VecData& fields = node->getDataFields();
        //const sofa::core::objectmodel::Base::VecLink& links = node->getLinks();

        std::map< std::string, std::vector<QTabulationModifyObject* > > groupTabulation;

        //If we operate on a Node, we have to ...
        bool isNode = (simulation::Node::DynamicCast(node) != NULL);
        if (isNode)
        {
            //if (dialogFlags_.REINIT_FLAG)
            {
                //add the widgets to apply some basic transformations

                m_tabs.push_back(new QTabulationModifyObject(this, node, item_, 1));
                groupTabulation[std::string("Property")].push_back(m_tabs.back());
                connect(m_tabs.back(), SIGNAL(nodeNameModification(simulation::Node *)), this, SIGNAL(nodeNameModification(simulation::Node *)));

                transformation = new QTransformationWidget(m_tabs.back(), QString("Transformation"));
                m_tabs.back()->layout()->add(transformation);
                m_tabs.back()->externalWidgetAddition(transformation->getNumWidgets());
                connect(transformation, SIGNAL(TransformationDirty(bool)), buttonUpdate, SLOT(setEnabled(bool)));
                connect(transformation, SIGNAL(TransformationDirty(bool)), this, SIGNAL(componentDirty(bool)));
            }
        }

        std::vector<std::string> tabNames;
        //Put first the Property Tab
        //tabNames.push_back("Property");

        for (sofa::core::objectmodel::Base::VecData::const_iterator it = fields.begin(); it != fields.end(); ++it)
        {
            core::objectmodel::BaseData* data = *it;
            if (!data)
            {
                std::cerr << "ERROR: NULL Data in " << node->getName() << std::endl;
                continue;
            }

            if (data->getName().empty()) continue; // ignore unnamed data

            if (!data->getGroup())
            {
                std::cerr << "ERROR: NULL group for Data " << data->getName() << " in " << node->getName() << std::endl;
                continue;
            }

            //For each Data of the current Object
            //We determine where it belongs:
            std::string currentGroup = data->getGroup();

            if (currentGroup.empty()) currentGroup = "Property";

            QString dataFilt = dataFilter->text();
            if (!dataFilt.isEmpty())
            {
                QString qcurGroup(currentGroup.c_str());
                QString qdatName(data->getName().c_str());
                if (!qcurGroup.contains(dataFilt, Qt::CaseInsensitive)
                    && !qdatName.contains(dataFilt, Qt::CaseInsensitive))
                {
                    continue;
                }
            }

#ifdef DEBUG_GUI
            std::cout << "GUI: add Data " << data->getName() << " in " << currentGroup << std::endl;
#endif
            QTabulationModifyObject* currentTab = NULL;
            bool useScroll = (currentGroup[currentGroup.length() - 1] == '_');
            std::vector<QTabulationModifyObject* > &tabs = groupTabulation[currentGroup];
            bool newTab = false;
            if (tabs.empty()) tabNames.push_back(currentGroup);
            if (tabs.empty() || (!useScroll && tabs.back()->isFull()))
            {
                newTab = true;
                m_tabs.push_back(new QTabulationModifyObject(this, node, item_, tabs.size() + 1));
                tabs.push_back(m_tabs.back());
            }
            currentTab = tabs.back();
            currentTab->addData(data, getFlags());
            if (newTab)
            {
                connect(buttonUpdate, SIGNAL(clicked()), currentTab, SLOT(updateDataValue()));
                connect(buttonOk, SIGNAL(clicked()), currentTab, SLOT(updateDataValue()));
                connect(this, SIGNAL(updateDataWidgets()), currentTab, SLOT(updateWidgetValue()));
                connect(showHelp, SIGNAL(toggled(bool)), currentTab, SLOT(showHelp(bool)));
            
                connect(currentTab, SIGNAL(TabDirty(bool)), buttonUpdate, SLOT(setEnabled(bool)));
            }

#ifdef DEBUG_GUI
            std::cout << "GUI: added Data " << data->getName() << " in " << currentGroup << std::endl;
#endif
        }
#ifdef DEBUG_GUI
        std::cout << "GUI: end Data" << std::endl;
#endif


#ifdef DEBUG_GUI
        std::cout << "GUI: end Link" << std::endl;
#endif

        for (std::vector<std::string>::const_iterator it = tabNames.begin(), itend = tabNames.end(); it != itend; ++it)
        {
            std::string groupName = *it;
            std::vector<QTabulationModifyObject* > &tabs = groupTabulation[groupName];

#ifdef SOFAGUIQT_SCROLL
            bool useScroll = true;
#else
            bool useScroll = false;
#endif
            if (groupName[groupName.length() - 1] == '_')
            {
                groupName = std::string(groupName, 0, groupName.length() - 1);
                useScroll = true;
            }

            for (unsigned int i = 0; i < tabs.size(); ++i)
            {
                QString nameTab;
                if (tabs.size() == 1) nameTab = groupName.c_str();
                else                  nameTab = QString(groupName.c_str()) + " " + QString::number(tabs[i]->getIndex()) + "/" + QString::number(tabs.size());
#ifdef DEBUG_GUI
                std::cout << "GUI: add Tab " << nameTab.ascii() << std::endl;
#endif

                if (!useScroll)
                {
                    dialogTab->addTab(tabs[i], nameTab);
                }
                else
                {
                    QScrollArea* scroll = new QScrollArea(this);
                    tabs[i]->addStretch();
                    scroll->setWidget(tabs[i]);
                    scroll->setWidgetResizable(true);
                    dialogTab->addTab(scroll, nameTab);
                }
            }
        }
    }
}

//**************************************************************************************************************************************
//Called each time a new step of the simulation if computed
void ModifyObject::updateTables()
{
#ifdef DEBUG_GUI
    std::cout << "GUI>emit updateDataWidgets()" << std::endl;
#endif
    emit updateDataWidgets();
#ifdef DEBUG_GUI
    std::cout << "GUI<emit updateDataWidgets()" << std::endl;
#endif

#ifdef SOFA_HAVE_QWT
    if (energy)
    {
        energy->step();
        if (dialogTab->currentPage() == energy) energy->updateVisualization();
    }

    if (momentum)
    {
        momentum->step();
        if (dialogTab->currentPage() == momentum) momentum->updateVisualization();
    }
#endif

    if(node)
    {
        updateConsole();
    }
}

void ModifyObject::reject   ()
{
    if (node)
    {
#ifdef DEBUG_GUI
        std::cout << "GUI>emit endObjectModification(" << node->getName() << ")" << std::endl;
#endif
        emit endObjectModification(node);
#ifdef DEBUG_GUI
        std::cout << "GUI<emit endObjectModification(" << node->getName() << ")" << std::endl;
#endif
    }

    const QString dataModifiedString = parseDataModified();
    if (!dataModifiedString.isEmpty())
    {
        emit  dataModified( dataModifiedString  );
    }

//          else if (data) emit endDataModification(data);
    emit(dialogClosed(Id_));
    deleteLater();
    QDialog::reject();
} //When closing a window, inform the parent.

void ModifyObject::accept   ()
{
    updateValues();

    const QString dataModifiedString = parseDataModified();
    if (!dataModifiedString.isEmpty())
    {
        emit  dataModified( dataModifiedString  );
    }

    if (node)
    {
#ifdef DEBUG_GUI
        std::cout << "GUI>emit endObjectModification(" << node->getName() << ")" << std::endl;
#endif
        emit endObjectModification(node);
#ifdef DEBUG_GUI
        std::cout << "GUI<emit endObjectModification(" << node->getName() << ")" << std::endl;
#endif
    }
//          else if (data) emit endDataModification(data);
    emit(dialogClosed(Id_));
    deleteLater();
    QDialog::accept();
} //if closing by using Ok button, update the values

QString ModifyObject::parseDataModified()
{
    QString cat;

    for (std::size_t i = 0; i < m_tabs.size(); ++i)
    {
        const QString tabString = m_tabs[i]->getDataModifiedString();
        if (!tabString.isEmpty())
        {
            cat += tabString;
            if (i != (m_tabs.size() - 1))
            {
                cat += "\n";
            }
        }
    }

    return cat;
}

} // namespace qt

} // namespace gui

} // namespace sofa
