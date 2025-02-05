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
#include "QDisplayDataWidget.h"

#include <QTableWidget>

#include <QPalette>
#ifdef SOFA_QT4
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <Q3GroupBox>
#include <QLabel>
#include <QValidator>
#else
#include <qlayout.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qvalidator.h>
#endif

#define TEXTSIZE_THRESHOLD 45

namespace sofa
{

using namespace core::objectmodel;

namespace gui
{
namespace qt
{
QDisplayDataWidget::QDisplayDataWidget(QWidget* parent,
        BaseData* data,
        const ModifyObjectFlags& flags,
        Q3ListViewItem* componentReference, 
        QSofaListView* listView):Q3GroupBox(parent),
    data_(data),
    flags_(flags),
    datainfowidget_(NULL),
    datawidget_(NULL),
    numWidgets_(0),
    listView_(listView),
    componentReference_(componentReference)
{
    setAutoFillBackground(true);

    if(data_ == NULL)
        return;

    const std::string& name = data_->getName();
    const std::string help = (std::string(data_->getHelp()) != "TODO") ? std::string(data_->getHelp()) : std::string("");
    const std::string valuetype = data_->getValueTypeString();

#ifndef SOFAGUIQT_COMPACT
    if(!flags.PROPERTY_WIDGET_FLAG)
    {
        setTitle(name.c_str());
        setInsideMargin(4);
        setInsideSpacing(2);
    }

    const std::string label_text = help;
    const std::string popup_text = valuetype;
#else
    if(!flags.PROPERTY_WIDGET_FLAG)
    {
        setTitle(name.c_str());
        setInsideMargin(0);
        setInsideSpacing(0);
        //setMargin(0);
        //setFlat(true);
        //setStyleSheet("QGroupBox{border:0;}");
        
    }
    const std::string label_text = help; // = name;
    std::string popup_text = help;
    if (!valuetype.empty())
    {
        if (!popup_text.empty()) popup_text += '\n';
        popup_text += valuetype;
    }
#endif

    if (!label_text.empty())
    {
        datainfowidget_ = new QDisplayDataInfoWidget(this,label_text,data_,flags.LINKPATH_MODIFIABLE_FLAG);
#ifndef SOFAGUIQT_COMPACT
        numWidgets_ += datainfowidget_->getNumLines()/3;
#else
        datainfowidget_->setVisible(false);
#endif
    }


    if (!popup_text.empty())
        setToolTip(popup_text.c_str());

    DataWidget::CreatorArgument dwarg;
    dwarg.name =  data_->getName();
    dwarg.data = data_;
    dwarg.parent = this;
    dwarg.readOnly = (data_->isReadOnly() && flags.READONLY_FLAG);

    if( dynamic_cast<core::objectmodel::DataFileName*>(data_) != NULL )
    {
        // a bit of a hack for DataFileName widgets.
        // A custom widget is used by default if we run this code from the Modeler

        std::string widgetName=data_->getWidget();
        if( widgetName.empty() && flags.MODELER_FLAG )
        {
            data_->setWidget("widget_filename");
        }
    }

    datawidget_= DataWidget::CreateDataWidget(dwarg);

    if (datawidget_ == 0)
    {
        datawidget_ = new QDataSimpleEdit(this,dwarg.data->getName().c_str(), dwarg.data);
        datawidget_->createWidgets();
        datawidget_->setReadOnly(dwarg.readOnly);
        assert(datawidget_ != NULL);
    }
    datawidget_->setContentsMargins(0, 0, 0, 0);

    if (componentReference != nullptr)
    {
        QPushButton *link = new QPushButton("&Link", this);
        this->layout()->add(link);
        connect(link, SIGNAL(clicked()), this, SLOT(openLink()));
    }

    const sofa::defaulttype::AbstractTypeInfo *typeinfo = data->getValueTypeInfo();
    if (typeinfo->ValidInfo())
    {
        QPushButton *json_widget = new QPushButton("", this);
        json_widget->setPixmap(style()->standardPixmap(QStyle::SP_FileDialogDetailedView));
        json_widget->setFixedSize(QSize(30, 30));
        this->datawidget_->layout()->add(json_widget);
        connect(json_widget, SIGNAL(clicked()), this, SLOT(openJsonWidget()));
    }

    //std::cout << "WIDGET created for data " << dwarg.data << " : " << dwarg.name << " : " << dwarg.data->getValueTypeString() << std::endl;
    numWidgets_ += datawidget_->sizeWidget();
    connect(datawidget_,SIGNAL(WidgetDirty(bool)), this, SIGNAL ( WidgetDirty(bool) ) );
    connect(this, SIGNAL( WidgetUpdate() ), datawidget_, SLOT( updateWidgetValue() ) );
    connect(this, SIGNAL( DataUpdate() ), datawidget_, SLOT(updateDataValue() ) );
    connect(datawidget_,SIGNAL(DataOwnerDirty(bool)),this,SIGNAL(DataOwnerDirty(bool)) );
    connect(datawidget_,SIGNAL(dataValueChanged(QString)),this,SIGNAL(dataValueChanged(QString)) );
    if(flags.PROPERTY_WIDGET_FLAG)
    {
        QPushButton *refresh = new QPushButton(QIcon((sofa::helper::system::DataRepository.getFirstPath() + "/textures/refresh.png").c_str()), "", this);
        refresh->setHidden(true);
        refresh->setFixedSize(QSize(16, 16));

        ++numWidgets_;

        connect(datawidget_,SIGNAL(WidgetDirty(bool)), refresh, SLOT ( setVisible(bool) ) );
        connect(refresh, SIGNAL(clicked()), this, SLOT(UpdateData()));
        connect(refresh, SIGNAL(clicked(bool)), refresh, SLOT(setVisible(bool)));

        setStyleSheet("QGroupBox{border:0;}");
        setInsideMargin(0);
        setInsideSpacing(0);
        /*setMargin(0);
        setLineWidth(0);
        setMidLineWidth(0);*/

        setColumns(numWidgets_ + 1);
    }
    else
    {
        setColumns(datawidget_->numColumnWidget());
    }
}

void QDisplayDataWidget::UpdateData()
{
    emit DataUpdate();
}

void QDisplayDataWidget::UpdateWidgets()
{
    emit WidgetUpdate();
}

void QDisplayDataWidget::showHelp(bool v)
{
    //const std::string& name = data_->getName();
    const std::string help = (std::string(data_->getHelp()) != "TODO") ? std::string(data_->getHelp()) : std::string("");
    const std::string valuetype = data_->getValueTypeString();

    if (datainfowidget_)
    {
        datainfowidget_->setVisible(v);
    }

    std::string popup_text;
    if (v)
    {
        popup_text = valuetype;
    }
    else
    {
        popup_text = help;
        if (!valuetype.empty())
        {
            if (!popup_text.empty()) popup_text += '\n';
            popup_text += valuetype;
        }
    }

    if (!popup_text.empty())
        setToolTip(popup_text.c_str());

    this->adjustSize();
}

void QDisplayDataWidget::openLink()
{
    if (componentReference_ != nullptr && listView_ != nullptr)
    {
        Q3ListViewItem* item = listView_->currentItem();
        listView_->setCurrentItem(componentReference_);
        listView_->updateMatchingObjectmodel(componentReference_);
        listView_->RunSofaDoubleClicked(componentReference_);
        listView_->setCurrentItem(item);
    }
}

void QDisplayDataWidget::openJsonWidget()
{
    QDataJsonEdit* jsonWidget = new QDataJsonEdit(NULL, this->data_->getName().c_str(), this->data_);
    jsonWidget->createWidgets();
    connect(jsonWidget,SIGNAL(WidgetDirty(bool)), this, SIGNAL ( WidgetDirty(bool) ) );
    connect(this, SIGNAL( WidgetUpdate() ), jsonWidget, SLOT( updateWidgetValue() ) );
    connect(jsonWidget,SIGNAL(DataOwnerDirty(bool)),this,SIGNAL(DataOwnerDirty(bool)) );
    connect(jsonWidget,SIGNAL(dataValueChanged(QString)),this,SIGNAL(dataValueChanged(QString)) );
    connect(this, SIGNAL(destroyed()), jsonWidget, SLOT(close()));

    jsonWidget->setWindowTitle((this->data_->getOwner()->getName() + "  " + this->data_->getName()).c_str());
    jsonWidget->show();
}

QDataSimpleEdit::QDataSimpleEdit(QWidget* parent, const char* name, BaseData* data):
    DataWidget(parent,name,data)
{
}
bool QDataSimpleEdit::createWidgets()
{
    lastValue = QString( getBaseData()->getValueString().c_str() );
    QLayout* layout = new QHBoxLayout(this);
    if( lastValue.length() > TEXTSIZE_THRESHOLD )
    {
        innerWidget_.type = TEXTEDIT;
        innerWidget_.widget.textEdit = new QTextEdit(this); innerWidget_.widget.textEdit->setText(lastValue);
        connect(innerWidget_.widget.textEdit , SIGNAL( textChanged() ), this, SLOT ( setWidgetDirty() ) );
        layout->add(innerWidget_.widget.textEdit);
    }
    else
    {
        innerWidget_.type = LINEEDIT;
        innerWidget_.widget.lineEdit  = new QLineEdit(this);
        innerWidget_.widget.lineEdit->setText(lastValue);
        connect( innerWidget_.widget.lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT( setWidgetDirty() ) );
        layout->add(innerWidget_.widget.lineEdit);
    }




    return true;
}

void QDataSimpleEdit::setDataReadOnly(bool readOnly)
{
    if(innerWidget_.type == TEXTEDIT)
    {
        innerWidget_.widget.textEdit->setReadOnly(readOnly);
    }
    else if(innerWidget_.type == LINEEDIT)
    {
        innerWidget_.widget.lineEdit->setReadOnly(readOnly);
    }
}

void QDataSimpleEdit::readFromData()
{
    lastValue = QString( getBaseData()->getValueString().c_str() );
    if(innerWidget_.type == TEXTEDIT)
    {
        innerWidget_.widget.textEdit->setText(lastValue);
    }
    else if(innerWidget_.type == LINEEDIT)
    {
        innerWidget_.widget.lineEdit->setText(lastValue);
    }
}

void QDataSimpleEdit::writeToData()
{
    if(getBaseData())
    {
        if( innerWidget_.type == TEXTEDIT)
        {
            lastValue = innerWidget_.widget.textEdit->text().ascii();
        }
        else if( innerWidget_.type == LINEEDIT)
        {
            lastValue = innerWidget_.widget.lineEdit->text().ascii();
        }
        std::string value = lastValue.ascii();
        getBaseData()->read(value);
    }
}

bool QDataSimpleEdit::checkDirty()
{
    if( innerWidget_.type == TEXTEDIT)
    {
        return lastValue != innerWidget_.widget.textEdit->text();
    }
    else if( innerWidget_.type == LINEEDIT)
    {
        return lastValue != innerWidget_.widget.lineEdit->text();
    }
    return false;
}

/*json widget */
QDataJsonEdit::QDataJsonEdit(QWidget* parent, const char* name, BaseData* data)
   : DataWidget(parent,name,data)
{
    m_parser =  new sofa::core::dataparser::JsonDataParser(data->getName());
}

void QDataJsonEdit::updateVisibility()
{
}

bool QDataJsonEdit::createWidgets()
{
    const sofa::defaulttype::AbstractTypeInfo *typeinfo = getBaseData()->getValueTypeInfo();
    const void* valueVoidPtr = getBaseData()->getValueVoidPtr();
    std::string output;
    m_parser->fromData(output, valueVoidPtr, typeinfo);
    lastValue = QString( output.c_str() );
    QLayout* layout = new QHBoxLayout(this);
    if( lastValue.length() > TEXTSIZE_THRESHOLD )
    {
        innerWidget_.type = TEXTEDIT;
        innerWidget_.widget.textEdit = new QTextEdit(this); innerWidget_.widget.textEdit->setText(lastValue);
        connect(innerWidget_.widget.textEdit , SIGNAL( textChanged() ), this, SLOT ( setWidgetDirty() ) );
        layout->add(innerWidget_.widget.textEdit);
    }
    else
    {
        innerWidget_.type = LINEEDIT;
        innerWidget_.widget.lineEdit  = new QLineEdit(this);
        innerWidget_.widget.lineEdit->setText(lastValue);
        connect( innerWidget_.widget.lineEdit, SIGNAL(textChanged(const QString&)), this, SLOT( setWidgetDirty() ) );
        layout->add(innerWidget_.widget.lineEdit);
    }

    QPushButton* button = new QPushButton( this, "buttonEdit" );
    button->setText("&Edit");
    this->layout()->add(button);
    connect(button, SIGNAL(clicked()), this, SLOT(updateDataValue()));

    return true;
}

void QDataJsonEdit::setDataReadOnly(bool readOnly)
{
    if(innerWidget_.type == TEXTEDIT)
    {
        innerWidget_.widget.textEdit->setReadOnly(readOnly);
    }
    else if(innerWidget_.type == LINEEDIT)
    {
        innerWidget_.widget.lineEdit->setReadOnly(readOnly);
    }
}

void QDataJsonEdit::readFromData()
{
    const sofa::defaulttype::AbstractTypeInfo *typeinfo = getBaseData()->getValueTypeInfo();
    const void* valueVoidPtr = getBaseData()->getValueVoidPtr();
    std::string output;
    m_parser->fromData(output, valueVoidPtr, typeinfo);
    lastValue = QString( output.c_str() );

    if(innerWidget_.type == TEXTEDIT)
    {
        innerWidget_.widget.textEdit->setText(lastValue);
    }
    else if(innerWidget_.type == LINEEDIT)
    {
        innerWidget_.widget.lineEdit->setText(lastValue);
    }
}

void QDataJsonEdit::writeToData()
{
    if(getBaseData())
    {
        if( innerWidget_.type == TEXTEDIT)
        {
            lastValue = innerWidget_.widget.textEdit->text().ascii();
        }
        else if( innerWidget_.type == LINEEDIT)
        {
            lastValue = innerWidget_.widget.lineEdit->text().ascii();
        }
        std::string value = lastValue.ascii();
        const sofa::defaulttype::AbstractTypeInfo *typeinfo = getBaseData()->getValueTypeInfo();
        void* editVoidPtr = getBaseData()->beginEditVoidPtr();
        m_parser->toData(value, editVoidPtr, typeinfo);
        getBaseData()->endEditVoidPtr();
    }
}

bool QDataJsonEdit::checkDirty()
{
    if( innerWidget_.type == TEXTEDIT)
    {
        return lastValue != innerWidget_.widget.textEdit->text();
    }
    else if( innerWidget_.type == LINEEDIT)
    {
        return lastValue != innerWidget_.widget.lineEdit->text();
    }
    return false;
}

/* QPoissonRatioWidget */

QPoissonRatioWidget::QPoissonRatioWidget(QWidget * parent, const char * name, sofa::core::objectmodel::Data<double> *data)
    :TDataWidget<double>(parent,name,data)
{

}


bool QPoissonRatioWidget::createWidgets()
{
    QGridLayout* layout = new QGridLayout(this,2,3);

    lineEdit = new QLineEdit(this);
    lastValue = QString("-1.0");
    lineEdit->setText(lastValue);
    lineEdit->setMaximumSize(lineEdit->size());
    lineEdit->setAlignment(Qt::AlignHCenter);

    lineEdit->setValidator(new QDoubleValidator(0.0,0.5,2,this));

    layout->addWidget(lineEdit,0,1,Qt::AlignHCenter);
    QLabel* min = new QLabel(this);
    min->setText(QString("0.0"));
    min->setMaximumSize( min->sizeHint() );
    layout->addWidget(min,1,0,Qt::AlignHCenter);

    slider = new QSlider(Qt::Horizontal, this);
    slider->setRange(0,50); //max times 10 at the power 2 (2 digits after ".")
    slider->setTickmarks(QSlider::Below);
    slider->setTickInterval(5);
    layout->addWidget(slider,1,1,Qt::AlignHCenter);

    QLabel* max = new QLabel(this);
    max->setText(QString("0.5"));
    max->setMaximumSize ( max->sizeHint() );

    layout->addWidget(max,1,2,Qt::AlignHCenter);

    // synchronization between qslider and qlineedit
    connect(slider, SIGNAL( valueChanged(int) ), this, SLOT ( changeLineEditValue() ) );
    connect(slider, SIGNAL( sliderReleased()   ), this, SLOT ( changeLineEditValue() ) );
    connect(lineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT (changeSliderValue() ) );

    // synchronization between the widgets and the modify object dialog box
    connect(lineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( setWidgetDirty() ) );
    connect(slider, SIGNAL( sliderReleased()  ), this, SLOT ( setWidgetDirty() ) );
    connect(slider, SIGNAL( valueChanged(int) ), this, SLOT ( setWidgetDirty() ) );


    return true;
}

void QPoissonRatioWidget::setDataReadOnly(bool readOnly)
{
    lineEdit->setReadOnly(readOnly);
    slider->setEnabled(!readOnly);
}

void QPoissonRatioWidget::readFromData()
{
    double value = this->getData()->virtualGetValue();
    lastValue.setNum(value);
    lineEdit->setText(lastValue);
    changeSliderValue();
}

void QPoissonRatioWidget::writeToData()
{
    bool ok;
    lastValue = lineEdit->text();
    double d = lastValue.toDouble(&ok);
    if(ok)
    {
        this->getData()->virtualSetValue(d);
    }
}

bool QPoissonRatioWidget::checkDirty()
{
    return lastValue != lineEdit->text();
}

void QPoissonRatioWidget::changeLineEditValue()
{
    int v = slider->value();
    double db = (double)v / 100.;
    QString str;
    str.setNum(db);
    lineEdit->setText(str);
}

void QPoissonRatioWidget::changeSliderValue()
{
    bool ok;
    double v = lineEdit->text().toDouble(&ok);
    if(ok)
    {
        slider->setValue( (int)(v*100.) );
    }
}

helper::Creator<DataWidgetFactory, QPoissonRatioWidget> DWClass_Poissonratio("poissonRatio",false);

} // qt
} //gui
} //sofa

