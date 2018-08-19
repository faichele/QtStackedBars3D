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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Q_ABSTRACT_3D_AXIS_PRIVATE_H
#define Q_ABSTRACT_3D_AXIS_PRIVATE_H

#include "QVisualizationGlobals3D.h"
#include "QAbstractAxis3D.h"

namespace QtStackedBar3DVis
{

	class QAbstractAxis3DPrivate : public QObject
	{
		Q_OBJECT
	public:
		QAbstractAxis3DPrivate(QAbstractAxis3D *q, QAbstractAxis3D::AxisType type);
		virtual ~QAbstractAxis3DPrivate();

		void setOrientation(QAbstractAxis3D::AxisOrientation orientation);

		inline bool isDefaultAxis() { return m_isDefaultAxis; }
		inline void setDefaultAxis(bool isDefault) { m_isDefaultAxis = isDefault; }

		virtual void setRange(float min, float max, bool suppressWarnings = false);
		virtual void setMin(float min);
		virtual void setMax(float max);

	protected:
		virtual void updateLabels();
		virtual bool allowZero() = 0;
		virtual bool allowNegatives() = 0;
		virtual bool allowMinMaxSame() = 0;

		QAbstractAxis3D *q_ptr;

		QString m_title;
		QStringList m_labels;
		QAbstractAxis3D::AxisOrientation m_orientation;
		QAbstractAxis3D::AxisType m_type;
		bool m_isDefaultAxis;
		float m_min;
		float m_max;
		bool m_autoAdjust;
		float m_labelAutoRotation;
		bool m_titleVisible;
		bool m_titleFixed;

		friend class QAbstractAxis3D;
		friend class QValueAxis3D;
		friend class QCategoryAxis3D;
		friend class QScatterDataProxy3DPrivate;
		friend class QSurfaceDataProxy3DPrivate;
	};

}

#endif
