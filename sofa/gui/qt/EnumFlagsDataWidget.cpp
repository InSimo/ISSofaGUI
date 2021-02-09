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
#include "EnumFlagsDataWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <cassert>

namespace sofa
{
namespace gui
{
namespace qt
{

#define MAX_COLS 5

helper::Creator<DataWidgetFactory, EnumFlagsDataWidget> DWClass_EnumFlags("enum_flags", false);

bool EnumFlagsDataWidget::createWidgets()
{
    const sofa::defaulttype::AbstractEnumTypeInfo* enumTypeInfo = getAbstractEnumTypeInfo();
    if (!enumTypeInfo)
    {
        std::cerr << "EnumFlagsDataWidget: no AbstractEnumTypeInfo for BaseData " << this->getBaseData()->getName() << " of type " << this->getBaseData()->getValueTypeString() << std::endl;
        return false;
    }

    // find the number of items to have an idea of how many flags there is
    std::vector<std::string> items;
    enumTypeInfo->getAvailableItems(this->getBaseData(), items);
    std::reverse(items.begin(), items.end());

    // find all possible flags
    auto e = enumTypeInfo->createInstance();
    for (std::size_t i = 0u; i < items.size(); ++i)
    {
        enumTypeInfo->setDataValueInteger(e.get(), 1 << i);
        if (enumTypeInfo->getDataValueInteger(e.get()) != 1 << i) continue; // not a valid enumeration value
        const std::string s = enumTypeInfo->getDataEnumeratorString(e.get());

        _checkBoxes.emplace_back(new QCheckBox(this));
    }
    _checkBoxesLastValue.resize(_checkBoxes.size());

    // create widgets corresponding to each flag
    QVBoxLayout* layout = new QVBoxLayout(this);
    QGridLayout* grid = new QGridLayout(2 * std::ceil(static_cast<double>(_checkBoxes.size()) / MAX_COLS), std::min(static_cast<int>(_checkBoxes.size()), MAX_COLS));

    for (std::size_t i = 0u; i < _checkBoxes.size(); ++i)
    {
        enumTypeInfo->setDataValueInteger(e.get(), 1 << i);
        const std::string s = enumTypeInfo->getDataEnumeratorString(e.get());
        grid->addWidget(new QLabel(QString::fromStdString(s), this), 2 * (i / MAX_COLS), i % MAX_COLS, Qt::AlignHCenter);
        grid->addWidget(_checkBoxes[i], 1 + 2 * (i / MAX_COLS), i % MAX_COLS, Qt::AlignHCenter);

        connect(_checkBoxes[i], SIGNAL(toggled(bool)), this, SLOT(setWidgetDirty()));
    }

    layout->addLayout(grid);

    // add buttons to check or uncheck all checkboxes
    QHBoxLayout* hlayout = new QHBoxLayout();
    QPushButton* checkBtn1 = new QPushButton("Check all", this);
    hlayout->add(checkBtn1);
    connect(checkBtn1, SIGNAL(clicked()), this, SLOT(checkAll()));
    QPushButton* checkBtn2 = new QPushButton("Uncheck all", this);
    hlayout->add(checkBtn2);
    connect(checkBtn2, SIGNAL(clicked()), this, SLOT(uncheckAll()));

    layout->addLayout(hlayout);

    readFromData();

    return true;
}

const sofa::defaulttype::AbstractEnumTypeInfo* EnumFlagsDataWidget::getAbstractEnumTypeInfo() const
{
    const sofa::defaulttype::AbstractTypeInfo* typeInfo = this->getBaseData()->getValueTypeInfo();
    assert(typeInfo->IsEnum());
    return typeInfo->EnumType();
}

void EnumFlagsDataWidget::setDataReadOnly(bool readOnly)
{
    for (QCheckBox* cb : _checkBoxes)
    {
        cb->setEnabled(!readOnly);
    }
}

bool EnumFlagsDataWidget::checkDirty()
{
    return true;
}

void EnumFlagsDataWidget::readFromData()
{
    const sofa::defaulttype::AbstractEnumTypeInfo* enumTypeInfo = getAbstractEnumTypeInfo();
    const long long value = enumTypeInfo->getDataValueInteger(this->getBaseData()->getValueVoidPtr());

    for (std::size_t i = 0u; i < _checkBoxes.size(); ++i)
    {
        const bool flag = value & (1 << i);

        _checkBoxesLastValue[i] = flag;
        _checkBoxes[i]->setChecked(flag);
    }
}

void EnumFlagsDataWidget::writeToData()
{
    const sofa::defaulttype::AbstractEnumTypeInfo* enumTypeInfo = getAbstractEnumTypeInfo();
    long long value = 0;

    for (std::size_t i = 0u; i < _checkBoxes.size(); ++i)
    {
        const bool flag = _checkBoxes[i]->isChecked();
        _checkBoxesLastValue[i] = flag;
        if (flag)
        {
            value |= (1 << i);
        }
    }

    // the following does not work for enum flags as the given value does not exactly equal an enum value
    //enumTypeInfo->setDataValueInteger(this->getBaseData()->beginEditVoidPtr(), value);
    //this->getBaseData()->endEditVoidPtr();

    // directly write the underlying type of the enum to circumvent this issue
    const sofa::defaulttype::AbstractTypeInfo* valueTypeInfo = enumTypeInfo->getFinalValueType();
    assert(valueTypeInfo->IsSingleValue());
    valueTypeInfo->SingleValueType()->setFinalValueInteger(this->getBaseData()->beginEditVoidPtr(), 0, value);
    this->getBaseData()->endEditVoidPtr();
}

void EnumFlagsDataWidget::checkAll()
{
    for (QCheckBox* cb : _checkBoxes)
    {
        cb->setChecked(true);
    }
}

void EnumFlagsDataWidget::uncheckAll()
{
    for (QCheckBox* cb : _checkBoxes)
    {
        cb->setChecked(false);
    }
}

}
}
}
