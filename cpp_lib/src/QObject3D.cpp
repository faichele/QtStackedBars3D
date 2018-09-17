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

#include "QObject3DPrivate.h"
#include "QScene3DPrivate.h"

namespace QtStackedBar3DVis
{

	/*!
	\class QObject3D
	\inmodule QtDataVisualization
	\brief The QObject3D class is a simple base class for all the objects in a
	3D scene.
	\since QtDataVisualization 1.0

	Contains position information for an object in a 3D scene.
	The object is considered to be a single point in the coordinate space without dimensions.
	*/

	/*!
	\qmltype Object3D
	\inqmlmodule QtDataVisualization
	\since QtDataVisualization 1.0
	\ingroup datavisualization_qml
	\instantiates QObject3D
	\brief A base type for all the objects in a 3D scene.

	An uncreatable base type that contains position information for an object in
	a 3D scene. The object is considered to be a single point in the coordinate space without
	dimensions.
	*/

	/*!
	* \qmlproperty vector3d Object3D::position
	*
	* The 3D position of the object.
	*
	* \note Currently setting this property has no effect for Camera3D, as the position is handled
	* internally.
	*/

	/*!
	* Constructs a new 3D object with the position set to origin by default. An
	* optional \a parent parameter can be given and is then passed to the QObject
	* constructor.
	*/
	QObject3D::QObject3D(QObject *parent) :
		QObject(parent),
		d_ptr(new QObject3DPrivate(this))
	{
	}

	/*!
	*  Destroys the 3D object.
	*/
	QObject3D::~QObject3D()
	{
	}

	/*!
	* Copies the 3D object position from the given \a source 3D object to this 3D object instance.
	*/
	void QObject3D::copyValuesFrom(const QObject3D &source)
	{
		d_ptr->m_position = source.d_ptr->m_position;
		setDirty(true);
	}

	/*!
	* \property QObject3D::parentScene
	*
	* \brief The parent scene as a read only value.
	*
	* If the object has no parent scene, the value is 0.
	*/
	QScene3D *QObject3D::parentScene()
	{
		return qobject_cast<QScene3D *>(parent());
	}

	/*!
	* \property QObject3D::position
	*
	* \brief The 3D position of the object.
	*
	* \note Currently setting this property has no effect for QCamera3D, as the position is handled
	* internally.
	*/
	QVector3D QObject3D::position() const
	{
		return d_ptr->m_position;
	}

	void QObject3D::setPosition(const QVector3D &position)
	{
		if (d_ptr->m_position != position) {
			d_ptr->m_position = position;
			setDirty(true);
			emit positionChanged(d_ptr->m_position);
		}
	}

	/*!
	* Sets \a dirty to \c true if the 3D object has changed since the last update.
	*/
	void QObject3D::setDirty(bool dirty)
	{
		d_ptr->m_isDirty = dirty;
		if (parentScene())
			parentScene()->d_ptr->markDirty();
	}

	/*!
	* Returns whether the 3D object has changed.
	*/
	bool QObject3D::isDirty() const
	{
		return d_ptr->m_isDirty;
	}

	QObject3DPrivate::QObject3DPrivate(QObject3D *q) :
		q_ptr(q),
		m_isDirty(true)
	{
	}

	QObject3DPrivate::~QObject3DPrivate()
	{

	}

}
