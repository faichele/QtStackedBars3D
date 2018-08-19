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


#include "graphmodifier.h"
#include "QAbstractSeries3D.h"

#include "QCategoryAxis3D.h"
#include "QValueAxis3D.h"
#include "QBarDataProxy3D.h"
#include "QScene3D.h"
#include "QCamera3D.h"
#include "QBarSeries3D.h"
#include "QTheme3D.h"

#include <QtCore/QTime>
#include <QtWidgets/QComboBox>
#include <QtCore/qmath.h>

// const QString celsiusString = QString(QChar(0xB0)) + "C";

GraphModifier::GraphModifier(QBars3D *bargraph)
	: GraphModifierBase(bargraph),
	m_graph(bargraph),
	m_primarySeries(new QBarSeries3D),
	m_secondarySeries(new QBarSeries3D),
	m_barMesh(QAbstractSeries3D::MeshBevelBar)
{
	m_graph->setShadowQuality(QAbstractGraph3D::ShadowQualitySoftMedium);
	m_graph->activeTheme()->setBackgroundEnabled(false);
	m_graph->activeTheme()->setFont(QFont("Times New Roman", m_fontSize));
	m_graph->activeTheme()->setLabelBackgroundEnabled(true);
	m_graph->setMultiSeriesUniform(true);

	for (int i = 0; i < m_cycleCount; i++)
		m_cycles_1 << "Takt " + QString::number(i + 1);
	
	for (int i = 0; i < m_cycleCount; i++)
		m_cycles_2 << "Takt " + QString::number(i + 1);

	// m_cycles_2 << "2006" << "2007" << "2008" << "2009" << "2010" << "2011" << "2012" << "2013";

	m_deviationAxis->setTitle("Abweichung");
	m_deviationAxis->setSegmentCount(m_segments);
	m_deviationAxis->setSubSegmentCount(m_subSegments);
	m_deviationAxis->setRange(m_minval, m_maxval);
	m_deviationAxis->setLabelFormat(QString(QStringLiteral("%.1f ")));
	m_deviationAxis->setLabelAutoRotation(30.0f);
	m_deviationAxis->setTitleVisible(true);

	m_cyclesAxis_1->setTitle("Takt");
	m_cyclesAxis_1->setLabelAutoRotation(30.0f);
	m_cyclesAxis_1->setTitleVisible(true);
	m_cyclesAxis_2->setTitle("Takt");
	m_cyclesAxis_2->setLabelAutoRotation(30.0f);
	m_cyclesAxis_2->setTitleVisible(true);

	m_graph->setValueAxis(m_deviationAxis);
	m_graph->setRowAxis(m_cyclesAxis_1);
	m_graph->setColumnAxis(m_cyclesAxis_2);

	m_primarySeries->setItemLabelFormat(QStringLiteral("Differenz Vektor-Norm - @colLabel @rowLabel: @valueLabel"));
	m_primarySeries->setMesh(QAbstractSeries3D::MeshBevelBar);
	m_primarySeries->setMeshSmooth(false);

	m_secondarySeries->setItemLabelFormat(QStringLiteral("Winkel zwischen Vektoren - @colLabel @rowLabel: @valueLabel"));
	m_secondarySeries->setMesh(QAbstractSeries3D::MeshBevelBar);
	m_secondarySeries->setMeshSmooth(false);
	m_secondarySeries->setVisible(false);

	m_graph->addSeries(m_primarySeries);
	m_graph->addSeries(m_secondarySeries);

	changePresetCamera();

	resetCycleDeviationData();

	// Set up property animations for zooming to the selected bar
	QCamera3D *camera = m_graph->scene()->activeCamera();
	m_defaultAngleX = camera->xRotation();
	m_defaultAngleY = camera->yRotation();
	m_defaultZoom = camera->zoomLevel();
	m_defaultTarget = camera->target();

	m_animationCameraX.setTargetObject(camera);
	m_animationCameraY.setTargetObject(camera);
	m_animationCameraZoom.setTargetObject(camera);
	m_animationCameraTarget.setTargetObject(camera);

	m_animationCameraX.setPropertyName("xRotation");
	m_animationCameraY.setPropertyName("yRotation");
	m_animationCameraZoom.setPropertyName("zoomLevel");
	m_animationCameraTarget.setPropertyName("target");

	int duration = 1700;
	m_animationCameraX.setDuration(duration);
	m_animationCameraY.setDuration(duration);
	m_animationCameraZoom.setDuration(duration);
	m_animationCameraTarget.setDuration(duration);

	// The zoom always first zooms out above the graph and then zooms in
	qreal zoomOutFraction = 0.3;
	m_animationCameraX.setKeyValueAt(zoomOutFraction, QVariant::fromValue(0.0f));
	m_animationCameraY.setKeyValueAt(zoomOutFraction, QVariant::fromValue(90.0f));
	m_animationCameraZoom.setKeyValueAt(zoomOutFraction, QVariant::fromValue(50.0f));
	m_animationCameraTarget.setKeyValueAt(zoomOutFraction,
		QVariant::fromValue(QVector3D(0.0f, 0.0f, 0.0f)));
}

GraphModifier::~GraphModifier()
{
	delete m_graph;
}

void GraphModifier::changeStyle(int style)
{
	QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
	if (comboBox) 
	{
		m_barMesh = QAbstractSeries3D::Mesh(comboBox->itemData(style).toInt());
		m_primarySeries->setMesh(m_barMesh);
		m_secondarySeries->setMesh(m_barMesh);
	}
}

void GraphModifier::zoomToSelectedBar()
{
	m_animationCameraX.stop();
	m_animationCameraY.stop();
	m_animationCameraZoom.stop();
	m_animationCameraTarget.stop();

	QCamera3D *camera = m_graph->scene()->activeCamera();
	float currentX = camera->xRotation();
	float currentY = camera->yRotation();
	float currentZoom = camera->zoomLevel();
	QVector3D currentTarget = camera->target();

	m_animationCameraX.setStartValue(QVariant::fromValue(currentX));
	m_animationCameraY.setStartValue(QVariant::fromValue(currentY));
	m_animationCameraZoom.setStartValue(QVariant::fromValue(currentZoom));
	m_animationCameraTarget.setStartValue(QVariant::fromValue(currentTarget));

	QPoint selectedBar = m_graph->selectedSeries()
	? m_graph->selectedSeries()->selectedBar()
	: QBarSeries3D::invalidSelectionPosition();

	if (selectedBar != QBarSeries3D::invalidSelectionPosition()) {
	// Normalize selected bar position within axis range to determine target coordinates
	QVector3D endTarget;
	float xMin = m_graph->columnAxis()->min();
	float xRange = m_graph->columnAxis()->max() - xMin;
	float zMin = m_graph->rowAxis()->min();
	float zRange = m_graph->rowAxis()->max() - zMin;
	endTarget.setX((selectedBar.y() - xMin) / xRange * 2.0f - 1.0f);
	endTarget.setZ((selectedBar.x() - zMin) / zRange * 2.0f - 1.0f);

	// Rotate the camera so that it always points approximately to the graph center
	qreal endAngleX = 90.0 - qRadiansToDegrees(qAtan(qreal(endTarget.z() / endTarget.x())));
	if (endTarget.x() > 0.0f)
	endAngleX -= 180.0f;
	float barValue = m_graph->selectedSeries()->dataProxy()->itemAt(selectedBar.x(),
	selectedBar.y())->value();
	float endAngleY = barValue >= 0.0f ? 30.0f : -30.0f;
	if (m_graph->valueAxis()->reversed())
	endAngleY *= -1.0f;

	m_animationCameraX.setEndValue(QVariant::fromValue(float(endAngleX)));
	m_animationCameraY.setEndValue(QVariant::fromValue(endAngleY));
	m_animationCameraZoom.setEndValue(QVariant::fromValue(250));
	m_animationCameraTarget.setEndValue(QVariant::fromValue(endTarget));
	}
	else {
	// No selected bar, so return to the default view
	m_animationCameraX.setEndValue(QVariant::fromValue(m_defaultAngleX));
	m_animationCameraY.setEndValue(QVariant::fromValue(m_defaultAngleY));
	m_animationCameraZoom.setEndValue(QVariant::fromValue(m_defaultZoom));
	m_animationCameraTarget.setEndValue(QVariant::fromValue(m_defaultTarget));
	}

	m_animationCameraX.start();
	m_animationCameraY.start();
	m_animationCameraZoom.start();
	m_animationCameraTarget.start();
}

void GraphModifier::setSmoothBars(int smooth)
{
	m_smooth = bool(smooth);
	m_primarySeries->setMeshSmooth(m_smooth);
	m_secondarySeries->setMeshSmooth(m_smooth);
}

void GraphModifier::setSeriesVisibility(int enabled)
{
	m_secondarySeries->setVisible(bool(enabled));
}

void GraphModifier::setReverseValueAxis(int enabled)
{
	m_graph->valueAxis()->setReversed(enabled);
}

void GraphModifier::resetCycleDeviationData()
{
	// Set up data

	std::mt19937_64 rnd_generator;
	std::uniform_real_distribution<float> rnd_uniform_dist(-m_maxDeviation, m_maxDeviation);

	// Create data arrays
	QBarDataArray3D *dataSet = new QBarDataArray3D;
	QBarDataArray3D *dataSet2 = new QBarDataArray3D;
	QBarDataRow3D *dataRow;
	QBarDataRow3D *dataRow2;

	dataSet->reserve(m_cycleCount);
	for (int cycle_1 = 0; cycle_1 < m_cycleCount; cycle_1++) {
		// Create a data row
		dataRow = new QBarDataRow3D(m_cycleCount);
		dataRow2 = new QBarDataRow3D(m_cycleCount);
		for (int cycle_2 = 0; cycle_2 < m_cycleCount; cycle_2++) {
			// Add data to the row
			if (cycle_1 == cycle_2)
			{
				(*dataRow)[cycle_2].setValue(0.0);
				(*dataRow2)[cycle_2].setValue(0.0);
			}
			else
			{
				(*dataRow)[cycle_2].setValue(rnd_uniform_dist(rnd_generator));
				(*dataRow2)[cycle_2].setValue(rnd_uniform_dist(rnd_generator));
			}
		}
		// Add the row to the set
		dataSet->append(dataRow);
		dataSet2->append(dataRow2);
	}

	// Add data to the data proxy (the data proxy assumes ownership of it)
	m_primarySeries->dataProxy()->resetArray(dataSet, m_cycles_2, m_cycles_1);
	m_secondarySeries->dataProxy()->resetArray(dataSet2, m_cycles_2, m_cycles_1);
}

