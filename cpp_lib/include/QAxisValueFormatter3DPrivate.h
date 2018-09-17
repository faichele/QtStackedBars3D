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

#ifndef Q_AXIS_VALUE_FORMATTER_3D_PRIVATE_H
#define Q_AXIS_VALUE_FORMATTER_3D_PRIVATE_H

#include "QVisualizationGlobals3D.h"
#include "QAxisValueFormatter3D.h"
#include "QUtils3D.h"
#include <QtCore/QLocale>

namespace QtStackedBar3DVis
{
	class QValueAxis3D;

	class QAxisValueFormatter3DPrivate : public QObject
	{
		Q_OBJECT

	public:
		QAxisValueFormatter3DPrivate(QtStackedBar3DVis::QAxisValueFormatter3D *q);
		virtual ~QAxisValueFormatter3DPrivate();

		void recalculate();
		void doRecalculate();
		void populateCopy(QtStackedBar3DVis::QAxisValueFormatter3D &copy);
		void doPopulateCopy(QAxisValueFormatter3DPrivate &copy);

		QString stringForValue(qreal value, const QString &format);
		float positionAt(float value) const;
		float valueAt(float position) const;

		void setAxis(QValueAxis3D *axis);
		void markDirty(bool labelsChange);

		public Q_SLOTS:
		void markDirtyNoLabelChange();

	protected:
		QtStackedBar3DVis::QAxisValueFormatter3D *q_ptr;

		bool m_needsRecalculate;

		float m_min;
		float m_max;
		float m_rangeNormalizer;

		QVector<float> m_gridPositions;
		QVector<float> m_subGridPositions;
		QVector<float> m_labelPositions;
		QStringList m_labelStrings;

		QValueAxis3D *m_axis;

		QString m_previousLabelFormat;
		QByteArray m_labelFormatArray;
		QUtils3D::ParamType m_preparsedParamType;

		bool m_allowNegatives;
		bool m_allowZero;

		QLocale m_locale;
		QString m_formatPreStr;
		QString m_formatPostStr;
		int m_formatPrecision;
		char m_formatSpec;
		bool m_cLocaleInUse;

		friend class QAxisValueFormatter3D;
	};
}

#endif
