/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "QStackedBarDataItem3DPrivate.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QStackedBarDataItem3D
	* \inmodule QtDataVisualization
	* \brief The QStackedBarDataItem3D class provides a container for resolved data to be added to bar graphs.
	* \since QtDataVisualization 1.0
	*
	* A bar data item holds the data for a single rendered bar in a graph.
	* Bar data proxies parse data into QStackedBarDataItem3D instances for visualization.
	*
	* \sa QBarDataProxy3D, {Qt Data Visualization C++ Classes}
	*/

	/*!
	* Constructs a bar data item.
	*/
	QStackedBarDataItem3D::QStackedBarDataItem3D()
		: d_ptr(0) // private data doesn't exist by default (optimization)
	{
	}

	/*!
	* Constructs a bar data item with the value \a value.
	*/
    QStackedBarDataItem3D::QStackedBarDataItem3D(const QList<qreal>& values)
		: d_ptr(0),
		m_values(values)
	{
	}

	/*!
	* Constructs a bar data item with the value \a value and angle \a angle.
	*/
    QStackedBarDataItem3D::QStackedBarDataItem3D(const QList<qreal>& values, const QList<qreal>& angles)
		: d_ptr(0),
		m_values(values),
		m_angles(angles)
	{
	}

	/*!
	* Constructs a copy of \a other.
	*/
	QStackedBarDataItem3D::QStackedBarDataItem3D(const QStackedBarDataItem3D &other)
	{
		operator=(other);
	}

	/*!
	* Deletes a bar data item.
	*/
	QStackedBarDataItem3D::~QStackedBarDataItem3D()
	{
		delete d_ptr;
	}

	/*!
	*  Assigns a copy of \a other to this object.
	*/
	QStackedBarDataItem3D &QStackedBarDataItem3D::operator=(const QStackedBarDataItem3D &other)
	{
		m_values = other.m_values;
		m_angles = other.m_angles;
		if (other.d_ptr)
			createExtraData();
		else
			d_ptr = 0;
		return *this;
	}

	/*!
	* \fn void QStackedBarDataItem3D::setValue(float val)
	* Sets the value \a val to this data item.
	*/

	/*!
	* \fn float QStackedBarDataItem3D::value() const
	* Returns the value of this data item.
	*/

	/*!
	* \fn void QStackedBarDataItem3D::setRotation(float angle)
	* Sets the rotation angle \a angle in degrees for this data item.
	*/

	/*!
	* \fn float QStackedBarDataItem3D::rotation() const
	* Returns the rotation angle in degrees for this data item.
	*/

	/*!
	* \internal
	*/
	void QStackedBarDataItem3D::createExtraData()
	{
		if (!d_ptr)
			d_ptr = new QStackedBarDataItem3DPrivate;
	}

	QStackedBarDataItem3DPrivate::QStackedBarDataItem3DPrivate()
	{
	}

	QStackedBarDataItem3DPrivate::~QStackedBarDataItem3DPrivate()
	{
	}

}
