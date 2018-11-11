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

#include "QStackedBarRenderer3D.h"
#include "QCamera3DPrivate.h"
#include "QShaderHelper3D.h"
#include "QTextureHelper3D.h"
#include "QUtils3D.h"
#include "QStackedBarSeriesRenderCache3D.h"

#include <QtCore/qmath.h>
#include <QtGui/qmatrix4x4.h>

// You can verify that depth buffer drawing works correctly by uncommenting this.
// You should see the scene from  where the light is
//#define SHOW_DEPTH_TEXTURE_SCENE

namespace QtStackedBar3DVis
{

	const bool sliceGridLabels = true;

	QStackedBarsRenderer3D::QStackedBarsRenderer3D(QStackedBarsController3D *controller)
		: QAbstractRenderer3D(controller),
		m_cachedIsSlicingActivated(false),
		m_cachedRowCount(0),
		m_cachedColumnCount(0),
		m_selectedBar(0),
		m_sliceCache(0),
		m_sliceTitleItem(0),
		m_updateLabels(false),
		m_barShader(0),
		m_barGradientShader(0),
		m_depthShader(0),
		m_selectionShader(0),
		m_backgroundShader(0),
		m_bgrTexture(0),
		m_selectionTexture(0),
		m_depthFrameBuffer(0),
		m_selectionFrameBuffer(0),
		m_selectionDepthBuffer(0),
		m_shadowQualityToShader(100.0f),
		m_shadowQualityMultiplier(3),
		m_heightNormalizer(1.0f),
		m_backgroundAdjustment(0.0f),
		m_rowWidth(0),
		m_columnDepth(0),
		m_maxDimension(0),
		m_scaleX(0),
		m_scaleZ(0),
		m_scaleFactor(0),
		m_maxSceneSize(40.0f),
		m_visualSelectedBarPos(QStackedBarsController3D::invalidSelectionPosition()),
		m_selectedBarPos(QStackedBarsController3D::invalidSelectionPosition()),
		m_selectedSeriesCache(0),
		m_noZeroInRange(false),
		m_seriesScaleX(0.0f),
		m_seriesScaleZ(0.0f),
		m_seriesStep(0.0f),
		m_seriesStart(0.0f),
		m_clickedPosition(QStackedBarsController3D::invalidSelectionPosition()),
		m_keepSeriesUniform(false),
		m_haveUniformColorSeries(false),
		m_haveGradientSeries(false),
		m_zeroPosition(0.0f),
		m_xScaleFactor(1.0f),
		m_zScaleFactor(1.0f),
		m_floorLevel(0.0f),
		m_actualFloorLevel(0.0f)
	{
		m_axisCacheY.setScale(2.0f);
		m_axisCacheY.setTranslate(-1.0f);

		initializeOpenGL();
	}

	QStackedBarsRenderer3D::~QStackedBarsRenderer3D()
	{
		fixContextBeforeDelete();

		if (QOpenGLContext::currentContext()) {
			m_textureHelper->glDeleteFramebuffers(1, &m_selectionFrameBuffer);
			m_textureHelper->glDeleteRenderbuffers(1, &m_selectionDepthBuffer);
			m_textureHelper->deleteTexture(&m_selectionTexture);
			m_textureHelper->glDeleteFramebuffers(1, &m_depthFrameBuffer);
			m_textureHelper->deleteTexture(&m_bgrTexture);
		}

		delete m_barShader;
		delete m_barGradientShader;
		delete m_depthShader;
		delete m_selectionShader;
		delete m_backgroundShader;
	}

	void QStackedBarsRenderer3D::initializeOpenGL()
	{
		QAbstractRenderer3D::initializeOpenGL();

		// Initialize shaders

		// Init depth shader (for shadows). Init in any case, easier to handle shadow activation if done via api.
		initDepthShader();

		// Init selection shader
		initSelectionShader();

		// Load grid line mesh
		loadGridLineMesh();

		// Load background mesh (we need to be initialized first)
		loadBackgroundMesh();
	}

	void QStackedBarsRenderer3D::fixCameraTarget(QVector3D &target)
	{
		target.setX(target.x() * m_xScaleFactor);
		target.setY(0.0f);
		target.setZ(target.z() * -m_zScaleFactor);
	}

	void QStackedBarsRenderer3D::getVisibleItemBounds(QVector3D &minBounds, QVector3D &maxBounds)
	{
		// The inputs are the item bounds in OpenGL coordinates.
		// The outputs limit these bounds to visible ranges, normalized to range [-1, 1]
		// Volume shader flips the Y and Z axes, so we need to set negatives of actual values to those
		float itemRangeX = (maxBounds.x() - minBounds.x());
		float itemRangeY = (maxBounds.y() - minBounds.y());
		float itemRangeZ = (maxBounds.z() - minBounds.z());

		if (minBounds.x() < -m_xScaleFactor)
			minBounds.setX(-1.0f + (2.0f * qAbs(minBounds.x() + m_xScaleFactor) / itemRangeX));
		else
			minBounds.setX(-1.0f);

		if (minBounds.y() < -1.0f + m_backgroundAdjustment)
			minBounds.setY(-(-1.0f + (2.0f * qAbs(minBounds.y() + 1.0f - m_backgroundAdjustment) / itemRangeY)));
		else
			minBounds.setY(1.0f);

		if (minBounds.z() < -m_zScaleFactor)
			minBounds.setZ(-(-1.0f + (2.0f * qAbs(minBounds.z() + m_zScaleFactor) / itemRangeZ)));
		else
			minBounds.setZ(1.0f);

		if (maxBounds.x() > m_xScaleFactor)
			maxBounds.setX(1.0f - (2.0f * qAbs(maxBounds.x() - m_xScaleFactor) / itemRangeX));
		else
			maxBounds.setX(1.0f);

		if (maxBounds.y() > 1.0f + m_backgroundAdjustment)
			maxBounds.setY(-(1.0f - (2.0f * qAbs(maxBounds.y() - 1.0f - m_backgroundAdjustment) / itemRangeY)));
		else
			maxBounds.setY(-1.0f);

		if (maxBounds.z() > m_zScaleFactor)
			maxBounds.setZ(-(1.0f - (2.0f * qAbs(maxBounds.z() - m_zScaleFactor) / itemRangeZ)));
		else
			maxBounds.setZ(-1.0f);
	}

	void QStackedBarsRenderer3D::updateData()
	{
		int minRow = m_axisCacheZ.min();
		int maxRow = m_axisCacheZ.max();
		int minCol = m_axisCacheX.min();
		int maxCol = m_axisCacheX.max();
		int newRows = maxRow - minRow + 1;
		int newColumns = maxCol - minCol + 1;
		int dataRowCount = 0;
		int maxDataRowCount = 0;

		m_seriesScaleX = 1.0f / float(m_visibleSeriesCount);
		m_seriesStep = 1.0f / float(m_visibleSeriesCount);
		m_seriesStart = -((float(m_visibleSeriesCount) - 1.0f) / 2.0f) * m_seriesStep;

		if (m_keepSeriesUniform)
			m_seriesScaleZ = m_seriesScaleX;
		else
			m_seriesScaleZ = 1.0f;

		if (m_cachedRowCount != newRows || m_cachedColumnCount != newColumns) {
			// Force update for selection related items
			m_sliceCache = 0;
			m_sliceTitleItem = 0;

			m_cachedColumnCount = newColumns;
			m_cachedRowCount = newRows;
			// Calculate max scene size
			GLfloat sceneRatio = qMin(GLfloat(newColumns) / GLfloat(newRows),
				GLfloat(newRows) / GLfloat(newColumns));
			m_maxSceneSize = 2.0f * qSqrt(sceneRatio * newColumns * newRows);
		}

		calculateSceneScalingFactors();

		m_zeroPosition = m_axisCacheY.formatter()->positionAt(m_actualFloorLevel);

		foreach(QSeriesRenderCache3D *baseCache, m_renderCacheList) {
			QStackedBarSeriesRenderCache3D *cache = static_cast<QStackedBarSeriesRenderCache3D *>(baseCache);
			if (cache->isVisible()) {
				const QStackedBarSeries3D *currentSeries = cache->series();
				QStackedBarRenderItemArray3D &renderArray = cache->renderArray();
				bool dimensionsChanged = false;
				if (newRows != renderArray.size()
					|| newColumns != renderArray.at(0).size()) {
					// Destroy old render items and reallocate new array
					dimensionsChanged = true;
					renderArray.resize(newRows);
					for (int i = 0; i < newRows; i++)
						renderArray[i].resize(newColumns);
					cache->sliceArray().clear();
				}

				if (cache->dataDirty() || dimensionsChanged) {
					QStackedBarDataProxy3D *dataProxy = currentSeries->dataProxy();
					dataRowCount = dataProxy->rowCount();
					if (maxDataRowCount < dataRowCount)
						maxDataRowCount = qMin(dataRowCount, newRows);
					int dataRowIndex = minRow;
					for (int i = 0; i < newRows; i++) {
						QStackedBarRenderItemRow3D &renderRow = renderArray[i];
						const QStackedBarDataRow3D *dataRow = 0;
						if (dataRowIndex < dataRowCount)
							dataRow = dataProxy->rowAt(dataRowIndex);
						updateRenderRow(dataRow, renderRow);
						dataRowIndex++;
					}
					cache->setDataDirty(false);
				}
			}
		}

		// Reset selected bar to update selection
		updateSelectedBar(m_selectedBarPos,
			m_selectedSeriesCache ? m_selectedSeriesCache->series() : 0);
	}

	void QStackedBarsRenderer3D::updateRenderRow(const QStackedBarDataRow3D *dataRow, QStackedBarRenderItemRow3D &renderRow)
	{
		int j = 0;
		int renderRowSize = renderRow.size();
		int startIndex = m_axisCacheX.min();

		int sectionCount = 1;
		if (dataRow) {
			int updateSize = qMin((dataRow->size() - startIndex), renderRowSize);
			int dataColIndex = startIndex;
			for (; j < updateSize; j++) {
				updateRenderItem(dataRow->at(dataColIndex), renderRow[j]);
				dataColIndex++;
			}

			if (dataRow->size() > 0 && dataRow->at(0).values().size() > 0)
			{
				sectionCount = dataRow->at(0).values().size();
			}
		}

        QList<qreal> initialValues;
		for (int k = 0; k < sectionCount; k++)
			initialValues << 0.0f;

		for (; j < renderRowSize; j++) {
			renderRow[j].setValues(initialValues);
			renderRow[j].setHeights(initialValues);
			renderRow[j].setRotation(identityQuaternion);
		}
	}

	void QStackedBarsRenderer3D::updateRenderItem(const QStackedBarDataItem3D &dataItem, QStackedBarRenderItem3D &renderItem)
	{
		//qDebug() << "updateRenderItem -- renderItem: " << renderItem.position() << ", dataItem: " << dataItem.values();
        const QList<qreal>& values = dataItem.values();
        QList<qreal> heightValues;
		float totalHeight = 0.0f;
		foreach(float value, values)
		{
			float heightValue = m_axisCacheY.formatter()->positionAt(value);
			//qDebug() << "height for value " << value << " = " << heightValue;

			totalHeight += heightValue;

			if (m_noZeroInRange) {
				if (m_hasNegativeValues) {
					heightValue = -1.0f + heightValue;
					if (heightValue > 0.0f)
						heightValue = 0.0f;
				}
				else {
					if (heightValue < 0.0f)
						heightValue = 0.0f;
				}
			}
			else {
				heightValue -= m_zeroPosition;
			}
			if (m_axisCacheY.reversed())
				heightValue = -heightValue;
		
			heightValues.append(heightValue); // For testing purposes
		}
		renderItem.setValues(values);
		renderItem.setHeights(heightValues);

        const QList<qreal>& angles = dataItem.rotations();
		if (angles.size() > 0) 
		{
			// Render items all share the same angle/rotation
			renderItem.setRotation(QQuaternion::fromAxisAndAngle(
				upVector, angles.at(0)));
		}
		else 
		{
			renderItem.setRotation(identityQuaternion);
		}
	}

	void QStackedBarsRenderer3D::updateSeries(const QList<QAbstractSeries3D *> &seriesList)
	{
		QAbstractRenderer3D::updateSeries(seriesList);

		bool noSelection = true;
		int seriesCount = seriesList.size();
		int visualIndex = 0;
		m_haveUniformColorSeries = false;
		m_haveGradientSeries = false;
		for (int i = 0; i < seriesCount; i++) {
			QStackedBarSeries3D *barSeries = static_cast<QStackedBarSeries3D *>(seriesList[i]);
			QStackedBarSeriesRenderCache3D *cache =
				static_cast<QStackedBarSeriesRenderCache3D *>(m_renderCacheList.value(barSeries));
			if (barSeries->isVisible()) {
				if (noSelection
					&& barSeries->selectedBar() != QStackedBarSeries3D::invalidSelectionPosition()) {
					if (selectionLabel() != cache->itemLabel())
						m_selectionLabelDirty = true;
					noSelection = false;
				}
				cache->setVisualIndex(visualIndex++);
				if (cache->colorStyle() == QTheme3D::ColorStyleUniform)
					m_haveUniformColorSeries = true;
				else
					m_haveGradientSeries = true;
			}
			else {
				cache->setVisualIndex(-1);
			}

		}
		if (noSelection) {
			if (!selectionLabel().isEmpty())
				m_selectionLabelDirty = true;
			m_selectedSeriesCache = 0;
		}
	}

	QSeriesRenderCache3D *QStackedBarsRenderer3D::createNewCache(QAbstractSeries3D *series)
	{
		return new QStackedBarSeriesRenderCache3D(series, this);
	}

	void QStackedBarsRenderer3D::updateRows(const QVector<QStackedBarsController3D::ChangeRow> &rows)
	{
		int minRow = m_axisCacheZ.min();
		int maxRow = m_axisCacheZ.max();
		QStackedBarSeriesRenderCache3D *cache = 0;
		const QStackedBarSeries3D *prevSeries = 0;
		const QStackedBarDataArray3D *dataArray = 0;

		foreach(QStackedBarsController3D::ChangeRow item, rows) {
			const int row = item.row;
			if (row < minRow || row > maxRow)
				continue;
			QStackedBarSeries3D *currentSeries = item.series;
            if (currentSeries != prevSeries)
            {
				cache = static_cast<QStackedBarSeriesRenderCache3D *>(m_renderCacheList.value(currentSeries));
				prevSeries = currentSeries;
				dataArray = item.series->dataProxy()->array();
                qDebug() << "Updating QStackedBarSeries3D: dataArray size = " << dataArray->size();
                qDebug() << "dataArray = " << *dataArray;
                // Invisible series render caches are not updated, but instead just marked dirty, so that
				// they can be completely recalculated when they are turned visible.
				if (!cache->isVisible() && !cache->dataDirty())
					cache->setDataDirty(true);
			}
            if (cache->isVisible())
            {
				updateRenderRow(dataArray->at(row), cache->renderArray()[row - minRow]);
				if (m_cachedIsSlicingActivated
					&& cache == m_selectedSeriesCache
					&& m_selectedBarPos.x() == row) {
					m_selectionDirty = true; // Need to update slice view
				}
			}
		}
	}

	void QStackedBarsRenderer3D::updateItems(const QVector<QStackedBarsController3D::ChangeItem> &items)
	{
		int minRow = m_axisCacheZ.min();
		int maxRow = m_axisCacheZ.max();
		int minCol = m_axisCacheX.min();
		int maxCol = m_axisCacheX.max();
		QStackedBarSeriesRenderCache3D *cache = 0;
		const QStackedBarSeries3D *prevSeries = 0;
		const QStackedBarDataArray3D *dataArray = 0;

		foreach(QStackedBarsController3D::ChangeItem item, items) {
			const int row = item.point.x();
			const int col = item.point.y();
			if (row < minRow || row > maxRow || col < minCol || col > maxCol)
				continue;
			QStackedBarSeries3D *currentSeries = item.series;
			if (currentSeries != prevSeries) {
				cache = static_cast<QStackedBarSeriesRenderCache3D *>(m_renderCacheList.value(currentSeries));
				prevSeries = currentSeries;
				dataArray = item.series->dataProxy()->array();
				// Invisible series render caches are not updated, but instead just marked dirty, so that
				// they can be completely recalculated when they are turned visible.
				if (!cache->isVisible() && !cache->dataDirty())
					cache->setDataDirty(true);
			}
			if (cache->isVisible()) {
				updateRenderItem(dataArray->at(row)->at(col),
					cache->renderArray()[row - minRow][col - minCol]);
				if (m_cachedIsSlicingActivated
					&& cache == m_selectedSeriesCache
					&& m_selectedBarPos == QPoint(row, col)) {
					m_selectionDirty = true; // Need to update slice view
				}
			}
		}
	}

	void QStackedBarsRenderer3D::updateScene(QScene3D *scene)
	{
		if (!m_noZeroInRange) {
			scene->activeCamera()->d_ptr->setMinYRotation(-90.0);
			scene->activeCamera()->d_ptr->setMaxYRotation(90.0);
		}
		else {
			if ((m_hasNegativeValues && !m_axisCacheY.reversed())
				|| (!m_hasNegativeValues && m_axisCacheY.reversed())) {
				scene->activeCamera()->d_ptr->setMinYRotation(-90.0f);
				scene->activeCamera()->d_ptr->setMaxYRotation(0.0);
			}
			else {
				scene->activeCamera()->d_ptr->setMinYRotation(0.0f);
				scene->activeCamera()->d_ptr->setMaxYRotation(90.0);
			}
		}

		QAbstractRenderer3D::updateScene(scene);

		updateSlicingActive(scene->isSlicingActive());
	}

	void QStackedBarsRenderer3D::render(GLuint defaultFboHandle)
	{
		// Handle GL state setup for FBO buffers and clearing of the render surface
		QAbstractRenderer3D::render(defaultFboHandle);

		if (m_axisCacheY.positionsDirty())
			m_axisCacheY.updateAllPositions();

		drawScene(defaultFboHandle);
		if (m_cachedIsSlicingActivated)
			drawSlicedScene();
	}

	void QStackedBarsRenderer3D::drawSlicedScene()
	{
		if (m_cachedSelectionMode.testFlag(QAbstractGraph3D::SelectionRow)
			== m_cachedSelectionMode.testFlag(QAbstractGraph3D::SelectionColumn)) {
			qWarning("Invalid selection mode. Either QAbstractGraph3D::SelectionRow or"
				" QAbstractGraph3D::SelectionColumn must be set before calling"
				" setSlicingActive(true).");
			return;
		}

		GLfloat barPosX = 0;
		QVector3D lightPos;
		QVector4D lightColor = QUtils3D::vectorFromColor(m_cachedTheme->lightColor());

		// Specify viewport
		glViewport(m_secondarySubViewport.x(),
			m_secondarySubViewport.y(),
			m_secondarySubViewport.width(),
			m_secondarySubViewport.height());

		// Set up projection matrix
		QMatrix4x4 projectionMatrix;
		GLfloat viewPortRatio = (GLfloat)m_primarySubViewport.width()
			/ (GLfloat)m_primarySubViewport.height();
		if (m_useOrthoProjection) {
			GLfloat orthoRatio = 2.0f / m_autoScaleAdjustment;
			projectionMatrix.ortho(-viewPortRatio * orthoRatio, viewPortRatio * orthoRatio,
				-orthoRatio, orthoRatio,
				0.0f, 100.0f);
		}
		else {
			projectionMatrix.perspective(35.0f, viewPortRatio, 0.1f, 100.0f);
		}

		// Set view matrix
		QMatrix4x4 viewMatrix;

		// Adjust scaling (zoom rate based on aspect ratio)
		GLfloat camZPosSliced = cameraDistance / m_autoScaleAdjustment;

		viewMatrix.lookAt(QVector3D(0.0f, 0.0f, camZPosSliced), zeroVector, upVector);

		// Set light position
		lightPos = QVector3D(0.0f, 0.0f, camZPosSliced * 2.0f);

		const QCamera3D *activeCamera = m_cachedScene->activeCamera();

		// Draw the selected row / column
		QMatrix4x4 projectionViewMatrix = projectionMatrix * viewMatrix;
		bool rowMode = m_cachedSelectionMode.testFlag(QAbstractGraph3D::SelectionRow);
		bool itemMode = m_cachedSelectionMode.testFlag(QAbstractGraph3D::SelectionItem);

		GLfloat barPosYAdjustment = -0.8f; // Translate to -1.0 + 0.2 for row/column labels
		GLfloat gridAdjustment = 1.0f + barPosYAdjustment - m_backgroundAdjustment;
		GLfloat scaleFactor = 0.0f;
		if (rowMode)
			scaleFactor = (1.1f * m_rowWidth) / m_scaleFactor;
		else
			scaleFactor = (1.1f * m_columnDepth) / m_scaleFactor;
		GLfloat barLabelYPos = barPosYAdjustment - labelMargin;
		GLfloat zeroPosAdjustment = 0.0f;
		GLfloat directionMultiplier = 2.0f;
		GLfloat directionBase = 0.0f;
		if (m_axisCacheY.reversed()) {
			directionMultiplier = -2.0f;
			directionBase = -2.0f;
		}
		zeroPosAdjustment = directionBase +
			directionMultiplier * m_axisCacheY.min() / m_heightNormalizer;
		zeroPosAdjustment = qBound(-2.0f, zeroPosAdjustment, 0.0f);

		// Draw grid lines
		if (m_cachedTheme->isGridEnabled()) {
			glDisable(GL_DEPTH_TEST);
			QShaderHelper3D *lineShader;
			if (m_isOpenGLES)
				lineShader = m_selectionShader; // Plain color shader for GL_LINES
			else
				lineShader = m_backgroundShader;

			// Bind line shader
			lineShader->bind();

			// Set unchanging shader bindings
			QVector4D lineColor = QUtils3D::vectorFromColor(m_cachedTheme->gridLineColor());
			lineShader->setUniformValue(lineShader->lightP(), lightPos);
			lineShader->setUniformValue(lineShader->view(), viewMatrix);
			lineShader->setUniformValue(lineShader->color(), lineColor);
			lineShader->setUniformValue(lineShader->ambientS(),
				m_cachedTheme->ambientLightStrength()
				+ m_cachedTheme->lightStrength() / 7.0f);
			lineShader->setUniformValue(lineShader->lightS(), 0.0f);
			lineShader->setUniformValue(lineShader->lightColor(), lightColor);

			// Horizontal lines
			if (m_axisCacheY.segmentCount() > 0) {
				int gridLineCount = m_axisCacheY.gridLineCount();

				QVector3D gridLineScale(scaleFactor, gridLineWidth, gridLineWidth);
				bool noZero = true;
				QMatrix4x4 MVPMatrix;
				QMatrix4x4 itModelMatrix;

				for (int line = 0; line < gridLineCount; line++) {
					QMatrix4x4 modelMatrix;
					GLfloat gridPos = m_axisCacheY.gridLinePosition(line) + gridAdjustment;
					modelMatrix.translate(0.0f, gridPos, 0.0f);
					modelMatrix.scale(gridLineScale);
					itModelMatrix = modelMatrix;
					MVPMatrix = projectionViewMatrix * modelMatrix;

					// Set the rest of the shader bindings
					lineShader->setUniformValue(lineShader->model(), modelMatrix);
					lineShader->setUniformValue(lineShader->nModel(),
						itModelMatrix.inverted().transposed());
					lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

					// Draw the object
					if (m_isOpenGLES)
						m_drawer->drawLine(lineShader);
					else
						m_drawer->drawObject(lineShader, m_gridLineObj);

					// Check if we have a line at zero position already
					if (gridPos == (barPosYAdjustment + zeroPosAdjustment))
						noZero = false;
				}

				// Draw a line at zero, if none exists
				if (!m_noZeroInRange && noZero) {
					QMatrix4x4 modelMatrix;
					modelMatrix.translate(0.0f, barPosYAdjustment - zeroPosAdjustment, 0.0f);
					modelMatrix.scale(gridLineScale);
					itModelMatrix = modelMatrix;
					MVPMatrix = projectionViewMatrix * modelMatrix;

					// Set the rest of the shader bindings
					lineShader->setUniformValue(lineShader->model(), modelMatrix);
					lineShader->setUniformValue(lineShader->nModel(),
						itModelMatrix.inverted().transposed());
					lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);
					lineShader->setUniformValue(lineShader->color(),
						QUtils3D::vectorFromColor(
							m_cachedTheme->labelTextColor()));

					// Draw the object
					if (m_isOpenGLES)
						m_drawer->drawLine(lineShader);
					else
						m_drawer->drawObject(lineShader, m_gridLineObj);
				}
			}

			if (sliceGridLabels) {
				// Bind label shader
				m_labelShader->bind();
				glCullFace(GL_BACK);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				// Draw grid labels
				int labelNbr = 0;
				int labelCount = m_axisCacheY.labelCount();
				QVector3D labelTrans = QVector3D(scaleFactor + labelMargin, 0.0f, 0.0f);

				for (int i = 0; i < labelCount; i++) {
					if (m_axisCacheY.labelItems().size() > labelNbr) {
						const QLabelItem3D &axisLabelItem = *m_axisCacheY.labelItems().at(labelNbr);
						GLfloat gridPos = m_axisCacheY.labelPosition(i) + gridAdjustment;
						labelTrans.setY(gridPos);
						m_dummyBarRenderItem.setTranslation(labelTrans);
						m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix,
							projectionMatrix, zeroVector, identityQuaternion, 0,
							m_cachedSelectionMode, m_labelShader, m_labelObj,
							activeCamera, true, true, QDrawer3D::LabelMid, Qt::AlignLeft);
					}
					labelNbr++;
				}
				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
			}
		}

		// Draw bars
		QVector3D modelMatrixScaler(m_scaleX * m_seriesScaleX, 0.0f, m_scaleZ * m_seriesScaleZ);
		if (!rowMode) {
			modelMatrixScaler.setX(m_scaleZ * m_seriesScaleZ);
			modelMatrixScaler.setZ(m_scaleX * m_seriesScaleX);
		}

		// Set common bar shader bindings
		m_barShader->bind();
		m_barShader->setUniformValue(m_barShader->lightP(), lightPos);
		m_barShader->setUniformValue(m_barShader->view(), viewMatrix);
		m_barShader->setUniformValue(m_barShader->lightS(), 0.15f);
		m_barShader->setUniformValue(m_barShader->ambientS(),
			m_cachedTheme->ambientLightStrength()
			+ m_cachedTheme->lightStrength() / 7.0f);
		m_barShader->setUniformValue(m_barShader->lightColor(), lightColor);
		m_barGradientShader->bind();
		m_barGradientShader->setUniformValue(m_barGradientShader->lightP(), lightPos);
		m_barGradientShader->setUniformValue(m_barGradientShader->view(), viewMatrix);
		m_barGradientShader->setUniformValue(m_barGradientShader->lightS(), 0.15f);
		m_barGradientShader->setUniformValue(m_barGradientShader->ambientS(),
			m_cachedTheme->ambientLightStrength()
			+ m_cachedTheme->lightStrength() / 7.0f);
		m_barGradientShader->setUniformValue(m_barGradientShader->gradientMin(), 0.0f);
		m_barGradientShader->setUniformValue(m_barGradientShader->lightColor(), lightColor);

		// Default to uniform shader
		QShaderHelper3D *barShader = m_barShader;
		barShader->bind();

		QTheme3D::ColorStyle previousColorStyle = QTheme3D::ColorStyleUniform;
		QTheme3D::ColorStyle colorStyle = QTheme3D::ColorStyleUniform;
		QObjectHelper3D *barObj = 0;
		QVector4D highlightColor;
		QVector4D baseColor;
		GLuint highlightGradientTexture = 0;
		GLuint baseGradientTexture = 0;
		bool colorStyleIsUniform = true;
		int firstVisualIndex = m_renderCacheList.size();
		QVector<QStackedBarRenderSliceItem3D> *firstVisualSliceArray = 0;
		QStackedBarRenderSliceItem3D *selectedItem = 0;

		QQuaternion seriesRotation;
		foreach(QSeriesRenderCache3D *baseCache, m_renderCacheList) {
			if (baseCache->isVisible()
				&& (baseCache == m_selectedSeriesCache
					|| m_cachedSelectionMode.testFlag(QAbstractGraph3D::SelectionMultiSeries))) {
				QStackedBarSeriesRenderCache3D *cache = static_cast<QStackedBarSeriesRenderCache3D *>(baseCache);
				QVector<QStackedBarRenderSliceItem3D> &sliceArray = cache->sliceArray();
				int sliceCount = sliceArray.size();
				if (firstVisualIndex > cache->visualIndex()) {
					firstVisualIndex = cache->visualIndex();
					firstVisualSliceArray = &sliceArray;
				}

				barObj = cache->object();
				colorStyle = cache->colorStyle();
				colorStyleIsUniform = (colorStyle == QTheme3D::ColorStyleUniform);
				if (colorStyleIsUniform) {
					highlightColor = cache->singleHighlightColor();
					baseColor = cache->baseColor();
				}
				else {
					highlightGradientTexture = cache->singleHighlightGradientTexture();
					baseGradientTexture = cache->baseGradientTexture();
				}

				// Rebind shader if it has changed
				if (colorStyleIsUniform != (previousColorStyle == QTheme3D::ColorStyleUniform)) {
					if (colorStyleIsUniform)
						barShader = m_barShader;
					else
						barShader = m_barGradientShader;
					barShader->bind();
				}

				if (!colorStyleIsUniform && (previousColorStyle != colorStyle)
					&& (colorStyle == QTheme3D::ColorStyleObjectGradient)) {
					m_barGradientShader->setUniformValue(m_barGradientShader->gradientHeight(), 0.5f);
				}

				previousColorStyle = colorStyle;
				seriesRotation = cache->meshRotation();
				bool selectedSeries = (cache == m_selectedSeriesCache);

				for (int bar = 0; bar < sliceCount; bar++) {
					QStackedBarRenderSliceItem3D &item = cache->sliceArray()[bar];
					if (selectedSeries && itemMode && sliceGridLabels
						&& m_visualSelectedBarPos.x() == item.position().x()
						&& m_visualSelectedBarPos.y() == item.position().y()) {
						selectedItem = &item;
					}

                    const QList<qreal>& itemValues = item.values();
					if (!itemValues.size())
						continue;

                    const QList<qreal>& itemHeights = item.heights();

					float totalHeight = 0.0f;
					foreach(float height, itemHeights)
					{
						if (height < 0)
							glCullFace(GL_FRONT);
						else
							glCullFace(GL_BACK);

						QMatrix4x4 MVPMatrix;
						QMatrix4x4 modelMatrix;
						QMatrix4x4 itModelMatrix;
						QQuaternion barRotation = item.rotation();
						GLfloat barPosY = item.translation().y() + barPosYAdjustment - zeroPosAdjustment;

						if (rowMode) {
							barPosX = item.translation().x();
						}
						else {
							barPosX = -(item.translation().z()); // flip z; frontmost bar to the left
							barRotation *= m_yRightAngleRotation;
						}

						modelMatrix.translate(barPosX, barPosY, 0.0f);

						totalHeight += height;

						modelMatrixScaler.setY(totalHeight);

						if (!seriesRotation.isIdentity())
							barRotation *= seriesRotation;

						if (!barRotation.isIdentity()) {
							modelMatrix.rotate(barRotation);
							itModelMatrix.rotate(barRotation);
						}

						modelMatrix.scale(modelMatrixScaler);
						itModelMatrix.scale(modelMatrixScaler);

						MVPMatrix = projectionViewMatrix * modelMatrix;

						QVector4D barColor;
						GLuint gradientTexture = 0;

						if (itemMode && m_visualSelectedBarPos.x() == item.position().x()
							&& m_visualSelectedBarPos.y() == item.position().y()) {
							if (colorStyleIsUniform)
								barColor = highlightColor;
							else
								gradientTexture = highlightGradientTexture;
						}
						else {
							if (colorStyleIsUniform)
								barColor = baseColor;
							else
								gradientTexture = baseGradientTexture;
						}

						if (totalHeight != 0) {
							// Set shader bindings
							barShader->setUniformValue(barShader->model(), modelMatrix);
							barShader->setUniformValue(barShader->nModel(),
								itModelMatrix.inverted().transposed());
							barShader->setUniformValue(barShader->MVP(), MVPMatrix);
							if (colorStyleIsUniform) {
								barShader->setUniformValue(barShader->color(), barColor);
							}
							else if (colorStyle == QTheme3D::ColorStyleRangeGradient) {
								barShader->setUniformValue(barShader->gradientHeight(),
									(qAbs(totalHeight) / m_gradientFraction));
							}

							// Draw the object
							m_drawer->drawObject(barShader,
								barObj,
								gradientTexture);
						}
					}
				}
			}
		}

		// Draw labels
		m_labelShader->bind();
		glDisable(GL_DEPTH_TEST);
		glCullFace(GL_BACK);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		QStackedBarRenderItem3D *dummyItem(0);
		const QLabelItem3D &sliceSelectionLabel = *m_sliceTitleItem;
		QVector3D positionComp(0.0f, m_autoScaleAdjustment, 0.0f);

		// Draw labels for bars
		QVector3D sliceValueRotation(0.0f, 0.0f, 90.0f);
		QVector3D sliceLabelRotation(0.0f, 0.0f, -45.0f);
		QQuaternion totalSliceValueRotation = QUtils3D::calculateRotation(sliceValueRotation);
		QQuaternion totalSliceLabelRotation = QUtils3D::calculateRotation(sliceLabelRotation);

		int labelCount = m_sliceCache->labelItems().size();

		for (int labelNo = 0; labelNo < labelCount; labelNo++) {
			// Check for invalid usage (no selection when setting slicing active)
			if (!firstVisualSliceArray) {
				qWarning("No slice data found. Make sure there is a valid selection.");
				continue;
			}

			// Get labels from first series only
			const QStackedBarRenderSliceItem3D &item = firstVisualSliceArray->at(labelNo);
			m_dummyBarRenderItem.setTranslation(QVector3D(item.translation().x(),
				barLabelYPos,
				item.translation().z()));

			// Draw labels
			m_drawer->drawLabel(m_dummyBarRenderItem, *m_sliceCache->labelItems().at(labelNo),
				viewMatrix, projectionMatrix, positionComp, totalSliceLabelRotation,
				0, m_cachedSelectionMode, m_labelShader,
				m_labelObj, activeCamera, false, false, QDrawer3D::LabelMid,
				Qt::AlignmentFlag(Qt::AlignLeft | Qt::AlignTop), true);
		}

		if (!sliceGridLabels) {
			foreach(QSeriesRenderCache3D *baseCache, m_renderCacheList) {
				if (baseCache->isVisible()) {
					QStackedBarSeriesRenderCache3D *cache = static_cast<QStackedBarSeriesRenderCache3D *>(baseCache);
					QVector<QStackedBarRenderSliceItem3D> &sliceArray = cache->sliceArray();
					int sliceCount = sliceArray.size();
					for (int col = 0; col < sliceCount; col++) {
						QStackedBarRenderSliceItem3D &item = sliceArray[col];

                        const QList<qreal>& itemValues = item.values();
                        const QList<qreal>& itemHeights = item.heights();

						unsigned int valueIdx = 0;
						foreach(float value, itemValues)
						{
							float height = itemHeights.at(valueIdx);
							// Draw values
							if (height != 0.0f || (!m_noZeroInRange && value == 0.0f)) {
								// Create label texture if we need it
								if (item.sliceLabel().isNull() || m_updateLabels) {
									QString valueLabelText = m_axisCacheY.formatter()->stringForValue(
										qreal(value), m_axisCacheY.labelFormat());
									item.setSliceLabel(valueLabelText);
									m_drawer->generateLabelItem(item.sliceLabelItem(), item.sliceLabel());
									m_updateLabels = false;
								}
								Qt::AlignmentFlag alignment =
									(height > 0) ? Qt::AlignLeft : Qt::AlignRight;
								QDrawer3D::LabelPosition labelPos =
									(height < 0) ? QDrawer3D::LabelBelow : QDrawer3D::LabelOver;
								m_dummyBarRenderItem.setTranslation(QVector3D(item.translation().x(),
									barPosYAdjustment
									- zeroPosAdjustment
									+ height,
									item.translation().z()));

								m_drawer->drawLabel(m_dummyBarRenderItem, item.sliceLabelItem(), viewMatrix,
									projectionMatrix, zeroVector, totalSliceValueRotation,
									height, m_cachedSelectionMode, m_labelShader,
									m_labelObj, activeCamera, false, false, labelPos,
									alignment, true);
							}
							valueIdx++;
						}
					}
				}
			}
		}
		else if (selectedItem) {
			// Only draw value for selected item when grid labels are on
			// Create label texture if we need it
			
            const QList<qreal> itemValues = selectedItem->values();
            const QList<qreal> itemHeights = selectedItem->heights();

			unsigned int valueIdx = 0;
			foreach(float value, itemValues)
			{
				float height = itemHeights.at(valueIdx);
				if (selectedItem->sliceLabel().isNull() || m_updateLabels) {
					QString valueLabelText = m_axisCacheY.formatter()->stringForValue(
						qreal(value), m_axisCacheY.labelFormat());
					selectedItem->setSliceLabel(valueLabelText);
					m_drawer->generateLabelItem(selectedItem->sliceLabelItem(), selectedItem->sliceLabel());
					m_updateLabels = false;
				}
				Qt::AlignmentFlag alignment = (height > 0) ? Qt::AlignLeft : Qt::AlignRight;
				QDrawer3D::LabelPosition labelPos =
					(height < 0) ? QDrawer3D::LabelBelow : QDrawer3D::LabelOver;
				m_dummyBarRenderItem.setTranslation(QVector3D(selectedItem->translation().x(),
					barPosYAdjustment - zeroPosAdjustment
					+ height,
					selectedItem->translation().z()));

				m_drawer->drawLabel(m_dummyBarRenderItem, selectedItem->sliceLabelItem(), viewMatrix,
					projectionMatrix, zeroVector, totalSliceValueRotation,
					height, m_cachedSelectionMode, m_labelShader,
					m_labelObj, activeCamera, false, false, labelPos,
					alignment, true);

				valueIdx++;
			}
		}

		// Draw labels for axes
		if (rowMode) {
			if (m_sliceTitleItem) {
				m_drawer->drawLabel(*dummyItem, sliceSelectionLabel, viewMatrix, projectionMatrix,
					positionComp, identityQuaternion, 0, m_cachedSelectionMode,
					m_labelShader, m_labelObj, activeCamera, false, false,
					QDrawer3D::LabelTop, Qt::AlignCenter, true);
			}
			m_drawer->drawLabel(*dummyItem, m_axisCacheX.titleItem(), viewMatrix, projectionMatrix,
				positionComp, identityQuaternion, 0, m_cachedSelectionMode,
				m_labelShader, m_labelObj, activeCamera, false, false,
				QDrawer3D::LabelBottom, Qt::AlignCenter, true);
		}
		else {
			m_drawer->drawLabel(*dummyItem, m_axisCacheZ.titleItem(), viewMatrix, projectionMatrix,
				positionComp, identityQuaternion, 0, m_cachedSelectionMode,
				m_labelShader,
				m_labelObj, activeCamera, false, false, QDrawer3D::LabelBottom,
				Qt::AlignCenter, true);
			if (m_sliceTitleItem) {
				m_drawer->drawLabel(*dummyItem, sliceSelectionLabel, viewMatrix, projectionMatrix,
					positionComp, identityQuaternion, 0, m_cachedSelectionMode,
					m_labelShader,
					m_labelObj, activeCamera, false, false, QDrawer3D::LabelTop,
					Qt::AlignCenter, true);
			}
		}
		// Y-axis label
		QVector3D labelTrans = QVector3D(-scaleFactor - labelMargin, 0.2f, 0.0f); // y = 0.2 for row/column labels (see barPosYAdjustment)
		m_dummyBarRenderItem.setTranslation(labelTrans);
		m_drawer->drawLabel(m_dummyBarRenderItem, m_axisCacheY.titleItem(), viewMatrix,
			projectionMatrix, zeroVector, totalSliceValueRotation, 0,
			m_cachedSelectionMode, m_labelShader, m_labelObj, activeCamera,
			false, false, QDrawer3D::LabelMid, Qt::AlignBottom);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		// Release shader
		glUseProgram(0);
	}

	void QStackedBarsRenderer3D::drawScene(GLuint defaultFboHandle)
	{
		GLint startBar = 0;
		GLint stopBar = 0;
		GLint stepBar = 0;

		GLint startRow = 0;
		GLint stopRow = 0;
		GLint stepRow = 0;

		GLfloat backgroundRotation = 0;

		GLfloat colPos = 0;
		GLfloat rowPos = 0;

		const QCamera3D *activeCamera = m_cachedScene->activeCamera();

		glViewport(m_primarySubViewport.x(),
			m_primarySubViewport.y(),
			m_primarySubViewport.width(),
			m_primarySubViewport.height());

		// Set up projection matrix
		QMatrix4x4 projectionMatrix;
		GLfloat viewPortRatio = (GLfloat)m_primarySubViewport.width()
			/ (GLfloat)m_primarySubViewport.height();
		if (m_useOrthoProjection) {
			GLfloat orthoRatio = 2.0f;
			projectionMatrix.ortho(-viewPortRatio * orthoRatio, viewPortRatio * orthoRatio,
				-orthoRatio, orthoRatio,
				0.0f, 100.0f);
		}
		else {
			projectionMatrix.perspective(45.0f, viewPortRatio, 0.1f, 100.0f);
		}

		// Get the view matrix
		QMatrix4x4 viewMatrix = activeCamera->d_ptr->viewMatrix();

		// Calculate drawing order
		// Draw order is reversed to optimize amount of drawing (ie. draw front objects first,
		// depth test handles not needing to draw objects behind them)
		if (viewMatrix.row(0).x() > 0) {
			startRow = 0;
			stopRow = m_cachedRowCount;
			stepRow = 1;
			m_zFlipped = false;
		}
		else {
			startRow = m_cachedRowCount - 1;
			stopRow = -1;
			stepRow = -1;
			m_zFlipped = true;
		}
		if (viewMatrix.row(0).z() <= 0) {
			startBar = 0;
			stopBar = m_cachedColumnCount;
			stepBar = 1;
			m_xFlipped = false;
		}
		else {
			startBar = m_cachedColumnCount - 1;
			stopBar = -1;
			stepBar = -1;
			m_xFlipped = true;
		}

		// Check if we're viewing the scene from below
		if (viewMatrix.row(2).y() < 0)
			m_yFlipped = true;
		else
			m_yFlipped = false;

		// calculate background rotation based on view matrix rotation
		if (viewMatrix.row(0).x() > 0 && viewMatrix.row(0).z() <= 0)
			backgroundRotation = 270.0f;
		else if (viewMatrix.row(0).x() > 0 && viewMatrix.row(0).z() > 0)
			backgroundRotation = 180.0f;
		else if (viewMatrix.row(0).x() <= 0 && viewMatrix.row(0).z() > 0)
			backgroundRotation = 90.0f;
		else if (viewMatrix.row(0).x() <= 0 && viewMatrix.row(0).z() <= 0)
			backgroundRotation = 0.0f;

		// Get light position from the scene
		QVector3D lightPos = m_cachedScene->activeLight()->position();

		// Skip depth rendering if we're in slice mode
		// Introduce regardless of shadow quality to simplify logic
		QMatrix4x4 depthViewMatrix;
		QMatrix4x4 depthProjectionMatrix;
		QMatrix4x4 depthProjectionViewMatrix;

		QMatrix4x4 projectionViewMatrix = projectionMatrix * viewMatrix;

		QStackedBarRenderItem3D *selectedBar(0);

		if (m_cachedShadowQuality > QAbstractGraph3D::ShadowQualityNone && !m_isOpenGLES) {
			// Render scene into a depth texture for using with shadow mapping
			// Enable drawing to depth framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, m_depthFrameBuffer);
			glClear(GL_DEPTH_BUFFER_BIT);

			// Bind depth shader
			m_depthShader->bind();

			// Set viewport for depth map rendering. Must match texture size. Larger values give smoother shadows.
			// Depth viewport must always start from 0, 0, as it is rendered into a texture, not screen
			glViewport(0, 0,
				m_primarySubViewport.width() * m_shadowQualityMultiplier,
				m_primarySubViewport.height() * m_shadowQualityMultiplier);

			// Get the depth view matrix
			// It may be possible to hack lightPos here if we want to make some tweaks to shadow
			QVector3D depthLightPos = activeCamera->d_ptr->calculatePositionRelativeToCamera(
				zeroVector, 0.0f, 3.5f / m_autoScaleAdjustment);
			depthViewMatrix.lookAt(depthLightPos, zeroVector, upVector);

			// Set the depth projection matrix
			depthProjectionMatrix.perspective(10.0f, viewPortRatio, 3.0f, 100.0f);
			depthProjectionViewMatrix = depthProjectionMatrix * depthViewMatrix;

			// Draw bars to depth buffer
			QVector3D shadowScaler(m_scaleX * m_seriesScaleX * 0.9f, 0.0f,
				m_scaleZ * m_seriesScaleZ * 0.9f);
			foreach(QSeriesRenderCache3D *baseCache, m_renderCacheList) {
				if (baseCache->isVisible()) {
					QStackedBarSeriesRenderCache3D *cache = static_cast<QStackedBarSeriesRenderCache3D *>(baseCache);
					float seriesPos = m_seriesStart + m_seriesStep * cache->visualIndex() + 0.5f;
					QObjectHelper3D *barObj = cache->object();
					QQuaternion seriesRotation(cache->meshRotation());
					const QStackedBarRenderItemArray3D &renderArray = cache->renderArray();
					for (int row = startRow; row != stopRow; row += stepRow) {
						const QStackedBarRenderItemRow3D &renderRow = renderArray.at(row);
						for (int bar = startBar; bar != stopBar; bar += stepBar) {
							const QStackedBarRenderItem3D &item = renderRow.at(bar);

                            const QList<qreal>& itemValues = item.values();

							if (!itemValues.size())
								continue;

                            const QList<qreal>& itemHeights = item.heights();

							GLfloat shadowOffset = 0.0f;

							float totalHeight = 0.0f;
                            foreach(qreal height, itemHeights)
							{
								// Set front face culling for negative valued bars and back face culling
								// for positive valued bars to remove peter-panning issues

								totalHeight += std::fabs(height);

								if (totalHeight > 0) {
									glCullFace(GL_BACK);
									if (m_yFlipped)
										shadowOffset = 0.015f;
								}
								else {
									glCullFace(GL_FRONT);
									if (!m_yFlipped)
										shadowOffset = -0.015f;
								}

								if (m_cachedTheme->isBackgroundEnabled() && m_reflectionEnabled
									&& ((m_yFlipped && totalHeight > 0.0)
										|| (!m_yFlipped && totalHeight < 0.0))) {
									continue;
								}

								QMatrix4x4 modelMatrix;
								QMatrix4x4 MVPMatrix;

								colPos = (bar + seriesPos) * (m_cachedBarSpacing.width());
								rowPos = (row + 0.5f) * (m_cachedBarSpacing.height());

								// Draw shadows for bars "on the other side" a bit off ground to avoid
								// seeing shadows through the ground
								modelMatrix.translate((colPos - m_rowWidth) / m_scaleFactor,
									totalHeight + shadowOffset,
									(m_columnDepth - rowPos) / m_scaleFactor);
								// Scale the bars down in X and Z to reduce self-shadowing issues
								shadowScaler.setY(totalHeight);
								if (!seriesRotation.isIdentity() || !item.rotation().isIdentity())
									modelMatrix.rotate(seriesRotation * item.rotation());
								modelMatrix.scale(shadowScaler);

								MVPMatrix = depthProjectionViewMatrix * modelMatrix;

								m_depthShader->setUniformValue(m_depthShader->MVP(), MVPMatrix);

								// 1st attribute buffer : vertices
								glEnableVertexAttribArray(m_depthShader->posAtt());
								glBindBuffer(GL_ARRAY_BUFFER, barObj->vertexBuf());
								glVertexAttribPointer(m_depthShader->posAtt(), 3, GL_FLOAT, GL_FALSE, 0,
									(void *)0);

								// Index buffer
								glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, barObj->elementBuf());

								// Draw the triangles
								glDrawElements(GL_TRIANGLES, barObj->indexCount(), GL_UNSIGNED_INT,
									(void *)0);

								// Free buffers
								glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
								glBindBuffer(GL_ARRAY_BUFFER, 0);

								glDisableVertexAttribArray(m_depthShader->posAtt());
							}
						}
					}
				}
			}

			QAbstractRenderer3D::drawCustomItems(RenderingDepth, m_depthShader, viewMatrix,
				projectionViewMatrix,
				depthProjectionViewMatrix, m_depthTexture,
				m_shadowQualityToShader);

			// Disable drawing to depth framebuffer (= enable drawing to screen)
			glBindFramebuffer(GL_FRAMEBUFFER, defaultFboHandle);

			// Reset culling to normal
			glCullFace(GL_BACK);

			// Revert to original viewport
			glViewport(m_primarySubViewport.x(),
				m_primarySubViewport.y(),
				m_primarySubViewport.width(),
				m_primarySubViewport.height());
		}

		// Do position mapping when necessary
		if (m_graphPositionQueryPending) {
			QVector3D graphDimensions(m_xScaleFactor, 0.0f, m_zScaleFactor);
			queriedGraphPosition(projectionViewMatrix, graphDimensions, defaultFboHandle);

			// Y is always at floor level
			m_queriedGraphPosition.setY(0.0f);
			emit needRender();
		}

		// Skip selection mode drawing if we're slicing or have no selection mode
		if (!m_cachedIsSlicingActivated && m_cachedSelectionMode > QAbstractGraph3D::SelectionNone
			&& m_selectionState == SelectOnScene
			&& (m_visibleSeriesCount > 0 || !m_customRenderCache.isEmpty())
			&& m_selectionTexture) {
			// Bind selection shader
			m_selectionShader->bind();

			// Draw bars to selection buffer
			glBindFramebuffer(GL_FRAMEBUFFER, m_selectionFrameBuffer);
			glViewport(0, 0,
				m_primarySubViewport.width(),
				m_primarySubViewport.height());

			glEnable(GL_DEPTH_TEST); // Needed, otherwise the depth render buffer is not used
			glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Set clear color to white (= selectionSkipColor)
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Needed for clearing the frame buffer
			glDisable(GL_DITHER); // disable dithering, it may affect colors if enabled
			foreach(QSeriesRenderCache3D *baseCache, m_renderCacheList) {
				if (baseCache->isVisible()) {
					QStackedBarSeriesRenderCache3D *cache = static_cast<QStackedBarSeriesRenderCache3D *>(baseCache);
					float seriesPos = m_seriesStart + m_seriesStep * cache->visualIndex() + 0.5f;
					QObjectHelper3D *barObj = cache->object();
					QQuaternion seriesRotation(cache->meshRotation());
					const QStackedBarRenderItemArray3D &renderArray = cache->renderArray();
					for (int row = startRow; row != stopRow; row += stepRow) {
						const QStackedBarRenderItemRow3D &renderRow = renderArray.at(row);
						for (int bar = startBar; bar != stopBar; bar += stepBar) {
							const QStackedBarRenderItem3D &item = renderRow.at(bar);

                            const QList<qreal>& itemValues = item.values();

							if (!itemValues.size())
								continue;

                            const QList<qreal> itemHeights = item.heights();
							float totalHeight = 0.0f;
                            foreach(qreal height, itemHeights)
							{
								totalHeight += std::fabs(height);

								if (totalHeight < 0)
									glCullFace(GL_FRONT);
								else
									glCullFace(GL_BACK);

								QMatrix4x4 modelMatrix;
								QMatrix4x4 MVPMatrix;

								colPos = (bar + seriesPos) * (m_cachedBarSpacing.width());
								rowPos = (row + 0.5f) * (m_cachedBarSpacing.height());

								modelMatrix.translate((colPos - m_rowWidth) / m_scaleFactor,
									totalHeight,
									(m_columnDepth - rowPos) / m_scaleFactor);
								if (!seriesRotation.isIdentity() || !item.rotation().isIdentity())
									modelMatrix.rotate(seriesRotation * item.rotation());
								modelMatrix.scale(QVector3D(m_scaleX * m_seriesScaleX,
									totalHeight,
									m_scaleZ * m_seriesScaleZ));

								MVPMatrix = projectionViewMatrix * modelMatrix;

								QVector4D barColor = QVector4D(GLfloat(row) / 255.0f,
									GLfloat(bar) / 255.0f,
									GLfloat(cache->visualIndex()) / 255.0f,
									itemAlpha);

								m_selectionShader->setUniformValue(m_selectionShader->MVP(), MVPMatrix);
								m_selectionShader->setUniformValue(m_selectionShader->color(), barColor);

								m_drawer->drawSelectionObject(m_selectionShader, barObj);
							}
						}
					}
				}
			}
			glCullFace(GL_BACK);
			QAbstractRenderer3D::drawCustomItems(RenderingSelection, m_selectionShader,
				viewMatrix,
				projectionViewMatrix, depthProjectionViewMatrix,
				m_depthTexture, m_shadowQualityToShader);
			drawLabels(true, activeCamera, viewMatrix, projectionMatrix);
			drawBackground(backgroundRotation, depthProjectionViewMatrix, projectionViewMatrix,
				viewMatrix, false, true);
			glEnable(GL_DITHER);

			// Read color under cursor
			QVector4D clickedColor = QUtils3D::getSelection(m_inputPosition, m_viewport.height());
			m_clickedPosition = selectionColorToArrayPosition(clickedColor);
			m_clickedSeries = selectionColorToSeries(clickedColor);
			m_clickResolved = true;

			emit needRender();

			// Revert to original render target and viewport
			glBindFramebuffer(GL_FRAMEBUFFER, defaultFboHandle);
			glViewport(m_primarySubViewport.x(),
				m_primarySubViewport.y(),
				m_primarySubViewport.width(),
				m_primarySubViewport.height());
		}

		if (m_reflectionEnabled) {
			//
			// Draw reflections
			//
			glDisable(GL_DEPTH_TEST);
			glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
			glEnable(GL_STENCIL_TEST);
			glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 1, 0xffffffff);

			// Draw background stencil
			drawBackground(backgroundRotation, depthProjectionViewMatrix, projectionViewMatrix,
				viewMatrix);

			glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
			glEnable(GL_DEPTH_TEST);

			glStencilFunc(GL_EQUAL, 1, 0xffffffff);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

			// Set light
			QVector3D reflectionLightPos = lightPos;
			reflectionLightPos.setY(-(lightPos.y()));
			m_cachedScene->activeLight()->setPosition(reflectionLightPos);

			// Draw bar reflections
			(void)drawBars(&selectedBar, depthProjectionViewMatrix,
				projectionViewMatrix, viewMatrix,
				startRow, stopRow, stepRow,
				startBar, stopBar, stepBar, -1.0f);

			QAbstractRenderer3D::drawCustomItems(RenderingNormal, m_customItemShader,
				viewMatrix, projectionViewMatrix,
				depthProjectionViewMatrix, m_depthTexture,
				m_shadowQualityToShader, -1.0f);

			// Reset light
			m_cachedScene->activeLight()->setPosition(lightPos);

			glDisable(GL_STENCIL_TEST);

			glCullFace(GL_BACK);
		}

		//
		// Draw the real scene
		//
		// Draw background
		if (m_reflectionEnabled) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			drawBackground(backgroundRotation, depthProjectionViewMatrix, projectionViewMatrix,
				viewMatrix, true);
			glDisable(GL_BLEND);
		}
		else {
			drawBackground(backgroundRotation, depthProjectionViewMatrix, projectionViewMatrix,
				viewMatrix);
		}

		// Draw bars
		bool barSelectionFound = drawBars(&selectedBar, depthProjectionViewMatrix,
			projectionViewMatrix, viewMatrix,
			startRow, stopRow, stepRow,
			startBar, stopBar, stepBar);

		// Draw grid lines
		drawGridLines(depthProjectionViewMatrix, projectionViewMatrix, viewMatrix);

		// Draw custom items
		QAbstractRenderer3D::drawCustomItems(RenderingNormal, m_customItemShader, viewMatrix,
			projectionViewMatrix, depthProjectionViewMatrix,
			m_depthTexture, m_shadowQualityToShader);

		// Draw labels
		drawLabels(false, activeCamera, viewMatrix, projectionMatrix);

		// Handle selected bar label generation
		if (barSelectionFound) {
			// Print value of selected bar
			glDisable(GL_DEPTH_TEST);
			// Draw the selection label
			QLabelItem3D &labelItem = selectionLabelItem();
			if (m_selectedBar != selectedBar || m_updateLabels || !labelItem.textureId()
				|| m_selectionLabelDirty) {
				QString labelText = selectionLabel();
				if (labelText.isNull() || m_selectionLabelDirty) {
					labelText = m_selectedSeriesCache->itemLabel();
					setSelectionLabel(labelText);
					m_selectionLabelDirty = false;
				}
				m_drawer->generateLabelItem(labelItem, labelText);
				m_selectedBar = selectedBar;
			}

            const QList<qreal>& itemHeights = m_selectedBar->heights();
			float totalHeight = 0.0f;
			float previousHeight = 0.0f;
			unsigned int itemIdx = 0;
			/*foreach(float height, itemHeights)
			{
				float usedHeight = totalHeight;
				if (itemIdx >= 1)
				{
					previousHeight = 0.0f;
					for (int m = 0; m <= itemIdx - 1; m++)
					{
						//qDebug() << " adding " << itemHeights[m];
						previousHeight += itemHeights[m];
					}
					//qDebug() << " total previousHeight = " << previousHeight;
					usedHeight += previousHeight;
				}

				QDrawer3D::LabelPosition position = QDrawer3D::LabelBelow;
					// usedHeight >= 0 ? QDrawer3D::LabelOver : QDrawer3D::LabelBelow;

				m_drawer->drawLabel(*selectedBar, labelItem, viewMatrix, projectionMatrix,
					zeroVector, identityQuaternion, usedHeight,
					m_cachedSelectionMode, m_labelShader,
					m_labelObj, activeCamera, true, false, position, Qt::AlignCenter, false, false, false);

				totalHeight += height;
			}*/

			foreach(float height, itemHeights)
			{
				totalHeight += height;
			}

			m_drawer->drawLabel(*selectedBar, labelItem, viewMatrix, projectionMatrix,
				zeroVector, identityQuaternion, totalHeight / 2.0f,
				m_cachedSelectionMode, m_labelShader,
				m_labelObj, activeCamera, true, false, QDrawer3D::LabelBelow, Qt::AlignCenter, false, false, true);

			// Reset label update flag; they should have been updated when we get here
			m_updateLabels = false;

			glEnable(GL_DEPTH_TEST);
		}
		else {
			m_selectedBar = 0;
		}

		glDisable(GL_BLEND);

		// Release shader
		glUseProgram(0);
		m_selectionDirty = false;
	}

	bool QStackedBarsRenderer3D::drawBars(QStackedBarRenderItem3D **selectedBar,
		const QMatrix4x4 &depthProjectionViewMatrix,
		const QMatrix4x4 &projectionViewMatrix, const QMatrix4x4 &viewMatrix,
		GLint startRow, GLint stopRow, GLint stepRow,
		GLint startBar, GLint stopBar, GLint stepBar, GLfloat reflection)
	{
		QVector3D lightPos = m_cachedScene->activeLight()->position();
		QVector4D lightColor = QUtils3D::vectorFromColor(m_cachedTheme->lightColor());

        qDebug() << "drawRows(): row " << startRow << " to " << stopRow << ", bar " << startBar << " to " << stopBar << ", stepBar = " << stepBar;

		bool rowMode = m_cachedSelectionMode.testFlag(QAbstractGraph3D::SelectionRow);

		QShaderHelper3D *barShader = 0;
		GLuint gradientTexture = 0;
		QTheme3D::ColorStyle previousColorStyle = QTheme3D::ColorStyleUniform;

		// Set unchanging shader bindings
		if (m_haveGradientSeries) {
			m_barGradientShader->bind();
			m_barGradientShader->setUniformValue(m_barGradientShader->lightP(), lightPos);
			m_barGradientShader->setUniformValue(m_barGradientShader->view(), viewMatrix);
			m_barGradientShader->setUniformValue(m_barGradientShader->ambientS(),
				m_cachedTheme->ambientLightStrength());
			m_barGradientShader->setUniformValue(m_barGradientShader->gradientMin(), 0.0f);
			m_barGradientShader->setUniformValue(m_barGradientShader->lightColor(), lightColor);
		}

		if (m_haveUniformColorSeries) {
			m_barShader->bind();
			m_barShader->setUniformValue(m_barShader->lightP(), lightPos);
			m_barShader->setUniformValue(m_barShader->view(), viewMatrix);
			m_barShader->setUniformValue(m_barShader->ambientS(),
				m_cachedTheme->ambientLightStrength());
			m_barShader->setUniformValue(m_barShader->lightColor(), lightColor);
			barShader = m_barShader;
		}
		else {
			barShader = m_barGradientShader;
			previousColorStyle = QTheme3D::ColorStyleRangeGradient;
		}

		int sliceReserveAmount = 0;
		if (m_selectionDirty && m_cachedIsSlicingActivated) {
			// Slice doesn't own its items, no need to delete them - just clear
			if (rowMode)
				sliceReserveAmount = m_cachedColumnCount;
			else
				sliceReserveAmount = m_cachedRowCount;

			// Set slice cache, i.e. axis cache from where slice labels are taken
			if (rowMode)
				m_sliceCache = &m_axisCacheX;
			else
				m_sliceCache = &m_axisCacheZ;
			m_sliceTitleItem = 0;
		}

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.5f, 1.0f);

		GLfloat adjustedLightStrength = m_cachedTheme->lightStrength() / 10.0f;
		GLfloat adjustedHighlightStrength = m_cachedTheme->highlightLightStrength() / 10.0f;

		bool barSelectionFound = false;

		QVector4D baseColor;
		QVector4D barColor;
		QVector3D modelScaler(m_scaleX * m_seriesScaleX, 0.0f, m_scaleZ * m_seriesScaleZ);
		bool somethingSelected =
			(m_visualSelectedBarPos != QStackedBarsController3D::invalidSelectionPosition());
        foreach(QSeriesRenderCache3D *baseCache, m_renderCacheList)
        {
			if (baseCache->isVisible()) {
				QStackedBarSeriesRenderCache3D *cache = static_cast<QStackedBarSeriesRenderCache3D *>(baseCache);
				float seriesPos = m_seriesStart + m_seriesStep * cache->visualIndex() + 0.5f;
				QObjectHelper3D *barObj = cache->object();
				QQuaternion seriesRotation(cache->meshRotation());
				QTheme3D::ColorStyle colorStyle = cache->colorStyle();
				QStackedBarRenderItemArray3D &renderArray = cache->renderArray();
				bool colorStyleIsUniform = (colorStyle == QTheme3D::ColorStyleUniform);
				if (sliceReserveAmount)
					cache->sliceArray().resize(sliceReserveAmount);

				// Rebind shader if it has changed
				if (colorStyleIsUniform != (previousColorStyle == QTheme3D::ColorStyleUniform)) {
					if (colorStyleIsUniform)
						barShader = m_barShader;
					else
						barShader = m_barGradientShader;
					barShader->bind();
				}

				if (colorStyleIsUniform) {
					baseColor = cache->baseColor();
				}
				else if ((previousColorStyle != colorStyle)
					&& (colorStyle == QTheme3D::ColorStyleObjectGradient)) {
					m_barGradientShader->setUniformValue(m_barGradientShader->gradientHeight(), 0.5f);
				}

				// Always use base color when no selection mode
				if (m_cachedSelectionMode == QAbstractGraph3D::SelectionNone) {
					if (colorStyleIsUniform)
						barColor = baseColor;
					else
						gradientTexture = cache->baseGradientTexture();
				}

				previousColorStyle = colorStyle;
				const QList<QVector4D>& itemSectionColors = cache->sectionColors();

                for (int row = startRow; row != stopRow; row += stepRow)
                {
					QStackedBarRenderItemRow3D &renderRow = renderArray[row];
                    for (int bar = startBar; bar != stopBar; bar += stepBar)
                    {
						QStackedBarRenderItem3D &item = renderRow[bar];

                        const QList<qreal>& itemValues = item.values();
                        const QList<qreal>& itemHeights = item.heights();
                        // qDebug() << "Item " << item.position() << " itemHeights: " << itemHeights << " with itemValues: " << itemValues;

						unsigned int itemIdx = 0;
                        qreal totalHeight = 0.0f;
                        qreal previousHeight = 0.0f;
                        foreach(qreal height, itemHeights)
						{
							totalHeight += height;

							float adjustedHeight = reflection * totalHeight;
							if (adjustedHeight < 0)
								glCullFace(GL_FRONT);
							else
								glCullFace(GL_BACK);

							QMatrix4x4 modelMatrix;
							QMatrix4x4 itModelMatrix;
							QMatrix4x4 MVPMatrix;

							GLfloat colPos = (bar + seriesPos) * (m_cachedBarSpacing.width());
							GLfloat rowPos = (row + 0.5f) * (m_cachedBarSpacing.height());

                            qreal usedHeight = totalHeight;
							if (itemIdx >= 1)
							{
								previousHeight = 0.0f;
								for (int m = 0; m <= itemIdx - 1; m++)
								{
                                    // qDebug() << " Adding " << itemHeights[m];
									previousHeight += itemHeights[m];
								}
                                // qDebug() << " Total previousHeight = " << previousHeight;
								usedHeight += previousHeight;
							}

							modelMatrix.translate((colPos - m_rowWidth) / m_scaleFactor,
								usedHeight,
								(m_columnDepth - rowPos) / m_scaleFactor);

                            // qDebug() << "Translation for bar: (" << ((colPos - m_rowWidth) / m_scaleFactor) << ", " << usedHeight << ", " << ((m_columnDepth - rowPos) / m_scaleFactor) << ")";
                            // qDebug() << "totalHeight = " << totalHeight << ", height = " << height << ", previousHeight = " << previousHeight <<  ", usedHeight = " << usedHeight;

							modelScaler.setY(height);

							if (!seriesRotation.isIdentity() || !item.rotation().isIdentity()) {
								QQuaternion totalRotation = seriesRotation * item.rotation();
								modelMatrix.rotate(totalRotation);
								itModelMatrix.rotate(totalRotation);
							}
							modelMatrix.scale(modelScaler);
							itModelMatrix.scale(modelScaler);
#ifdef SHOW_DEPTH_TEXTURE_SCENE
							MVPMatrix = depthProjectionViewMatrix * modelMatrix;
#else
							MVPMatrix = projectionViewMatrix * modelMatrix;
#endif
							GLfloat lightStrength = m_cachedTheme->lightStrength();
							GLfloat shadowLightStrength = adjustedLightStrength;

							if (m_cachedSelectionMode > QAbstractGraph3D::SelectionNone) {
								QStackedBarsController3D::SelectionType selectionType =
									QStackedBarsController3D::SelectionNone;
								if (somethingSelected)
									selectionType = isSelected(row, bar, cache);

                                switch (selectionType)
                                {
                                    case QStackedBarsController3D::SelectionItem:
                                    {
                                        if (colorStyleIsUniform)
                                        {
                                            // barColor = cache->singleHighlightColor();
                                            barColor = itemSectionColors.at(itemIdx);
                                        }
                                        else
                                        {
                                            gradientTexture = cache->singleHighlightGradientTexture();
                                        }
                                        lightStrength = m_cachedTheme->highlightLightStrength();
                                        shadowLightStrength = adjustedHighlightStrength;
                                        // Insert position data into render item
                                        // We have no ownership, don't delete the previous one
                                        if (!m_cachedIsSlicingActivated
                                            && m_selectedSeriesCache == cache) {
                                            *selectedBar = &item;
                                            (*selectedBar)->setPosition(QPoint(row, bar));
                                            item.setTranslation(modelMatrix.column(3).toVector3D());
                                            barSelectionFound = true;
                                        }
                                        if (m_selectionDirty && m_cachedIsSlicingActivated) {
                                            QVector3D translation = modelMatrix.column(3).toVector3D();
                                            if (m_cachedSelectionMode & QAbstractGraph3D::SelectionColumn
                                                && m_visibleSeriesCount > 1) {
                                                translation.setZ((m_columnDepth
                                                    - ((row + seriesPos)
                                                        * (m_cachedBarSpacing.height())))
                                                    / m_scaleFactor);
                                            }
                                            item.setTranslation(translation);
                                            item.setPosition(QPoint(row, bar));
                                            if (rowMode)
                                                cache->sliceArray()[bar].setItem(item);
                                            else
                                                cache->sliceArray()[row].setItem(item);
                                        }
                                        break;
                                    }
                                    case QStackedBarsController3D::SelectionRow: {
                                        // Current bar is on the same row as the selected bar
                                        if (colorStyleIsUniform)
                                        {
                                            // barColor = cache->multiHighlightColor();
                                            barColor = itemSectionColors.at(itemIdx);
                                        }
                                        else
                                        {
                                            gradientTexture = cache->multiHighlightGradientTexture();
                                        }
                                        lightStrength = m_cachedTheme->highlightLightStrength();
                                        shadowLightStrength = adjustedHighlightStrength;
                                        if (m_cachedIsSlicingActivated) {
                                            item.setTranslation(modelMatrix.column(3).toVector3D());
                                            item.setPosition(QPoint(row, bar));
                                            if (m_selectionDirty) {
                                                if (!m_sliceTitleItem && m_axisCacheZ.labelItems().size() > row)
                                                    m_sliceTitleItem = m_axisCacheZ.labelItems().at(row);
                                                cache->sliceArray()[bar].setItem(item);
                                            }
                                        }
                                        break;
                                    }
                                    case QStackedBarsController3D::SelectionColumn: {
                                        // Current bar is on the same column as the selected bar
                                        if (colorStyleIsUniform)
                                        {
                                            // barColor = cache->multiHighlightColor();
                                            barColor = itemSectionColors.at(itemIdx);
                                        }
                                        else
                                        {
                                            gradientTexture = cache->multiHighlightGradientTexture();
                                        }

                                        lightStrength = m_cachedTheme->highlightLightStrength();
                                        shadowLightStrength = adjustedHighlightStrength;
                                        if (m_cachedIsSlicingActivated) {
                                            QVector3D translation = modelMatrix.column(3).toVector3D();
                                            if (m_visibleSeriesCount > 1) {
                                                translation.setZ((m_columnDepth
                                                    - ((row + seriesPos)
                                                        * (m_cachedBarSpacing.height())))
                                                    / m_scaleFactor);
                                            }
                                            item.setTranslation(translation);
                                            item.setPosition(QPoint(row, bar));
                                            if (m_selectionDirty) {
                                                if (!m_sliceTitleItem && m_axisCacheX.labelItems().size() > bar)
                                                    m_sliceTitleItem = m_axisCacheX.labelItems().at(bar);
                                                cache->sliceArray()[row].setItem(item);
                                            }
                                        }
                                        break;
                                    }
                                    case QStackedBarsController3D::SelectionNone:
                                    {
                                        // Current bar is not selected, nor on a row or column
                                        if (colorStyleIsUniform)
                                        {
                                            if (itemIdx < itemSectionColors.size())
                                            {
                                                barColor = itemSectionColors.at(itemIdx);
                                            }
                                            else
                                            {
                                                barColor = baseColor;
                                                qWarning() << "itemIdx " << itemIdx << " is larger than itemSectionColors.size() = " << itemSectionColors.size() << "; using baseColor.";
                                            }
                                        }
                                        else
                                        {
                                            gradientTexture = cache->baseGradientTexture();
                                        }
                                        // qDebug() << "Bar color: " << barColor;
                                        break;
                                    }
								}
							}

							if (height == 0) {
								continue;
							}
							else if ((m_reflectionEnabled
								&& (reflection == 1.0f
									|| (reflection != 1.0f
										&& ((m_yFlipped && height < 0.0)
											|| (!m_yFlipped && height > 0.0)))))
								|| !m_reflectionEnabled) {
								// Skip drawing of 0-height bars and reflections of bars on the "wrong side"
								// Set shader bindings
								barShader->setUniformValue(barShader->model(), modelMatrix);
								barShader->setUniformValue(barShader->nModel(),
									itModelMatrix.transposed().inverted());
								barShader->setUniformValue(barShader->MVP(), MVPMatrix);
								if (colorStyleIsUniform) {
									barShader->setUniformValue(barShader->color(), barColor);
								}
								else if (colorStyle == QTheme3D::ColorStyleRangeGradient) {
                                    barShader->setUniformValue(barShader->gradientHeight(),
                                        (GLfloat) (qAbs(height) / m_gradientFraction));
								}

								if (((m_reflectionEnabled && reflection == 1.0f
									&& m_cachedShadowQuality > QAbstractGraph3D::ShadowQualityNone)
									|| m_cachedShadowQuality > QAbstractGraph3D::ShadowQualityNone)
									&& !m_isOpenGLES) {
									// Set shadow shader bindings
									QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
									barShader->setUniformValue(barShader->shadowQ(),
										m_shadowQualityToShader);
									barShader->setUniformValue(barShader->depth(), depthMVPMatrix);
									barShader->setUniformValue(barShader->lightS(), shadowLightStrength);
									barShader->setUniformValue(barShader->lightColor(), lightColor);

									// Draw the object
									m_drawer->drawObject(barShader, barObj, gradientTexture,
										m_depthTexture);
								}
								else {
									// Set shadowless shader bindings
									if (m_reflectionEnabled && reflection != 1.0f
                                        && m_cachedShadowQuality > QAbstractGraph3D::ShadowQualityNone)
                                    {
										barShader->setUniformValue(barShader->lightS(),
											adjustedLightStrength);
									}
                                    else
                                    {
										barShader->setUniformValue(barShader->lightS(), lightStrength);
									}

									// Draw the object
									m_drawer->drawObject(barShader, barObj, gradientTexture);
								}
							}
							itemIdx++;
						}
					}
				}
			}
		}
		glDisable(GL_POLYGON_OFFSET_FILL);

		// Reset culling
		glCullFace(GL_BACK);

		return barSelectionFound;
	}

	void QStackedBarsRenderer3D::drawBackground(GLfloat backgroundRotation,
		const QMatrix4x4 &depthProjectionViewMatrix,
		const QMatrix4x4 &projectionViewMatrix,
		const QMatrix4x4 &viewMatrix, bool reflectingDraw,
		bool drawingSelectionBuffer)
	{
		// Draw background
		if (m_cachedTheme->isBackgroundEnabled() && m_backgroundObj) {
			QVector3D lightPos = m_cachedScene->activeLight()->position();
			QVector4D lightColor = QUtils3D::vectorFromColor(m_cachedTheme->lightColor());
			GLfloat adjustedLightStrength = m_cachedTheme->lightStrength() / 10.0f;
			QShaderHelper3D *shader = 0;

			// Bind background shader
			if (drawingSelectionBuffer)
				shader = m_selectionShader; // Use single color shader when drawing to selection buffer
			else
				shader = m_backgroundShader;
			shader->bind();

			QMatrix4x4 modelMatrix;
			QMatrix4x4 MVPMatrix;
			QMatrix4x4 itModelMatrix;

			QVector3D backgroundScaler(m_scaleXWithBackground, m_scaleYWithBackground,
				m_scaleZWithBackground);
			QVector4D backgroundColor = QUtils3D::vectorFromColor(m_cachedTheme->backgroundColor());
			if (m_reflectionEnabled)
				backgroundColor.setW(backgroundColor.w() * m_reflectivity);

			// Set shader bindings
			shader->setUniformValue(shader->lightP(), lightPos);
			shader->setUniformValue(shader->view(), viewMatrix);
			if (drawingSelectionBuffer) {
				// Use selectionSkipColor for background when drawing to selection buffer
				shader->setUniformValue(shader->color(), selectionSkipColor);
			}
			else {
				shader->setUniformValue(shader->color(), backgroundColor);
			}
			shader->setUniformValue(shader->ambientS(),
				m_cachedTheme->ambientLightStrength() * 2.0f);
			shader->setUniformValue(shader->lightColor(), lightColor);

			// Draw floor
			modelMatrix.scale(backgroundScaler);

			if (m_yFlipped)
				modelMatrix.rotate(m_xRightAngleRotation);
			else
				modelMatrix.rotate(m_xRightAngleRotationNeg);

			itModelMatrix = modelMatrix;

	#ifdef SHOW_DEPTH_TEXTURE_SCENE
			MVPMatrix = depthProjectionViewMatrix * modelMatrix;
	#else
			MVPMatrix = projectionViewMatrix * modelMatrix;
	#endif
			// Set changed shader bindings
			shader->setUniformValue(shader->model(), modelMatrix);
			shader->setUniformValue(shader->nModel(), itModelMatrix.inverted().transposed());
			shader->setUniformValue(shader->MVP(), MVPMatrix);

			if (m_cachedShadowQuality > QAbstractGraph3D::ShadowQualityNone && !m_isOpenGLES) {
				// Set shadow shader bindings
				QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
				shader->setUniformValue(shader->depth(), depthMVPMatrix);
				// Draw the object
				m_drawer->drawObject(shader, m_gridLineObj, 0, m_depthTexture);
			}
			else {
				// Draw the object
				m_drawer->drawObject(shader, m_gridLineObj);
			}

			// Draw walls
			modelMatrix = QMatrix4x4();
			itModelMatrix = QMatrix4x4();
			modelMatrix.translate(0.0f, m_backgroundAdjustment, 0.0f);

			modelMatrix.scale(backgroundScaler);
			itModelMatrix.scale(backgroundScaler);
			modelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);
			itModelMatrix.rotate(backgroundRotation, 0.0f, 1.0f, 0.0f);

	#ifdef SHOW_DEPTH_TEXTURE_SCENE
			MVPMatrix = depthProjectionViewMatrix * modelMatrix;
	#else
			MVPMatrix = projectionViewMatrix * modelMatrix;
	#endif

			// Set changed shader bindings
			shader->setUniformValue(shader->model(), modelMatrix);
			shader->setUniformValue(shader->nModel(), itModelMatrix.inverted().transposed());
			shader->setUniformValue(shader->MVP(), MVPMatrix);
			if (!m_reflectionEnabled || (m_reflectionEnabled && reflectingDraw)) {
				if (m_cachedShadowQuality > QAbstractGraph3D::ShadowQualityNone && !m_isOpenGLES) {
					// Set shadow shader bindings
					QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
					shader->setUniformValue(shader->shadowQ(), m_shadowQualityToShader);
					shader->setUniformValue(shader->depth(), depthMVPMatrix);
					shader->setUniformValue(shader->lightS(), adjustedLightStrength);

					// Draw the object
					m_drawer->drawObject(shader, m_backgroundObj, 0, m_depthTexture);
				}
				else {
					// Set shadowless shader bindings
					shader->setUniformValue(shader->lightS(), m_cachedTheme->lightStrength());

					// Draw the object
					m_drawer->drawObject(shader, m_backgroundObj);
				}
			}
		}
	}

	void QStackedBarsRenderer3D::drawGridLines(const QMatrix4x4 &depthProjectionViewMatrix,
		const QMatrix4x4 &projectionViewMatrix,
		const QMatrix4x4 &viewMatrix)
	{
		if (m_cachedTheme->isGridEnabled()) {
			QShaderHelper3D *lineShader;
			if (m_isOpenGLES)
				lineShader = m_selectionShader; // Plain color shader for GL_LINES
			else
				lineShader = m_backgroundShader;

			QQuaternion lineRotation;

			QVector3D lightPos = m_cachedScene->activeLight()->position();
			QVector4D lightColor = QUtils3D::vectorFromColor(m_cachedTheme->lightColor());

			// Bind bar shader
			lineShader->bind();

			// Set unchanging shader bindings
			QVector4D barColor = QUtils3D::vectorFromColor(m_cachedTheme->gridLineColor());
			lineShader->setUniformValue(lineShader->lightP(), lightPos);
			lineShader->setUniformValue(lineShader->view(), viewMatrix);
			lineShader->setUniformValue(lineShader->color(), barColor);
			lineShader->setUniformValue(lineShader->ambientS(), m_cachedTheme->ambientLightStrength());
			lineShader->setUniformValue(lineShader->lightColor(), lightColor);
			if (m_cachedShadowQuality > QAbstractGraph3D::ShadowQualityNone && !m_isOpenGLES) {
				// Set shadowed shader bindings
				lineShader->setUniformValue(lineShader->shadowQ(), m_shadowQualityToShader);
				lineShader->setUniformValue(lineShader->lightS(),
					m_cachedTheme->lightStrength() / 20.0f);
			}
			else {
				// Set shadowless shader bindings
				lineShader->setUniformValue(lineShader->lightS(),
					m_cachedTheme->lightStrength() / 2.5f);
			}

			GLfloat yFloorLinePosition = gridLineOffset;
			if (m_yFlipped)
				yFloorLinePosition = -yFloorLinePosition;

			QVector3D gridLineScaler(m_scaleXWithBackground, gridLineWidth, gridLineWidth);

			if (m_yFlipped)
				lineRotation = m_xRightAngleRotation;
			else
				lineRotation = m_xRightAngleRotationNeg;

			// Floor lines: rows
			for (GLfloat row = 0.0f; row <= m_cachedRowCount; row++) {
				QMatrix4x4 modelMatrix;
				QMatrix4x4 MVPMatrix;
				QMatrix4x4 itModelMatrix;

				GLfloat rowPos = row * m_cachedBarSpacing.height();
				modelMatrix.translate(0.0f, yFloorLinePosition,
					(m_columnDepth - rowPos) / m_scaleFactor);
				modelMatrix.scale(gridLineScaler);
				itModelMatrix.scale(gridLineScaler);
				modelMatrix.rotate(lineRotation);
				itModelMatrix.rotate(lineRotation);

				MVPMatrix = projectionViewMatrix * modelMatrix;

				// Set the rest of the shader bindings
				lineShader->setUniformValue(lineShader->model(), modelMatrix);
				lineShader->setUniformValue(lineShader->nModel(),
					itModelMatrix.inverted().transposed());
				lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

				if (m_isOpenGLES) {
					m_drawer->drawLine(lineShader);
				}
				else {
					if (m_cachedShadowQuality > QAbstractGraph3D::ShadowQualityNone) {
						// Set shadow shader bindings
						QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
						lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
						// Draw the object
						m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
					}
					else {
						// Draw the object
						m_drawer->drawObject(lineShader, m_gridLineObj);
					}
				}
			}

			// Floor lines: columns
			if (m_isOpenGLES)
				lineRotation = m_yRightAngleRotation;
			gridLineScaler = QVector3D(gridLineWidth, gridLineWidth, m_scaleZWithBackground);
			for (GLfloat bar = 0.0f; bar <= m_cachedColumnCount; bar++) {
				QMatrix4x4 modelMatrix;
				QMatrix4x4 MVPMatrix;
				QMatrix4x4 itModelMatrix;

				GLfloat colPos = bar * m_cachedBarSpacing.width();
				modelMatrix.translate((m_rowWidth - colPos) / m_scaleFactor,
					yFloorLinePosition, 0.0f);
				modelMatrix.scale(gridLineScaler);
				itModelMatrix.scale(gridLineScaler);
				modelMatrix.rotate(lineRotation);
				itModelMatrix.rotate(lineRotation);

				MVPMatrix = projectionViewMatrix * modelMatrix;

				// Set the rest of the shader bindings
				lineShader->setUniformValue(lineShader->model(), modelMatrix);
				lineShader->setUniformValue(lineShader->nModel(),
					itModelMatrix.inverted().transposed());
				lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

				if (m_isOpenGLES) {
					m_drawer->drawLine(lineShader);
				}
				else {
					if (m_cachedShadowQuality > QAbstractGraph3D::ShadowQualityNone) {
						// Set shadow shader bindings
						QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
						lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
						// Draw the object
						m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
					}
					else {
						// Draw the object
						m_drawer->drawObject(lineShader, m_gridLineObj);
					}
				}
			}

			if (m_axisCacheY.segmentCount() > 0) {
				// Wall lines: back wall
				int gridLineCount = m_axisCacheY.gridLineCount();

				GLfloat zWallLinePosition = -m_scaleZWithBackground + gridLineOffset;
				if (m_zFlipped)
					zWallLinePosition = -zWallLinePosition;

				gridLineScaler = QVector3D(m_scaleXWithBackground, gridLineWidth, gridLineWidth);
				for (int line = 0; line < gridLineCount; line++) {
					QMatrix4x4 modelMatrix;
					QMatrix4x4 MVPMatrix;
					QMatrix4x4 itModelMatrix;

					modelMatrix.translate(0.0f,
						m_axisCacheY.gridLinePosition(line),
						zWallLinePosition);
					modelMatrix.scale(gridLineScaler);
					itModelMatrix.scale(gridLineScaler);
					if (m_zFlipped) {
						modelMatrix.rotate(m_xFlipRotation);
						itModelMatrix.rotate(m_xFlipRotation);
					}

					MVPMatrix = projectionViewMatrix * modelMatrix;

					// Set the rest of the shader bindings
					lineShader->setUniformValue(lineShader->model(), modelMatrix);
					lineShader->setUniformValue(lineShader->nModel(),
						itModelMatrix.inverted().transposed());
					lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

					if (m_isOpenGLES) {
						m_drawer->drawLine(lineShader);
					}
					else {
						if (m_cachedShadowQuality > QAbstractGraph3D::ShadowQualityNone) {
							// Set shadow shader bindings
							QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
							lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
							// Draw the object
							m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
						}
						else {
							// Draw the object
							m_drawer->drawObject(lineShader, m_gridLineObj);
						}
					}
				}

				// Wall lines: side wall
				GLfloat xWallLinePosition = -m_scaleXWithBackground + gridLineOffset;
				if (m_xFlipped)
					xWallLinePosition = -xWallLinePosition;

				if (m_xFlipped)
					lineRotation = m_yRightAngleRotationNeg;
				else
					lineRotation = m_yRightAngleRotation;

				gridLineScaler = QVector3D(gridLineWidth, gridLineWidth, m_scaleZWithBackground);
				for (int line = 0; line < gridLineCount; line++) {
					QMatrix4x4 modelMatrix;
					QMatrix4x4 MVPMatrix;
					QMatrix4x4 itModelMatrix;

					modelMatrix.translate(xWallLinePosition,
						m_axisCacheY.gridLinePosition(line),
						0.0f);
					modelMatrix.scale(gridLineScaler);
					itModelMatrix.scale(gridLineScaler);
					modelMatrix.rotate(lineRotation);
					itModelMatrix.rotate(lineRotation);

					MVPMatrix = projectionViewMatrix * modelMatrix;

					// Set the rest of the shader bindings
					lineShader->setUniformValue(lineShader->model(), modelMatrix);
					lineShader->setUniformValue(lineShader->nModel(),
						itModelMatrix.inverted().transposed());
					lineShader->setUniformValue(lineShader->MVP(), MVPMatrix);

					if (m_isOpenGLES) {
						m_drawer->drawLine(lineShader);
					}
					else {
						if (m_cachedShadowQuality > QAbstractGraph3D::ShadowQualityNone) {
							// Set shadow shader bindings
							QMatrix4x4 depthMVPMatrix = depthProjectionViewMatrix * modelMatrix;
							lineShader->setUniformValue(lineShader->depth(), depthMVPMatrix);
							// Draw the object
							m_drawer->drawObject(lineShader, m_gridLineObj, 0, m_depthTexture);
						}
						else {
							// Draw the object
							m_drawer->drawObject(lineShader, m_gridLineObj);
						}
					}
				}
			}
		}
	}

	void QStackedBarsRenderer3D::drawLabels(bool drawSelection, const QCamera3D *activeCamera,
		const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix) {
		QShaderHelper3D *shader = 0;
		GLfloat alphaForValueSelection = labelValueAlpha / 255.0f;
		GLfloat alphaForRowSelection = labelRowAlpha / 255.0f;
		GLfloat alphaForColumnSelection = labelColumnAlpha / 255.0f;
		if (drawSelection) {
			shader = m_selectionShader;
			// m_selectionShader is already bound
		}
		else {
			shader = m_labelShader;
			shader->bind();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		glEnable(GL_POLYGON_OFFSET_FILL);

		float labelAutoAngle = m_axisCacheY.labelAutoRotation();
		float labelAngleFraction = labelAutoAngle / 90.0f;
		float fractionCamY = activeCamera->yRotation() * labelAngleFraction;
		float fractionCamX = activeCamera->xRotation() * labelAngleFraction;
		float labelsMaxWidth = 0.0f;

		int startIndex;
		int endIndex;
		int indexStep;

		// Y Labels
		int labelCount = m_axisCacheY.labelCount();
		GLfloat labelMarginXTrans = labelMargin;
		GLfloat labelMarginZTrans = labelMargin;
		GLfloat labelXTrans = m_scaleXWithBackground;
		GLfloat labelZTrans = m_scaleZWithBackground;
		QVector3D backLabelRotation(0.0f, -90.0f, 0.0f);
		QVector3D sideLabelRotation(0.0f, 0.0f, 0.0f);
		Qt::AlignmentFlag backAlignment = (m_xFlipped != m_zFlipped) ? Qt::AlignLeft : Qt::AlignRight;
		Qt::AlignmentFlag sideAlignment = (m_xFlipped == m_zFlipped) ? Qt::AlignLeft : Qt::AlignRight;

		if (!m_xFlipped) {
			labelXTrans = -labelXTrans;
			labelMarginXTrans = -labelMargin;
		}
		if (m_zFlipped) {
			labelZTrans = -labelZTrans;
			labelMarginZTrans = -labelMargin;
		}

		if (labelAutoAngle == 0.0f) {
			if (!m_xFlipped)
				backLabelRotation.setY(90.0f);
			if (m_zFlipped)
				sideLabelRotation.setY(180.f);
		}
		else {
			// Orient side labels somewhat towards the camera
			if (m_xFlipped) {
				if (m_zFlipped)
					sideLabelRotation.setY(180.0f + (2.0f * labelAutoAngle) - fractionCamX);
				else
					sideLabelRotation.setY(-fractionCamX);
				backLabelRotation.setY(-90.0f + labelAutoAngle - fractionCamX);
			}
			else {
				if (m_zFlipped)
					sideLabelRotation.setY(180.0f - (2.0f * labelAutoAngle) - fractionCamX);
				else
					sideLabelRotation.setY(-fractionCamX);
				backLabelRotation.setY(90.0f - labelAutoAngle - fractionCamX);
			}
		}
		sideLabelRotation.setX(-fractionCamY);
		backLabelRotation.setX(-fractionCamY);

		QQuaternion totalSideRotation = QUtils3D::calculateRotation(sideLabelRotation);
		QQuaternion totalBackRotation = QUtils3D::calculateRotation(backLabelRotation);

		QVector3D backLabelTrans = QVector3D(labelXTrans, 0.0f,
			labelZTrans + labelMarginZTrans);
		QVector3D sideLabelTrans = QVector3D(-labelXTrans - labelMarginXTrans,
			0.0f, -labelZTrans);

		if (m_yFlipped) {
			startIndex = labelCount - 1;
			endIndex = -1;
			indexStep = -1;
		}
		else {
			startIndex = 0;
			endIndex = labelCount;
			indexStep = 1;
		}
		float offsetValue = 0.0f;
		for (int i = startIndex; i != endIndex; i = i + indexStep) {
			backLabelTrans.setY(m_axisCacheY.labelPosition(i));
			sideLabelTrans.setY(backLabelTrans.y());

			glPolygonOffset(offsetValue++ / -10.0f, 1.0f);

			const QLabelItem3D &axisLabelItem = *m_axisCacheY.labelItems().at(i);

			if (drawSelection) {
				QVector4D labelColor = QVector4D(0.0f, 0.0f, i / 255.0f,
					alphaForValueSelection);
				shader->setUniformValue(shader->color(), labelColor);
			}

			// Back wall
			m_dummyBarRenderItem.setTranslation(backLabelTrans);
			m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
				zeroVector, totalBackRotation, 0, m_cachedSelectionMode,
				shader, m_labelObj, activeCamera,
				true, true, QDrawer3D::LabelMid, backAlignment, false, drawSelection);

			// Side wall
			m_dummyBarRenderItem.setTranslation(sideLabelTrans);
			m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
				zeroVector, totalSideRotation, 0, m_cachedSelectionMode,
				shader, m_labelObj, activeCamera,
				true, true, QDrawer3D::LabelMid, sideAlignment, false, drawSelection);

			labelsMaxWidth = qMax(labelsMaxWidth, float(axisLabelItem.size().width()));
		}

		if (!drawSelection && m_axisCacheY.isTitleVisible()) {
			sideLabelTrans.setY(m_backgroundAdjustment);
			backLabelTrans.setY(m_backgroundAdjustment);
			drawAxisTitleY(sideLabelRotation, backLabelRotation, sideLabelTrans, backLabelTrans,
				totalSideRotation, totalBackRotation, m_dummyBarRenderItem, activeCamera,
				labelsMaxWidth, viewMatrix, projectionMatrix, shader);
		}

		// Z labels
		// Calculate the positions for row and column labels and store them
		labelsMaxWidth = 0.0f;
		labelAutoAngle = m_axisCacheZ.labelAutoRotation();
		labelAngleFraction = labelAutoAngle / 90.0f;
		fractionCamY = activeCamera->yRotation() * labelAngleFraction;
		fractionCamX = activeCamera->xRotation() * labelAngleFraction;
		GLfloat labelYAdjustment = 0.005f;
		GLfloat colPosValue = m_scaleXWithBackground + labelMargin;
		GLfloat rowPosValue = m_scaleZWithBackground + labelMargin;
		GLfloat rowPos = 0.0f;
		GLfloat colPos = 0.0f;
		Qt::AlignmentFlag alignment = (m_xFlipped == m_zFlipped) ? Qt::AlignLeft : Qt::AlignRight;
		QVector3D labelRotation;

		if (labelAutoAngle == 0.0f) {
			if (m_zFlipped)
				labelRotation.setY(180.0f);
			if (m_yFlipped) {
				if (m_zFlipped)
					labelRotation.setY(180.0f);
				else
					labelRotation.setY(0.0f);
				labelRotation.setX(90.0f);
			}
			else {
				labelRotation.setX(-90.0f);
			}
		}
		else {
			if (m_zFlipped)
				labelRotation.setY(180.0f);
			if (m_yFlipped) {
				if (m_zFlipped) {
					if (m_xFlipped) {
						labelRotation.setX(90.0f - (labelAutoAngle - fractionCamX)
							* (-labelAutoAngle - fractionCamY) / labelAutoAngle);
						labelRotation.setZ(labelAutoAngle + fractionCamY);
					}
					else {
						labelRotation.setX(90.0f + (labelAutoAngle + fractionCamX)
							* (labelAutoAngle + fractionCamY) / labelAutoAngle);
						labelRotation.setZ(-labelAutoAngle - fractionCamY);
					}
				}
				else {
					if (m_xFlipped) {
						labelRotation.setX(90.0f + (labelAutoAngle - fractionCamX)
							* -(labelAutoAngle + fractionCamY) / labelAutoAngle);
						labelRotation.setZ(-labelAutoAngle - fractionCamY);
					}
					else {
						labelRotation.setX(90.0f - (labelAutoAngle + fractionCamX)
							* (labelAutoAngle + fractionCamY) / labelAutoAngle);
						labelRotation.setZ(labelAutoAngle + fractionCamY);
					}
				}
			}
			else {
				if (m_zFlipped) {
					if (m_xFlipped) {
						labelRotation.setX(-90.0f + (labelAutoAngle - fractionCamX)
							* (-labelAutoAngle + fractionCamY) / labelAutoAngle);
						labelRotation.setZ(-labelAutoAngle + fractionCamY);
					}
					else {
						labelRotation.setX(-90.0f - (labelAutoAngle + fractionCamX)
							* (labelAutoAngle - fractionCamY) / labelAutoAngle);
						labelRotation.setZ(labelAutoAngle - fractionCamY);
					}
				}
				else {
					if (m_xFlipped) {
						labelRotation.setX(-90.0f - (labelAutoAngle - fractionCamX)
							* (-labelAutoAngle + fractionCamY) / labelAutoAngle);
						labelRotation.setZ(labelAutoAngle - fractionCamY);
					}
					else {
						labelRotation.setX(-90.0f + (labelAutoAngle + fractionCamX)
							* (labelAutoAngle - fractionCamY) / labelAutoAngle);
						labelRotation.setZ(-labelAutoAngle + fractionCamY);
					}
				}
			}
		}

		QQuaternion totalRotation = QUtils3D::calculateRotation(labelRotation);
		labelCount = qMin(m_axisCacheZ.labelCount(), m_cachedRowCount);
		if (m_zFlipped) {
			startIndex = 0;
			endIndex = labelCount;
			indexStep = 1;
		}
		else {
			startIndex = labelCount - 1;
			endIndex = -1;
			indexStep = -1;
		}
		offsetValue = 0.0f;
		for (int row = startIndex; row != endIndex; row = row + indexStep) {
			// Go through all rows and get position of max+1 or min-1 column, depending on x flip
			// We need only positions for them, labels have already been generated
			rowPos = (row + 0.5f) * m_cachedBarSpacing.height();
			if (m_xFlipped)
				colPos = -colPosValue;
			else
				colPos = colPosValue;

			glPolygonOffset(offsetValue++ / -10.0f, 1.0f);

			QVector3D labelPos = QVector3D(colPos,
				labelYAdjustment, // raise a bit over background to avoid depth "glimmering"
				(m_columnDepth - rowPos) / m_scaleFactor);

			m_dummyBarRenderItem.setTranslation(labelPos);
			const QLabelItem3D &axisLabelItem = *m_axisCacheZ.labelItems().at(row);

			if (drawSelection) {
				QVector4D labelColor = QVector4D(row / 255.0f, 0.0f, 0.0f, alphaForRowSelection);
				shader->setUniformValue(shader->color(), labelColor);
			}

			m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
				zeroVector, totalRotation, 0, m_cachedSelectionMode,
				shader, m_labelObj, activeCamera,
				true, true, QDrawer3D::LabelMid, alignment,
				false, drawSelection);
			labelsMaxWidth = qMax(labelsMaxWidth, float(axisLabelItem.size().width()));
		}

		if (!drawSelection && m_axisCacheZ.isTitleVisible()) {
			QVector3D titleTrans(colPos, 0.0f, 0.0f);
			drawAxisTitleZ(labelRotation, titleTrans, totalRotation, m_dummyBarRenderItem,
				activeCamera, labelsMaxWidth, viewMatrix, projectionMatrix, shader);
		}

		// X labels
		labelsMaxWidth = 0.0f;
		labelAutoAngle = m_axisCacheX.labelAutoRotation();
		labelAngleFraction = labelAutoAngle / 90.0f;
		fractionCamY = activeCamera->yRotation() * labelAngleFraction;
		fractionCamX = activeCamera->xRotation() * labelAngleFraction;
		alignment = (m_xFlipped != m_zFlipped) ? Qt::AlignLeft : Qt::AlignRight;
		if (labelAutoAngle == 0.0f) {
			labelRotation = QVector3D(-90.0f, 90.0f, 0.0f);
			if (m_xFlipped)
				labelRotation.setY(-90.0f);
			if (m_yFlipped) {
				if (m_xFlipped)
					labelRotation.setY(-90.0f);
				else
					labelRotation.setY(90.0f);
				labelRotation.setX(90.0f);
			}
		}
		else {
			if (m_xFlipped)
				labelRotation.setY(-90.0f);
			else
				labelRotation.setY(90.0f);
			if (m_yFlipped) {
				if (m_zFlipped) {
					if (m_xFlipped) {
						labelRotation.setX(90.0f - (2.0f * labelAutoAngle - fractionCamX)
							* (labelAutoAngle + fractionCamY) / labelAutoAngle);
						labelRotation.setZ(-labelAutoAngle - fractionCamY);
					}
					else {
						labelRotation.setX(90.0f - (2.0f * labelAutoAngle + fractionCamX)
							* (labelAutoAngle + fractionCamY) / labelAutoAngle);
						labelRotation.setZ(labelAutoAngle + fractionCamY);
					}
				}
				else {
					if (m_xFlipped) {
						labelRotation.setX(90.0f + fractionCamX
							* -(labelAutoAngle + fractionCamY) / labelAutoAngle);
						labelRotation.setZ(labelAutoAngle + fractionCamY);
					}
					else {
						labelRotation.setX(90.0f - fractionCamX
							* (-labelAutoAngle - fractionCamY) / labelAutoAngle);
						labelRotation.setZ(-labelAutoAngle - fractionCamY);
					}
				}
			}
			else {
				if (m_zFlipped) {
					if (m_xFlipped) {
						labelRotation.setX(-90.0f + (2.0f * labelAutoAngle - fractionCamX)
							* (labelAutoAngle - fractionCamY) / labelAutoAngle);
						labelRotation.setZ(labelAutoAngle - fractionCamY);
					}
					else {
						labelRotation.setX(-90.0f + (2.0f * labelAutoAngle + fractionCamX)
							* (labelAutoAngle - fractionCamY) / labelAutoAngle);
						labelRotation.setZ(-labelAutoAngle + fractionCamY);
					}
				}
				else {
					if (m_xFlipped) {
						labelRotation.setX(-90.0f - fractionCamX
							* (-labelAutoAngle + fractionCamY) / labelAutoAngle);
						labelRotation.setZ(-labelAutoAngle + fractionCamY);
					}
					else {
						labelRotation.setX(-90.0f + fractionCamX
							* -(labelAutoAngle - fractionCamY) / labelAutoAngle);
						labelRotation.setZ(labelAutoAngle - fractionCamY);
					}
				}
			}
		}

		totalRotation = QUtils3D::calculateRotation(labelRotation);
		labelCount = qMin(m_axisCacheX.labelCount(), m_cachedColumnCount);
		if (m_xFlipped) {
			startIndex = labelCount - 1;
			endIndex = -1;
			indexStep = -1;
		}
		else {
			startIndex = 0;
			endIndex = labelCount;
			indexStep = 1;
		}
		offsetValue = 0.0f;
		for (int column = startIndex; column != endIndex; column = column + indexStep) {
			// Go through all columns and get position of max+1 or min-1 row, depending on z flip
			// We need only positions for them, labels have already been generated
			colPos = (column + 0.5f) * m_cachedBarSpacing.width();
			if (m_zFlipped)
				rowPos = -rowPosValue;
			else
				rowPos = rowPosValue;

			glPolygonOffset(offsetValue++ / -10.0f, 1.0f);

			QVector3D labelPos = QVector3D((colPos - m_rowWidth) / m_scaleFactor,
				labelYAdjustment, // raise a bit over background to avoid depth "glimmering"
				rowPos);

			m_dummyBarRenderItem.setTranslation(labelPos);
			const QLabelItem3D &axisLabelItem = *m_axisCacheX.labelItems().at(column);

			if (drawSelection) {
				QVector4D labelColor = QVector4D(0.0f, column / 255.0f, 0.0f,
					alphaForColumnSelection);
				shader->setUniformValue(shader->color(), labelColor);
			}

			m_drawer->drawLabel(m_dummyBarRenderItem, axisLabelItem, viewMatrix, projectionMatrix,
				zeroVector, totalRotation, 0, m_cachedSelectionMode,
				shader, m_labelObj, activeCamera,
				true, true, QDrawer3D::LabelMid, alignment, false, drawSelection);
			labelsMaxWidth = qMax(labelsMaxWidth, float(axisLabelItem.size().width()));
		}

		if (!drawSelection && m_axisCacheX.isTitleVisible()) {
			QVector3D titleTrans(0.0f, 0.0f, rowPos);
			drawAxisTitleX(labelRotation, titleTrans, totalRotation, m_dummyBarRenderItem,
				activeCamera, labelsMaxWidth, viewMatrix, projectionMatrix, shader);
		}

	#if 0 // Debug label
		static QLabelItem3D debugLabelItem;
		QString debugLabelString(QStringLiteral("Flips: x:%1 y:%2 z:%3 xr:%4 yr:%5"));
		QString finalDebugString = debugLabelString.arg(m_xFlipped).arg(m_yFlipped).arg(m_zFlipped)
			.arg(activeCamera->xRotation()).arg(activeCamera->yRotation());
		m_dummyBarRenderItem.setTranslation(QVector3D(m_xFlipped ? -1.5f : 1.5f,
			m_yFlipped ? 1.5f : -1.5f,
			m_zFlipped ? -1.5f : 1.5f));

		m_drawer->generateLabelItem(debugLabelItem, finalDebugString);
		m_drawer->drawLabel(m_dummyBarRenderItem, debugLabelItem, viewMatrix, projectionMatrix,
			zeroVector, identityQuaternion, 0, m_cachedSelectionMode,
			shader, m_labelObj, activeCamera,
			true, false, QDrawer3D::LabelMid, Qt::AlignHCenter, false, drawSelection);
	#endif
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	void QStackedBarsRenderer3D::updateMultiSeriesScaling(bool uniform)
	{
		m_keepSeriesUniform = uniform;

		// Recalculate scale factors
		m_seriesScaleX = 1.0f / float(m_visibleSeriesCount);
		if (m_keepSeriesUniform)
			m_seriesScaleZ = m_seriesScaleX;
		else
			m_seriesScaleZ = 1.0f;
	}

	void QStackedBarsRenderer3D::updateBarSpecs(GLfloat thicknessRatio, const QSizeF &spacing, bool relative)
	{
		// Convert ratio to QSizeF, as we need it in that format for autoscaling calculations
		m_cachedBarThickness.setWidth(1.0f);
		m_cachedBarThickness.setHeight(1.0f / thicknessRatio);

		if (relative) {
			m_cachedBarSpacing.setWidth((m_cachedBarThickness.width() * 2)
				* (spacing.width() + 1.0f));
			m_cachedBarSpacing.setHeight((m_cachedBarThickness.height() * 2)
				* (spacing.height() + 1.0f));
		}
		else {
			m_cachedBarSpacing = m_cachedBarThickness * 2 + spacing * 2;
		}

		// Slice mode doesn't update correctly without this
		if (m_cachedIsSlicingActivated)
			m_selectionDirty = true;

		// Calculate here and at setting sample space
		calculateSceneScalingFactors();
	}

	void QStackedBarsRenderer3D::updateAxisRange(QAbstractAxis3D::AxisOrientation orientation, float min,
		float max)
	{
		QAbstractRenderer3D::updateAxisRange(orientation, min, max);

		if (orientation == QAbstractAxis3D::AxisOrientationY)
			calculateHeightAdjustment();
	}

	void QStackedBarsRenderer3D::updateAxisReversed(QAbstractAxis3D::AxisOrientation orientation, bool enable)
	{
		QAbstractRenderer3D::updateAxisReversed(orientation, enable);
		if (orientation == QAbstractAxis3D::AxisOrientationY)
			calculateHeightAdjustment();
	}


	void QStackedBarsRenderer3D::updateSelectedBar(const QPoint &position, QStackedBarSeries3D *series)
	{
		m_selectedBarPos = position;
		m_selectedSeriesCache = static_cast<QStackedBarSeriesRenderCache3D *>(m_renderCacheList.value(series, 0));
		m_selectionDirty = true;
		m_selectionLabelDirty = true;

		if (!m_selectedSeriesCache
			|| !m_selectedSeriesCache->isVisible()
			|| m_selectedSeriesCache->renderArray().isEmpty()) {
			m_visualSelectedBarPos = QStackedBarsController3D::invalidSelectionPosition();
			return;
		}

		int adjustedZ = m_selectedBarPos.x() - int(m_axisCacheZ.min());
		int adjustedX = m_selectedBarPos.y() - int(m_axisCacheX.min());
		int maxZ = m_selectedSeriesCache->renderArray().size() - 1;
		int maxX = maxZ >= 0 ? m_selectedSeriesCache->renderArray().at(0).size() - 1 : -1;

		if (m_selectedBarPos == QStackedBarsController3D::invalidSelectionPosition()
			|| adjustedZ < 0 || adjustedZ > maxZ
			|| adjustedX < 0 || adjustedX > maxX) {
			m_visualSelectedBarPos = QStackedBarsController3D::invalidSelectionPosition();
		}
		else {
			m_visualSelectedBarPos = QPoint(adjustedZ, adjustedX);
		}
	}

	void QStackedBarsRenderer3D::resetClickedStatus()
	{
		m_clickedPosition = QStackedBarsController3D::invalidSelectionPosition();
		m_clickedSeries = 0;
	}

	void QStackedBarsRenderer3D::updateShadowQuality(QAbstractGraph3D::ShadowQuality quality)
	{
		m_cachedShadowQuality = quality;
		switch (quality) {
		case QAbstractGraph3D::ShadowQualityLow:
			m_shadowQualityToShader = 33.3f;
			m_shadowQualityMultiplier = 1;
			break;
		case QAbstractGraph3D::ShadowQualityMedium:
			m_shadowQualityToShader = 100.0f;
			m_shadowQualityMultiplier = 3;
			break;
		case QAbstractGraph3D::ShadowQualityHigh:
			m_shadowQualityToShader = 200.0f;
			m_shadowQualityMultiplier = 5;
			break;
		case QAbstractGraph3D::ShadowQualitySoftLow:
			m_shadowQualityToShader = 7.5f;
			m_shadowQualityMultiplier = 1;
			break;
		case QAbstractGraph3D::ShadowQualitySoftMedium:
			m_shadowQualityToShader = 10.0f;
			m_shadowQualityMultiplier = 3;
			break;
		case QAbstractGraph3D::ShadowQualitySoftHigh:
			m_shadowQualityToShader = 15.0f;
			m_shadowQualityMultiplier = 4;
			break;
		default:
			m_shadowQualityToShader = 0.0f;
			m_shadowQualityMultiplier = 1;
			break;
		}

		handleShadowQualityChange();

		// Re-init depth buffer
		updateDepthBuffer();

		// Redraw to handle both reflections and shadows on background
		if (m_reflectionEnabled)
			needRender();
	}

	void QStackedBarsRenderer3D::loadBackgroundMesh()
	{
		QObjectHelper3D::resetObjectHelper(this, m_backgroundObj,
			QStringLiteral(":/defaultMeshes/backgroundNoFloor"));
	}

	void QStackedBarsRenderer3D::updateTextures()
	{
		QAbstractRenderer3D::updateTextures();

		// QDrawer3D has changed; this flag needs to be checked when checking if we need to update labels
		m_updateLabels = true;
	}

	void QStackedBarsRenderer3D::fixMeshFileName(QString &fileName, QAbstractSeries3D::Mesh mesh)
	{
		if (!m_cachedTheme->isBackgroundEnabled()) {
			// Load full version of meshes that have it available
			// Note: Minimal, Point, and Arrow not supported in bar charts
			if (mesh != QAbstractSeries3D::MeshSphere)
				fileName.append(QStringLiteral("Full"));
		}
	}

	void QStackedBarsRenderer3D::calculateSceneScalingFactors()
	{
		// Calculate scene scaling and translation factors
		m_rowWidth = (m_cachedColumnCount * m_cachedBarSpacing.width()) / 2.0f;
		m_columnDepth = (m_cachedRowCount * m_cachedBarSpacing.height()) / 2.0f;
		m_maxDimension = qMax(m_rowWidth, m_columnDepth);
		m_scaleFactor = qMin((m_cachedColumnCount * (m_maxDimension / m_maxSceneSize)),
			(m_cachedRowCount * (m_maxDimension / m_maxSceneSize)));

		// Single bar scaling
		m_scaleX = m_cachedBarThickness.width() / m_scaleFactor;
		m_scaleZ = m_cachedBarThickness.height() / m_scaleFactor;

		// Whole graph scale factors
		m_xScaleFactor = m_rowWidth / m_scaleFactor;
		m_zScaleFactor = m_columnDepth / m_scaleFactor;

		if (m_requestedMargin < 0.0f) {
			m_hBackgroundMargin = 0.0f;
			m_vBackgroundMargin = 0.0f;
		}
		else {
			m_hBackgroundMargin = m_requestedMargin;
			m_vBackgroundMargin = m_requestedMargin;
		}

		m_scaleXWithBackground = m_xScaleFactor + m_hBackgroundMargin;
		m_scaleYWithBackground = 1.0f + m_vBackgroundMargin;
		m_scaleZWithBackground = m_zScaleFactor + m_hBackgroundMargin;

		updateCameraViewport();
		updateCustomItemPositions();
	}

	void QStackedBarsRenderer3D::calculateHeightAdjustment()
	{
		float min = m_axisCacheY.min();
		float max = m_axisCacheY.max();
		GLfloat newAdjustment = 1.0f;
		m_actualFloorLevel = qBound(min, m_floorLevel, max);
		GLfloat maxAbs = qFabs(max - m_actualFloorLevel);

		// Check if we have negative values
		if (min < m_actualFloorLevel)
			m_hasNegativeValues = true;
		else if (min >= m_actualFloorLevel)
			m_hasNegativeValues = false;

		if (max < m_actualFloorLevel) {
			m_heightNormalizer = GLfloat(qFabs(min) - qFabs(max));
			maxAbs = qFabs(max) - qFabs(min);
		}
		else {
			m_heightNormalizer = GLfloat(max - min);
		}

		// Height fractions are used in gradient calculations and are therefore doubled
		// Note that if max or min is exactly zero, we still consider it outside the range
		if (max <= m_actualFloorLevel || min >= m_actualFloorLevel) {
			m_noZeroInRange = true;
			m_gradientFraction = 2.0f;
		}
		else {
			m_noZeroInRange = false;
			GLfloat minAbs = qFabs(min - m_actualFloorLevel);
			m_gradientFraction = qMax(minAbs, maxAbs) / m_heightNormalizer * 2.0f;
		}

		// Calculate translation adjustment for background floor
		newAdjustment = (qBound(0.0f, (maxAbs / m_heightNormalizer), 1.0f) - 0.5f) * 2.0f;
		if (m_axisCacheY.reversed())
			newAdjustment = -newAdjustment;

		if (newAdjustment != m_backgroundAdjustment) {
			m_backgroundAdjustment = newAdjustment;
			m_axisCacheY.setTranslate(m_backgroundAdjustment - 1.0f);
		}
	}

	QStackedBarsController3D::SelectionType QStackedBarsRenderer3D::isSelected(int row, int bar,
		const QStackedBarSeriesRenderCache3D *cache)
	{
		QStackedBarsController3D::SelectionType isSelectedType = QStackedBarsController3D::SelectionNone;

		if ((m_cachedSelectionMode.testFlag(QAbstractGraph3D::SelectionMultiSeries)
			&& m_selectedSeriesCache) || cache == m_selectedSeriesCache) {
			if (row == m_visualSelectedBarPos.x() && bar == m_visualSelectedBarPos.y()
				&& (m_cachedSelectionMode.testFlag(QAbstractGraph3D::SelectionItem))) {
				isSelectedType = QStackedBarsController3D::SelectionItem;
			}
			else if (row == m_visualSelectedBarPos.x()
				&& (m_cachedSelectionMode.testFlag(QAbstractGraph3D::SelectionRow))) {
				isSelectedType = QStackedBarsController3D::SelectionRow;
			}
			else if (bar == m_visualSelectedBarPos.y()
				&& (m_cachedSelectionMode.testFlag(QAbstractGraph3D::SelectionColumn))) {
				isSelectedType = QStackedBarsController3D::SelectionColumn;
			}
		}

		return isSelectedType;
	}

	QPoint QStackedBarsRenderer3D::selectionColorToArrayPosition(const QVector4D &selectionColor)
	{
		QPoint position = QStackedBarsController3D::invalidSelectionPosition();
		m_clickedType = QAbstractGraph3D::ElementNone;
		m_selectedLabelIndex = -1;
		m_selectedCustomItemIndex = -1;
		if (selectionColor.w() == itemAlpha) {
			// Normal selection item
			position = QPoint(int(selectionColor.x() + int(m_axisCacheZ.min())),
				int(selectionColor.y()) + int(m_axisCacheX.min()));
			// Pass item clicked info to input handler
			m_clickedType = QAbstractGraph3D::ElementSeries;
		}
		else if (selectionColor.w() == labelRowAlpha) {
			// Row selection
			if (m_cachedSelectionMode.testFlag(QAbstractGraph3D::SelectionRow)) {
				// Use column from previous selection in case we have row + column mode
				GLint previousCol = qMax(0, m_selectedBarPos.y()); // Use 0 if previous is invalid
				position = QPoint(int(selectionColor.x() + int(m_axisCacheZ.min())), previousCol);
			}
			m_selectedLabelIndex = selectionColor.x();
			// Pass label clicked info to input handler
			m_clickedType = QAbstractGraph3D::ElementAxisZLabel;
		}
		else if (selectionColor.w() == labelColumnAlpha) {
			// Column selection
			if (m_cachedSelectionMode.testFlag(QAbstractGraph3D::SelectionColumn)) {
				// Use row from previous selection in case we have row + column mode
				GLint previousRow = qMax(0, m_selectedBarPos.x()); // Use 0 if previous is invalid
				position = QPoint(previousRow, int(selectionColor.y()) + int(m_axisCacheX.min()));
			}
			m_selectedLabelIndex = selectionColor.y();
			// Pass label clicked info to input handler
			m_clickedType = QAbstractGraph3D::ElementAxisXLabel;
		}
		else if (selectionColor.w() == labelValueAlpha) {
			// Value selection
			position = QStackedBarsController3D::invalidSelectionPosition();
			m_selectedLabelIndex = selectionColor.z();
			// Pass label clicked info to input handler
			m_clickedType = QAbstractGraph3D::ElementAxisYLabel;
		}
		else if (selectionColor.w() == customItemAlpha) {
			// Custom item selection
			position = QStackedBarsController3D::invalidSelectionPosition();
			m_selectedCustomItemIndex = int(selectionColor.x())
				+ (int(selectionColor.y()) << 8)
				+ (int(selectionColor.z()) << 16);
			m_clickedType = QAbstractGraph3D::ElementCustomItem;
		}
		return position;
	}

	QStackedBarSeries3D *QStackedBarsRenderer3D::selectionColorToSeries(const QVector4D &selectionColor)
	{
		if (selectionColor == selectionSkipColor) {
			return 0;
		}
		else {
			int seriesIndexFromColor(selectionColor.z());
			foreach(QSeriesRenderCache3D *baseCache, m_renderCacheList) {
				QStackedBarSeriesRenderCache3D *cache = static_cast<QStackedBarSeriesRenderCache3D *>(baseCache);
				if (cache->visualIndex() == seriesIndexFromColor)
					return cache->series();
			}
		}
		return 0;
	}

	void QStackedBarsRenderer3D::updateSlicingActive(bool isSlicing)
	{
		if (isSlicing == m_cachedIsSlicingActivated)
			return;

		m_cachedIsSlicingActivated = isSlicing;

		if (!m_cachedIsSlicingActivated) {
			// We need to re-init selection buffer in case there has been a resize
			initSelectionBuffer();
			initCursorPositionBuffer();
		}

		updateDepthBuffer(); // Re-init depth buffer as well
		m_selectionDirty = true;
	}

	void QStackedBarsRenderer3D::initShaders(const QString &vertexShader, const QString &fragmentShader)
	{
		if (m_barShader)
			delete m_barShader;
		m_barShader = new QShaderHelper3D(this, vertexShader, fragmentShader);
		m_barShader->initialize();
	}

	void QStackedBarsRenderer3D::initGradientShaders(const QString &vertexShader, const QString &fragmentShader)
	{
		if (m_barGradientShader)
			delete m_barGradientShader;
		m_barGradientShader = new QShaderHelper3D(this, vertexShader, fragmentShader);
		m_barGradientShader->initialize();
	}

	void QStackedBarsRenderer3D::initSelectionShader()
	{
		if (m_selectionShader)
			delete m_selectionShader;
		m_selectionShader = new QShaderHelper3D(this, QStringLiteral(":/shaders/vertexPlainColor"),
			QStringLiteral(":/shaders/fragmentPlainColor"));
		m_selectionShader->initialize();
	}

	void QStackedBarsRenderer3D::initSelectionBuffer()
	{
		m_textureHelper->deleteTexture(&m_selectionTexture);

		if (m_cachedIsSlicingActivated || m_primarySubViewport.size().isEmpty())
			return;

		m_selectionTexture = m_textureHelper->createSelectionTexture(m_primarySubViewport.size(),
			m_selectionFrameBuffer,
			m_selectionDepthBuffer);
	}

	void QStackedBarsRenderer3D::initDepthShader()
	{
		if (!m_isOpenGLES) {
			if (m_depthShader)
				delete m_depthShader;
			m_depthShader = new QShaderHelper3D(this, QStringLiteral(":/shaders/vertexDepth"),
				QStringLiteral(":/shaders/fragmentDepth"));
			m_depthShader->initialize();
		}
	}

	void QStackedBarsRenderer3D::updateDepthBuffer()
	{
		if (!m_isOpenGLES) {
			m_textureHelper->deleteTexture(&m_depthTexture);

			if (m_primarySubViewport.size().isEmpty())
				return;

			if (m_cachedShadowQuality > QAbstractGraph3D::ShadowQualityNone) {
				m_depthTexture =
					m_textureHelper->createDepthTextureFrameBuffer(m_primarySubViewport.size(),
						m_depthFrameBuffer,
						m_shadowQualityMultiplier);
				if (!m_depthTexture)
					lowerShadowQuality();
			}
		}
	}

	void QStackedBarsRenderer3D::initBackgroundShaders(const QString &vertexShader,
		const QString &fragmentShader)
	{
		if (m_backgroundShader)
			delete m_backgroundShader;
		m_backgroundShader = new QShaderHelper3D(this, vertexShader, fragmentShader);
		m_backgroundShader->initialize();
	}

	QVector3D QStackedBarsRenderer3D::convertPositionToTranslation(const QVector3D &position, bool isAbsolute)
	{
		float xTrans = 0.0f;
		float yTrans = 0.0f;
		float zTrans = 0.0f;
		if (!isAbsolute) {
			// Convert row and column to translation on graph
			xTrans = (((position.x() - m_axisCacheX.min() + 0.5f) * m_cachedBarSpacing.width())
				- m_rowWidth) / m_scaleFactor;
			zTrans = (m_columnDepth - ((position.z() - m_axisCacheZ.min() + 0.5f)
				* m_cachedBarSpacing.height())) / m_scaleFactor;
			yTrans = m_axisCacheY.positionAt(position.y());
		}
		else {
			xTrans = position.x() * m_xScaleFactor;
			yTrans = position.y() + m_backgroundAdjustment;
			zTrans = position.z() * -m_zScaleFactor;
		}
		return QVector3D(xTrans, yTrans, zTrans);
	}

	void QStackedBarsRenderer3D::updateAspectRatio(float ratio)
	{
		Q_UNUSED(ratio)
	}

	void QStackedBarsRenderer3D::updateFloorLevel(float level)
	{
		foreach(QSeriesRenderCache3D *cache, m_renderCacheList)
			cache->setDataDirty(true);
		m_floorLevel = level;
		calculateHeightAdjustment();
	}

	void QStackedBarsRenderer3D::updateMargin(float margin)
	{
		QAbstractRenderer3D::updateMargin(margin);
		calculateSceneScalingFactors();
	}

}
