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

#ifndef Q_BARS_CONTROLLER_3D_H
#define Q_BARS_CONTROLLER_3D_H

#include "QVisualizationGlobals3D.h"
#include "QAbstractController3D.h"

namespace QtStackedBar3DVis
{

	class QBarsRenderer3D;
	class QBarSeries3D;

	struct QBarsChangeBitField3D {
		bool multiSeriesScalingChanged : 1;
		bool barSpecsChanged : 1;
		bool selectedBarChanged : 1;
		bool rowsChanged : 1;
		bool itemChanged : 1;
		bool floorLevelChanged : 1;

		QBarsChangeBitField3D() :
			multiSeriesScalingChanged(true),
			barSpecsChanged(true),
			selectedBarChanged(true),
			rowsChanged(false),
			itemChanged(false),
			floorLevelChanged(false)
		{
		}
	};

	class QT_STACKEDBAR3D_EXPORT QBarsController3D : public QAbstractController3D
	{
		Q_OBJECT

	public:
		struct ChangeItem {
			QBarSeries3D *series;
			QPoint point;
		};
		struct ChangeRow {
			QBarSeries3D *series;
			int row;
		};

	private:
		QBarsChangeBitField3D m_changeTracker;
		QVector<ChangeItem> m_changedItems;
		QVector<ChangeRow> m_changedRows;

		// Interaction
		QPoint m_selectedBar;     // Points to row & column in data window.
		QBarSeries3D *m_selectedBarSeries; // Points to the series for which the bar is selected in
										   // single series selection cases.
		QBarSeries3D *m_primarySeries; // Category axis labels are taken from the primary series

									   // Look'n'feel
		bool m_isMultiSeriesUniform;
		bool m_isBarSpecRelative;
		GLfloat m_barThicknessRatio;
		QSizeF m_barSpacing;
		float m_floorLevel;

		// Rendering
		QBarsRenderer3D *m_renderer;

	public:
		explicit QBarsController3D(QRect rect, QScene3D *scene = 0);
		~QBarsController3D();

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

		inline QBarSeries3D *selectedSeries() const { return m_selectedBarSeries; }

		void setSelectionMode(QAbstractGraph3D::SelectionFlags mode);
		void setSelectedBar(const QPoint &position, QBarSeries3D *series, bool enterSlice);
		virtual void clearSelection();

		virtual void handleAxisAutoAdjustRangeChangedInOrientation(
			QAbstractAxis3D::AxisOrientation orientation, bool autoAdjust);
		virtual void handleSeriesVisibilityChangedBySender(QObject *sender);
		virtual void handlePendingClick();

		static QPoint invalidSelectionPosition();

		virtual void setAxisX(QAbstractAxis3D *axis);
		virtual void setAxisZ(QAbstractAxis3D *axis);

		virtual void setPrimarySeries(QBarSeries3D *series);
		virtual QBarSeries3D *primarySeries() const;
		virtual void addSeries(QAbstractSeries3D *series);
		virtual void removeSeries(QAbstractSeries3D *series);
		virtual void insertSeries(int index, QAbstractSeries3D *series);
		virtual QList<QBarSeries3D *> barSeriesList();

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
		void primarySeriesChanged(QBarSeries3D *series);
		void selectedSeriesChanged(QBarSeries3D *series);

	protected:
		virtual QAbstractAxis3D *createDefaultAxis(QAbstractAxis3D::AxisOrientation orientation);

	private:
		void adjustSelectionPosition(QPoint &pos, const QBarSeries3D *series);

		Q_DISABLE_COPY(QBarsController3D)
	};

}

#endif
