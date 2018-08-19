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

#ifndef Q_STACKED_BAR_DATA_ITEM_3D_H
#define Q_STACKED_BAR_DATA_ITEM_3D_H

#include "QVisualizationGlobals3D.h"

namespace QtStackedBar3DVis
{

	class QStackedBarDataItem3DPrivate;

	class QT_STACKEDBAR3D_EXPORT QStackedBarDataItem3D
	{
	public:
		QStackedBarDataItem3D();
		QStackedBarDataItem3D(const QList<float>& values);
		QStackedBarDataItem3D(const QList<float>& values, const QList<float>& angles);
		QStackedBarDataItem3D(const QStackedBarDataItem3D &other);
		~QStackedBarDataItem3D();

		QStackedBarDataItem3D &operator=(const QStackedBarDataItem3D &other);

		inline void setValues(const QList<float>& values) { m_values = values; }
		inline const QList<float> values() const { return m_values; }
		inline void setRotations(const QList<float>& angles) { m_angles = angles; }
		inline const QList<float> rotations() const { return m_angles; }

	protected:
		void createExtraData();

		QStackedBarDataItem3DPrivate *d_ptr;

	private:
		QList<float> m_values;
		QList<float> m_angles;
	};

}

#endif
