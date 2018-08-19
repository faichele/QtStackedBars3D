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

#ifndef Q_STACKED_BARS_3D_H
#define Q_STACKED_BARS_3D_H

#include "QAbstractGraph3D.h"
#include "QValueAxis3D.h"
#include "QCategoryAxis3D.h"
#include "QStackedBarSeries3D.h"

namespace QtStackedBar3DVis
{

	class QStackedBars3DPrivate;

	class QT_STACKEDBAR3D_EXPORT QStackedBars3D : public QAbstractGraph3D
	{
		Q_OBJECT
			Q_PROPERTY(bool multiSeriesUniform READ isMultiSeriesUniform WRITE setMultiSeriesUniform NOTIFY multiSeriesUniformChanged)
			Q_PROPERTY(float barThickness READ barThickness WRITE setBarThickness NOTIFY barThicknessChanged)
			Q_PROPERTY(QSizeF barSpacing READ barSpacing WRITE setBarSpacing NOTIFY barSpacingChanged)
			Q_PROPERTY(bool barSpacingRelative READ isBarSpacingRelative WRITE setBarSpacingRelative NOTIFY barSpacingRelativeChanged)
			Q_PROPERTY(QCategoryAxis3D *rowAxis READ rowAxis WRITE setRowAxis NOTIFY rowAxisChanged)
			Q_PROPERTY(QCategoryAxis3D *columnAxis READ columnAxis WRITE setColumnAxis NOTIFY columnAxisChanged)
			Q_PROPERTY(QValueAxis3D *valueAxis READ valueAxis WRITE setValueAxis NOTIFY valueAxisChanged)
			Q_PROPERTY(QStackedBarSeries3D *primarySeries READ primarySeries WRITE setPrimarySeries NOTIFY primarySeriesChanged)
			Q_PROPERTY(QStackedBarSeries3D *selectedSeries READ selectedSeries NOTIFY selectedSeriesChanged)
			Q_PROPERTY(float floorLevel READ floorLevel WRITE setFloorLevel NOTIFY floorLevelChanged)

	public:
		explicit QStackedBars3D(const QSurfaceFormat *format = Q_NULLPTR, QWindow *parent = Q_NULLPTR);
		virtual ~QStackedBars3D();

		void setPrimarySeries(QStackedBarSeries3D *series);
		QStackedBarSeries3D *primarySeries() const;
		void addSeries(QStackedBarSeries3D *series);
		void removeSeries(QStackedBarSeries3D *series);
		void insertSeries(int index, QStackedBarSeries3D *series);
		QList<QStackedBarSeries3D *> seriesList() const;

		void setMultiSeriesUniform(bool uniform);
		bool isMultiSeriesUniform() const;

		void setBarThickness(float thicknessRatio);
		float barThickness() const;

		void setBarSpacing(const QSizeF &spacing);
		QSizeF barSpacing() const;

		void setBarSpacingRelative(bool relative);
		bool isBarSpacingRelative() const;

		void setRowAxis(QCategoryAxis3D *axis);
		QCategoryAxis3D *rowAxis() const;
		void setColumnAxis(QCategoryAxis3D *axis);
		QCategoryAxis3D *columnAxis() const;
		void setValueAxis(QValueAxis3D *axis);
		QValueAxis3D *valueAxis() const;
		void addAxis(QAbstractAxis3D *axis);
		void releaseAxis(QAbstractAxis3D *axis);
		QList<QAbstractAxis3D *> axes() const;

		QStackedBarSeries3D *selectedSeries() const;
		void setFloorLevel(float level);
		float floorLevel() const;

	Q_SIGNALS:
		void multiSeriesUniformChanged(bool uniform);
		void barThicknessChanged(float thicknessRatio);
		void barSpacingChanged(const QSizeF &spacing);
		void barSpacingRelativeChanged(bool relative);
		void rowAxisChanged(QCategoryAxis3D *axis);
		void columnAxisChanged(QCategoryAxis3D *axis);
		void valueAxisChanged(QValueAxis3D *axis);
		void primarySeriesChanged(QStackedBarSeries3D *series);
		void selectedSeriesChanged(QStackedBarSeries3D *series);
		void floorLevelChanged(float level);

	private:
		QStackedBars3DPrivate *dptr();
		const QStackedBars3DPrivate *dptrc() const;
		Q_DISABLE_COPY(QStackedBars3D)
	};

}

#endif
