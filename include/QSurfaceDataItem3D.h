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

#ifndef Q_SURFACE_DATA_ITEM_3D_H
#define Q_SURFACE_DATA_ITEM_3D_H

#include "QVisualizationGlobals3D.h"
#include <QtGui/QVector3D>

namespace QtStackedBar3DVis
{

	class QSurfaceDataItem3DPrivate;

	class QT_STACKEDBAR3D_EXPORT QSurfaceDataItem3D
	{
	public:
		QSurfaceDataItem3D();
		QSurfaceDataItem3D(const QVector3D &position);
		QSurfaceDataItem3D(const QSurfaceDataItem3D &other);
		~QSurfaceDataItem3D();

		QSurfaceDataItem3D &operator=(const QSurfaceDataItem3D &other);

		inline void setPosition(const QVector3D &pos) { m_position = pos; }
		inline QVector3D position() const { return m_position; }
		inline void setX(float value) { m_position.setX(value); }
		inline void setY(float value) { m_position.setY(value); }
		inline void setZ(float value) { m_position.setZ(value); }
		inline float x() const { return m_position.x(); }
		inline float y() const { return m_position.y(); }
		inline float z() const { return m_position.z(); }

	protected:
		void createExtraData();

		QSurfaceDataItem3DPrivate *d_ptr;

	private:
		QVector3D m_position;
	};

}

#endif
