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
#ifndef SOFA_GUI_QT_DISPLAYLINKWIDGET_H
#define SOFA_GUI_QT_DISPLAYLINKWIDGET_H

#include "LinkWidget.h"
#include "QSofaListView.h"
#ifdef SOFA_QT4
#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <Q3GroupBox>
#include <QSlider>
#include <Q3ListViewItem>
#else
#include <qslider.h>
#include <qwidget.h>
#include <qtextedit.h>
#include <qlineedit.h>
#include <qgroupbox.h>
#endif

#ifndef SOFA_QT4
typedef QGroupBox Q3GroupBox;
typedef QTextEdit   Q3TextEdit;
#endif

namespace sofa
{

namespace gui
{

namespace qt
{

class LinkWidget;
class QDisplayLinkInfoWidget;
struct ModifyObjectFlags;

class QDisplayLinkWidget : public Q3GroupBox
{
    Q_OBJECT
public:
    QDisplayLinkWidget(QWidget* parent,
            core::objectmodel::BaseLink* link,
            const ModifyObjectFlags& flags,
            sofa::helper::vector<Q3ListViewItem*> componentReference,
            QSofaListView* listView);
    unsigned int getNumWidgets() const { return numWidgets_;};

public slots:
    void UpdateLink();              //QWidgets ---> BaseLink
    void UpdateWidgets();           //BaseLink ---> QWidget
    void showHelp(bool);

signals:
    void WidgetDirty(bool);
    void WidgetUpdate();
    void LinkUpdate();
    void LinkOwnerDirty(bool);
protected:
    core::objectmodel::BaseLink* link_;
    QDisplayLinkInfoWidget*  linkinfowidget_;
    LinkWidget* linkwidget_;
    unsigned int numWidgets_;
};



class QLinkSimpleEdit : public LinkWidget
{
    Q_OBJECT
    typedef enum QEditType { TEXTEDIT, LINEEDIT } QEditType;
    typedef union QEditWidgetPtr
    {
        QLineEdit* lineEdit;
        QTextEdit* textEdit;
    } QEditWidgetPtr;

    typedef struct QSimpleEdit
    {
        QEditType type;
        QEditWidgetPtr widget;
        QPushButton* reference;
    } QSimpleEdit;
public :
    QLinkSimpleEdit(QWidget*, const char* name, core::objectmodel::BaseLink*, sofa::helper::vector<Q3ListViewItem*> componentReference, QSofaListView* listView);
    virtual unsigned int numColumnWidget() {return 3;}
    virtual unsigned int sizeWidget() {return 1;}
    virtual bool createWidgets();
    
public slots:
    void openLink();

protected:
    virtual void readFromLink();
    virtual void writeToLink();
    QSimpleEdit innerWidget_;
    sofa::helper::vector<Q3ListViewItem*> m_componentReference;
    QSofaListView* listView_;
};

} // namespace qt

} // namespace gui

} // namespace sofa

#endif // SOFA_GUI_QT_DISPLAYLINKWIDGET_H
