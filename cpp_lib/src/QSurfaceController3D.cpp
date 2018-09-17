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

#include "QSurfaceController3D.h"
#include "QSurfaceRenderer3D.h"
#include "QValueAxis3DPrivate.h"
#include "QSurfaceDataProxy3DPrivate.h"
#include "QSurfaceSeries3D.h"
#include "QAbstractSeries3DPrivate.h"
#include "QSurfaceSeries3DPrivate.h"

#include <QtCore/QMutexLocker>

namespace QtStackedBar3DVis
{

	QSurfaceController3D::QSurfaceController3D(QRect rect, QScene3D *scene)
		: QAbstractController3D(rect, scene),
		m_renderer(0),
		m_selectedPoint(invalidSelectionPosition()),
		m_selectedSeries(0),
		m_flatShadingSupported(true),
		m_flipHorizontalGrid(false)
	{
		// Setting a null axis creates a new default axis according to orientation and graph type.
		// Note: these cannot be set in the QAbstractController3D constructor, as they will call virtual
		//       functions implemented by subclasses.
		setAxisX(0);
		setAxisY(0);
		setAxisZ(0);
	}

	QSurfaceController3D::~QSurfaceController3D()
	{
	}

	void QSurfaceController3D::initializeOpenGL()
	{
		QMutexLocker mutexLocker(&m_renderMutex);

		// Initialization is called multiple times when Qt Quick components are used
		if (isInitialized())
			return;

		m_renderer = new QSurfaceRenderer3D(this);
		setRenderer(m_renderer);

		emitNeedRender();
	}

	void QSurfaceController3D::synchDataToRenderer()
	{
		QMutexLocker mutexLocker(&m_renderMutex);

		if (!isInitialized())
			return;

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

		if (m_changeTracker.selectedPointChanged) {
			m_renderer->updateSelectedPoint(m_selectedPoint, m_selectedSeries);
			m_changeTracker.selectedPointChanged = false;
		}

		if (m_changeTracker.flipHorizontalGridChanged) {
			m_renderer->updateFlipHorizontalGrid(m_flipHorizontalGrid);
			m_changeTracker.flipHorizontalGridChanged = false;
		}

		if (m_changeTracker.surfaceTextureChanged) {
			m_renderer->updateSurfaceTextures(m_changedTextures);
			m_changeTracker.surfaceTextureChanged = false;
			m_changedTextures.clear();
		}
	}

	void QSurfaceController3D::handleAxisAutoAdjustRangeChangedInOrientation(
		QAbstractAxis3D::AxisOrientation orientation, bool autoAdjust)
	{
		Q_UNUSED(orientation)
			Q_UNUSED(autoAdjust)

			adjustAxisRanges();
	}

	void QSurfaceController3D::handleAxisRangeChangedBySender(QObject *sender)
	{
		QAbstractController3D::handleAxisRangeChangedBySender(sender);

		// Update selected point - may be moved offscreen
		setSelectedPoint(m_selectedPoint, m_selectedSeries, false);
	}

	void QSurfaceController3D::handleSeriesVisibilityChangedBySender(QObject *sender)
	{
		QAbstractController3D::handleSeriesVisibilityChangedBySender(sender);

		// Visibility changes may require disabling slicing,
		// so just reset selection to ensure everything is still valid.
		setSelectedPoint(m_selectedPoint, m_selectedSeries, false);
	}

	void QSurfaceController3D::handlePendingClick()
	{
		// This function is called while doing the sync, so it is okay to query from renderer
		QPoint position = m_renderer->clickedPosition();
		QSurfaceSeries3D *series = static_cast<QSurfaceSeries3D *>(m_renderer->clickedSeries());

		setSelectedPoint(position, series, true);

		QAbstractController3D::handlePendingClick();

		m_renderer->resetClickedStatus();
	}

	QPoint QSurfaceController3D::invalidSelectionPosition()
	{
		static QPoint invalidSelectionPoint(-1, -1);
		return invalidSelectionPoint;
	}

	bool QSurfaceController3D::isFlatShadingSupported()
	{
		return m_flatShadingSupported;
	}

	void QSurfaceController3D::addSeries(QAbstractSeries3D *series)
	{
		Q_ASSERT(series && series->type() == QAbstractSeries3D::SeriesTypeSurface);

		QAbstractController3D::addSeries(series);

		QSurfaceSeries3D *surfaceSeries = static_cast<QSurfaceSeries3D *>(series);
		if (surfaceSeries->selectedPoint() != invalidSelectionPosition())
			setSelectedPoint(surfaceSeries->selectedPoint(), surfaceSeries, false);

		if (!surfaceSeries->texture().isNull())
			updateSurfaceTexture(surfaceSeries);
	}

	void QSurfaceController3D::removeSeries(QAbstractSeries3D *series)
	{
		bool wasVisible = (series && series->d_ptr->m_controller == this && series->isVisible());

		QAbstractController3D::removeSeries(series);

		if (m_selectedSeries == series)
			setSelectedPoint(invalidSelectionPosition(), 0, false);

		if (wasVisible)
			adjustAxisRanges();
	}

	QList<QSurfaceSeries3D *> QSurfaceController3D::surfaceSeriesList()
	{
		QList<QAbstractSeries3D *> abstractSeriesList = seriesList();
		QList<QSurfaceSeries3D *> surfaceSeriesList;
		foreach(QAbstractSeries3D *abstractSeries, abstractSeriesList) {
			QSurfaceSeries3D *surfaceSeries = qobject_cast<QSurfaceSeries3D *>(abstractSeries);
			if (surfaceSeries)
				surfaceSeriesList.append(surfaceSeries);
		}

		return surfaceSeriesList;
	}

	void QSurfaceController3D::setFlipHorizontalGrid(bool flip)
	{
		if (m_flipHorizontalGrid != flip) {
			m_flipHorizontalGrid = flip;
			m_changeTracker.flipHorizontalGridChanged = true;
			emit flipHorizontalGridChanged(flip);
			emitNeedRender();
		}
	}

	bool QSurfaceController3D::flipHorizontalGrid() const
	{
		return m_flipHorizontalGrid;
	}

	void QSurfaceController3D::setSelectionMode(QAbstractGraph3D::SelectionFlags mode)
	{
		// Currently surface only supports row and column modes when also slicing
		if ((mode.testFlag(QAbstractGraph3D::SelectionRow)
			|| mode.testFlag(QAbstractGraph3D::SelectionColumn))
			&& !mode.testFlag(QAbstractGraph3D::SelectionSlice)) {
			qWarning("Unsupported selection mode.");
			return;
		}
		else if (mode.testFlag(QAbstractGraph3D::SelectionSlice)
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
				setSelectedPoint(m_selectedPoint, m_selectedSeries, true);

				// Special case: Always deactivate slicing when changing away from slice
				// automanagement, as this can't be handled in setSelectedBar.
				if (!mode.testFlag(QAbstractGraph3D::SelectionSlice)
					&& oldMode.testFlag(QAbstractGraph3D::SelectionSlice)) {
					scene()->setSlicingActive(false);
				}
			}
		}
	}

	void QSurfaceController3D::setSelectedPoint(const QPoint &position, QSurfaceSeries3D *series,
		bool enterSlice)
	{
		// If the selection targets non-existent point, clear selection instead.
		QPoint pos = position;

		// Series may already have been removed, so check it before setting the selection.
		if (!m_seriesList.contains(series))
			series = 0;

		const QSurfaceDataProxy3D *proxy = 0;
		if (series)
			proxy = series->dataProxy();

		if (!proxy)
			pos = invalidSelectionPosition();

		if (pos != invalidSelectionPosition()) {
			int maxRow = proxy->rowCount() - 1;
			int maxCol = proxy->columnCount() - 1;

			if (pos.x() < 0 || pos.x() > maxRow || pos.y() < 0 || pos.y() > maxCol)
				pos = invalidSelectionPosition();
		}

		if (selectionMode().testFlag(QAbstractGraph3D::SelectionSlice)) {
			if (pos == invalidSelectionPosition() || !series->isVisible()) {
				scene()->setSlicingActive(false);
			}
			else {
				// If the selected point is outside data window, or there is no selected point, disable slicing
				float axisMinX = m_axisX->min();
				float axisMaxX = m_axisX->max();
				float axisMinZ = m_axisZ->min();
				float axisMaxZ = m_axisZ->max();

				QSurfaceDataItem3D item = proxy->array()->at(pos.x())->at(pos.y());
				if (item.x() < axisMinX || item.x() > axisMaxX
					|| item.z() < axisMinZ || item.z() > axisMaxZ) {
					scene()->setSlicingActive(false);
				}
				else if (enterSlice) {
					scene()->setSlicingActive(true);
				}
			}
			emitNeedRender();
		}

		if (pos != m_selectedPoint || series != m_selectedSeries) {
			bool seriesChanged = (series != m_selectedSeries);
			m_selectedPoint = pos;
			m_selectedSeries = series;
			m_changeTracker.selectedPointChanged = true;

			// Clear selection from other series and finally set new selection to the specified series
			foreach(QAbstractSeries3D *otherSeries, m_seriesList) {
				QSurfaceSeries3D *surfaceSeries = static_cast<QSurfaceSeries3D *>(otherSeries);
				if (surfaceSeries != m_selectedSeries)
					surfaceSeries->dptr()->setSelectedPoint(invalidSelectionPosition());
			}
			if (m_selectedSeries)
				m_selectedSeries->dptr()->setSelectedPoint(m_selectedPoint);

			if (seriesChanged)
				emit selectedSeriesChanged(m_selectedSeries);

			emitNeedRender();
		}
	}

	void QSurfaceController3D::clearSelection()
	{
		setSelectedPoint(invalidSelectionPosition(), 0, false);
	}

	void QSurfaceController3D::handleArrayReset()
	{
		QSurfaceSeries3D *series;
		if (qobject_cast<QSurfaceDataProxy3D *>(sender()))
			series = static_cast<QSurfaceDataProxy3D *>(sender())->series();
		else
			series = static_cast<QSurfaceSeries3D *>(sender());

		if (series->isVisible()) {
			adjustAxisRanges();
			m_isDataDirty = true;
		}
		if (!m_changedSeriesList.contains(series))
			m_changedSeriesList.append(series);

		// Clear selection unless still valid
		setSelectedPoint(m_selectedPoint, m_selectedSeries, false);
		series->d_ptr->markItemLabelDirty();
		emitNeedRender();
	}

	void QSurfaceController3D::handleFlatShadingSupportedChange(bool supported)
	{
		// Handle renderer flat surface support indicator signal. This happens exactly once per renderer.
		if (m_flatShadingSupported != supported) {
			m_flatShadingSupported = supported;
			// Emit the change for all added surfaces
			foreach(QAbstractSeries3D *series, m_seriesList) {
				QSurfaceSeries3D *surfaceSeries = static_cast<QSurfaceSeries3D *>(series);
				emit surfaceSeries->flatShadingSupportedChanged(m_flatShadingSupported);
			}
		}
	}

	void QSurfaceController3D::handleRowsChanged(int startIndex, int count)
	{
		QSurfaceSeries3D *series = static_cast<QSurfaceDataProxy3D *>(QObject::sender())->series();
		int oldChangeCount = m_changedRows.size();
		if (!oldChangeCount)
			m_changedRows.reserve(count);

		int selectedRow = m_selectedPoint.x();
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
				if (series == m_selectedSeries && selectedRow == candidate)
					series->d_ptr->markItemLabelDirty();
			}
		}
		if (count) {
			m_changeTracker.rowsChanged = true;

			if (series->isVisible())
				adjustAxisRanges();
			emitNeedRender();
		}
	}

	void QSurfaceController3D::handleItemChanged(int rowIndex, int columnIndex)
	{
		QSurfaceDataProxy3D *sender = static_cast<QSurfaceDataProxy3D *>(QObject::sender());
		QSurfaceSeries3D *series = sender->series();

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

			if (series == m_selectedSeries && m_selectedPoint == candidate)
				series->d_ptr->markItemLabelDirty();

			if (series->isVisible())
				adjustAxisRanges();
			emitNeedRender();
		}
	}

	void QSurfaceController3D::handleRowsAdded(int startIndex, int count)
	{
		Q_UNUSED(startIndex)
			Q_UNUSED(count)
			QSurfaceSeries3D *series = static_cast<QSurfaceDataProxy3D *>(sender())->series();
		if (series->isVisible()) {
			adjustAxisRanges();
			m_isDataDirty = true;
		}
		if (!m_changedSeriesList.contains(series))
			m_changedSeriesList.append(series);
		emitNeedRender();
	}

	void QSurfaceController3D::handleRowsInserted(int startIndex, int count)
	{
		Q_UNUSED(startIndex)
			Q_UNUSED(count)
			QSurfaceSeries3D *series = static_cast<QSurfaceDataProxy3D *>(sender())->series();
		if (series == m_selectedSeries) {
			// If rows inserted to selected series before the selection, adjust the selection
			int selectedRow = m_selectedPoint.x();
			if (startIndex <= selectedRow) {
				selectedRow += count;
				setSelectedPoint(QPoint(selectedRow, m_selectedPoint.y()), m_selectedSeries, false);
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

	void QSurfaceController3D::handleRowsRemoved(int startIndex, int count)
	{
		Q_UNUSED(startIndex)
			Q_UNUSED(count)
			QSurfaceSeries3D *series = static_cast<QSurfaceDataProxy3D *>(sender())->series();
		if (series == m_selectedSeries) {
			// If rows removed from selected series before the selection, adjust the selection
			int selectedRow = m_selectedPoint.x();
			if (startIndex <= selectedRow) {
				if ((startIndex + count) > selectedRow)
					selectedRow = -1; // Selected row removed
				else
					selectedRow -= count; // Move selected row down by amount of rows removed

				setSelectedPoint(QPoint(selectedRow, m_selectedPoint.y()), m_selectedSeries, false);
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

	void QSurfaceController3D::updateSurfaceTexture(QSurfaceSeries3D *series)
	{
		m_changeTracker.surfaceTextureChanged = true;

		if (!m_changedTextures.contains(series))
			m_changedTextures.append(series);

		emitNeedRender();
	}

	void QSurfaceController3D::adjustAxisRanges()
	{
		QValueAxis3D *valueAxisX = static_cast<QValueAxis3D *>(m_axisX);
		QValueAxis3D *valueAxisY = static_cast<QValueAxis3D *>(m_axisY);
		QValueAxis3D *valueAxisZ = static_cast<QValueAxis3D *>(m_axisZ);
		bool adjustX = (valueAxisX && valueAxisX->isAutoAdjustRange());
		bool adjustY = (valueAxisY && valueAxisY->isAutoAdjustRange());
		bool adjustZ = (valueAxisZ && valueAxisZ->isAutoAdjustRange());
		bool first = true;

		if (adjustX || adjustY || adjustZ) {
			float minValueX = 0.0f;
			float maxValueX = 0.0f;
			float minValueY = 0.0f;
			float maxValueY = 0.0f;
			float minValueZ = 0.0f;
			float maxValueZ = 0.0f;
			int seriesCount = m_seriesList.size();
			for (int series = 0; series < seriesCount; series++) {
				const QSurfaceSeries3D *surfaceSeries =
					static_cast<QSurfaceSeries3D *>(m_seriesList.at(series));
				const QSurfaceDataProxy3D *proxy = surfaceSeries->dataProxy();
				if (surfaceSeries->isVisible() && proxy) {
					QVector3D minLimits;
					QVector3D maxLimits;
					proxy->dptrc()->limitValues(minLimits, maxLimits, valueAxisX, valueAxisY, valueAxisZ);
					if (adjustX) {
						if (first) {
							// First series initializes the values
							minValueX = minLimits.x();
							maxValueX = maxLimits.x();
						}
						else {
							minValueX = qMin(minValueX, minLimits.x());
							maxValueX = qMax(maxValueX, maxLimits.x());
						}
					}
					if (adjustY) {
						if (first) {
							// First series initializes the values
							minValueY = minLimits.y();
							maxValueY = maxLimits.y();
						}
						else {
							minValueY = qMin(minValueY, minLimits.y());
							maxValueY = qMax(maxValueY, maxLimits.y());
						}
					}
					if (adjustZ) {
						if (first) {
							// First series initializes the values
							minValueZ = minLimits.z();
							maxValueZ = maxLimits.z();
						}
						else {
							minValueZ = qMin(minValueZ, minLimits.z());
							maxValueZ = qMax(maxValueZ, maxLimits.z());
						}
					}
					first = false;
				}
			}

			static const float adjustmentRatio = 20.0f;
			static const float defaultAdjustment = 1.0f;

			if (adjustX) {
				// If all points at same coordinate, need to default to some valid range
				float adjustment = 0.0f;
				if (minValueX == maxValueX) {
					if (adjustZ) {
						// X and Z are linked to have similar unit size, so choose the valid range based on it
						if (minValueZ == maxValueZ)
							adjustment = defaultAdjustment;
						else
							adjustment = qAbs(maxValueZ - minValueZ) / adjustmentRatio;
					}
					else {
						if (valueAxisZ)
							adjustment = qAbs(valueAxisZ->max() - valueAxisZ->min()) / adjustmentRatio;
						else
							adjustment = defaultAdjustment;
					}
				}
				valueAxisX->dptr()->setRange(minValueX - adjustment, maxValueX + adjustment, true);
			}
			if (adjustY) {
				// If all points at same coordinate, need to default to some valid range
				// Y-axis unit is not dependent on other axes, so simply adjust +-1.0f
				float adjustment = 0.0f;
				if (minValueY == maxValueY)
					adjustment = defaultAdjustment;
				valueAxisY->dptr()->setRange(minValueY - adjustment, maxValueY + adjustment, true);
			}
			if (adjustZ) {
				// If all points at same coordinate, need to default to some valid range
				float adjustment = 0.0f;
				if (minValueZ == maxValueZ) {
					if (adjustX) {
						// X and Z are linked to have similar unit size, so choose the valid range based on it
						if (minValueX == maxValueX)
							adjustment = defaultAdjustment;
						else
							adjustment = qAbs(maxValueX - minValueX) / adjustmentRatio;
					}
					else {
						if (valueAxisX)
							adjustment = qAbs(valueAxisX->max() - valueAxisX->min()) / adjustmentRatio;
						else
							adjustment = defaultAdjustment;
					}
				}
				valueAxisZ->dptr()->setRange(minValueZ - adjustment, maxValueZ + adjustment, true);
			}
		}
	}

}
