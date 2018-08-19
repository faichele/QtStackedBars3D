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

#include "QStackedBarsController3D.h"
#include "QAbstractController3DPrivate.h"
#include "QStackedBarRenderer3D.h"
#include "QValueAxis3DPrivate.h"
#include "QCategoryAxis3DPrivate.h"
#include "QStackedBarDataProxy3DPrivate.h"
#include "QStackedBarSeries3DPrivate.h"
#include "QThemeManager3D.h"
#include "QTheme3DPrivate.h"
#include <QtCore/QMutexLocker>

namespace QtStackedBar3DVis
{

	QStackedBarsController3D::QStackedBarsController3D(QRect boundRect, QScene3D *scene)
		: QAbstractController3D(boundRect, scene),
		m_selectedBar(invalidSelectionPosition()),
		m_selectedBarSeries(0),
		m_primarySeries(0),
		m_isMultiSeriesUniform(false),
		m_isBarSpecRelative(true),
		m_barThicknessRatio(1.0f),
		m_barSpacing(QSizeF(1.0, 1.0)),
		m_floorLevel(0.0f),
		m_renderer(0)
	{
		// Setting a null axis creates a new default axis according to orientation and graph type.
		// Note: these cannot be set in the QAbstractController3D constructor, as they will call virtual
		//       functions implemented by subclasses.
		setAxisX(0);
		setAxisY(0);
		setAxisZ(0);
	}

	QStackedBarsController3D::~QStackedBarsController3D()
	{
	}

	void QStackedBarsController3D::initializeOpenGL()
	{
		QMutexLocker mutexLocker(&m_renderMutex);

		// Initialization is called multiple times when Qt Quick components are used
		if (isInitialized())
			return;

		m_renderer = new QStackedBarsRenderer3D(this);

		setRenderer(m_renderer);

		mutexLocker.unlock();
		synchDataToRenderer();

		emitNeedRender();
	}

	void QStackedBarsController3D::synchDataToRenderer()
	{
		QMutexLocker mutexLocker(&m_renderMutex);

		if (!isInitialized())
			return;

		// Background change requires reloading the meshes in bar graphs, so dirty the series visuals
		if (d_ptr->themeManager()->activeTheme()->d_ptr->m_dirtyBits.backgroundEnabledDirty) {
			m_isSeriesVisualsDirty = true;
			foreach(QAbstractSeries3D *series, m_seriesList)
				series->d_ptr->m_changeTracker.meshChanged = true;
		}

		// If y range or reverse changed, scene needs to be updated to update camera limits
		bool needSceneUpdate = false;
		if (QAbstractController3D::m_changeTracker.axisYRangeChanged
			|| QAbstractController3D::m_changeTracker.axisYReversedChanged) {
			needSceneUpdate = true;
		}

		// Floor level update requires data update, so do before abstract sync
		if (m_changeTracker.floorLevelChanged) {
			m_renderer->updateFloorLevel(m_floorLevel);
			m_changeTracker.floorLevelChanged = false;
		}

		QAbstractController3D::synchDataToRenderer();

		// Notify changes to renderer
		if (m_changeTracker.rowsChanged) {
			m_renderer->updateRows(m_changedRows);
			m_changeTracker.rowsChanged = false;
			m_changedRows.clear();
		}

		if (m_changeTracker.itemChanged) {
			m_renderer->updateItems(m_changedItems);
			m_changeTracker.itemChanged = false;
			m_changedItems.clear();
		}

		if (m_changeTracker.multiSeriesScalingChanged) {
			m_renderer->updateMultiSeriesScaling(m_isMultiSeriesUniform);
			m_changeTracker.multiSeriesScalingChanged = false;
		}

		if (m_changeTracker.barSpecsChanged) {
			m_renderer->updateBarSpecs(m_barThicknessRatio, m_barSpacing, m_isBarSpecRelative);
			m_changeTracker.barSpecsChanged = false;
		}

		// Needs to be done after data is set, as it needs to know the visual array.
		if (m_changeTracker.selectedBarChanged) {
			m_renderer->updateSelectedBar(m_selectedBar, m_selectedBarSeries);
			m_changeTracker.selectedBarChanged = false;
		}

		// Since scene is updated before axis updates are handled, do another render pass to
		// properly update controller side camera limits.
		if (needSceneUpdate)
			m_scene->d_ptr->markDirty();
	}

	void QStackedBarsController3D::handleArrayReset()
	{
		QStackedBarSeries3D *series;
		if (qobject_cast<QStackedBarDataProxy3D *>(sender()))
			series = static_cast<QStackedBarDataProxy3D *>(sender())->series();
		else
			series = static_cast<QStackedBarSeries3D *>(sender());

		if (series->isVisible()) {
			adjustAxisRanges();
			m_isDataDirty = true;
			series->d_ptr->markItemLabelDirty();
		}
		if (!m_changedSeriesList.contains(series))
			m_changedSeriesList.append(series);
		// Clear selection unless still valid
		setSelectedBar(m_selectedBar, m_selectedBarSeries, false);
		emitNeedRender();
	}

	void QStackedBarsController3D::handleRowsAdded(int startIndex, int count)
	{
		Q_UNUSED(startIndex)
			Q_UNUSED(count)
			QStackedBarSeries3D *series = static_cast<QStackedBarDataProxy3D *>(sender())->series();
		if (series->isVisible()) {
			adjustAxisRanges();
			m_isDataDirty = true;
		}
		if (!m_changedSeriesList.contains(series))
			m_changedSeriesList.append(series);
		emitNeedRender();
	}

	void QStackedBarsController3D::handleRowsChanged(int startIndex, int count)
	{
		QStackedBarSeries3D *series = static_cast<QStackedBarDataProxy3D *>(sender())->series();
		int oldChangeCount = m_changedRows.size();
		if (!oldChangeCount)
			m_changedRows.reserve(count);

		for (int i = 0; i < count; i++) {
			bool newItem = true;
			int candidate = startIndex + i;
			for (int j = 0; j < oldChangeCount; j++) {
				const ChangeRow &oldChangeItem = m_changedRows.at(j);
				if (oldChangeItem.row == candidate && series == oldChangeItem.series) {
					newItem = false;
					break;
				}
			}
			if (newItem) {
				ChangeRow newChangeItem = { series, candidate };
				m_changedRows.append(newChangeItem);
				if (series == m_selectedBarSeries && m_selectedBar.x() == candidate)
					series->d_ptr->markItemLabelDirty();
			}
		}
		if (count) {
			m_changeTracker.rowsChanged = true;

			if (series->isVisible())
				adjustAxisRanges();

			// Clear selection unless still valid (row length might have changed)
			setSelectedBar(m_selectedBar, m_selectedBarSeries, false);
			emitNeedRender();
		}
	}

	void QStackedBarsController3D::handleRowsRemoved(int startIndex, int count)
	{
		Q_UNUSED(startIndex)
			Q_UNUSED(count)

			QStackedBarSeries3D *series = static_cast<QStackedBarDataProxy3D *>(sender())->series();
		if (series == m_selectedBarSeries) {
			// If rows removed from selected series before the selection, adjust the selection
			int selectedRow = m_selectedBar.x();
			if (startIndex <= selectedRow) {
				if ((startIndex + count) > selectedRow)
					selectedRow = -1; // Selected row removed
				else
					selectedRow -= count; // Move selected row down by amount of rows removed

				setSelectedBar(QPoint(selectedRow, m_selectedBar.y()), m_selectedBarSeries, false);
			}
		}

		if (series->isVisible()) {
			adjustAxisRanges();
			m_isDataDirty = true;
		}
		if (!m_changedSeriesList.contains(series))
			m_changedSeriesList.append(series);

		emitNeedRender();
	}

	void QStackedBarsController3D::handleRowsInserted(int startIndex, int count)
	{
		Q_UNUSED(startIndex)
			Q_UNUSED(count)
			QStackedBarSeries3D *series = static_cast<QStackedBarDataProxy3D *>(sender())->series();
		if (series == m_selectedBarSeries) {
			// If rows inserted to selected series before the selection, adjust the selection
			int selectedRow = m_selectedBar.x();
			if (startIndex <= selectedRow) {
				selectedRow += count;
				setSelectedBar(QPoint(selectedRow, m_selectedBar.y()), m_selectedBarSeries, false);
			}
		}

		if (series->isVisible()) {
			adjustAxisRanges();
			m_isDataDirty = true;
		}
		if (!m_changedSeriesList.contains(series))
			m_changedSeriesList.append(series);

		emitNeedRender();
	}

	void QStackedBarsController3D::handleItemChanged(int rowIndex, int columnIndex)
	{
		QStackedBarSeries3D *series = static_cast<QStackedBarDataProxy3D *>(sender())->series();

		bool newItem = true;
		QPoint candidate(rowIndex, columnIndex);
		foreach(ChangeItem item, m_changedItems) {
			if (item.point == candidate && item.series == series) {
				newItem = false;
				break;
			}
		}

		if (newItem) {
			ChangeItem newItem = { series, candidate };
			m_changedItems.append(newItem);
			m_changeTracker.itemChanged = true;

			if (series == m_selectedBarSeries && m_selectedBar == candidate)
				series->d_ptr->markItemLabelDirty();
			if (series->isVisible())
				adjustAxisRanges();
			emitNeedRender();
		}
	}

	void QStackedBarsController3D::handleDataRowLabelsChanged()
	{
		if (m_axisZ) {
			// Grab a sublist equal to data window (no need to have more labels in axis)
			int min = int(m_axisZ->min());
			int count = int(m_axisZ->max()) - min + 1;
			QStringList subList;
			if (m_primarySeries && m_primarySeries->dataProxy())
				subList = m_primarySeries->dataProxy()->rowLabels().mid(min, count);
			static_cast<QCategoryAxis3D *>(m_axisZ)->dptr()->setDataLabels(subList);
		}
	}

	void QStackedBarsController3D::handleDataColumnLabelsChanged()
	{
		if (m_axisX) {
			// Grab a sublist equal to data window (no need to have more labels in axis)
			int min = int(m_axisX->min());
			int count = int(m_axisX->max()) - min + 1;
			QStringList subList;
			if (m_primarySeries && m_primarySeries->dataProxy()) {
				subList = static_cast<QStackedBarDataProxy3D *>(m_primarySeries->dataProxy())
					->columnLabels().mid(min, count);
			}
			static_cast<QCategoryAxis3D *>(m_axisX)->dptr()->setDataLabels(subList);
		}
	}

	void QStackedBarsController3D::handleAxisAutoAdjustRangeChangedInOrientation(
		QAbstractAxis3D::AxisOrientation orientation, bool autoAdjust)
	{
		Q_UNUSED(orientation)
			Q_UNUSED(autoAdjust)
			adjustAxisRanges();
	}

	void QStackedBarsController3D::handleSeriesVisibilityChangedBySender(QObject *sender)
	{
		QAbstractController3D::handleSeriesVisibilityChangedBySender(sender);

		// Visibility changes may require disabling slicing,
		// so just reset selection to ensure everything is still valid.
		setSelectedBar(m_selectedBar, m_selectedBarSeries, false);
	}

	void QStackedBarsController3D::handlePendingClick()
	{
		// This function is called while doing the sync, so it is okay to query from renderer
		QPoint position = m_renderer->clickedPosition();
		QStackedBarSeries3D *series = static_cast<QStackedBarSeries3D *>(m_renderer->clickedSeries());

		setSelectedBar(position, series, true);

		QAbstractController3D::handlePendingClick();

		m_renderer->resetClickedStatus();
	}

	QPoint QStackedBarsController3D::invalidSelectionPosition()
	{
		static QPoint invalidSelectionPos(-1, -1);
		return invalidSelectionPos;
	}

	void QStackedBarsController3D::setAxisX(QAbstractAxis3D *axis)
	{
		QAbstractController3D::setAxisX(axis);
		handleDataColumnLabelsChanged();
	}

	void QStackedBarsController3D::setAxisZ(QAbstractAxis3D *axis)
	{
		QAbstractController3D::setAxisZ(axis);
		handleDataRowLabelsChanged();
	}

	void QStackedBarsController3D::setPrimarySeries(QStackedBarSeries3D *series)
	{
		if (!series) {
			if (m_seriesList.size())
				series = static_cast<QStackedBarSeries3D *>(m_seriesList.at(0));
		}
		else if (!m_seriesList.contains(series)) {
			// Add nonexistent series.
			addSeries(series);
		}

		if (m_primarySeries != series) {
			m_primarySeries = series;
			handleDataRowLabelsChanged();
			handleDataColumnLabelsChanged();
			emit primarySeriesChanged(m_primarySeries);
		}
	}

	QStackedBarSeries3D *QStackedBarsController3D::primarySeries() const
	{
		return m_primarySeries;
	}

	void QStackedBarsController3D::addSeries(QAbstractSeries3D *series)
	{
		insertSeries(m_seriesList.size(), series);
	}

	void QStackedBarsController3D::removeSeries(QAbstractSeries3D *series)
	{
		bool wasVisible = (series && series->d_ptr->m_controller == this && series->isVisible());

		QAbstractController3D::removeSeries(series);

		if (m_selectedBarSeries == series)
			setSelectedBar(invalidSelectionPosition(), 0, false);

		if (wasVisible)
			adjustAxisRanges();

		// If primary series is removed, reset it to default
		if (series == m_primarySeries) {
			if (m_seriesList.size())
				m_primarySeries = static_cast<QStackedBarSeries3D *>(m_seriesList.at(0));
			else
				m_primarySeries = 0;

			handleDataRowLabelsChanged();
			handleDataColumnLabelsChanged();

			emit primarySeriesChanged(m_primarySeries);
		}
	}

	void QStackedBarsController3D::insertSeries(int index, QAbstractSeries3D *series)
	{
		Q_ASSERT(series && series->type() == QAbstractSeries3D::SeriesTypeBar);

		int oldSize = m_seriesList.size();

		QAbstractController3D::insertSeries(index, series);

		if (oldSize != m_seriesList.size()) {
			QStackedBarSeries3D *barSeries = static_cast<QStackedBarSeries3D *>(series);
			if (!oldSize) {
				m_primarySeries = barSeries;
				handleDataRowLabelsChanged();
				handleDataColumnLabelsChanged();
			}

			if (barSeries->selectedBar() != invalidSelectionPosition())
				setSelectedBar(barSeries->selectedBar(), barSeries, false);

			if (!oldSize)
				emit primarySeriesChanged(m_primarySeries);
		}
	}

	QList<QStackedBarSeries3D *> QStackedBarsController3D::barSeriesList()
	{
		QList<QAbstractSeries3D *> abstractSeriesList = seriesList();
		QList<QStackedBarSeries3D *> barSeriesList;
		foreach(QAbstractSeries3D *abstractSeries, abstractSeriesList) {
			QStackedBarSeries3D *barSeries = qobject_cast<QStackedBarSeries3D *>(abstractSeries);
			if (barSeries)
				barSeriesList.append(barSeries);
		}

		return barSeriesList;
	}

	void QStackedBarsController3D::handleAxisRangeChangedBySender(QObject *sender)
	{
		// Data window changed
		if (sender == m_axisX || sender == m_axisZ) {
			if (sender == m_axisX)
				handleDataColumnLabelsChanged();
			if (sender == m_axisZ)
				handleDataRowLabelsChanged();
		}

		QAbstractController3D::handleAxisRangeChangedBySender(sender);

		// Update selected bar - may be moved offscreen
		setSelectedBar(m_selectedBar, m_selectedBarSeries, false);
	}

	void QStackedBarsController3D::setMultiSeriesScaling(bool uniform)
	{
		m_isMultiSeriesUniform = uniform;

		m_changeTracker.multiSeriesScalingChanged = true;
		emitNeedRender();
	}

	bool QStackedBarsController3D::multiSeriesScaling() const
	{
		return m_isMultiSeriesUniform;
	}

	void QStackedBarsController3D::setBarSpecs(GLfloat thicknessRatio, const QSizeF &spacing, bool relative)
	{
		m_barThicknessRatio = thicknessRatio;
		m_barSpacing = spacing;
		m_isBarSpecRelative = relative;

		m_changeTracker.barSpecsChanged = true;
		emitNeedRender();
	}

	GLfloat QStackedBarsController3D::barThickness()
	{
		return m_barThicknessRatio;
	}

	QSizeF QStackedBarsController3D::barSpacing()
	{
		return m_barSpacing;
	}

	bool QStackedBarsController3D::isBarSpecRelative()
	{
		return m_isBarSpecRelative;
	}

	void QStackedBarsController3D::setFloorLevel(float level)
	{
		m_floorLevel = level;
		m_isDataDirty = true;
		m_changeTracker.floorLevelChanged = true;
		emitNeedRender();
	}

	float QStackedBarsController3D::floorLevel() const
	{
		return m_floorLevel;
	}

	void QStackedBarsController3D::setSelectionMode(QAbstractGraph3D::SelectionFlags mode)
	{
		if (mode.testFlag(QAbstractGraph3D::SelectionSlice)
			&& (mode.testFlag(QAbstractGraph3D::SelectionRow)
				== mode.testFlag(QAbstractGraph3D::SelectionColumn))) {
			qWarning("Must specify one of either row or column selection mode in conjunction with slicing mode.");
		}
		else {
			QAbstractGraph3D::SelectionFlags oldMode = selectionMode();

			QAbstractController3D::setSelectionMode(mode);

			if (mode != oldMode) {
				// Refresh selection upon mode change to ensure slicing is correctly updated
				// according to series the visibility.
				setSelectedBar(m_selectedBar, m_selectedBarSeries, true);

				// Special case: Always deactivate slicing when changing away from slice
				// automanagement, as this can't be handled in setSelectedBar.
				if (!mode.testFlag(QAbstractGraph3D::SelectionSlice)
					&& oldMode.testFlag(QAbstractGraph3D::SelectionSlice)) {
					scene()->setSlicingActive(false);
				}
			}
		}
	}

	void QStackedBarsController3D::setSelectedBar(const QPoint &position, QStackedBarSeries3D *series, bool enterSlice)
	{
		// If the selection targets non-existent bar, clear selection instead.
		QPoint pos = position;

		// Series may already have been removed, so check it before setting the selection.
		if (!m_seriesList.contains(series))
			series = 0;

		adjustSelectionPosition(pos, series);

		if (selectionMode().testFlag(QAbstractGraph3D::SelectionSlice)) {
			// If the selected bar is outside data window, or there is no visible selected bar,
			// disable slicing.
			if (pos.x() < m_axisZ->min() || pos.x() > m_axisZ->max()
				|| pos.y() < m_axisX->min() || pos.y() > m_axisX->max()
				|| !series->isVisible()) {
				scene()->setSlicingActive(false);
			}
			else if (enterSlice) {
				scene()->setSlicingActive(true);
			}
			emitNeedRender();
		}

		if (pos != m_selectedBar || series != m_selectedBarSeries) {
			bool seriesChanged = (series != m_selectedBarSeries);
			m_selectedBar = pos;
			m_selectedBarSeries = series;
			m_changeTracker.selectedBarChanged = true;

			// Clear selection from other series and finally set new selection to the specified series
			foreach(QAbstractSeries3D *otherSeries, m_seriesList) {
				QStackedBarSeries3D *barSeries = static_cast<QStackedBarSeries3D *>(otherSeries);
				if (barSeries != m_selectedBarSeries)
					barSeries->dptr()->setSelectedBar(invalidSelectionPosition());
			}
			if (m_selectedBarSeries)
				m_selectedBarSeries->dptr()->setSelectedBar(m_selectedBar);

			if (seriesChanged)
				emit selectedSeriesChanged(m_selectedBarSeries);

			emitNeedRender();
		}
	}

	void QStackedBarsController3D::clearSelection()
	{
		setSelectedBar(invalidSelectionPosition(), 0, false);
	}

	void QStackedBarsController3D::adjustAxisRanges()
	{
		QCategoryAxis3D *categoryAxisZ = static_cast<QCategoryAxis3D *>(m_axisZ);
		QCategoryAxis3D *categoryAxisX = static_cast<QCategoryAxis3D *>(m_axisX);
		QValueAxis3D *valueAxis = static_cast<QValueAxis3D *>(m_axisY);

		bool adjustZ = (categoryAxisZ && categoryAxisZ->isAutoAdjustRange());
		bool adjustX = (categoryAxisX && categoryAxisX->isAutoAdjustRange());
		bool adjustY = (valueAxis && categoryAxisX && categoryAxisZ && valueAxis->isAutoAdjustRange());

		if (adjustZ || adjustX || adjustY) {
			int maxRowCount = 0;
			int maxColumnCount = 0;
			float minValue = 0.0f;
			float maxValue = 0.0f;

			// First figure out row and column counts
			int seriesCount = m_seriesList.size();
			if (adjustZ || adjustX) {
				for (int series = 0; series < seriesCount; series++) {
					const QStackedBarSeries3D *barSeries =
						static_cast<QStackedBarSeries3D *>(m_seriesList.at(series));
					if (barSeries->isVisible()) {
						const QStackedBarDataProxy3D *proxy = barSeries->dataProxy();

						if (adjustZ && proxy) {
							int rowCount = proxy->rowCount();
							if (rowCount)
								rowCount--;

							maxRowCount = qMax(maxRowCount, rowCount);
						}

						if (adjustX && proxy) {
							const QStackedBarDataArray3D *array = proxy->array();
							int columnCount = 0;
							for (int i = 0; i < array->size(); i++) {
								if (columnCount < array->at(i)->size())
									columnCount = array->at(i)->size();
							}
							if (columnCount)
								columnCount--;

							maxColumnCount = qMax(maxColumnCount, columnCount);
						}
					}
				}
				// Call private implementations of setRange to avoid unsetting auto adjust flag
				if (adjustZ)
					categoryAxisZ->dptr()->setRange(0.0f, float(maxRowCount), true);
				if (adjustX)
					categoryAxisX->dptr()->setRange(0.0f, float(maxColumnCount), true);
			}

			// Now that we know the row and column ranges, figure out the value axis range
			if (adjustY) {
				for (int series = 0; series < seriesCount; series++) {
					const QStackedBarSeries3D *barSeries =
						static_cast<QStackedBarSeries3D *>(m_seriesList.at(series));
					if (barSeries->isVisible()) {
						const QStackedBarDataProxy3D *proxy = barSeries->dataProxy();
						if (adjustY && proxy) {
							QPair<GLfloat, GLfloat> limits =
								proxy->dptrc()->limitValues(categoryAxisZ->min(),
									categoryAxisZ->max(),
									categoryAxisX->min(),
									categoryAxisX->max());
							if (!series) {
								// First series initializes the values
								minValue = limits.first;
								maxValue = limits.second;
							}
							else {
								minValue = qMin(minValue, limits.first);
								maxValue = qMax(maxValue, limits.second);
							}
						}
					}
				}

				if (maxValue < 0.0f)
					maxValue = 0.0f;
				if (minValue > 0.0f)
					minValue = 0.0f;
				if (minValue == 0.0f && maxValue == 0.0f) {
					// Only zero value values in data set, set range to something.
					minValue = 0.0f;
					maxValue = 1.0f;
				}
				valueAxis->dptr()->setRange(minValue, maxValue, true);
			}
		}
	}

	// Invalidate selection position if outside data for the series
	void QStackedBarsController3D::adjustSelectionPosition(QPoint &pos, const QStackedBarSeries3D *series)
	{
		const QStackedBarDataProxy3D *proxy = 0;
		if (series)
			proxy = series->dataProxy();

		if (!proxy)
			pos = invalidSelectionPosition();

		if (pos != invalidSelectionPosition()) {
			int maxRow = proxy->rowCount() - 1;
			int maxCol = (pos.x() <= maxRow && pos.x() >= 0 && proxy->rowAt(pos.x()))
				? proxy->rowAt(pos.x())->size() - 1 : -1;

			if (pos.x() < 0 || pos.x() > maxRow || pos.y() < 0 || pos.y() > maxCol)
				pos = invalidSelectionPosition();
		}
	}

	QAbstractAxis3D *QStackedBarsController3D::createDefaultAxis(QAbstractAxis3D::AxisOrientation orientation)
	{
		QAbstractAxis3D *defaultAxis = 0;

		if (orientation == QAbstractAxis3D::AxisOrientationY)
			defaultAxis = createDefaultValueAxis();
		else
			defaultAxis = createDefaultCategoryAxis();

		return defaultAxis;
	}

}
