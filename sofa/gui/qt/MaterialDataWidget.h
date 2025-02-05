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
#ifndef MATERIAL_DATAWIDGET_H
#define MATERIAL_DATAWIDGET_H
#include "DataWidget.h"
#include <sofa/core/loader/Material.h>
#include <sofa/defaulttype/Vec.h>


#ifdef SOFA_QT4
#include <QColorDialog>
#include <QPainter>
#include <QStyle>
#include <QCheckBox>
#include <QComboBox>
#include <QColor>
#include <QPixmap>
#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>
#include <QColorDialog>
#else
#include <qcolor.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qcolordialog.h>
#include <qpixmap.h>
#include <qvalidator.h>
#include <qlineedit.h>
#include <qlabel.h>
#endif

namespace sofa
{
namespace gui
{
namespace qt
{

class RGBAColorPicker : public QWidget
{
    Q_OBJECT
signals:
    void hasChanged();
public:
    RGBAColorPicker(QWidget* parent);
    void setColor( const sofa::defaulttype::Vec4f& color );
    sofa::defaulttype::Vec4f getColor() const;
protected:
    QRgb _rgba;
    QLineEdit* _r;
    QLineEdit* _g;
    QLineEdit* _b;
    QLineEdit* _a;
    QPushButton* _colorButton;

protected slots:
    void updateRGBAColor();
    void redrawColorButton();
    void raiseQColorDialog();
};

class MaterialDataWidget : public TDataWidget<sofa::core::loader::Material>
{
    Q_OBJECT
public:
    MaterialDataWidget(QWidget* parent,
            const char* name,
            core::objectmodel::Data<sofa::core::loader::Material>* data):
        TDataWidget<sofa::core::loader::Material>(parent,name,data)
        , _nameEdit(NULL)
        , _ambientPicker(NULL)
        , _emissivePicker(NULL)
        , _specularPicker(NULL)
        , _diffusePicker(NULL)
        , _shininessEdit(NULL)
        , _ambientCheckBox(NULL)
        , _emissiveCheckBox(NULL)
        , _specularCheckBox(NULL)
        , _diffuseCheckBox(NULL)
        , _shininessCheckBox(NULL)
    {}

    virtual bool createWidgets();
    virtual unsigned int numColumnWidget() {return 1;}

protected:
    virtual void setDataReadOnly(bool readOnly);
    virtual bool checkDirty();
    virtual void readFromData();
    virtual void writeToData();
    QLineEdit* _nameEdit;
    QString _nameEditLastValue;
    RGBAColorPicker* _ambientPicker;
    sofa::defaulttype::Vec4f _ambientPickerLastValue;
    RGBAColorPicker* _emissivePicker;
    sofa::defaulttype::Vec4f _emissivePickerLastValue;
    RGBAColorPicker* _specularPicker;
    sofa::defaulttype::Vec4f _specularPickerLastValue;    
    RGBAColorPicker* _diffusePicker;
    sofa::defaulttype::Vec4f _diffusePickerLastValue;    
    QLineEdit*  _shininessEdit;
    QString     _shininessEditLastValue;    
    QCheckBox* _ambientCheckBox;
    bool       _ambientCheckBoxLastValue;
    QCheckBox* _emissiveCheckBox;
    bool       _emissiveCheckBoxLastValue;
    QCheckBox* _specularCheckBox;
    bool       _specularCheckBoxLastValue;
    QCheckBox* _diffuseCheckBox;
    bool       _diffuseCheckBoxLastValue;
    QCheckBox* _shininessCheckBox;
    bool       _shininessCheckBoxLastValue;
};


typedef helper::vector<sofa::core::loader::Material> VectorMaterial;
class VectorMaterialDataWidget : public TDataWidget< VectorMaterial >
{
    Q_OBJECT
public:
    VectorMaterialDataWidget(QWidget* parent,
            const char* name,
            core::objectmodel::Data< helper::vector<sofa::core::loader::Material> >* data):
        TDataWidget< helper::vector<sofa::core::loader::Material> >(parent,name,data),
        _materialDataWidget(NULL),
        _currentMaterial(0,data->isDisplayed(),data->isReadOnly()),
        _comboBox(NULL),
        _currentMaterialPos(0)
    {

    };

    virtual bool createWidgets();
    virtual unsigned int numColumnWidget() {return 1;}


protected:
    virtual void setDataReadOnly(bool readOnly);
    virtual bool checkDirty();
    virtual void readFromData();
    virtual void writeToData();

    MaterialDataWidget* _materialDataWidget;
    VectorMaterial _vectorEditedMaterial;
    core::objectmodel::Data<sofa::core::loader::Material> _currentMaterial;
    QComboBox* _comboBox;
    int _currentMaterialPos;
protected slots:
    void changeMaterial( int );
};
}
}


}

#endif

