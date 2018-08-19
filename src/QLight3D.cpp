/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#include "QLight3D.h"
#include "QLight3DPrivate.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QLight3D
	* \inmodule QtDataVisualization
	* \brief Representation of a light source in 3D space.
	* \since QtDataVisualization 1.0
	*
	* QLight3D represents a monochrome light source in 3D space.
	*
	* \note Default light has isAutoPosition() \c true.
	*/

	/*!
	* \qmltype Light3D
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.0
	* \ingroup datavisualization_qml
	* \instantiates QLight3D
	* \brief Representation of a light source in 3D space.
	*
	* Light3D represents a monochrome light source in 3D space.
	*
	* \note Default light has autoPosition \c true.
	*/

	/*!
	* \qmlproperty bool Light3D::autoPosition
	* \since QtDataVisualization 1.3
	* Defines whether the light position follows the camera automatically.
	* \note Has no effect if shadows are enabled. Remember to disable shadows before setting light's
	* position, or it will be overwritten by automatic positioning if this
	* property is \c false.
	*/

	/*!
	* Constructs a new 3D light located at origin. An optional \a parent parameter can be given
	* and is then passed to QObject constructor.
	*/
	QLight3D::QLight3D(QObject *parent) :
		QObject3D(parent),
		d_ptr(new QLight3DPrivate(this))
	{
	}

	/*!
	* Destroys the light object.
	*/
	QLight3D::~QLight3D()
	{
	}

	/*!
	* \property QLight3D::autoPosition
	* \since QtDataVisualization 5.9
	* \brief Whether the light position follows the camera automatically.
	* \note Has no effect if shadows are enabled. Remember to disable shadows before setting light's
	* position, or it will be overwritten by automatic positioning if
	* \c isAutoPosition() is \c false.
	*/
	void QLight3D::setAutoPosition(bool enabled)
	{
		if (enabled != d_ptr->m_automaticLight) {
			d_ptr->m_automaticLight = enabled;
			setDirty(true);
			emit autoPositionChanged(enabled);
		}
	}

	bool QLight3D::isAutoPosition()
	{
		return d_ptr->m_automaticLight;
	}

	QLight3DPrivate::QLight3DPrivate(QLight3D *q) :
		q_ptr(q),
		m_automaticLight(false)
	{
	}

	QLight3DPrivate::~QLight3DPrivate()
	{
	}

	void QLight3DPrivate::sync(QLight3D &other)
	{
		if (q_ptr->isDirty()) {
			other.setPosition(q_ptr->position());
			other.setAutoPosition(q_ptr->isAutoPosition());
			q_ptr->setDirty(false);
		}
	}

}
