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

#ifndef SOFA_GUI_QT_QTABLEUPDATER_H
#define SOFA_GUI_QT_QTABLEUPDATER_H


#include "SofaGUIQt.h"


#include <qspinbox.h>
#include <qtable.h>



namespace sofa
{
namespace gui
{
namespace qt
{


class QTableUpdater : virtual public QTable
{
    Q_OBJECT
public:
    QTableUpdater ( int numRows, int numCols, QWidget * parent = 0, const char * name = 0 ):
        QTable(numRows, numCols, parent, name)
    {};
public slots:
    void setDisplayed(bool b) {this->setShown(b);}
    void resizeTableV( int number )
    {
        QSpinBox *spinBox = (QSpinBox *) sender();
        QString header;
        if( spinBox == NULL)
        {
            return;
        }
        if (number != numRows())
        {
            setNumRows(number);
        }
    }

    void resizeTableH( int number )
    {
        QSpinBox *spinBox = (QSpinBox *) sender();
        QString header;
        if( spinBox == NULL)
        {
            return;
        }
        if (number != numCols())
        {
            setNumCols(number);

        }
    }

};

}
}
}

#endif
