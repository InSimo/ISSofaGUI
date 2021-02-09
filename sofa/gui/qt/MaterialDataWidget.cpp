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
#include "MaterialDataWidget.h"
#include <limits>

#ifndef SOFA_QT4
#include <qcombobox.h>
#endif

namespace sofa
{
namespace gui
{
namespace qt
{

helper::Creator<DataWidgetFactory,MaterialDataWidget> DWClass_MeshMaterial("default",true);
helper::Creator<DataWidgetFactory,VectorMaterialDataWidget> DWClass_MeshVectorMaterial("default",true);
RGBAColorPicker::RGBAColorPicker(QWidget* parent):QWidget(parent)
{

    _r = new QLineEdit(this);
    _r->setValidator(new QIntValidator(0,255,this));
    _g = new QLineEdit(this);
    _g->setValidator(new QIntValidator(0,255,this));
    _b = new QLineEdit(this);
    _b->setValidator(new QIntValidator(0,255,this));
    _a = new QLineEdit(this);
    _a->setValidator(new QIntValidator(0,255,this));
    _colorButton = new QPushButton(this);
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->add(_colorButton);
    layout->add(new QLabel("r",this));
    layout->add(_r);
    layout->add(new QLabel("g",this));
    layout->add(_g);
    layout->add(new QLabel("b",this));
    layout->add(_b);
    layout->add(new QLabel("a",this));
    layout->add(_a);
    connect( _r , SIGNAL( textChanged(const QString & ) ) ,this , SIGNAL( hasChanged() ) );
    connect( _g , SIGNAL( textChanged(const QString & ) ) ,this , SIGNAL( hasChanged() ) );
    connect( _b , SIGNAL( textChanged(const QString & ) ) ,this , SIGNAL( hasChanged() ) );
    connect( _a , SIGNAL( textChanged(const QString & ) ) ,this , SIGNAL( hasChanged() ) );
    connect( _r , SIGNAL( returnPressed() ) ,this , SLOT( updateRGBAColor() ) );
    connect( _g , SIGNAL( returnPressed() ) ,this , SLOT( updateRGBAColor() ) );
    connect( _b , SIGNAL( returnPressed() ) ,this , SLOT( updateRGBAColor() ) );
    connect( _a , SIGNAL( returnPressed() ) ,this , SLOT( updateRGBAColor() ) );
    connect( _r , SIGNAL( lostFocus() ) ,this , SLOT( updateRGBAColor() ) );
    connect( _g , SIGNAL( lostFocus() ) ,this , SLOT( updateRGBAColor() ) );
    connect( _b , SIGNAL( lostFocus() ) ,this , SLOT( updateRGBAColor() ) );
    connect( _a , SIGNAL( lostFocus() ) ,this , SLOT( updateRGBAColor() ) );
    connect( _colorButton, SIGNAL( clicked() ), this, SLOT( raiseQColorDialog() ) );

}

defaulttype::Vec4f RGBAColorPicker::getColor() const
{
    typedef unsigned char uchar;
    const uchar max = std::numeric_limits<uchar>::max();
    defaulttype::Vec4f color;
    float r = _r->text().toFloat();
    float g = _g->text().toFloat();
    float b = _b->text().toFloat();
    float a = _a->text().toFloat();
    r /= max;
    g /= max;
    b /= max;
    a /= max;

    color[0] = r;
    color[1] = g;
    color[2] = b;
    color[3] = a;
    return color;



}

void RGBAColorPicker::updateRGBAColor()
{
    typedef unsigned char uchar;
    defaulttype::Vec4f color;
    const uchar r = _r->text().toInt();
    const uchar g = _g->text().toInt();
    const uchar b = _b->text().toInt();
    const uchar a = _a->text().toInt();
    _rgba = qRgba(r,g,b,a);
    redrawColorButton();
}

void RGBAColorPicker::setColor(const sofa::defaulttype::Vec4f& color)
{
    typedef unsigned char uchar;
    const uchar max = std::numeric_limits<uchar>::max();
    const uchar r = uchar(  max * color[0] );
    const uchar g = uchar(  max * color[1] );
    const uchar b = uchar(  max * color[2] );
    const uchar a = uchar(  max * color[3] );
    QString str;
    str.setNum(r);
    _r->setText(str);
    str.setNum(g);
    _g->setText(str);
    str.setNum(b);
    _b->setText(str);
    str.setNum(a);
    _a->setText(str);
    _rgba = qRgba(r,g,b,a);

    redrawColorButton();

}

void RGBAColorPicker::redrawColorButton()
{
    int w=_colorButton->width();
    int h=_colorButton->height();

    QPixmap *pix=new QPixmap(25,20);
    pix->fill(QColor(qRed(_rgba), qGreen(_rgba), qBlue(_rgba)));
    _colorButton->setPixmap(*pix);

    _colorButton->resize(w,h);
}

void RGBAColorPicker::raiseQColorDialog()
{
    typedef unsigned char uchar;
    const uchar max = std::numeric_limits<uchar>::max();
    int r,g,b,a;
    bool ok;
    defaulttype::Vec4f color;
    QColor qcolor = QColorDialog::getRgba(_rgba,&ok,this);
    if( ok )
    {
        QRgb rgba=qcolor.rgb();
        r=qRed(rgba);
        g=qGreen(rgba);
        b=qBlue(rgba);
        a=qAlpha(rgba);
        color[0] = (float)r / max;
        color[1] = (float)g / max;
        color[2] = (float)b / max;
        color[3] = (float)a / max;
        setColor(color);
        emit hasChanged();
    }
}

bool MaterialDataWidget::createWidgets()
{

    _nameEdit = new QLineEdit(this);
    _ambientPicker = new RGBAColorPicker(this);
    _ambientCheckBox = new QCheckBox(this);
    _emissivePicker = new RGBAColorPicker(this);
    _emissiveCheckBox = new QCheckBox(this);
    _specularPicker = new RGBAColorPicker(this);
    _specularCheckBox = new QCheckBox(this);
    _diffusePicker = new RGBAColorPicker(this);
    _diffuseCheckBox = new QCheckBox(this);
    _shininessEdit = new QLineEdit(this);
    _shininessEdit->setValidator(new QDoubleValidator(this) );
    _shininessCheckBox = new QCheckBox(this);
    QVBoxLayout* layout = new QVBoxLayout(this);

    QGridLayout* grid = new QGridLayout(5,3);
    grid->setSpacing(1);
    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->add(new QLabel("Name",this));
    hlayout->add(_nameEdit);

    grid->addWidget(_ambientCheckBox,0,0,Qt::AlignHCenter);
    grid->addWidget(new QLabel("Ambient",this),0,1,Qt::AlignHCenter);
    grid->addWidget(_ambientPicker,0,2,Qt::AlignHCenter);

    grid->addWidget(_emissiveCheckBox,1,0,Qt::AlignHCenter);
    grid->addWidget(new QLabel("Emissive",this),1,1,Qt::AlignHCenter);
    grid->addWidget(_emissivePicker,1,2,Qt::AlignHCenter);

    grid->addWidget(_diffuseCheckBox,2,0,Qt::AlignHCenter);
    grid->addWidget(new QLabel("Diffuse",this),2,1,Qt::AlignHCenter);
    grid->addWidget(_diffusePicker,2,2,Qt::AlignHCenter);

    grid->addWidget(_specularCheckBox,3,0,Qt::AlignHCenter);
    grid->addWidget(new QLabel("Specular",this),3,1,Qt::AlignHCenter);
    grid->addWidget(_specularPicker,3,2,Qt::AlignHCenter);

    grid->addWidget(_shininessCheckBox,4,0,Qt::AlignHCenter);
    grid->addWidget(new QLabel("Shininess",this),4,1,Qt::AlignHCenter);
    grid->addWidget(_shininessEdit,4,2,Qt::AlignHCenter);

    layout->addLayout(hlayout);
    layout->addLayout(grid);


    connect(_nameEdit, SIGNAL( textChanged( const QString & ) ), this , SLOT( setWidgetDirty() ) );
    connect(_shininessEdit, SIGNAL( textChanged( const QString & ) ), this , SLOT( setWidgetDirty() ) );

    connect(_ambientCheckBox, SIGNAL( toggled( bool ) ), this , SLOT( setWidgetDirty() ) );
    connect(_ambientCheckBox, SIGNAL( toggled( bool ) ), _ambientPicker , SLOT( setEnabled(bool ) ) );

    connect(_emissiveCheckBox, SIGNAL( toggled( bool ) ), this , SLOT( setWidgetDirty() ) );
    connect(_emissiveCheckBox, SIGNAL( toggled( bool ) ), _emissivePicker, SLOT( setEnabled(bool) ) );

    connect(_specularCheckBox, SIGNAL( toggled( bool ) ), this , SLOT( setWidgetDirty() ) );
    connect(_specularCheckBox, SIGNAL( toggled( bool ) ), _specularPicker, SLOT( setEnabled(bool) ) );

    connect(_diffuseCheckBox, SIGNAL( toggled( bool ) ), this , SLOT( setWidgetDirty() ) );
    connect(_diffuseCheckBox, SIGNAL( toggled( bool ) ), _diffusePicker, SLOT( setEnabled(bool) ) );

    connect(_shininessCheckBox, SIGNAL( toggled( bool ) ), this, SLOT( setWidgetDirty() ) );
    connect(_shininessCheckBox, SIGNAL( toggled( bool ) ), _shininessEdit, SLOT( setEnabled(bool) ) );

    connect(_ambientPicker, SIGNAL( hasChanged() ), this, SLOT( setWidgetDirty() ) );
    connect(_emissivePicker, SIGNAL( hasChanged() ), this, SLOT( setWidgetDirty() ) );
    connect(_specularPicker, SIGNAL( hasChanged() ), this, SLOT( setWidgetDirty() ) );
    connect(_diffusePicker, SIGNAL( hasChanged() ), this, SLOT( setWidgetDirty() ) );

    readFromData();

    return true;
}
void MaterialDataWidget::setDataReadOnly(bool readOnly)
{
    _nameEdit->setReadOnly(readOnly);
    _ambientPicker->setEnabled(!readOnly);
    _ambientCheckBox->setEnabled(!readOnly);
    _emissivePicker->setEnabled(!readOnly);
    _emissiveCheckBox->setEnabled(!readOnly);
    _specularPicker->setEnabled(!readOnly);
    _specularCheckBox->setEnabled(!readOnly);
    _diffusePicker->setEnabled(!readOnly);
    _diffuseCheckBox->setEnabled(!readOnly);
    _shininessEdit->setReadOnly(readOnly);
    _shininessCheckBox->setEnabled(!readOnly);
}

bool MaterialDataWidget::checkDirty()
{
    return true;
}

void MaterialDataWidget::readFromData()
{
    using namespace sofa::core::loader;
    const Material& material = getData()->virtualGetValue();
    _nameEditLastValue = QString( material.name.c_str() );
    _ambientCheckBoxLastValue = material.useAmbient;
    _emissiveCheckBoxLastValue = material.useEmissive;
    _diffuseCheckBoxLastValue = material.useDiffuse;
    _specularCheckBoxLastValue = material.useSpecular;
    _shininessCheckBoxLastValue = material.useShininess;
    _shininessEditLastValue.setNum(material.shininess);

    _ambientPickerLastValue = ( material.ambient );
    _emissivePickerLastValue = ( material.emissive );
    _specularPickerLastValue = ( material.specular );
    _diffusePickerLastValue = ( material.diffuse );

    _nameEdit->setText( _nameEditLastValue );
    _ambientCheckBox->setChecked( _ambientCheckBoxLastValue );
    _emissiveCheckBox->setChecked( _emissiveCheckBoxLastValue );
    _diffuseCheckBox->setChecked( _diffuseCheckBoxLastValue );
    _specularCheckBox->setChecked( _specularCheckBoxLastValue );
    _shininessCheckBox->setChecked( _shininessCheckBoxLastValue );
    _shininessEdit->setText( _shininessEditLastValue );

    _ambientPicker->setColor( _ambientPickerLastValue );
    _emissivePicker->setColor( _emissivePickerLastValue );
    _specularPicker->setColor( _specularPickerLastValue );
    _diffusePicker->setColor( _diffusePickerLastValue );

    _ambientPicker->setEnabled( _ambientCheckBox->isChecked() );
    _emissivePicker->setEnabled( _emissiveCheckBox->isChecked() );
    _specularPicker->setEnabled( _specularCheckBox->isChecked() );
    _diffusePicker->setEnabled( _diffuseCheckBox->isChecked() );
}
void MaterialDataWidget::writeToData()
{
    using namespace sofa::core::loader;
    Material* material = getData()->virtualBeginEdit();

    _nameEditLastValue = _nameEdit->text();
    _ambientPickerLastValue     = _ambientPicker->getColor();
    _diffusePickerLastValue     = _diffusePicker->getColor();
    _emissivePickerLastValue    = _emissivePicker->getColor();
    _specularPickerLastValue    = _specularPicker->getColor();
    _shininessEditLastValue     = _shininessEdit->text();
    _ambientCheckBoxLastValue   = _ambientCheckBox->isChecked();
    _diffuseCheckBoxLastValue   = _diffuseCheckBox->isChecked();
    _shininessCheckBoxLastValue = _shininessCheckBox->isChecked();
    _emissiveCheckBoxLastValue  = _emissiveCheckBox->isChecked();
    _specularCheckBoxLastValue  = _specularCheckBox->isChecked();

    material->name         = _nameEditLastValue.ascii();
    material->ambient      = _ambientPickerLastValue;
    material->diffuse      = _diffusePickerLastValue;
    material->emissive     = _emissivePickerLastValue;
    material->specular     = _specularPickerLastValue;
    material->shininess    = _shininessEditLastValue.toFloat();
    material->useAmbient   = _ambientCheckBoxLastValue;
    material->useDiffuse   = _diffuseCheckBoxLastValue;
    material->useShininess = _shininessCheckBoxLastValue;
    material->useEmissive  = _emissiveCheckBoxLastValue;
    material->useSpecular  = _specularCheckBoxLastValue;
    
    getData()->virtualEndEdit();
}


bool VectorMaterialDataWidget::createWidgets()
{
    if( getData()->virtualGetValue().empty() )
    {
        return false;
    }
    _comboBox = new QComboBox(this);
    _materialDataWidget = new MaterialDataWidget(this,this->name(),&_currentMaterial);
    _materialDataWidget->createWidgets();
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->add(_comboBox);
    layout->add(_materialDataWidget);

    connect( _comboBox, SIGNAL(activated(int)  ), this, SLOT( changeMaterial( int) ) );
    connect( _materialDataWidget, SIGNAL( WidgetDirty(bool) ) ,this, SLOT( setWidgetDirty(bool) ) );

    readFromData();

    return true;
}

void VectorMaterialDataWidget::setDataReadOnly(bool readOnly)
{
    if (_materialDataWidget)
        _materialDataWidget->setReadOnly(readOnly);
}

void VectorMaterialDataWidget::readFromData()
{
    VectorMaterial::const_iterator iter;
    const VectorMaterial& vecMaterial = getData()->virtualGetValue();
    if( vecMaterial.empty() )
    {
        return;
    }
    _comboBox->clear();
    _vectorEditedMaterial.clear();
    std::copy(vecMaterial.begin(), vecMaterial.end(), std::back_inserter(_vectorEditedMaterial) );
    for( iter = _vectorEditedMaterial.begin(); iter != _vectorEditedMaterial.end(); ++iter )
    {
        _comboBox->insertItem ( QString( (*iter).name.c_str() ) );
    }
    _currentMaterialPos = 0;
    _comboBox->setCurrentItem(_currentMaterialPos);
    _currentMaterial.setValue(_vectorEditedMaterial[_currentMaterialPos]);
    _materialDataWidget->setData(&_currentMaterial);
    _materialDataWidget->updateWidgetValue();
}

void VectorMaterialDataWidget::changeMaterial( int index )
{
    using namespace sofa::core::loader;

    //Save previous Material
    _materialDataWidget->updateDataValue();
    Material mat(_currentMaterial.virtualGetValue() );
    _vectorEditedMaterial[_currentMaterialPos] = mat;

    //Update current Material
    _currentMaterialPos = index;
    _currentMaterial.setValue(_vectorEditedMaterial[index]);

    //Update Widget
    _materialDataWidget->setData(&_currentMaterial);
    _materialDataWidget->updateWidgetValue();
}

void VectorMaterialDataWidget::writeToData()
{
    using namespace sofa::core::loader;

    _materialDataWidget->updateDataValue();
    Material mat(_currentMaterial.virtualGetValue() );
    _vectorEditedMaterial[_currentMaterialPos] = mat;

    VectorMaterial* vecMaterial = getData()->virtualBeginEdit();
    assert(vecMaterial->size() == _vectorEditedMaterial.size() );
    std::copy(_vectorEditedMaterial.begin(), _vectorEditedMaterial.end(), vecMaterial->begin() );

    getData()->virtualEndEdit();

}

bool VectorMaterialDataWidget::checkDirty()
{
    return true;
}

}
}
}
