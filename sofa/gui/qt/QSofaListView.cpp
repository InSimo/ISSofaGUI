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
#include "QSofaListView.h"
#include "GraphListenerQListView.h"
#include "AddObject.h"
#include "ModifyObject.h"
#include "GenGraphForm.h"
#include "RealGUI.h"
#include <sofa/simulation/common/DeleteVisitor.h>

#include <sofa/simulation/common/TransformationVisitor.h>

#ifdef SOFA_QT4
#include <Q3PopupMenu>
#else
#include <qapplication.h>
#include <qpopupmenu.h>
#endif



using namespace sofa::simulation;
using namespace sofa::core::objectmodel;
namespace sofa
{
namespace gui
{
namespace qt
{

#ifndef SOFA_QT4
typedef QPopupMenu Q3PopupMenu;
#endif




QSofaListView::QSofaListView(const SofaListViewAttribute& attribute,
        QWidget* parent,
        const char* name,
        Qt::WFlags f):
    Q3ListView(parent,name,f),
    graphListener_(NULL),
    AddObjectDialog_(NULL),
    attribute_(attribute)
{
    //List of objects
    //Read the object.txt that contains the information about the objects which can be added to the scenes whithin a given BoundingBox and scale range
    std::string object ( "config/object.txt" );

    if (sofa::helper::system::DataRepository.findFile(object, "", nullptr))
    {
        list_object.clear();
        std::ifstream end(object.c_str());
        std::string s;
        while( end >> s )
        {
            list_object.push_back(s);
        }
        end.close();
    }

    //Creation of the file dialog
    AddObjectDialog_ = new AddObject ( &list_object, this );
    AddObjectDialog_->hide();


    addColumn(QString());
    header()->setClickEnabled(false, header()->count() - 1);
    header()->setResizeEnabled(false, header()->count() - 1);
    header()->setLabel(0, QString());

	searchName_ = true;
	searchType_ = true;
    activatedFilter_ = true;
	displayChildrenWhenParentMatches_ = true;

    setRootIsDecorated(true);
    setTreeStepSize(15);
    graphListener_ = new GraphListenerQListView(this);
#ifdef SOFA_QT4
    connect(this,SIGNAL(rightButtonClicked(Q3ListViewItem*,const QPoint&, int)) ,this,SLOT(RunSofaRightClicked(Q3ListViewItem*,const QPoint&, int)) );
    connect(this,SIGNAL(doubleClicked(Q3ListViewItem*) ), this, SLOT(RunSofaDoubleClicked(Q3ListViewItem*)) );
    connect(this,SIGNAL(clicked(Q3ListViewItem*) ), this, SLOT(updateMatchingObjectmodel(Q3ListViewItem*)) );
    connect(this,SIGNAL(selectionChanged(Q3ListViewItem*)), this, SLOT(updateMatchingObjectmodel(Q3ListViewItem*)));
#else
    connect(this,SIGNAL(rightButtonClicked(QListViewItem*,const QPoint&, int)) ,this,SLOT(RunSofaRightClicked(QListViewItem*,const QPoint&, int)) );
    connect(this,SIGNAL(doubleClicked(QListViewItem*) ), this, SLOT(RunSofaDoubleClicked(QListViewItem*)) );
    connect(this,SIGNAL(clicked(QListViewItem*) ), this, SLOT(updateMatchingObjectmodel(QListViewItem*)) );
    connect(this,SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(updateMatchingObjectmodel(QListViewItem*)));

#endif
}

QSofaListView::~QSofaListView()
{

    delete graphListener_;
}

void QSofaListView::Clear(Node* rootNode)
{

    if(graphListener_ != NULL)
    {
        /*if(attribute_ = VISUAL){
          simulation::getSimulation()->getVisualRoot()->removeListener(graphListener_);
        }*/
        delete graphListener_;
    }

    CloseAllDialogs();
    clear();
    graphListener_ = new GraphListenerQListView(this);
    setSorting ( -1 );
    header()->hide();
    graphListener_->addChild ( NULL, rootNode );
    graphListener_->freeze ( rootNode );
    std::map<Base*, Q3ListViewItem* >::iterator graph_iterator;

    for (graph_iterator = graphListener_->items.begin();
            graph_iterator != graphListener_->items.end();
            ++graph_iterator)
    {
        Node* node = Node::DynamicCast(graph_iterator->first);
        if (node!=NULL && !node->isActive())
        {
            object_.ptr.Node = node;
            object_.type  = typeNode;
            emit RequestActivation(object_.ptr.Node, node->isActive());
        }
    }

}

void QSofaListView::CloseAllDialogs()
{
    emit( Close() );
    assert( map_modifyObjectWindow.empty() );
    assert( map_modifyDialogOpened.empty() );

}

void QSofaListView::modifyUnlock(void* Id)
{
    map_modifyDialogOpened.erase( Id );
    map_modifyObjectWindow.erase( Id );
}

bool QSofaListView::nameMatchesFilter(Q3ListViewItem* item, QString filter, bool bIsNode, bool exactMatch = false)
{
    if (!searchName_) return false;

    bool match = false;

    emit Lock(true);

    if (bIsNode)
    {
        if (exactMatch)
        {
            match = (item->text(0).compare(filter, Qt::CaseInsensitive)) == 0;
        }
        else
        {
            match = item->text(0).contains(filter, Qt::CaseInsensitive);
        }
    }
    else
    {
        QStringList list = item->text(0).split(' ');

        if (list.size() >= 2)
        {
            list.pop_front();
            if (exactMatch)
            {
                match = (list.back().compare(filter, Qt::CaseInsensitive)) == 0;
            }
            else
            {
                match = list.join(" ").contains(filter, Qt::CaseInsensitive);
            }
        }

    }

    emit Lock(false);

    return match;
}

bool QSofaListView::typeMatchesFilter(Q3ListViewItem* item, QString filter, bool bIsNode, bool exactMatch = false)
{
    if (bIsNode || !searchType_) return false;

    bool match = false;

    emit Lock(true);

    QStringList list = item->text(0).split(' ');

    emit Lock(false);

    if (!list.isEmpty())
    {
        if (exactMatch)
        {
            match = (list.front().compare(filter, Qt::CaseInsensitive)) == 0;
        }
        else
        {
            match = list.front().contains(filter, Qt::CaseInsensitive);
        }
    }

    return match;
}

bool QSofaListView::shouldDisplayNode(Q3ListViewItem* item, bool parentMatched, bool bIsNode)
{
	emit Lock(true);

	bool noOption = !searchName_ && !searchType_ ; // No option activated : ignore filter and display node anyway

    bool display = noOption || filter_.isEmpty() ||
        (parentMatched && displayChildrenWhenParentMatches_);
    
    if (!display)
    {
        QStringList filterList = filter_.split(' ');

        display = true;
        for (int fl = 0; fl < filterList.size(); ++fl)
        {
            QString str = filterList.at(fl);

            bool exactMatch = false;
            if (str.contains("\"")) //search for exact match
            {
                str = str.remove(QChar('"'));
                exactMatch = true;
            }

            if (str.contains("/"))
            {
                if (bIsNode)
                {
                    str = str.remove(QChar('/'));
                    display &= nameMatchesFilter(item, str, bIsNode, exactMatch) || typeMatchesFilter(item, str, bIsNode, exactMatch);
                }
                else
                    display = false;
            }
            else if (str.contains("!"))
            {
                Base* base = graphListener_->findObject(item);
                Node* node = Node::DynamicCast(base);
                BaseObject * bo = dynamic_cast<BaseObject *>(base);
                bool warn = node;
                warn |= (bo && bo->getWarnings().empty());

                display = !warn;
            }
            else
            {
                display &= nameMatchesFilter(item, str, bIsNode, exactMatch) || typeMatchesFilter(item, str, bIsNode, exactMatch);
            }
        }
    }

	emit Lock(false);

	return display;
}

bool QSofaListView::isItemANode(Q3ListViewItem* item)
{
	Base* base;
	Node* node;

	base = graphListener_->findObject(item);
	node = Node::DynamicCast(base);

	return node != NULL;
}

typedef std::vector< std::pair<Q3ListViewItem*, bool> > MatchingNodesList;

// Updates the Visible value of the nodes based on options and filter
bool QSofaListView::fillFilteredList(MatchingNodesList& nodesList, Q3ListViewItem* item, bool parentMatched)
{
	bool hasVisibleItem = false;

    emit Lock(true);
	while (item)
	{
		std::pair<Q3ListViewItem*, bool> itemDisplayValue;
		itemDisplayValue.first = item;

		if (shouldDisplayNode(item, parentMatched, isItemANode(item)))
		{
            Base* base = graphListener_->findObject(item);
            Node* node = Node::DynamicCast(base);
            if (!node || (node->isActive() || !activatedFilter_))
            {
                itemDisplayValue.second = true; // setVisible value
                fillFilteredList(nodesList, item->firstChild(), true);
                hasVisibleItem = true;
            }
		}
		else
		{
			bool hasVisibleChild = fillFilteredList(nodesList, item->firstChild(), false);
			if (hasVisibleChild)
			{
				itemDisplayValue.second = true; // setVisible value
				hasVisibleItem = true;
			}
			else
			{
				itemDisplayValue.second = false; // setVisible value
			}
		}
		nodesList.push_back(itemDisplayValue);

		item = item->nextSibling();
	}
    emit Lock(false);

	return hasVisibleItem;
}

void QSofaListView::showMatchingNodes(const MatchingNodesList& nodesList)
{
	emit Lock(true);

	for (MatchingNodesList::const_reverse_iterator it = nodesList.rbegin(); it != nodesList.rend(); ++it)
	{
		if (it->first)
		{
			it->first->setVisible(it->second);
		}
	}

	emit Lock(false);
}

void QSofaListView::applyFilter()
{
	//Must setVisible on parent before children to prevent bugs -> need to store in a temporary list and setVisible after algorithm is finished.
	MatchingNodesList nodesList;

    Q3ListViewItem* root = firstChild();
	fillFilteredList(nodesList, root, false);

	showMatchingNodes(nodesList);
}

void QSofaListView::collapseNode()
{
    collapseNode(currentItem());
}

void QSofaListView::collapseNode(Q3ListViewItem* item)
{
    if (!item) return;
    emit Lock(true);
    Q3ListViewItem* child;
    child = item->firstChild();
    while ( child != NULL )
    {
        child->setOpen ( false );
        child = child->nextSibling();
    }
    item->setOpen ( true );
    emit Lock(false);
}

void QSofaListView::expandNode()
{
    expandNode(currentItem());
}

void QSofaListView::expandNode(Q3ListViewItem* item)
{
    if (!item) return;
    emit Lock(true);
    item->setOpen ( true );
    if ( item != NULL )
    {
        Q3ListViewItem* child;
        child = item->firstChild();
        while ( child != NULL )
        {
            item = child;
            child->setOpen ( true );
            expandNode(item);
            child = child->nextSibling();
        }
    }
    emit Lock(false);
}

void QSofaListView::updateMatchingObjectmodel(Q3ListViewItem* item)
{
    BaseData* data = NULL;
    Base* base = NULL;
    BaseObject* object = NULL;
    Node* node = NULL;
    if(item == NULL)
    {
        object_.ptr.Node = NULL;
    }
    else
    {
        base = graphListener_->findObject(item);
        if(base == NULL)
        {
            data = graphListener_->findData(item);
            assert(data);
            object_.ptr.Data = data;
            object_.type = typeData;
            base = data->getOwner();
        }
        else
        {
            node = Node::DynamicCast(base);
            if (node == NULL)
            {
                object = BaseObject::DynamicCast(base);
                object_.ptr.Object = object;
                object_.type = typeObject;
            }
            else
            {
                object_.ptr.Node = node;
                object_.type = typeNode;
            }
        }
    }
    if (base != selectedComponent_.get())
    {
        selectedComponent_ = base;
        emit selectedComponentChanged(base);
    }
}

void QSofaListView::updateMatchingObjectmodel()
{
    updateMatchingObjectmodel(currentItem());
}
void QSofaListView::Freeze()
{
    Node* groot = Node::DynamicCast( graphListener_->findObject(firstChild()) );

    assert(groot);
    graphListener_->freeze(groot);
}

void QSofaListView::Unfreeze()
{
    Node* groot = Node::DynamicCast(graphListener_->findObject(firstChild()) );
    assert(groot);
    graphListener_->unfreeze(groot);
}

void QSofaListView::focusObject()
{
    if( object_.isObject())
        emit( focusChanged(object_.ptr.Object));

}
void QSofaListView::focusNode()
{
    if( object_.isNode())
        emit( focusChanged(object_.ptr.Node));
}


/*****************************************************************************************************************/
void QSofaListView::RunSofaRightClicked( Q3ListViewItem *item,
        const QPoint& point,
        int index )
{

    if( item == NULL) return;
    //updateMatchingObjectmodel();
    bool object_hasData = false;
    if(object_.type == typeObject)
    {
        object_hasData = object_.ptr.Object->getDataFields().size() > 0 ? true : false;
    }
    Q3PopupMenu *contextMenu = new Q3PopupMenu ( this, "ContextMenu" );
    if( object_.isNode() )
    {
        int index_menu = contextMenu->insertItem("Focus", this,SLOT(focusNode()) );
        bool enable = object_.ptr.Node->f_bbox.getValue().isValid() && !object_.ptr.Node->f_bbox.getValue().isFlat();
        contextMenu->setItemEnabled(index_menu,enable);
    }
    if( object_.isObject() )
    {
        int index_menu = contextMenu->insertItem("Focus", this,SLOT( focusObject() ) );
        bool enable = object_.ptr.Object->f_bbox.getValue().isValid() && !object_.ptr.Object->f_bbox.getValue().isFlat() ;
        contextMenu->setItemEnabled(index_menu,enable);
    }
    contextMenu->insertSeparator();

    //Creation of the context Menu
    if ( object_.type == typeNode)
    {
        contextMenu->insertItem ( "Collapse", this, SLOT ( collapseNode() ) );
        contextMenu->insertItem ( "Expand", this, SLOT ( expandNode() ) );
        contextMenu->insertSeparator ();
        /*****************************************************************************************************************/
        if (object_.ptr.Node->isActive())
            contextMenu->insertItem ( "Deactivate", this, SLOT ( DeactivateNode() ) );
        else
            contextMenu->insertItem ( "Activate", this, SLOT ( ActivateNode() ) );
        contextMenu->insertSeparator ();
        /*****************************************************************************************************************/

        contextMenu->insertItem("Set filter on node", this, SLOT(setFilterOnNode()));

        contextMenu->insertItem("Enable Draw", this, SLOT(enableDraw()));
        contextMenu->insertItem("Disable Draw", this, SLOT(disableDraw()));

        contextMenu->insertSeparator();

        contextMenu->insertItem ( "Save Node", this, SLOT ( SaveNode() ) );
        contextMenu->insertItem ( "Export OBJ", this, SLOT ( exportOBJ() ) );

        if ( attribute_ == SIMULATION)
        {
            contextMenu->insertItem ( "Add Node", this, SLOT ( RaiseAddObject() ) );

            int index_menu = contextMenu->insertItem ( "Remove Node", this, SLOT ( RemoveNode() ) );
            //If one of the elements or child of the current node is beeing modified, you cannot allow the user to erase the node
            if ( !isNodeErasable ( object_.ptr.Node ) )
                contextMenu->setItemEnabled ( index_menu,false );
        }
    }
    contextMenu->insertItem ( "Modify", this, SLOT ( Modify() ) );
    if(object_hasData)
    {
        if(item->childCount() > 0)
        {
            contextMenu->insertItem("Hide Datas",this, SLOT ( HideDatas() ) );
        }
        else
        {
            contextMenu->insertItem("Show Datas", this, SLOT ( ShowDatas() ) );
        }
    }
    contextMenu->popup ( point, index );
}

void QSofaListView::RunSofaDoubleClicked(Q3ListViewItem* item)
{
    if(item == NULL)
    {
        return;
    }

    item->setOpen( !item->isOpen());
    Modify();

}

/*****************************************************************************************************************/
void QSofaListView::nodeNameModification(simulation::Node* node)
{
    Q3ListViewItem *item=graphListener_->items[node];

    QString nameToUse(node->getName().c_str());
    item->setText(0,nameToUse);

    nameToUse=QString("MultiNode ")+nameToUse;

    typedef std::multimap<Q3ListViewItem *, Q3ListViewItem*>::iterator ItemIterator;
    std::pair<ItemIterator,ItemIterator> range=graphListener_->nodeWithMultipleParents.equal_range(item);

    for (ItemIterator it=range.first; it!=range.second; ++it) it->second->setText(0,nameToUse);
}


void QSofaListView::DeactivateNode()
{
    emit RequestActivation(object_.ptr.Node,false);
    currentItem()->setOpen(false);

}

void QSofaListView::ActivateNode()
{
    emit RequestActivation(object_.ptr.Node,true);
}

void QSofaListView::setFilterOnNode()
{
    setFilter(QString("\"") + QString(object_.ptr.Node->getName().c_str()));
}

void QSofaListView::enableDraw()
{
    emit requestDrawActivation(object_.ptr.Node, true);
}

void QSofaListView::disableDraw()
{
    emit requestDrawActivation(object_.ptr.Node, false);
}

void QSofaListView::SaveNode()
{
    if( object_.ptr.Node != NULL)
    {
        emit Lock(true);
        Node * node = object_.ptr.Node;
        emit RequestSaving(node);
        emit Lock(false);

    }
}
void QSofaListView::exportOBJ()
{
    if( object_.ptr.Node != NULL)
    {
        emit Lock(true);
        Node * node = object_.ptr.Node;
        emit RequestExportOBJ(node,true);
        emit Lock(false);
    }
}
void QSofaListView::RaiseAddObject()
{
    emit Lock(true);
    assert(AddObjectDialog_);

    std::string path( ((RealGUI*) (qApp->mainWidget()))->windowFilePath().ascii());
    AddObjectDialog_->setPath ( path );
    AddObjectDialog_->show();
    AddObjectDialog_->raise();
    emit Lock(false);

}
void QSofaListView::RemoveNode()
{
    if( object_.type == typeNode)
    {
        emit Lock(true);
        Node* node = object_.ptr.Node;
        if ( node == node->getRoot() )
        {
            //Attempt to destroy the Root node : create an empty node to handle new graph interaction
            Node::SPtr root = simulation::getSimulation()->createNewGraph( "Root" );
            graphListener_->removeChild ( NULL, node);
            graphListener_->addChild ( NULL, root.get() );
            emit RootNodeChanged(root.get(),NULL);
        }
        else
        {
            node->detachFromGraph();
            node->execute<simulation::DeleteVisitor>(sofa::core::ExecParams::defaultInstance());
            emit NodeRemoved();
        }
        emit Lock(false);
    }
}
void QSofaListView::Modify()
{
    void *current_Id_modifyDialog = NULL;
    emit Lock(true);

    if ( currentItem() != NULL )
    {
        ModifyObjectFlags dialogFlags = ModifyObjectFlags();
        dialogFlags.setFlagsForSofa();
        ModifyObject* dialogModifyObject = NULL;

        QString title = currentItem()->text(0);

        if (object_.type == typeData)       //user clicked on a data
        {
            current_Id_modifyDialog = object_.ptr.Data;
        }
        if (object_.type == typeNode)
        {
            current_Id_modifyDialog = object_.ptr.Node;
        }
        if(object_.type == typeObject)
        {
            current_Id_modifyDialog = object_.ptr.Object;

            sofa::core::objectmodel::BaseObject* baseObj = sofa::core::objectmodel::BaseObject::DynamicCast(object_.ptr.Object);
            if (baseObj)
            {
                sofa::core::objectmodel::BaseNode * bn = sofa::core::objectmodel::BaseNode::DynamicCast(baseObj->getContext());
                if (bn && !bn->getPathName().empty())
                    title = title + QString(" (") + QString(bn->getPathName().c_str()) + QString(")");
            }
        }
        assert(current_Id_modifyDialog != NULL);

        //Opening of a dialog window automatically created

        std::map< void*, QDialog* >::iterator testWindow =  map_modifyObjectWindow.find( current_Id_modifyDialog);
        if ( testWindow != map_modifyObjectWindow.end())
        {
            //Object already being modified: no need to open a new window
            (*testWindow).second->raise();
            emit Lock(false);
            return;
        }

        dialogModifyObject = new ModifyObject(current_Id_modifyDialog,currentItem(),this, dialogFlags, graphListener_->items, title);
        if(object_.type == typeData)
            dialogModifyObject->createDialog(object_.ptr.Data);
        if(object_.type == typeNode)
            dialogModifyObject->createDialog((Base*)object_.ptr.Node);
        if(object_.type  == typeObject)
            dialogModifyObject->createDialog((Base*)object_.ptr.Object);

        map_modifyDialogOpened.insert( std::make_pair ( current_Id_modifyDialog, currentItem()) );
        map_modifyObjectWindow.insert( std::make_pair(current_Id_modifyDialog, dialogModifyObject));
        connect ( dialogModifyObject, SIGNAL( objectUpdated() ), this, SIGNAL( Updated() ));
        connect ( this, SIGNAL( Close() ), dialogModifyObject, SLOT( closeNow() ) );
        connect ( dialogModifyObject, SIGNAL( dialogClosed(void *) ) , this, SLOT( modifyUnlock(void *)));
        connect ( dialogModifyObject, SIGNAL( nodeNameModification(simulation::Node*) ) , this, SLOT( nodeNameModification(simulation::Node*) ));
        connect ( dialogModifyObject, SIGNAL( dataModified(QString) ), this, SIGNAL( dataModified(QString) ) );
        dialogModifyObject->show();
        dialogModifyObject->raise();
    }
    emit Lock(false);
}

void QSofaListView::UpdateOpenedDialogs()
{
    std::map<void*,QDialog*>::const_iterator iter;
    for(iter = map_modifyObjectWindow.begin(); iter != map_modifyObjectWindow.end() ; ++iter)
    {
        ModifyObject* modify = reinterpret_cast<ModifyObject*>(iter->second);
        modify->updateTables();
    }
}

void QSofaListView::setFilter(const QString &newFilter)
{
	emit Lock(true);

	filter_ = newFilter;
	applyFilter();

	emit Lock(false);
}

void QSofaListView::setSearchName(bool value)
{
	emit Lock(true);

	bool oldValue = searchName_;

	searchName_ = value;

	if (oldValue != searchName_)
		applyFilter();

	emit Lock(false);
}

void QSofaListView::setSearchType(bool value)
{
	emit Lock(true);
	bool oldValue = searchType_;

	searchType_ = value;

	if (oldValue != searchType_)
		applyFilter();

	emit Lock(false);
}

void QSofaListView::setActivatedFilter(bool value)
{
    emit Lock(true);
    bool oldValue = activatedFilter_;

    activatedFilter_ = value;

    if (oldValue != activatedFilter_)
        applyFilter();

    emit Lock(false);
}

void QSofaListView::setDisplayChildrenWhenParentMatches(bool value)
{
	emit Lock(true);
	bool oldValue = displayChildrenWhenParentMatches_;

	displayChildrenWhenParentMatches_ = value;

	if (oldValue != displayChildrenWhenParentMatches_)
		applyFilter();

	emit Lock(false);
}

void QSofaListView::HideDatas()
{
    if( object_.type == typeObject )
    {
        emit Lock(true);
        Unfreeze();
        graphListener_->removeDatas(object_.ptr.Object);
        Freeze();
        emit Lock(false);
    }
}

void QSofaListView::ShowDatas()
{
    if ( object_.type == typeObject )
    {
        emit Lock(true);
        Unfreeze();
        graphListener_->addDatas(object_.ptr.Object);
        Freeze();
        emit Lock(false);
    }
}
/*****************************************************************************************************************/
// Test if a node can be erased in the graph : the condition is that none of its children has a menu modify opened
bool QSofaListView::isNodeErasable ( BaseNode* node)
{
    Q3ListViewItem* item = graphListener_->items[node];
    if(item == NULL)
    {
        return false;
    }
    // check if there is already a dialog opened for that item in the graph
    std::map< void*, Q3ListViewItem*>::iterator it;
    for (it = map_modifyDialogOpened.begin(); it != map_modifyDialogOpened.end(); ++it)
    {
        if (it->second == item) return false;
    }

    //check the item childs
    Q3ListViewItem *child = item->firstChild();
    while (child != NULL)
    {
        for( it = map_modifyDialogOpened.begin(); it != map_modifyDialogOpened.end(); ++it)
        {
            if( it->second == child) return false;
        }
        child = child->nextSibling();
    }
    return true;

}

void QSofaListView::Export()
{
    Node* root = Node::DynamicCast(graphListener_->findObject(firstChild()));
    assert(root);
    GenGraphForm* form = new sofa::gui::qt::GenGraphForm;
    form->setScene ( root );
    std::string gname(((RealGUI*) (qApp->mainWidget()))->windowFilePath().ascii());
    std::size_t gpath = gname.find_last_of("/\\");
    std::size_t gext = gname.rfind('.');
    if (gext != std::string::npos && (gpath == std::string::npos || gext > gpath))
        gname = gname.substr(0,gext);
    form->filename->setText(gname.c_str());
    form->show();
}


void QSofaListView::loadObject ( std::string path, double dx, double dy, double dz,  double rx, double ry, double rz,double scale )
{
    emit Lock(true);
    //Verify if the file exists
    if ( !sofa::helper::system::DataRepository.findFile ( path ) ) return;
    path = sofa::helper::system::DataRepository.getFile ( path );

    //If we add the object without clicking on the graph (direct use of the method),
    //the object will be added to the root node
    if ( currentItem() == NULL )
    {
        for ( std::map<core::objectmodel::Base*, Q3ListViewItem* >::iterator it = graphListener_->items.begin() ;
                it != graphListener_->items.end() ; ++ it )
        {
            if ( ( *it ).second->itemPos() == 0 ) //Root node position
            {
                object_.ptr.Node = sofa::simulation::Node::DynamicCast( ( *it ).first );
                object_.type = typeNode;
                break;
            }
        }
        assert(object_.ptr.Node != NULL);
    }

    //We allow unlock the graph to make all the changes now
    graphListener_->unfreeze ( object_.ptr.Node );

    //Loading of the xml file
    simulation::xml::BaseElement* xml = simulation::xml::loadFromFile ( path.c_str() );
    if ( xml == NULL ) return;

    // helper::system::SetDirectory chdir ( path.c_str() );

    //std::cout << "Initializing objects"<<std::endl;
    if ( !xml->init() )  std::cerr << "Objects initialization failed."<<std::endl;

    Node* new_node = Node::DynamicCast( xml->getObject() );
    if ( new_node == NULL )
    {
        std::cerr << "Objects initialization failed."<<std::endl;
        delete xml;
        return ;
    }

    new_node->addListener(graphListener_);
    if ( object_.ptr.Node && new_node)
    {
        if ( object_.ptr.Node->child.empty() &&  object_.ptr.Node->object.empty() )
        {
            //Temporary Root : the current graph is empty, and has only a single node "Root"
            object_.ptr.Node->detachFromGraph();
            graphListener_->addChild ( NULL, new_node );
            simulation::getSimulation()->init(new_node);
            emit RootNodeChanged(new_node, path.c_str());
        }
        else
        {
            object_.ptr.Node->addChild (new_node );
            simulation::getSimulation()->init(new_node);
            emit NodeAdded();
        }
    }
    graphListener_->freeze(object_.ptr.Node);
    transformObject ( new_node, dx, dy, dz, rx,ry,rz,scale );
    emit Lock(false);
    object_.ptr.Node =  NULL;
}

void QSofaListView::transformObject ( Node *node, double dx, double dy, double dz,  double rx, double ry, double rz, double scale )
{
    if ( node == NULL ) return;
    //const SReal conversionDegRad = 3.141592653/180.0;
    //Vector3 rotationVector = Vector3(rx,ry,rz)*conversionDegRad;
    TransformationVisitor transform(sofa::core::ExecParams::defaultInstance());
    transform.setTranslation(dx,dy,dz);
    transform.setRotation(rx,ry,rz);
    transform.setScale(scale,scale,scale);
    transform.execute(node);
}








} //sofa
} // gui
} //qt



