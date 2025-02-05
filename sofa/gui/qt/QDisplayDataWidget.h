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
#ifndef SOFA_GUI_QT_DISPLAYDATAWIDGET_H
#define SOFA_GUI_QT_DISPLAYDATAWIDGET_H

#include "DataWidget.h"
#include "ModifyObject.h"
#include <sofa/core/dataparser/JsonDataParser.h>

#ifdef SOFA_QT4
#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <Q3GroupBox>
#include <QSlider>
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
namespace core
{
namespace objectmodel
{
class BaseData;
}
}
namespace gui
{
namespace qt
{

class DataWidget;
class QDisplayDataInfoWidget;
struct ModifyObjectFlags;

class QDisplayDataWidget : public Q3GroupBox
{
    Q_OBJECT
public:
    QDisplayDataWidget(QWidget* parent,
            core::objectmodel::BaseData* data,
            const ModifyObjectFlags& flags, 
            Q3ListViewItem* componentReference = nullptr,
            QSofaListView* listView = nullptr);
    unsigned int getNumWidgets() const { return numWidgets_;}

    ModifyObjectFlags flag() {return flags_;}

public slots:
    void UpdateData();              //QWidgets ---> BaseData
    void UpdateWidgets();           //BaseData ---> QWidget
    void showHelp(bool);
    void openLink();
    void openJsonWidget();

signals:
    void WidgetDirty(bool);
    void WidgetUpdate();
    void DataUpdate();
    void DataOwnerDirty(bool);
    void dataValueChanged(QString);

protected:
    core::objectmodel::BaseData* data_;
    ModifyObjectFlags flags_;
    QDisplayDataInfoWidget*  datainfowidget_;
    DataWidget* datawidget_;
    unsigned int numWidgets_;
    Q3ListViewItem* componentReference_;
    QSofaListView* listView_;
};



class QDataSimpleEdit : public DataWidget
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
    } QSimpleEdit;
public :
    QDataSimpleEdit(QWidget*, const char* name, core::objectmodel::BaseData*);
    virtual unsigned int numColumnWidget() {return 3;}
    virtual unsigned int sizeWidget() {return 1;}
    virtual bool createWidgets();
protected:
    virtual void setDataReadOnly(bool readOnly);
    virtual void readFromData();
    virtual void writeToData();
    virtual bool checkDirty();
    QSimpleEdit innerWidget_;
    QString lastValue;
};


class QDataJsonEdit : public DataWidget
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
    } QSimpleEdit;
public :
    QDataJsonEdit(QWidget*, const char* name, core::objectmodel::BaseData*);
    virtual unsigned int numColumnWidget() {return 3;}
    virtual unsigned int sizeWidget() {return 1;}
    virtual bool createWidgets();
    virtual void updateVisibility();
protected:
    virtual void setDataReadOnly(bool readOnly);
    virtual void readFromData();
    virtual void writeToData();
    virtual bool checkDirty();
    QSimpleEdit innerWidget_;
    QString lastValue;
    sofa::core::dataparser::JsonDataParser* m_parser;
};

class QPoissonRatioWidget : public TDataWidget<double>
{
    Q_OBJECT
public :
    QPoissonRatioWidget(QWidget*, const char*, core::objectmodel::Data<double>*);
    virtual bool createWidgets();

protected slots :
    void changeLineEditValue();
    void changeSliderValue();

protected:
    virtual void setDataReadOnly(bool readOnly);
    virtual void readFromData();
    virtual void writeToData();
    virtual bool checkDirty();
    QSlider* slider;
    QLineEdit* lineEdit;
    QString lastValue;
};





} // qt
} // gui
} //sofa

#endif // SOFA_GUI_QT_DISPLAYDATAWIDGET_H

