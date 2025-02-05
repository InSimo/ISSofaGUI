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
#ifndef SOFA_GUI_QT_QSOFALISTVIEW_H
#define SOFA_GUI_QT_QSOFALISTVIEW_H

#ifdef SOFA_QT4
#include <QWidget>
#include <Q3ListView>
#include <Q3ListViewItem>
#include <Q3Header>
#include <QPushButton>
#else
#include <qwidget.h>
#include <qlistview.h>
#include <qheader.h>
#include <qpushbutton.h>
#endif

#include "SofaGUIQt.h"
#include <sofa/simulation/common/Node.h>
#include <sofa/core/objectmodel/BaseData.h>
#include <sofa/core/objectmodel/BaseObject.h>

#ifndef SOFA_QT4
typedef QListView Q3ListView;
typedef QListViewItem Q3ListViewItem;

#endif
#include <map>

namespace sofa
{

namespace gui
{
namespace qt
{

class AddObject;
class GraphListenerQListView;


enum ObjectModelType { typeNode, typeObject, typeData };
typedef union ObjectModelPtr
{
    sofa::simulation::Node* Node;
    core::objectmodel::BaseObject* Object;
    core::objectmodel::BaseData* Data;
} ObjectModelPtr;

typedef struct ObjectModel
{
public:
    ObjectModelType type;
    ObjectModelPtr ptr;
    bool isNode()   { return type == typeNode;   }
    bool isObject() { return type == typeObject; }
    bool isData()   { return type == typeData;   }
} ObjectModel;

enum SofaListViewAttribute
{
    SIMULATION,
    VISUAL,
    MODELER
};

class SOFA_SOFAGUIQT_API QSofaListView : public Q3ListView
{
    Q_OBJECT
public:
    QSofaListView(const SofaListViewAttribute& attribute,
            QWidget* parent=0,
            const char* name=0,
            Qt::WFlags f = 0 );
    ~QSofaListView();

    GraphListenerQListView* getListener() const { return  graphListener_; };
    void Clear(sofa::simulation::Node* rootNode);
    void Freeze();
    void Unfreeze();
    SofaListViewAttribute getAttribute() const { return attribute_; };
	void applyFilter();
public Q_SLOTS:
    void Export();
    void CloseAllDialogs();
    void UpdateOpenedDialogs();

	void setFilter( const QString & );
	void setSearchName( bool );
	void setSearchType( bool );
    void setActivatedFilter(bool);
	void setDisplayChildrenWhenParentMatches( bool );
    void updateMatchingObjectmodel(Q3ListViewItem* item);
    void Modify();
    void RunSofaDoubleClicked(Q3ListViewItem*);

Q_SIGNALS:
    void Close();
    void Lock(bool);
    void RequestSaving(sofa::simulation::Node*);
    void RequestExportOBJ(sofa::simulation::Node* node, bool exportMTL);
    void RequestActivation(sofa::simulation::Node*,bool);
    void RootNodeChanged(sofa::simulation::Node* newroot, const char* newpath);
    void NodeRemoved();
    void Updated();
    void NodeAdded();
    void selectedComponentChanged(sofa::core::objectmodel::Base*);
    void focusChanged(sofa::core::objectmodel::BaseObject*);
    void focusChanged(sofa::core::objectmodel::BaseNode*);
    void dataModified( QString );
    void requestDrawActivation(sofa::simulation::Node*, bool status);

protected Q_SLOTS:
    void SaveNode();
    void exportOBJ();
    void collapseNode();
    void expandNode();
    void modifyUnlock(void* Id);
    void RaiseAddObject();
    void RemoveNode();
    void HideDatas();
    void ShowDatas();
    void DeactivateNode();
    void ActivateNode();
    void setFilterOnNode();
    void enableDraw();
    void disableDraw();
    void loadObject ( std::string path, double dx, double dy, double dz,  double rx, double ry, double rz,double scale );

    void RunSofaRightClicked( Q3ListViewItem *item, const QPoint& point, int index );

    void nodeNameModification( simulation::Node*);
    void focusObject();
    void focusNode();
protected:
	bool nameMatchesFilter(Q3ListViewItem*, QString, bool, bool);
	bool typeMatchesFilter(Q3ListViewItem*, QString, bool, bool);
	bool isItemANode(Q3ListViewItem*);
	bool shouldDisplayNode(Q3ListViewItem*, bool, bool);
	bool fillFilteredList(std::vector< std::pair<Q3ListViewItem*, bool> >&, Q3ListViewItem*, bool);
	void showMatchingNodes(const std::vector< std::pair<Q3ListViewItem*, bool> >& nodesList);

    void collapseNode(Q3ListViewItem* item);
    void expandNode(Q3ListViewItem* item);
    void transformObject ( sofa::simulation::Node *node, double dx, double dy, double dz,  double rx, double ry, double rz, double scale );
    bool isNodeErasable( core::objectmodel::BaseNode* node);
    void updateMatchingObjectmodel();
    std::list<core::objectmodel::BaseNode*> collectNodesToChange(core::objectmodel::BaseNode* node);
    std::map< void*, Q3ListViewItem* > map_modifyDialogOpened;
    std::map< void*, QDialog* > map_modifyObjectWindow;
    GraphListenerQListView* graphListener_;
    std::vector< std::string > list_object;
    AddObject* AddObjectDialog_;
    ObjectModel object_;
    sofa::core::objectmodel::Base::SPtr selectedComponent_;
    SofaListViewAttribute attribute_;

	QString	filter_;
	bool searchName_;
	bool searchType_;
    bool activatedFilter_;
	bool displayChildrenWhenParentMatches_;

};

} //sofa
} //gui
}//qt

#endif // SOFA_GUI_QT_QSOFALISTVIEW_H


