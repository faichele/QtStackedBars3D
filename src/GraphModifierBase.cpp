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


#include "GraphModifierBase.h"
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

GraphModifierBase::GraphModifierBase(QAbstractGraph3D* graph)
	: m_graph(graph),
	m_xRotation(0.0f),
	m_yRotation(0.0f),
	m_fontSize(30),
	m_segments(4),
	m_subSegments(3),
	m_minval(-200.0f),
	m_maxval(200.0f),
	m_smooth(false),
	m_cycleCount(25),
	m_maxDeviation(10),
	m_deviationAxis(new QValueAxis3D),
	m_cyclesAxis_1(new QCategoryAxis3D),
	m_cyclesAxis_2(new QCategoryAxis3D)
{
	
}

GraphModifierBase::~GraphModifierBase()
{

}

void GraphModifierBase::changeRange(int range)
{
	if (range >= m_cycles_2.count())
		m_cyclesAxis_1->setRange(0, m_cycles_2.count() - 1);
	else
		m_cyclesAxis_1->setRange(range, range);
}

void GraphModifierBase::changePresetCamera()
{
	m_animationCameraX.stop();
	m_animationCameraY.stop();
	m_animationCameraZoom.stop();
	m_animationCameraTarget.stop();

	// Restore camera target in case animation has changed it
	m_graph->scene()->activeCamera()->setTarget(QVector3D(0.0f, 0.0f, 0.0f));

	static int preset = QCamera3D::CameraPresetFront;

	m_graph->scene()->activeCamera()->setCameraPreset((QCamera3D::CameraPreset)preset);

	if (++preset > QCamera3D::CameraPresetDirectlyBelow)
		preset = QCamera3D::CameraPresetFrontLow;
}

void GraphModifierBase::changeTheme(int theme)
{
	QTheme3D *currentTheme = m_graph->activeTheme();
	currentTheme->setType(QTheme3D::Theme(theme));
	emit backgroundEnabledChanged(currentTheme->isBackgroundEnabled());
	emit gridEnabledChanged(currentTheme->isGridEnabled());
	emit fontChanged(currentTheme->font());
	emit fontSizeChanged(currentTheme->font().pointSize());
}

void GraphModifierBase::changeLabelBackground()
{
	m_graph->activeTheme()->setLabelBackgroundEnabled(!m_graph->activeTheme()->isLabelBackgroundEnabled());
}

void GraphModifierBase::changeSelectionMode(int selectionMode)
{
	QComboBox *comboBox = qobject_cast<QComboBox *>(sender());
	if (comboBox) {
		int flags = comboBox->itemData(selectionMode).toInt();
		m_graph->setSelectionMode(QAbstractGraph3D::SelectionFlags(flags));
	}
}

void GraphModifierBase::changeFont(const QFont &font)
{
	QFont newFont = font;
	m_graph->activeTheme()->setFont(newFont);
}

void GraphModifierBase::changeFontSize(int fontsize)
{
	m_fontSize = fontsize;
	QFont font = m_graph->activeTheme()->font();
	font.setPointSize(m_fontSize);
	m_graph->activeTheme()->setFont(font);
}

void GraphModifierBase::shadowQualityUpdatedByVisual(QAbstractGraph3D::ShadowQuality sq)
{
	int quality = int(sq);
	// Updates the UI component to show correct shadow quality
	emit shadowQualityChanged(quality);
}

void GraphModifierBase::changeLabelRotation(int rotation)
{
	m_deviationAxis->setLabelAutoRotation(float(rotation));
	m_cyclesAxis_2->setLabelAutoRotation(float(rotation));
	m_cyclesAxis_1->setLabelAutoRotation(float(rotation));
}

void GraphModifierBase::setAxisTitleVisibility(int enabled)
{
	m_deviationAxis->setTitleVisible((bool) enabled);
	m_cyclesAxis_2->setTitleVisible((bool) enabled);
	m_cyclesAxis_1->setTitleVisible((bool) enabled);
}

void GraphModifierBase::setAxisTitleFixed(int enabled)
{
	m_deviationAxis->setTitleFixed((bool) enabled);
	m_cyclesAxis_2->setTitleFixed((bool) enabled);
	m_cyclesAxis_1->setTitleFixed((bool) enabled);
}

void GraphModifierBase::changeShadowQuality(int quality)
{
	QAbstractGraph3D::ShadowQuality sq = QAbstractGraph3D::ShadowQuality(quality);
	m_graph->setShadowQuality(sq);
	emit shadowQualityChanged(quality);
}

void GraphModifierBase::rotateX(int rotation)
{
	m_xRotation = rotation;
	m_graph->scene()->activeCamera()->setCameraPosition(m_xRotation, m_yRotation);
}

void GraphModifierBase::rotateY(int rotation)
{
	m_yRotation = rotation;
	m_graph->scene()->activeCamera()->setCameraPosition(m_xRotation, m_yRotation);
}

void GraphModifierBase::setBackgroundEnabled(int enabled)
{
	m_graph->activeTheme()->setBackgroundEnabled(bool(enabled));
}

void GraphModifierBase::setGridEnabled(int enabled)
{
	m_graph->activeTheme()->setGridEnabled(bool(enabled));
}

void GraphModifierBase::setReflection(int enabled)
{
	m_graph->setReflection((bool) enabled);
}

void CurrentControlTarget::switchTarget()
{
	if (m_oldModifier != NULL)
	{
		qDebug() << "Disconnecting old GraphModifier signals.";
		QObject::disconnect(m_rotationSliderX, SIGNAL(valueChanged(int)), m_oldModifier, SLOT(rotateX(int)));
		QObject::disconnect(m_rotationSliderY, SIGNAL(valueChanged(int)), m_oldModifier, SLOT(rotateY(int)));
	
		QObject::disconnect(m_labelButton, SIGNAL(clicked()), m_oldModifier,
			SLOT(changeLabelBackground()));
		QObject::disconnect(m_cameraButton, SIGNAL(clicked()), m_oldModifier,
			SLOT(changePresetCamera()));
		QObject::disconnect(m_zoomToSelectedButton, SIGNAL(clicked()), m_oldModifier,
			SLOT(zoomToSelectedBar()));

		QObject::disconnect(m_backgroundCheckBox, SIGNAL(stateChanged(int)), m_oldModifier,
			SLOT(setBackgroundEnabled(int)));
		QObject::disconnect(m_gridCheckBox, SIGNAL(stateChanged(int)), m_oldModifier,
			SLOT(setGridEnabled(int)));
		QObject::disconnect(m_seriesCheckBox, SIGNAL(stateChanged(int)), m_oldModifier,
			SLOT(setSeriesVisibility(int)));
		QObject::disconnect(m_reverseValueAxisCheckBox, SIGNAL(stateChanged(int)), m_oldModifier,
			SLOT(setReverseValueAxis(int)));
		QObject::disconnect(m_reflectionCheckBox, SIGNAL(stateChanged(int)), m_oldModifier,
			SLOT(setReflection(int)));
		QObject::disconnect(m_smoothCheckBox, SIGNAL(stateChanged(int)), m_oldModifier,
			SLOT(setSmoothBars(int)));

		QObject::disconnect(m_oldModifier, SIGNAL(backgroundEnabledChanged(bool)),
			m_backgroundCheckBox, SLOT(setChecked(bool)));
		QObject::disconnect(m_oldModifier, SIGNAL(gridEnabledChanged(bool)),
			m_gridCheckBox, SLOT(setChecked(bool)));
	
		QObject::disconnect(m_rangeList, SIGNAL(currentIndexChanged(int)), m_oldModifier,
			SLOT(changeRange(int)));
		QObject::disconnect(m_barStyleList, SIGNAL(currentIndexChanged(int)), m_oldModifier,
			SLOT(changeStyle(int)));
		QObject::disconnect(m_selectionModeList, SIGNAL(currentIndexChanged(int)), m_oldModifier,
			SLOT(changeSelectionMode(int)));
		QObject::disconnect(m_themeList, SIGNAL(currentIndexChanged(int)), m_oldModifier,
			SLOT(changeTheme(int)));
		QObject::disconnect(m_shadowQuality, SIGNAL(currentIndexChanged(int)), m_oldModifier,
			SLOT(changeShadowQuality(int)));

		QObject::disconnect(m_oldModifier, SIGNAL(shadowQualityChanged(int)), m_shadowQuality,
			SLOT(setCurrentIndex(int)));

		QObject::disconnect(m_fontList, SIGNAL(currentFontChanged(const QFont&)), m_oldModifier,
			SLOT(changeFont(const QFont&)));
		QObject::disconnect(m_oldModifier, SIGNAL(fontChanged(QFont)), m_fontList,
			SLOT(setCurrentFont(QFont)));

		QObject::disconnect(m_fontSizeSlider, SIGNAL(valueChanged(int)), m_oldModifier,
			SLOT(changeFontSize(int)));
		QObject::disconnect(m_oldModifier, SIGNAL(fontSizeChanged(int)), m_fontSizeSlider,
			SLOT(setValue(int)));

		QObject::disconnect(m_axisLabelRotationSlider, SIGNAL(valueChanged(int)), m_oldModifier,
			SLOT(changeLabelRotation(int)));

		QObject::disconnect(m_axisTitlesVisibleCB, SIGNAL(stateChanged(int)), m_oldModifier,
			SLOT(setAxisTitleVisibility(int)));
		QObject::disconnect(m_axisTitlesFixedCB, SIGNAL(stateChanged(int)), m_oldModifier,
			SLOT(setAxisTitleFixed(int)));
	}

	if (m_activeModifier != NULL)
	{
		qDebug() << "Connecting new GraphModifier signals.";
		QObject::connect(m_rotationSliderX, SIGNAL(valueChanged(int)), m_activeModifier, SLOT(rotateX(int)));
		QObject::connect(m_rotationSliderY, SIGNAL(valueChanged(int)), m_activeModifier, SLOT(rotateY(int)));
		QObject::connect(m_labelButton, SIGNAL(clicked()), m_activeModifier,
			SLOT(changeLabelBackground()));
		QObject::connect(m_cameraButton, SIGNAL(clicked()), m_activeModifier,
			SLOT(changePresetCamera()));
		QObject::connect(m_zoomToSelectedButton, SIGNAL(clicked()), m_activeModifier,
			SLOT(zoomToSelectedBar()));

		QObject::connect(m_backgroundCheckBox, SIGNAL(stateChanged(int)), m_activeModifier,
			SLOT(setBackgroundEnabled(int)));
		QObject::connect(m_gridCheckBox, SIGNAL(stateChanged(int)), m_activeModifier,
			SLOT(setGridEnabled(int)));
		QObject::connect(m_seriesCheckBox, SIGNAL(stateChanged(int)), m_activeModifier,
			SLOT(setSeriesVisibility(int)));
		QObject::connect(m_reverseValueAxisCheckBox, SIGNAL(stateChanged(int)), m_activeModifier,
			SLOT(setReverseValueAxis(int)));
		QObject::connect(m_reflectionCheckBox, SIGNAL(stateChanged(int)), m_activeModifier,
			SLOT(setReflection(int)));
		QObject::connect(m_smoothCheckBox, SIGNAL(stateChanged(int)), m_activeModifier,
			SLOT(setSmoothBars(int)));

		QObject::connect(m_activeModifier, SIGNAL(backgroundEnabledChanged(bool)),
			m_backgroundCheckBox, SLOT(setChecked(bool)));
		QObject::connect(m_activeModifier, SIGNAL(gridEnabledChanged(bool)),
			m_gridCheckBox, SLOT(setChecked(bool)));
	
		QObject::connect(m_rangeList, SIGNAL(currentIndexChanged(int)), m_activeModifier,
			SLOT(changeRange(int)));
		QObject::connect(m_barStyleList, SIGNAL(currentIndexChanged(int)), m_activeModifier,
			SLOT(changeStyle(int)));
		QObject::connect(m_selectionModeList, SIGNAL(currentIndexChanged(int)), m_activeModifier,
			SLOT(changeSelectionMode(int)));
		QObject::connect(m_themeList, SIGNAL(currentIndexChanged(int)), m_activeModifier,
			SLOT(changeTheme(int)));
		QObject::connect(m_shadowQuality, SIGNAL(currentIndexChanged(int)), m_activeModifier,
			SLOT(changeShadowQuality(int)));

		QObject::connect(m_activeModifier, SIGNAL(shadowQualityChanged(int)), m_shadowQuality,
			SLOT(setCurrentIndex(int)));

		QObject::connect(m_fontList, SIGNAL(currentFontChanged(const QFont&)), m_activeModifier,
			SLOT(changeFont(const QFont&)));
		QObject::connect(m_activeModifier, SIGNAL(fontChanged(QFont)), m_fontList,
			SLOT(setCurrentFont(QFont)));

		QObject::connect(m_fontSizeSlider, SIGNAL(valueChanged(int)), m_activeModifier,
			SLOT(changeFontSize(int)));
		QObject::connect(m_activeModifier, SIGNAL(fontSizeChanged(int)), m_fontSizeSlider,
			SLOT(setValue(int)));

		QObject::connect(m_axisLabelRotationSlider, SIGNAL(valueChanged(int)), m_activeModifier,
			SLOT(changeLabelRotation(int)));

		QObject::connect(m_axisTitlesVisibleCB, SIGNAL(stateChanged(int)), m_activeModifier,
			SLOT(setAxisTitleVisibility(int)));
		QObject::connect(m_axisTitlesFixedCB, SIGNAL(stateChanged(int)), m_activeModifier,
			SLOT(setAxisTitleFixed(int)));
	}
	
	/*
	QObject::connect(widgetgraph, &QBars3D::shadowQualityChanged, modifier,
		&GraphModifier::shadowQualityUpdatedByVisual);
	*/
}