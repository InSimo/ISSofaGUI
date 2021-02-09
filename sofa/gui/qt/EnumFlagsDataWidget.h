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
#include <sofa/helper/vector.h>

#include <QCheckBox>

namespace sofa
{
namespace gui
{
namespace qt
{

class EnumFlagsDataWidget : public DataWidget
{
    Q_OBJECT
public:
    EnumFlagsDataWidget(QWidget* parent,
                        const char* name,
                        core::objectmodel::BaseData* data):
        DataWidget(parent, name, data)
    {}

    template<class T>
    static T* create(T* instance, const CreatorArgument& arg)
    {
        if (std::strcmp(arg.data->getWidget(), "enum_flags")) return nullptr; // "enum_flags" widget name required since this widget does not use a specific type of Data
        return DataWidget::create(instance, arg);
    }

    virtual bool createWidgets() override;
    virtual unsigned int numColumnWidget() override { return _checkBoxes.size(); }

protected:
    const sofa::defaulttype::AbstractEnumTypeInfo* getAbstractEnumTypeInfo() const;

    virtual void setDataReadOnly(bool readOnly) override;
    virtual bool checkDirty() override;
    virtual void readFromData() override;
    virtual void writeToData() override;

    helper::vector<QCheckBox*> _checkBoxes;
    helper::vector<bool>       _checkBoxesLastValue;

public Q_SLOTS:
    void checkAll();
    void uncheckAll();
};

}
}
}

#endif

