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

#include "QAbstractController3DPrivate.h"
#include "QAbstractAxis3DPrivate.h"
#include "QValueAxis3DPrivate.h"
#include "QCategoryAxis3DPrivate.h"
#include "QAbstractRenderer3D.h"
#include "QAbstractInputHandler3DPrivate.h"
#include "QAbstractSeries3DPrivate.h"
#include "QTouchInputHandler3D.h"

#include "QThemeManager3D.h"
#include "QTheme3DPrivate.h"
#include "QCustomItem3DPrivate.h"
#include "QUtils3D.h"

#include <QtCore/QThread>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtCore/QMutexLocker>

namespace QtStackedBar3DVis
{
	 QAbstractController3D::QAbstractController3D(QRect initialViewport, QScene3D *scene,
		QObject *parent) :
		QObject(parent),
		d_ptr(new QAbstractController3DPrivate(this)),
		// m_themeManager(new QThemeManager3D(this)),
		m_selectionMode(QAbstractGraph3D::SelectionItem),
		m_shadowQuality(QAbstractGraph3D::ShadowQualityMedium),
		m_useOrthoProjection(false),
		m_aspectRatio(2.0),
		m_horizontalAspectRatio(0.0),
		m_optimizationHints(QAbstractGraph3D::OptimizationDefault),
		m_reflectionEnabled(false),
		m_reflectivity(0.5),
		m_locale(QLocale::c()),
		m_scene(scene),
		m_activeInputHandler(0),
		m_axisX(0),
		m_axisY(0),
		m_axisZ(0),
		m_renderer(0),
		m_isDataDirty(true),
		m_isCustomDataDirty(true),
		m_isCustomItemDirty(true),
		m_isSeriesVisualsDirty(true),
		m_renderPending(false),
		m_isPolar(false),
		m_radialLabelOffset(1.0f),
		m_measureFps(false),
		m_numFrames(0),
		m_currentFps(0.0),
		m_clickedType(QAbstractGraph3D::ElementNone),
		m_selectedLabelIndex(-1),
		m_selectedCustomItemIndex(-1),
		m_margin(-1.0)
	{
		if (!m_scene)
			m_scene = new QScene3D;
		m_scene->setParent(this);

		// Set initial theme
		QTheme3D *defaultTheme = new QTheme3D(QTheme3D::ThemeQt);
		defaultTheme->d_ptr->setDefaultTheme(true);
		setActiveTheme(defaultTheme);

		m_scene->d_ptr->setViewport(initialViewport);
		m_scene->activeLight()->setAutoPosition(true);

		// Create initial default input handler
		QAbstractInputHandler3D *inputHandler;
		inputHandler = new QTouchInputHandler3D();
		inputHandler->d_ptr->m_isDefaultHandler = true;
		setActiveInputHandler(inputHandler);
		connect(m_scene->d_ptr.data(), &QScene3DPrivate::needRender, this,
			& QAbstractController3D::emitNeedRender);
	}

	 QAbstractController3D::~ QAbstractController3D()
	{
		destroyRenderer();
		delete m_scene;
		// delete m_themeManager;
		foreach(QCustomItem3D *item, m_customItems)
			delete item;
		m_customItems.clear();
	}

	void  QAbstractController3D::destroyRenderer()
	{
		QMutexLocker mutexLocker(&m_renderMutex);
		// Renderer can be in another thread, don't delete it directly in that case
		if (m_renderer && m_renderer->thread() && m_renderer->thread() != this->thread())
			m_renderer->deleteLater();
		else
			delete m_renderer;
		m_renderer = 0;
	}

	/**
	* @brief setRenderer Sets the renderer to be used. isInitialized returns true from this point onwards.
	* @param renderer Renderer to be used.
	*/
	void  QAbstractController3D::setRenderer(QAbstractRenderer3D *renderer)
	{
		// Note: This function must be called within render mutex
		m_renderer = renderer;

		// If renderer is created in different thread than controller, make sure renderer gets
		// destroyed before the render thread finishes.
		if (renderer->thread() != this->thread()) {
			QObject::connect(renderer->thread(), &QThread::finished, this,
				& QAbstractController3D::destroyRenderer, Qt::DirectConnection);
		}
	}

	void  QAbstractController3D::addSeries(QAbstractSeries3D *series)
	{
		insertSeries(m_seriesList.size(), series);
	}

	void  QAbstractController3D::insertSeries(int index, QAbstractSeries3D *series)
	{
		if (series) {
			if (m_seriesList.contains(series)) {
				int oldIndex = m_seriesList.indexOf(series);
				if (index != oldIndex) {
					m_seriesList.removeOne(series);
					if (oldIndex < index)
						index--;
					m_seriesList.insert(index, series);
				}
			}
			else {
				int oldSize = m_seriesList.size();
				m_seriesList.insert(index, series);
				series->d_ptr->setController(this);
				QObject::connect(series, &QAbstractSeries3D::visibilityChanged,
					this, & QAbstractController3D::handleSeriesVisibilityChanged);
				series->d_ptr->resetToTheme(*d_ptr->themeManager()->activeTheme(), oldSize, false);
			}
			if (series->isVisible())
				handleSeriesVisibilityChangedBySender(series);
		}
	}

	void  QAbstractController3D::removeSeries(QAbstractSeries3D *series)
	{
		if (series && series->d_ptr->m_controller == this) {
			m_seriesList.removeAll(series);
			QObject::disconnect(series, &QAbstractSeries3D::visibilityChanged,
				this, & QAbstractController3D::handleSeriesVisibilityChanged);
			series->d_ptr->setController(0);
			m_isDataDirty = true;
			m_isSeriesVisualsDirty = true;
			emitNeedRender();
		}
	}

	QList<QAbstractSeries3D *>  QAbstractController3D::seriesList()
	{
		return m_seriesList;
	}

	/**
	* @brief synchDataToRenderer Called on the render thread while main GUI thread is blocked before rendering.
	*/
	void  QAbstractController3D::synchDataToRenderer()
	{
		// Subclass implementations check for renderer validity already, so no need to check here.

		m_renderPending = false;

		// If there are pending queries, handle those first
		if (m_renderer->isGraphPositionQueryResolved())
			handlePendingGraphPositionQuery();

		if (m_renderer->isClickQueryResolved())
			handlePendingClick();

		startRecordingRemovesAndInserts();

		if (m_scene->d_ptr->m_sceneDirty)
			m_renderer->updateScene(m_scene);

		m_renderer->updateTheme(d_ptr->themeManager()->activeTheme());

		if (m_changeTracker.polarChanged) {
			m_renderer->updatePolar(m_isPolar);
			m_changeTracker.polarChanged = false;
		}

		if (m_changeTracker.radialLabelOffsetChanged) {
			m_renderer->updateRadialLabelOffset(m_radialLabelOffset);
			m_changeTracker.radialLabelOffsetChanged = false;
		}

		if (m_changeTracker.shadowQualityChanged) {
			m_renderer->updateShadowQuality(m_shadowQuality);
			m_changeTracker.shadowQualityChanged = false;
		}

		if (m_changeTracker.selectionModeChanged) {
			m_renderer->updateSelectionMode(m_selectionMode);
			m_changeTracker.selectionModeChanged = false;
		}

		if (m_changeTracker.projectionChanged) {
			m_renderer->m_useOrthoProjection = m_useOrthoProjection;
			m_changeTracker.projectionChanged = false;
		}

		if (m_changeTracker.aspectRatioChanged) {
			m_renderer->updateAspectRatio(float(m_aspectRatio));
			m_changeTracker.aspectRatioChanged = false;
		}

		if (m_changeTracker.horizontalAspectRatioChanged) {
			m_renderer->updateHorizontalAspectRatio(float(m_horizontalAspectRatio));
			m_changeTracker.horizontalAspectRatioChanged = false;
		}

		if (m_changeTracker.optimizationHintChanged) {
			m_renderer->updateOptimizationHint(m_optimizationHints);
			m_changeTracker.optimizationHintChanged = false;
		}

		if (m_changeTracker.reflectionChanged) {
			m_renderer->m_reflectionEnabled = m_reflectionEnabled;
			m_changeTracker.reflectionChanged = false;
		}

		if (m_changeTracker.reflectivityChanged) {
			// Invert value to match functionality to the property description
			m_renderer->m_reflectivity = -(m_reflectivity - 1.0);
			m_changeTracker.reflectivityChanged = false;
		}

		if (m_changeTracker.axisXFormatterChanged) {
			m_changeTracker.axisXFormatterChanged = false;
			if (m_axisX->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisX = static_cast<QValueAxis3D *>(m_axisX);
				m_renderer->updateAxisFormatter(QAbstractAxis3D::AxisOrientationX,
					valueAxisX->formatter());
			}
		}
		if (m_changeTracker.axisYFormatterChanged) {
			m_changeTracker.axisYFormatterChanged = false;
			if (m_axisY->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisY = static_cast<QValueAxis3D *>(m_axisY);
				m_renderer->updateAxisFormatter(QAbstractAxis3D::AxisOrientationY,
					valueAxisY->formatter());
			}
		}
		if (m_changeTracker.axisZFormatterChanged) {
			m_changeTracker.axisZFormatterChanged = false;
			if (m_axisZ->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisZ = static_cast<QValueAxis3D *>(m_axisZ);
				m_renderer->updateAxisFormatter(QAbstractAxis3D::AxisOrientationZ,
					valueAxisZ->formatter());
			}
		}

		if (m_changeTracker.axisXTypeChanged) {
			m_renderer->updateAxisType(QAbstractAxis3D::AxisOrientationX, m_axisX->type());
			m_changeTracker.axisXTypeChanged = false;
		}

		if (m_changeTracker.axisYTypeChanged) {
			m_renderer->updateAxisType(QAbstractAxis3D::AxisOrientationY, m_axisY->type());
			m_changeTracker.axisYTypeChanged = false;
		}

		if (m_changeTracker.axisZTypeChanged) {
			m_renderer->updateAxisType(QAbstractAxis3D::AxisOrientationZ, m_axisZ->type());
			m_changeTracker.axisZTypeChanged = false;
		}

		if (m_changeTracker.axisXTitleChanged) {
			m_renderer->updateAxisTitle(QAbstractAxis3D::AxisOrientationX, m_axisX->title());
			m_changeTracker.axisXTitleChanged = false;
		}

		if (m_changeTracker.axisYTitleChanged) {
			m_renderer->updateAxisTitle(QAbstractAxis3D::AxisOrientationY, m_axisY->title());
			m_changeTracker.axisYTitleChanged = false;
		}

		if (m_changeTracker.axisZTitleChanged) {
			m_renderer->updateAxisTitle(QAbstractAxis3D::AxisOrientationZ, m_axisZ->title());
			m_changeTracker.axisZTitleChanged = false;
		}

		if (m_changeTracker.axisXLabelsChanged) {
			m_renderer->updateAxisLabels(QAbstractAxis3D::AxisOrientationX, m_axisX->labels());
			m_changeTracker.axisXLabelsChanged = false;
		}

		if (m_changeTracker.axisYLabelsChanged) {
			m_renderer->updateAxisLabels(QAbstractAxis3D::AxisOrientationY, m_axisY->labels());
			m_changeTracker.axisYLabelsChanged = false;
		}
		if (m_changeTracker.axisZLabelsChanged) {
			m_renderer->updateAxisLabels(QAbstractAxis3D::AxisOrientationZ, m_axisZ->labels());
			m_changeTracker.axisZLabelsChanged = false;
		}

		if (m_changeTracker.axisXRangeChanged) {
			m_renderer->updateAxisRange(QAbstractAxis3D::AxisOrientationX, m_axisX->min(),
				m_axisX->max());
			m_changeTracker.axisXRangeChanged = false;
		}

		if (m_changeTracker.axisYRangeChanged) {
			m_renderer->updateAxisRange(QAbstractAxis3D::AxisOrientationY, m_axisY->min(),
				m_axisY->max());
			m_changeTracker.axisYRangeChanged = false;
		}

		if (m_changeTracker.axisZRangeChanged) {
			m_renderer->updateAxisRange(QAbstractAxis3D::AxisOrientationZ, m_axisZ->min(),
				m_axisZ->max());
			m_changeTracker.axisZRangeChanged = false;
		}

		if (m_changeTracker.axisXSegmentCountChanged) {
			m_changeTracker.axisXSegmentCountChanged = false;
			if (m_axisX->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisX = static_cast<QValueAxis3D *>(m_axisX);
				m_renderer->updateAxisSegmentCount(QAbstractAxis3D::AxisOrientationX,
					valueAxisX->segmentCount());
			}
		}

		if (m_changeTracker.axisYSegmentCountChanged) {
			m_changeTracker.axisYSegmentCountChanged = false;
			if (m_axisY->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisY = static_cast<QValueAxis3D *>(m_axisY);
				m_renderer->updateAxisSegmentCount(QAbstractAxis3D::AxisOrientationY,
					valueAxisY->segmentCount());
			}
		}

		if (m_changeTracker.axisZSegmentCountChanged) {
			m_changeTracker.axisZSegmentCountChanged = false;
			if (m_axisZ->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisZ = static_cast<QValueAxis3D *>(m_axisZ);
				m_renderer->updateAxisSegmentCount(QAbstractAxis3D::AxisOrientationZ,
					valueAxisZ->segmentCount());
			}
		}

		if (m_changeTracker.axisXSubSegmentCountChanged) {
			m_changeTracker.axisXSubSegmentCountChanged = false;
			if (m_axisX->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisX = static_cast<QValueAxis3D *>(m_axisX);
				m_renderer->updateAxisSubSegmentCount(QAbstractAxis3D::AxisOrientationX,
					valueAxisX->subSegmentCount());
			}
		}

		if (m_changeTracker.axisYSubSegmentCountChanged) {
			m_changeTracker.axisYSubSegmentCountChanged = false;
			if (m_axisY->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisY = static_cast<QValueAxis3D *>(m_axisY);
				m_renderer->updateAxisSubSegmentCount(QAbstractAxis3D::AxisOrientationY,
					valueAxisY->subSegmentCount());
			}
		}

		if (m_changeTracker.axisZSubSegmentCountChanged) {
			m_changeTracker.axisZSubSegmentCountChanged = false;
			if (m_axisZ->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisZ = static_cast<QValueAxis3D *>(m_axisZ);
				m_renderer->updateAxisSubSegmentCount(QAbstractAxis3D::AxisOrientationZ,
					valueAxisZ->subSegmentCount());
			}
		}

		if (m_changeTracker.axisXLabelFormatChanged) {
			m_changeTracker.axisXLabelFormatChanged = false;
			if (m_axisX->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisX = static_cast<QValueAxis3D *>(m_axisX);
				m_renderer->updateAxisLabelFormat(QAbstractAxis3D::AxisOrientationX,
					valueAxisX->labelFormat());
			}
		}

		if (m_changeTracker.axisYLabelFormatChanged) {
			m_changeTracker.axisYLabelFormatChanged = false;
			if (m_axisY->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisY = static_cast<QValueAxis3D *>(m_axisY);
				m_renderer->updateAxisLabelFormat(QAbstractAxis3D::AxisOrientationY,
					valueAxisY->labelFormat());
			}
		}

		if (m_changeTracker.axisZLabelFormatChanged) {
			m_changeTracker.axisZLabelFormatChanged = false;
			if (m_axisZ->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisZ = static_cast<QValueAxis3D *>(m_axisZ);
				m_renderer->updateAxisLabelFormat(QAbstractAxis3D::AxisOrientationZ,
					valueAxisZ->labelFormat());
			}
		}

		if (m_changeTracker.axisXReversedChanged) {
			m_changeTracker.axisXReversedChanged = false;
			if (m_axisX->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisX = static_cast<QValueAxis3D *>(m_axisX);
				m_renderer->updateAxisReversed(QAbstractAxis3D::AxisOrientationX,
					valueAxisX->reversed());
			}
		}

		if (m_changeTracker.axisYReversedChanged) {
			m_changeTracker.axisYReversedChanged = false;
			if (m_axisY->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisY = static_cast<QValueAxis3D *>(m_axisY);
				m_renderer->updateAxisReversed(QAbstractAxis3D::AxisOrientationY,
					valueAxisY->reversed());
			}
		}

		if (m_changeTracker.axisZReversedChanged) {
			m_changeTracker.axisZReversedChanged = false;
			if (m_axisZ->type() & QAbstractAxis3D::AxisTypeValue) {
				QValueAxis3D *valueAxisZ = static_cast<QValueAxis3D *>(m_axisZ);
				m_renderer->updateAxisReversed(QAbstractAxis3D::AxisOrientationZ,
					valueAxisZ->reversed());
			}
		}

		if (m_changeTracker.axisXLabelAutoRotationChanged) {
			m_renderer->updateAxisLabelAutoRotation(QAbstractAxis3D::AxisOrientationX,
				m_axisX->labelAutoRotation());
			m_changeTracker.axisXLabelAutoRotationChanged = false;
		}

		if (m_changeTracker.axisYLabelAutoRotationChanged) {
			m_renderer->updateAxisLabelAutoRotation(QAbstractAxis3D::AxisOrientationY,
				m_axisY->labelAutoRotation());
			m_changeTracker.axisYLabelAutoRotationChanged = false;
		}

		if (m_changeTracker.axisZLabelAutoRotationChanged) {
			m_renderer->updateAxisLabelAutoRotation(QAbstractAxis3D::AxisOrientationZ,
				m_axisZ->labelAutoRotation());
			m_changeTracker.axisZLabelAutoRotationChanged = false;
		}

		if (m_changeTracker.axisXTitleVisibilityChanged) {
			m_renderer->updateAxisTitleVisibility(QAbstractAxis3D::AxisOrientationX,
				m_axisX->isTitleVisible());
			m_changeTracker.axisXTitleVisibilityChanged = false;
		}
		if (m_changeTracker.axisYTitleVisibilityChanged) {
			m_renderer->updateAxisTitleVisibility(QAbstractAxis3D::AxisOrientationY,
				m_axisY->isTitleVisible());
			m_changeTracker.axisYTitleVisibilityChanged = false;
		}
		if (m_changeTracker.axisZTitleVisibilityChanged) {
			m_renderer->updateAxisTitleVisibility(QAbstractAxis3D::AxisOrientationZ,
				m_axisZ->isTitleVisible());
			m_changeTracker.axisZTitleVisibilityChanged = false;
		}
		if (m_changeTracker.axisXTitleFixedChanged) {
			m_renderer->updateAxisTitleFixed(QAbstractAxis3D::AxisOrientationX,
				m_axisX->isTitleFixed());
			m_changeTracker.axisXTitleFixedChanged = false;
		}
		if (m_changeTracker.axisYTitleFixedChanged) {
			m_renderer->updateAxisTitleFixed(QAbstractAxis3D::AxisOrientationY,
				m_axisY->isTitleFixed());
			m_changeTracker.axisYTitleFixedChanged = false;
		}
		if (m_changeTracker.axisZTitleFixedChanged) {
			m_renderer->updateAxisTitleFixed(QAbstractAxis3D::AxisOrientationZ,
				m_axisZ->isTitleFixed());
			m_changeTracker.axisZTitleFixedChanged = false;
		}

		if (m_changeTracker.marginChanged) {
			m_renderer->updateMargin(float(m_margin));
			m_changeTracker.marginChanged = false;
		}

		if (m_changedSeriesList.size()) {
			m_renderer->modifiedSeriesList(m_changedSeriesList);
			m_changedSeriesList.clear();
		}

		if (m_isSeriesVisualsDirty) {
			m_renderer->updateSeries(m_seriesList);
			m_isSeriesVisualsDirty = false;
		}

		if (m_isDataDirty) {
			// Series list supplied above in updateSeries() is used to access the data,
			// so no data needs to be passed in updateData()
			m_renderer->updateData();
			m_isDataDirty = false;
		}

		if (m_isCustomDataDirty) {
			m_renderer->updateCustomData(m_customItems);
			m_isCustomDataDirty = false;
		}

		if (m_isCustomItemDirty) {
			m_renderer->updateCustomItems();
			m_isCustomItemDirty = false;
		}
	}

	void  QAbstractController3D::render(const GLuint defaultFboHandle)
	{
		QMutexLocker mutexLocker(&m_renderMutex);

		// If not initialized, do nothing.
		if (!m_renderer)
			return;

		if (m_measureFps) {
			// Measure speed (as milliseconds per frame)
			m_numFrames++;
			int elapsed = m_frameTimer.elapsed();
			if (elapsed >= 1000) {
				m_currentFps = qreal(m_numFrames) * 1000.0 / qreal(elapsed);
				emit currentFpsChanged(m_currentFps);
				m_numFrames = 0;
				m_frameTimer.restart();
			}
			// To get meaningful framerate, don't just do render on demand.
			emitNeedRender();
		}

		m_renderer->render(defaultFboHandle);
	}

	void  QAbstractController3D::mouseDoubleClickEvent(QMouseEvent *event)
	{
		if (m_activeInputHandler)
			m_activeInputHandler->mouseDoubleClickEvent(event);
	}

	void  QAbstractController3D::touchEvent(QTouchEvent *event)
	{
		if (m_activeInputHandler)
			m_activeInputHandler->touchEvent(event);
	}

	void  QAbstractController3D::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
	{
		if (m_activeInputHandler)
			m_activeInputHandler->mousePressEvent(event, mousePos);
	}

	void  QAbstractController3D::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
	{
		if (m_activeInputHandler)
			m_activeInputHandler->mouseReleaseEvent(event, mousePos);
	}

	void  QAbstractController3D::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
	{
		if (m_activeInputHandler)
			m_activeInputHandler->mouseMoveEvent(event, mousePos);
	}

	#if QT_CONFIG(wheelevent)
	void  QAbstractController3D::wheelEvent(QWheelEvent *event)
	{
		if (m_activeInputHandler)
			m_activeInputHandler->wheelEvent(event);
	}
	#endif

	void  QAbstractController3D::handleThemeColorStyleChanged(QTheme3D::ColorStyle style)
	{
		// Set value for series that have not explicitly set this value
		foreach(QAbstractSeries3D *series, m_seriesList) {
			if (!series->d_ptr->m_themeTracker.colorStyleOverride) {
				series->setColorStyle(style);
				series->d_ptr->m_themeTracker.colorStyleOverride = false;
			}
		}
		markSeriesVisualsDirty();
	}

	void  QAbstractController3D::handleThemeBaseColorsChanged(const QList<QColor> &colors)
	{
		int colorIdx = 0;
		// Set value for series that have not explicitly set this value
		foreach(QAbstractSeries3D *series, m_seriesList) {
			if (!series->d_ptr->m_themeTracker.baseColorOverride) {
				series->setBaseColor(colors.at(colorIdx));
				series->d_ptr->m_themeTracker.baseColorOverride = false;
			}
			if (++colorIdx >= colors.size())
				colorIdx = 0;
		}
		markSeriesVisualsDirty();
	}

	void  QAbstractController3D::handleThemeBaseGradientsChanged(const QList<QLinearGradient> &gradients)
	{
		int gradientIdx = 0;
		// Set value for series that have not explicitly set this value
		foreach(QAbstractSeries3D *series, m_seriesList) {
			if (!series->d_ptr->m_themeTracker.baseGradientOverride) {
				series->setBaseGradient(gradients.at(gradientIdx));
				series->d_ptr->m_themeTracker.baseGradientOverride = false;
			}
			if (++gradientIdx >= gradients.size())
				gradientIdx = 0;
		}
		markSeriesVisualsDirty();
	}

	void  QAbstractController3D::handleThemeSingleHighlightColorChanged(const QColor &color)
	{
		// Set value for series that have not explicitly set this value
		foreach(QAbstractSeries3D *series, m_seriesList) {
			if (!series->d_ptr->m_themeTracker.singleHighlightColorOverride) {
				series->setSingleHighlightColor(color);
				series->d_ptr->m_themeTracker.singleHighlightColorOverride = false;
			}
		}
		markSeriesVisualsDirty();
	}

	void  QAbstractController3D::handleThemeSingleHighlightGradientChanged(
		const QLinearGradient &gradient)
	{
		// Set value for series that have not explicitly set this value
		foreach(QAbstractSeries3D *series, m_seriesList) {
			if (!series->d_ptr->m_themeTracker.singleHighlightGradientOverride) {
				series->setSingleHighlightGradient(gradient);
				series->d_ptr->m_themeTracker.singleHighlightGradientOverride = false;
			}
		}
		markSeriesVisualsDirty();
	}

	void  QAbstractController3D::handleThemeMultiHighlightColorChanged(const QColor &color)
	{
		// Set value for series that have not explicitly set this value
		foreach(QAbstractSeries3D *series, m_seriesList) {
			if (!series->d_ptr->m_themeTracker.multiHighlightColorOverride) {
				series->setMultiHighlightColor(color);
				series->d_ptr->m_themeTracker.multiHighlightColorOverride = false;
			}
		}
		markSeriesVisualsDirty();
	}

	void  QAbstractController3D::handleThemeMultiHighlightGradientChanged(const QLinearGradient &gradient)
	{
		// Set value for series that have not explicitly set this value
		foreach(QAbstractSeries3D *series, m_seriesList) {
			if (!series->d_ptr->m_themeTracker.multiHighlightGradientOverride) {
				series->setMultiHighlightGradient(gradient);
				series->d_ptr->m_themeTracker.multiHighlightGradientOverride = false;
			}
		}
		markSeriesVisualsDirty();
	}

	void  QAbstractController3D::handleThemeTypeChanged(QTheme3D::Theme theme)
	{
		Q_UNUSED(theme)

			// Changing theme type is logically equivalent of changing the entire theme
			// object, so reset all attached series to the new theme.

			QTheme3D *activeTheme = d_ptr->themeManager()->activeTheme();
		for (int i = 0; i < m_seriesList.size(); i++)
			m_seriesList.at(i)->d_ptr->resetToTheme(*activeTheme, i, true);
		markSeriesVisualsDirty();
	}

	void  QAbstractController3D::setAxisX(QAbstractAxis3D *axis)
	{
		// Setting null axis will always create new default axis
		if (!axis || axis != m_axisX) {
			setAxisHelper(QAbstractAxis3D::AxisOrientationX, axis, &m_axisX);
			emit axisXChanged(m_axisX);
		}
	}

	QAbstractAxis3D * QAbstractController3D::axisX() const
	{
		return m_axisX;
	}

	void  QAbstractController3D::setAxisY(QAbstractAxis3D *axis)
	{
		// Setting null axis will always create new default axis
		if (!axis || axis != m_axisY) {
			setAxisHelper(QAbstractAxis3D::AxisOrientationY, axis, &m_axisY);
			emit axisYChanged(m_axisY);
		}
	}

	QAbstractAxis3D * QAbstractController3D::axisY() const
	{
		return m_axisY;
	}

	void  QAbstractController3D::setAxisZ(QAbstractAxis3D *axis)
	{
		// Setting null axis will always create new default axis
		if (!axis || axis != m_axisZ) {
			setAxisHelper(QAbstractAxis3D::AxisOrientationZ, axis, &m_axisZ);
			emit axisZChanged(m_axisZ);
		}
	}

	QAbstractAxis3D * QAbstractController3D::axisZ() const
	{
		return m_axisZ;
	}

	void  QAbstractController3D::addAxis(QAbstractAxis3D *axis)
	{
		Q_ASSERT(axis);
		 QAbstractController3D *owner = qobject_cast< QAbstractController3D *>(axis->parent());
		if (owner != this) {
			Q_ASSERT_X(!owner, "addAxis", "Axis already attached to a graph.");
			axis->setParent(this);
		}
		if (!m_axes.contains(axis))
			m_axes.append(axis);
	}

	void  QAbstractController3D::releaseAxis(QAbstractAxis3D *axis)
	{
		if (axis && m_axes.contains(axis)) {
			// Clear the default status from released default axes
			if (axis->d_ptr->isDefaultAxis())
				axis->d_ptr->setDefaultAxis(false);

			// If the axis is in use, replace it with a temporary one
			switch (axis->orientation()) {
			case QAbstractAxis3D::AxisOrientationX:
				setAxisX(0);
				break;
			case QAbstractAxis3D::AxisOrientationY:
				setAxisY(0);
				break;
			case QAbstractAxis3D::AxisOrientationZ:
				setAxisZ(0);
				break;
			default:
				break;
			}

			m_axes.removeAll(axis);
			axis->setParent(0);
		}
	}

	QList<QAbstractAxis3D *>  QAbstractController3D::axes() const
	{
		return m_axes;
	}

	void  QAbstractController3D::addInputHandler(QAbstractInputHandler3D *inputHandler)
	{
		Q_ASSERT(inputHandler);
		 QAbstractController3D *owner = qobject_cast< QAbstractController3D *>(inputHandler->parent());
		if (owner != this) {
			Q_ASSERT_X(!owner, "addInputHandler",
				"Input handler already attached to another component.");
			inputHandler->setParent(this);
		}

		if (!m_inputHandlers.contains(inputHandler))
			m_inputHandlers.append(inputHandler);
	}

	void  QAbstractController3D::releaseInputHandler(QAbstractInputHandler3D *inputHandler)
	{
		if (inputHandler && m_inputHandlers.contains(inputHandler)) {
			// Clear the default status from released default input handler
			if (inputHandler->d_ptr->m_isDefaultHandler)
				inputHandler->d_ptr->m_isDefaultHandler = false;

			// If the input handler is in use, remove it
			if (m_activeInputHandler == inputHandler)
				setActiveInputHandler(0);

			m_inputHandlers.removeAll(inputHandler);
			inputHandler->setParent(0);
		}
	}

	void  QAbstractController3D::setActiveInputHandler(QAbstractInputHandler3D *inputHandler)
	{
		if (inputHandler == m_activeInputHandler)
			return;

		// If existing input handler is the default input handler, delete it
		if (m_activeInputHandler) {
			if (m_activeInputHandler->d_ptr->m_isDefaultHandler) {
				m_inputHandlers.removeAll(m_activeInputHandler);
				delete m_activeInputHandler;
			}
			else {
				// Disconnect the old input handler
				m_activeInputHandler->setScene(0);
				QObject::disconnect(m_activeInputHandler, 0, this, 0);
			}
		}

		// Assume ownership and connect to this controller's scene
		if (inputHandler)
			addInputHandler(inputHandler);

		m_activeInputHandler = inputHandler;
		if (m_activeInputHandler) {
			m_activeInputHandler->setScene(m_scene);

			// Connect the input handler
			QObject::connect(m_activeInputHandler, &QAbstractInputHandler3D::inputViewChanged, this,
				& QAbstractController3D::handleInputViewChanged);
			QObject::connect(m_activeInputHandler, &QAbstractInputHandler3D::positionChanged, this,
				& QAbstractController3D::handleInputPositionChanged);
		}

		// Notify change of input handler
		emit activeInputHandlerChanged(m_activeInputHandler);
	}

	QAbstractInputHandler3D*  QAbstractController3D::activeInputHandler()
	{
		return m_activeInputHandler;
	}

	QList<QAbstractInputHandler3D *>  QAbstractController3D::inputHandlers() const
	{
		return m_inputHandlers;
	}

	void  QAbstractController3D::addTheme(QTheme3D *theme)
	{
		d_ptr->themeManager()->addTheme(theme);
	}

	void  QAbstractController3D::releaseTheme(QTheme3D *theme)
	{
		QTheme3D *oldTheme = d_ptr->themeManager()->activeTheme();

		d_ptr->themeManager()->releaseTheme(theme);

		if (oldTheme != d_ptr->themeManager()->activeTheme())
			emit activeThemeChanged(d_ptr->themeManager()->activeTheme());
	}

	QList<QTheme3D *>  QAbstractController3D::themes() const
	{
		return d_ptr->themeManager()->themes();
	}

	void  QAbstractController3D::setActiveTheme(QTheme3D *theme, bool force)
	{
		if (theme != d_ptr->themeManager()->activeTheme()) {
			d_ptr->themeManager()->setActiveTheme(theme);
			m_changeTracker.themeChanged = true;
			// Default theme can be created by theme manager, so ensure we have correct theme
			QTheme3D *newActiveTheme = d_ptr->themeManager()->activeTheme();
			// Reset all attached series to the new theme
			for (int i = 0; i < m_seriesList.size(); i++)
				m_seriesList.at(i)->d_ptr->resetToTheme(*newActiveTheme, i, force);
			markSeriesVisualsDirty();
			emit activeThemeChanged(newActiveTheme);
		}
	}

	QTheme3D * QAbstractController3D::activeTheme() const
	{
		return d_ptr->themeManager()->activeTheme();
	}

	void  QAbstractController3D::setSelectionMode(QAbstractGraph3D::SelectionFlags mode)
	{
		if (mode != m_selectionMode) {
			m_selectionMode = mode;
			m_changeTracker.selectionModeChanged = true;
			emit selectionModeChanged(mode);
			emitNeedRender();
		}
	}

	QAbstractGraph3D::SelectionFlags  QAbstractController3D::selectionMode() const
	{
		return m_selectionMode;
	}

	void  QAbstractController3D::setShadowQuality(QAbstractGraph3D::ShadowQuality quality)
	{
		if (!m_useOrthoProjection)
			doSetShadowQuality(quality);
	}

	void  QAbstractController3D::doSetShadowQuality(QAbstractGraph3D::ShadowQuality quality)
	{
		if (quality != m_shadowQuality) {
			m_shadowQuality = quality;
			m_changeTracker.shadowQualityChanged = true;
			emit shadowQualityChanged(m_shadowQuality);
			emitNeedRender();
		}
	}

	QAbstractGraph3D::ShadowQuality  QAbstractController3D::shadowQuality() const
	{
		return m_shadowQuality;
	}

	void  QAbstractController3D::setOptimizationHints(QAbstractGraph3D::OptimizationHints hints)
	{
		if (hints != m_optimizationHints) {
			m_optimizationHints = hints;
			m_changeTracker.optimizationHintChanged = true;
			m_isDataDirty = true;
			emit optimizationHintsChanged(hints);
			emitNeedRender();
		}
	}

	QAbstractGraph3D::OptimizationHints  QAbstractController3D::optimizationHints() const
	{
		return m_optimizationHints;
	}

	bool  QAbstractController3D::shadowsSupported() const
	{
		return !isOpenGLES();
	}

	bool  QAbstractController3D::isSlicingActive() const
	{
		return m_scene->isSlicingActive();
	}

	void  QAbstractController3D::setSlicingActive(bool isSlicing)
	{
		m_scene->setSlicingActive(isSlicing);
	}

	QScene3D * QAbstractController3D::scene()
	{
		return m_scene;
	}

	void  QAbstractController3D::markDataDirty()
	{
		m_isDataDirty = true;

		markSeriesItemLabelsDirty();
		emitNeedRender();
	}

	void  QAbstractController3D::markSeriesVisualsDirty()
	{
		m_isSeriesVisualsDirty = true;
		emitNeedRender();
	}

	void  QAbstractController3D::requestRender(QOpenGLFramebufferObject *fbo)
	{
		QMutexLocker mutexLocker(&m_renderMutex);
		m_renderer->render(fbo->handle());
	}

	int  QAbstractController3D::addCustomItem(QCustomItem3D *item)
	{
		if (!item)
			return -1;

		int index = m_customItems.indexOf(item);

		if (index != -1)
			return index;

		item->setParent(this);
		connect(item->d_ptr.data(), &QCustomItem3DPrivate::needUpdate,
			this, & QAbstractController3D::updateCustomItem);
		m_customItems.append(item);
		item->d_ptr->resetDirtyBits();
		m_isCustomDataDirty = true;
		emitNeedRender();
		return m_customItems.count() - 1;
	}

	void  QAbstractController3D::deleteCustomItems()
	{
		foreach(QCustomItem3D *item, m_customItems)
			delete item;
		m_customItems.clear();
		m_isCustomDataDirty = true;
		emitNeedRender();
	}

	void  QAbstractController3D::deleteCustomItem(QCustomItem3D *item)
	{
		if (!item)
			return;

		m_customItems.removeOne(item);
		delete item;
		item = 0;
		m_isCustomDataDirty = true;
		emitNeedRender();
	}

	void  QAbstractController3D::deleteCustomItem(const QVector3D &position)
	{
		// Get the item for the position
		foreach(QCustomItem3D *item, m_customItems) {
			if (item->position() == position)
				deleteCustomItem(item);
		}
	}

	void  QAbstractController3D::releaseCustomItem(QCustomItem3D *item)
	{
		if (item && m_customItems.contains(item)) {
			disconnect(item->d_ptr.data(), &QCustomItem3DPrivate::needUpdate,
				this, & QAbstractController3D::updateCustomItem);
			m_customItems.removeOne(item);
			item->setParent(0);
			m_isCustomDataDirty = true;
			emitNeedRender();
		}
	}

	QList<QCustomItem3D *>  QAbstractController3D::customItems() const
	{
		return m_customItems;
	}

	void  QAbstractController3D::updateCustomItem()
	{
		m_isCustomItemDirty = true;
		emitNeedRender();
	}

	void  QAbstractController3D::handleAxisTitleChanged(const QString &title)
	{
		Q_UNUSED(title)
			handleAxisTitleChangedBySender(sender());
	}

	void  QAbstractController3D::handleAxisTitleChangedBySender(QObject *sender)
	{
		if (sender == m_axisX)
			m_changeTracker.axisXTitleChanged = true;
		else if (sender == m_axisY)
			m_changeTracker.axisYTitleChanged = true;
		else if (sender == m_axisZ)
			m_changeTracker.axisZTitleChanged = true;
		else
			qWarning() << __FUNCTION__ << "invoked for invalid axis";

		markSeriesItemLabelsDirty();
		emitNeedRender();
	}

	void  QAbstractController3D::handleAxisLabelsChanged()
	{
		handleAxisLabelsChangedBySender(sender());
	}

	void  QAbstractController3D::handleAxisLabelsChangedBySender(QObject *sender)
	{
		if (sender == m_axisX)
			m_changeTracker.axisXLabelsChanged = true;
		else if (sender == m_axisY)
			m_changeTracker.axisYLabelsChanged = true;
		else if (sender == m_axisZ)
			m_changeTracker.axisZLabelsChanged = true;
		else
			qWarning() << __FUNCTION__ << "invoked for invalid axis";

		markSeriesItemLabelsDirty();
		emitNeedRender();
	}

	void  QAbstractController3D::handleAxisRangeChanged(float min, float max)
	{
		Q_UNUSED(min)
			Q_UNUSED(max)
			handleAxisRangeChangedBySender(sender());
	}

	void  QAbstractController3D::handleAxisRangeChangedBySender(QObject *sender)
	{
		if (sender == m_axisX) {
			m_isDataDirty = true;
			m_changeTracker.axisXRangeChanged = true;
		}
		else if (sender == m_axisY) {
			m_isDataDirty = true;
			m_changeTracker.axisYRangeChanged = true;
		}
		else if (sender == m_axisZ) {
			m_isDataDirty = true;
			m_changeTracker.axisZRangeChanged = true;
		}
		else {
			qWarning() << __FUNCTION__ << "invoked for invalid axis";
		}
		emitNeedRender();
	}

	void  QAbstractController3D::handleAxisSegmentCountChanged(int count)
	{
		Q_UNUSED(count)
			handleAxisSegmentCountChangedBySender(sender());
	}

	void  QAbstractController3D::handleAxisSegmentCountChangedBySender(QObject *sender)
	{
		if (sender == m_axisX)
			m_changeTracker.axisXSegmentCountChanged = true;
		else if (sender == m_axisY)
			m_changeTracker.axisYSegmentCountChanged = true;
		else if (sender == m_axisZ)
			m_changeTracker.axisZSegmentCountChanged = true;
		else
			qWarning() << __FUNCTION__ << "invoked for invalid axis";
		emitNeedRender();
	}

	void  QAbstractController3D::handleAxisSubSegmentCountChanged(int count)
	{
		Q_UNUSED(count)
			handleAxisSubSegmentCountChangedBySender(sender());
	}

	void  QAbstractController3D::handleAxisSubSegmentCountChangedBySender(QObject *sender)
	{
		if (sender == m_axisX)
			m_changeTracker.axisXSubSegmentCountChanged = true;
		else if (sender == m_axisY)
			m_changeTracker.axisYSubSegmentCountChanged = true;
		else if (sender == m_axisZ)
			m_changeTracker.axisZSubSegmentCountChanged = true;
		else
			qWarning() << __FUNCTION__ << "invoked for invalid axis";
		emitNeedRender();
	}

	void  QAbstractController3D::handleAxisAutoAdjustRangeChanged(bool autoAdjust)
	{
		QObject *sender = QObject::sender();
		if (sender != m_axisX && sender != m_axisY && sender != m_axisZ)
			return;

		QAbstractAxis3D *axis = static_cast<QAbstractAxis3D*>(sender);
		handleAxisAutoAdjustRangeChangedInOrientation(axis->orientation(), autoAdjust);
	}

	void  QAbstractController3D::handleAxisLabelFormatChanged(const QString &format)
	{
		Q_UNUSED(format)
			handleAxisLabelFormatChangedBySender(sender());
	}

	void  QAbstractController3D::handleAxisReversedChanged(bool enable)
	{
		Q_UNUSED(enable)
			handleAxisReversedChangedBySender(sender());
	}

	void  QAbstractController3D::handleAxisFormatterDirty()
	{
		handleAxisFormatterDirtyBySender(sender());
	}

	void  QAbstractController3D::handleAxisLabelAutoRotationChanged(float angle)
	{
		Q_UNUSED(angle)
			handleAxisLabelAutoRotationChangedBySender(sender());
	}

	void  QAbstractController3D::handleAxisTitleVisibilityChanged(bool visible)
	{
		Q_UNUSED(visible)
			handleAxisTitleVisibilityChangedBySender(sender());
	}

	void  QAbstractController3D::handleAxisTitleFixedChanged(bool fixed)
	{
		Q_UNUSED(fixed)
			handleAxisTitleFixedChangedBySender(sender());
	}

	void  QAbstractController3D::handleInputViewChanged(QAbstractInputHandler3D::InputView view)
	{
		// When in automatic slicing mode, input view change to primary disables slice mode
		if (m_selectionMode.testFlag(QAbstractGraph3D::SelectionSlice)
			&& view == QAbstractInputHandler3D::InputViewOnPrimary) {
			setSlicingActive(false);
		}

		emitNeedRender();
	}

	void  QAbstractController3D::handleInputPositionChanged(const QPoint &position)
	{
		Q_UNUSED(position)
			emitNeedRender();
	}

	void  QAbstractController3D::handleSeriesVisibilityChanged(bool visible)
	{
		Q_UNUSED(visible)

			handleSeriesVisibilityChangedBySender(sender());
	}

	void  QAbstractController3D::handleRequestShadowQuality(QAbstractGraph3D::ShadowQuality quality)
	{
		setShadowQuality(quality);
	}

	void  QAbstractController3D::setMeasureFps(bool enable)
	{
		if (m_measureFps != enable) {
			m_measureFps = enable;
			m_currentFps = 0.0;

			if (enable) {
				m_frameTimer.start();
				m_numFrames = -1;
				emitNeedRender();
			}
			emit measureFpsChanged(enable);
		}
	}

	void  QAbstractController3D::handleAxisLabelFormatChangedBySender(QObject *sender)
	{
		// Label format changing needs to dirty the data so that labels are reset.
		if (sender == m_axisX) {
			m_isDataDirty = true;
			m_changeTracker.axisXLabelFormatChanged = true;
		}
		else if (sender == m_axisY) {
			m_isDataDirty = true;
			m_changeTracker.axisYLabelFormatChanged = true;
		}
		else if (sender == m_axisZ) {
			m_isDataDirty = true;
			m_changeTracker.axisZLabelFormatChanged = true;
		}
		else {
			qWarning() << __FUNCTION__ << "invoked for invalid axis";
		}
		emitNeedRender();
	}

	void  QAbstractController3D::handleAxisReversedChangedBySender(QObject *sender)
	{
		// Reversing change needs to dirty the data so item positions are recalculated
		if (sender == m_axisX) {
			m_isDataDirty = true;
			m_changeTracker.axisXReversedChanged = true;
		}
		else if (sender == m_axisY) {
			m_isDataDirty = true;
			m_changeTracker.axisYReversedChanged = true;
		}
		else if (sender == m_axisZ) {
			m_isDataDirty = true;
			m_changeTracker.axisZReversedChanged = true;
		}
		else {
			qWarning() << __FUNCTION__ << "invoked for invalid axis";
		}
		emitNeedRender();
	}

	void  QAbstractController3D::handleAxisFormatterDirtyBySender(QObject *sender)
	{
		// Sender is QValueAxis3DPrivate
		QValueAxis3D *valueAxis = static_cast<QValueAxis3DPrivate *>(sender)->qptr();
		if (valueAxis == m_axisX) {
			m_isDataDirty = true;
			m_changeTracker.axisXFormatterChanged = true;
		}
		else if (valueAxis == m_axisY) {
			m_isDataDirty = true;
			m_changeTracker.axisYFormatterChanged = true;
		}
		else if (valueAxis == m_axisZ) {
			m_isDataDirty = true;
			m_changeTracker.axisZFormatterChanged = true;
		}
		else {
			qWarning() << __FUNCTION__ << "invoked for invalid axis";
		}
		emitNeedRender();
	}

	void  QAbstractController3D::handleAxisLabelAutoRotationChangedBySender(QObject *sender)
	{
		if (sender == m_axisX)
			m_changeTracker.axisXLabelAutoRotationChanged = true;
		else if (sender == m_axisY)
			m_changeTracker.axisYLabelAutoRotationChanged = true;
		else if (sender == m_axisZ)
			m_changeTracker.axisZLabelAutoRotationChanged = true;
		else
			qWarning() << __FUNCTION__ << "invoked for invalid axis";

		emitNeedRender();
	}

	void  QAbstractController3D::handleAxisTitleVisibilityChangedBySender(QObject *sender)
	{
		if (sender == m_axisX)
			m_changeTracker.axisXTitleVisibilityChanged = true;
		else if (sender == m_axisY)
			m_changeTracker.axisYTitleVisibilityChanged = true;
		else if (sender == m_axisZ)
			m_changeTracker.axisZTitleVisibilityChanged = true;
		else
			qWarning() << __FUNCTION__ << "invoked for invalid axis";

		emitNeedRender();
	}

	void  QAbstractController3D::handleAxisTitleFixedChangedBySender(QObject *sender)
	{
		if (sender == m_axisX)
			m_changeTracker.axisXTitleFixedChanged = true;
		else if (sender == m_axisY)
			m_changeTracker.axisYTitleFixedChanged = true;
		else if (sender == m_axisZ)
			m_changeTracker.axisZTitleFixedChanged = true;
		else
			qWarning() << __FUNCTION__ << "invoked for invalid axis";

		emitNeedRender();
	}

	void  QAbstractController3D::handleSeriesVisibilityChangedBySender(QObject *sender)
	{
		QAbstractSeries3D *series = static_cast<QAbstractSeries3D *>(sender);
		series->d_ptr->m_changeTracker.visibilityChanged = true;

		m_isDataDirty = true;
		m_isSeriesVisualsDirty = true;

		adjustAxisRanges();

		emitNeedRender();
	}

	void  QAbstractController3D::markSeriesItemLabelsDirty()
	{
		for (int i = 0; i < m_seriesList.size(); i++)
			m_seriesList.at(i)->d_ptr->markItemLabelDirty();
	}

	bool  QAbstractController3D::isOpenGLES() const
	{
		return QUtils3D::isOpenGLES();
	}

	void  QAbstractController3D::setAxisHelper(QAbstractAxis3D::AxisOrientation orientation,
		QAbstractAxis3D *axis, QAbstractAxis3D **axisPtr)
	{
		// Setting null axis indicates using default axis
		if (!axis)
			axis = createDefaultAxis(orientation);

		// If old axis is default axis, delete it
		QAbstractAxis3D *oldAxis = *axisPtr;
		if (oldAxis) {
			if (oldAxis->d_ptr->isDefaultAxis()) {
				m_axes.removeAll(oldAxis);
				delete oldAxis;
				oldAxis = 0;
			}
			else {
				// Disconnect the old axis from use
				QObject::disconnect(oldAxis, 0, this, 0);
				oldAxis->d_ptr->setOrientation(QAbstractAxis3D::AxisOrientationNone);
			}
		}

		// Assume ownership
		addAxis(axis);

		// Connect the new axis
		*axisPtr = axis;

		axis->d_ptr->setOrientation(orientation);

		QObject::connect(axis, &QAbstractAxis3D::titleChanged,
			this, & QAbstractController3D::handleAxisTitleChanged);
		QObject::connect(axis, &QAbstractAxis3D::labelsChanged,
			this, & QAbstractController3D::handleAxisLabelsChanged);
		QObject::connect(axis, &QAbstractAxis3D::rangeChanged,
			this, & QAbstractController3D::handleAxisRangeChanged);
		QObject::connect(axis, &QAbstractAxis3D::autoAdjustRangeChanged,
			this, & QAbstractController3D::handleAxisAutoAdjustRangeChanged);
		QObject::connect(axis, &QAbstractAxis3D::labelAutoRotationChanged,
			this, & QAbstractController3D::handleAxisLabelAutoRotationChanged);
		QObject::connect(axis, &QAbstractAxis3D::titleVisibilityChanged,
			this, & QAbstractController3D::handleAxisTitleVisibilityChanged);
		QObject::connect(axis, &QAbstractAxis3D::titleFixedChanged,
			this, & QAbstractController3D::handleAxisTitleFixedChanged);

		if (orientation == QAbstractAxis3D::AxisOrientationX)
			m_changeTracker.axisXTypeChanged = true;
		else if (orientation == QAbstractAxis3D::AxisOrientationY)
			m_changeTracker.axisYTypeChanged = true;
		else if (orientation == QAbstractAxis3D::AxisOrientationZ)
			m_changeTracker.axisZTypeChanged = true;

		handleAxisTitleChangedBySender(axis);
		handleAxisLabelsChangedBySender(axis);
		handleAxisRangeChangedBySender(axis);
		handleAxisAutoAdjustRangeChangedInOrientation(axis->orientation(),
			axis->isAutoAdjustRange());
		handleAxisLabelAutoRotationChangedBySender(axis);
		handleAxisTitleVisibilityChangedBySender(axis);
		handleAxisTitleFixedChangedBySender(axis);

		if (axis->type() & QAbstractAxis3D::AxisTypeValue) {
			QValueAxis3D *valueAxis = static_cast<QValueAxis3D *>(axis);
			QObject::connect(valueAxis, &QValueAxis3D::segmentCountChanged,
				this, & QAbstractController3D::handleAxisSegmentCountChanged);
			QObject::connect(valueAxis, &QValueAxis3D::subSegmentCountChanged,
				this, & QAbstractController3D::handleAxisSubSegmentCountChanged);
			QObject::connect(valueAxis, &QValueAxis3D::labelFormatChanged,
				this, & QAbstractController3D::handleAxisLabelFormatChanged);
			QObject::connect(valueAxis, &QValueAxis3D::reversedChanged,
				this, & QAbstractController3D::handleAxisReversedChanged);
			QObject::connect(valueAxis->dptr(), &QValueAxis3DPrivate::formatterDirty,
				this, & QAbstractController3D::handleAxisFormatterDirty);

			handleAxisSegmentCountChangedBySender(valueAxis);
			handleAxisSubSegmentCountChangedBySender(valueAxis);
			handleAxisLabelFormatChangedBySender(valueAxis);
			handleAxisReversedChangedBySender(valueAxis);
			handleAxisFormatterDirtyBySender(valueAxis->dptr());

			valueAxis->formatter()->setLocale(m_locale);
		}
	}

	QAbstractAxis3D * QAbstractController3D::createDefaultAxis(
		QAbstractAxis3D::AxisOrientation orientation)
	{
		Q_UNUSED(orientation)

			// The default default axis is a value axis. If the graph type has a different default axis
			// for some orientation, this function needs to be overridden.
			QAbstractAxis3D *defaultAxis = createDefaultValueAxis();
		return defaultAxis;
	}

	QValueAxis3D * QAbstractController3D::createDefaultValueAxis()
	{
		// Default value axis has single segment, empty label format, and auto scaling
		QValueAxis3D *defaultAxis = new QValueAxis3D;
		defaultAxis->d_ptr->setDefaultAxis(true);

		return defaultAxis;
	}

	QCategoryAxis3D * QAbstractController3D::createDefaultCategoryAxis()
	{
		// Default category axis has no labels
		QCategoryAxis3D *defaultAxis = new QCategoryAxis3D;
		defaultAxis->d_ptr->setDefaultAxis(true);
		return defaultAxis;
	}

	void  QAbstractController3D::startRecordingRemovesAndInserts()
	{
		// Default implementation does nothing
	}

	void  QAbstractController3D::emitNeedRender()
	{
		if (!m_renderPending) {
			emit needRender();
			m_renderPending = true;
		}
	}

	void  QAbstractController3D::handlePendingClick()
	{
		m_clickedType = m_renderer->clickedType();
		m_selectedLabelIndex = m_renderer->m_selectedLabelIndex;
		m_selectedCustomItemIndex = m_renderer->m_selectedCustomItemIndex;

		// Invalidate query position to indicate the query has been handled, unless another
		// point has been queried.
		if (m_renderer->cachedClickQuery() == m_scene->selectionQueryPosition())
			m_scene->setSelectionQueryPosition(QScene3D::invalidSelectionPoint());

		m_renderer->clearClickQueryResolved();

		emit elementSelected(m_clickedType);
	}

	void  QAbstractController3D::handlePendingGraphPositionQuery()
	{
		m_queriedGraphPosition = m_renderer->queriedGraphPosition();

		// Invalidate query position to indicate the query has been handled, unless another
		// point has been queried.
		if (m_renderer->cachedGraphPositionQuery() == m_scene->graphPositionQuery())
			m_scene->setGraphPositionQuery(QScene3D::invalidSelectionPoint());

		m_renderer->clearGraphPositionQueryResolved();

		emit queriedGraphPositionChanged(m_queriedGraphPosition);
	}

	int  QAbstractController3D::selectedLabelIndex() const
	{
		int index = m_selectedLabelIndex;
		QAbstractAxis3D *axis = selectedAxis();
		if (axis && axis->labels().count() <= index)
			index = -1;
		return index;
	}

	QAbstractAxis3D * QAbstractController3D::selectedAxis() const
	{
		QAbstractAxis3D *axis = 0;
		QAbstractGraph3D::ElementType type = m_clickedType;
		switch (type) {
		case QAbstractGraph3D::ElementAxisXLabel:
			axis = axisX();
			break;
		case QAbstractGraph3D::ElementAxisYLabel:
			axis = axisY();
			break;
		case QAbstractGraph3D::ElementAxisZLabel:
			axis = axisZ();
			break;
		default:
			axis = 0;
			break;
		}

		return axis;
	}

	int  QAbstractController3D::selectedCustomItemIndex() const
	{
		int index = m_selectedCustomItemIndex;
		if (m_customItems.count() <= index)
			index = -1;
		return index;
	}

	QCustomItem3D * QAbstractController3D::selectedCustomItem() const
	{
		QCustomItem3D *item = 0;
		int index = selectedCustomItemIndex();
		if (index >= 0)
			item = m_customItems[index];
		return item;
	}

	QAbstractGraph3D::ElementType  QAbstractController3D::selectedElement() const
	{
		return m_clickedType;
	}

	void  QAbstractController3D::setOrthoProjection(bool enable)
	{
		if (enable != m_useOrthoProjection) {
			m_useOrthoProjection = enable;
			m_changeTracker.projectionChanged = true;
			emit orthoProjectionChanged(m_useOrthoProjection);
			// If changed to ortho, disable shadows
			if (m_useOrthoProjection)
				doSetShadowQuality(QAbstractGraph3D::ShadowQualityNone);
			emitNeedRender();
		}
	}

	bool  QAbstractController3D::isOrthoProjection() const
	{
		return m_useOrthoProjection;
	}

	void  QAbstractController3D::setAspectRatio(qreal ratio)
	{
		if (m_aspectRatio != ratio) {
			m_aspectRatio = ratio;
			m_changeTracker.aspectRatioChanged = true;
			emit aspectRatioChanged(m_aspectRatio);
			m_isDataDirty = true;
			emitNeedRender();
		}
	}

	qreal  QAbstractController3D::aspectRatio()
	{
		return m_aspectRatio;
	}

	void  QAbstractController3D::setHorizontalAspectRatio(qreal ratio)
	{
		if (m_horizontalAspectRatio != ratio) {
			m_horizontalAspectRatio = ratio;
			m_changeTracker.horizontalAspectRatioChanged = true;
			emit horizontalAspectRatioChanged(m_horizontalAspectRatio);
			m_isDataDirty = true;
			emitNeedRender();
		}
	}

	qreal  QAbstractController3D::horizontalAspectRatio() const
	{
		return m_horizontalAspectRatio;
	}

	void  QAbstractController3D::setReflection(bool enable)
	{
		if (m_reflectionEnabled != enable) {
			m_reflectionEnabled = enable;
			m_changeTracker.reflectionChanged = true;
			emit reflectionChanged(m_reflectionEnabled);
			emitNeedRender();
		}
	}

	bool  QAbstractController3D::reflection() const
	{
		return m_reflectionEnabled;
	}

	void  QAbstractController3D::setReflectivity(qreal reflectivity)
	{
		if (m_reflectivity != reflectivity) {
			m_reflectivity = reflectivity;
			m_changeTracker.reflectivityChanged = true;
			emit reflectivityChanged(m_reflectivity);
			emitNeedRender();
		}
	}

	qreal  QAbstractController3D::reflectivity() const
	{
		return m_reflectivity;
	}

	void  QAbstractController3D::setPolar(bool enable)
	{
		if (enable != m_isPolar) {
			m_isPolar = enable;
			m_changeTracker.polarChanged = true;
			m_isDataDirty = true;
			emit polarChanged(m_isPolar);
			emitNeedRender();
		}
	}

	bool  QAbstractController3D::isPolar() const
	{
		return m_isPolar;
	}

	void  QAbstractController3D::setRadialLabelOffset(float offset)
	{
		if (m_radialLabelOffset != offset) {
			m_radialLabelOffset = offset;
			m_changeTracker.radialLabelOffsetChanged = true;
			emit radialLabelOffsetChanged(m_radialLabelOffset);
			emitNeedRender();
		}
	}

	float  QAbstractController3D::radialLabelOffset() const
	{
		return m_radialLabelOffset;
	}

	void  QAbstractController3D::setLocale(const QLocale &locale)
	{
		if (m_locale != locale) {
			m_locale = locale;

			// Value axis formatters need to be updated
			QValueAxis3D *axis = qobject_cast<QValueAxis3D *>(m_axisX);
			if (axis)
				axis->formatter()->setLocale(m_locale);
			axis = qobject_cast<QValueAxis3D *>(m_axisY);
			if (axis)
				axis->formatter()->setLocale(m_locale);
			axis = qobject_cast<QValueAxis3D *>(m_axisZ);
			if (axis)
				axis->formatter()->setLocale(m_locale);
			emit localeChanged(m_locale);
		}
	}

	QLocale  QAbstractController3D::locale() const
	{
		return m_locale;
	}

	QVector3D  QAbstractController3D::queriedGraphPosition() const
	{
		return m_queriedGraphPosition;
	}

	void  QAbstractController3D::setMargin(qreal margin)
	{
		if (m_margin != margin) {
			m_margin = margin;
			m_changeTracker.marginChanged = true;
			emit marginChanged(margin);
			emitNeedRender();
		}
	}

	qreal  QAbstractController3D::margin() const
	{
		return m_margin;
	}

	QAbstractController3DPrivate::QAbstractController3DPrivate(QAbstractController3D* q)
	{
		q_ptr = q;
		m_themeManager = new QThemeManager3D(q);
	}

	QAbstractController3DPrivate::~QAbstractController3DPrivate()
	{
		delete m_themeManager;
	}

}
