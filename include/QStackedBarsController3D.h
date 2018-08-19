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

#ifndef Q_STACKED_BARS_CONTROLLER_3D_H
#define Q_STACKED_BARS_CONTROLLER_3D_H

#include "QVisualizationGlobals3D.h"
#include "QAbstractController3D.h"

namespace QtStackedBar3DVis
{

	class QStackedBarsRenderer3D;
	class QStackedBarSeries3D;

	struct QStackedBarsChangeBitField3D {
		bool multiSeriesScalingChanged : 1;
		bool barSpecsChanged : 1;
		bool selectedBarChanged : 1;
		bool rowsChanged : 1;
		bool itemChanged : 1;
		bool floorLevelChanged : 1;

		QStackedBarsChangeBitField3D() :
			multiSeriesScalingChanged(true),
			barSpecsChanged(true),
			selectedBarChanged(true),
			rowsChanged(false),
			itemChanged(false),
			floorLevelChanged(false)
		{
		}
	};

	class QT_STACKEDBAR3D_EXPORT QStackedBarsController3D : public QAbstractController3D
	{
		Q_OBJECT

	public:
		struct ChangeItem {
			QStackedBarSeries3D *series;
			QPoint point;
		};
		struct ChangeRow {
			QStackedBarSeries3D *series;
			int row;
		};

	private:
		QStackedBarsChangeBitField3D m_changeTracker;
		QVector<ChangeItem> m_changedItems;
		QVector<ChangeRow> m_changedRows;

		// Interaction
		QPoint m_selectedBar;     // Points to row & column in data window.
		QStackedBarSeries3D *m_selectedBarSeries; // Points to the series for which the bar is selected in
										   // single series selection cases.
		QStackedBarSeries3D *m_primarySeries; // Category axis labels are taken from the primary series

									   // Look'n'feel
		bool m_isMultiSeriesUniform;
		bool m_isBarSpecRelative;
		GLfloat m_barThicknessRatio;
		QSizeF m_barSpacing;
		float m_floorLevel;

		// Rendering
		QStackedBarsRenderer3D *m_renderer;

	public:
		explicit QStackedBarsController3D(QRect rect, QScene3D *scene = 0);
		~QStackedBarsController3D();

		virtual void initializeOpenGL();
		virtual void synchDataToRenderer();

		void setMultiSeriesScaling(bool uniform);
		bool multiSeriesScaling() const;

		// bar thickness, spacing between bars, and is spacing relative to thickness or absolute
		// y -component sets the thickness/spacing of z -direction
		// With relative 0.0f means side-to-side, 1.0f = one thickness in between
		void setBarSpecs(GLfloat thicknessRatio = 1.0f,
			const QSizeF &spacing = QSizeF(1.0, 1.0),
			bool relative = true);
		GLfloat barThickness();
		QSizeF barSpacing();
		bool isBarSpecRelative();
		void setFloorLevel(float level);
		float floorLevel() const;

		inline QStackedBarSeries3D *selectedSeries() const { return m_selectedBarSeries; }

		void setSelectionMode(QAbstractGraph3D::SelectionFlags mode);
		void setSelectedBar(const QPoint &position, QStackedBarSeries3D *series, bool enterSlice);
		virtual void clearSelection();

		virtual void handleAxisAutoAdjustRangeChangedInOrientation(
			QAbstractAxis3D::AxisOrientation orientation, bool autoAdjust);
		virtual void handleSeriesVisibilityChangedBySender(QObject *sender);
		virtual void handlePendingClick();

		static QPoint invalidSelectionPosition();

		virtual void setAxisX(QAbstractAxis3D *axis);
		virtual void setAxisZ(QAbstractAxis3D *axis);

		virtual void setPrimarySeries(QStackedBarSeries3D *series);
		virtual QStackedBarSeries3D *primarySeries() const;
		virtual void addSeries(QAbstractSeries3D *series);
		virtual void removeSeries(QAbstractSeries3D *series);
		virtual void insertSeries(int index, QAbstractSeries3D *series);
		virtual QList<QStackedBarSeries3D *> barSeriesList();

		virtual void handleAxisRangeChangedBySender(QObject *sender);
		virtual void adjustAxisRanges();

		public Q_SLOTS:
		void handleArrayReset();
		void handleRowsAdded(int startIndex, int count);
		void handleRowsChanged(int startIndex, int count);
		void handleRowsRemoved(int startIndex, int count);
		void handleRowsInserted(int startIndex, int count);
		void handleItemChanged(int rowIndex, int columnIndex);
		void handleDataRowLabelsChanged();
		void handleDataColumnLabelsChanged();

	Q_SIGNALS:
		void primarySeriesChanged(QStackedBarSeries3D *series);
		void selectedSeriesChanged(QStackedBarSeries3D *series);

	protected:
		virtual QAbstractAxis3D *createDefaultAxis(QAbstractAxis3D::AxisOrientation orientation);

	private:
		void adjustSelectionPosition(QPoint &pos, const QStackedBarSeries3D *series);

		Q_DISABLE_COPY(QStackedBarsController3D)
	};

}

#endif
