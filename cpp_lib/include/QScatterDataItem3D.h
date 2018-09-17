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

#ifndef Q_SCATTER_DATA_ITEM_3D_H
#define Q_SCATTER_DATA_ITEM_3D_H

#include "QVisualizationGlobals3D.h"
#include <QtGui/QQuaternion>

namespace QtStackedBar3DVis
{

	class QScatterDataItem3DPrivate;

	class QT_STACKEDBAR3D_EXPORT QScatterDataItem3D
	{
	public:
		QScatterDataItem3D();
		QScatterDataItem3D(const QVector3D &position);
		QScatterDataItem3D(const QVector3D &position, const QQuaternion &rotation);
		QScatterDataItem3D(const QScatterDataItem3D &other);
		~QScatterDataItem3D();

		QScatterDataItem3D &operator=(const QScatterDataItem3D &other);

		inline void setPosition(const QVector3D &pos) { m_position = pos; }
		inline QVector3D position() const { return m_position; }
		inline void setRotation(const QQuaternion &rot) { m_rotation = rot; }
		inline QQuaternion rotation() const { return m_rotation; }
		inline void setX(float value) { m_position.setX(value); }
		inline void setY(float value) { m_position.setY(value); }
		inline void setZ(float value) { m_position.setZ(value); }
		inline float x() const { return m_position.x(); }
		inline float y() const { return m_position.y(); }
		inline float z() const { return m_position.z(); }

	protected:
		void createExtraData();

		QScatterDataItem3DPrivate *d_ptr;

	private:
		QVector3D m_position;
		QQuaternion m_rotation;
	};
}

#endif
