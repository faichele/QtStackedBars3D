#ifndef GRAPH_MODIFIER_BASE_H
#define GRAPH_MODIFIER_BASE_H

#include <QtGui/QFont>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QPointer>
#include <QtCore/QPropertyAnimation>
#include <QtWidgets/QSlider>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFontComboBox>

#include "QAbstractGraph3D.h"
#include "QCategoryAxis3D.h"
#include "QValueAxis3D.h"

using namespace QtStackedBar3DVis;

class GraphModifierBase : public QObject
{
	Q_OBJECT
public:
	explicit GraphModifierBase(QAbstractGraph3D* graph);
	~GraphModifierBase();

	// virtual void resetCycleDeviationData();
	int getCycleCount() const { return m_cycleCount; }

public Q_SLOTS:
	virtual void rotateX(int rotation);
	virtual void rotateY(int rotation);
	virtual void changeLabelBackground();
	virtual void changePresetCamera();
	virtual void changeRange(int range);
	virtual void changeStyle(int style) = 0;
	virtual void changeSelectionMode(int selectionMode);
	virtual void changeTheme(int theme);
	virtual void changeShadowQuality(int quality);
	virtual void shadowQualityUpdatedByVisual(QAbstractGraph3D::ShadowQuality shadowQuality);
	virtual void changeLabelRotation(int rotation);
	virtual void setAxisTitleVisibility(int enabled);
	virtual void setAxisTitleFixed(int enabled);
	virtual void zoomToSelectedBar() = 0;
	virtual void setBackgroundEnabled(int enabled);
	virtual void setGridEnabled(int enabled);
	virtual void setSmoothBars(int smooth) = 0;
	virtual void setSeriesVisibility(int enabled) = 0;
	virtual void setReverseValueAxis(int enabled) = 0;
	virtual void setReflection(int enabled);
	virtual void changeFont(const QFont &font);
	virtual void changeFontSize(int fontsize);

Q_SIGNALS:
	void shadowQualityChanged(int quality);
	void backgroundEnabledChanged(bool enabled);
	void gridEnabledChanged(bool enabled);
	void fontChanged(QFont font);
	void fontSizeChanged(int size);

protected:
	float m_xRotation;
	float m_yRotation;
	int m_fontSize;
	int m_segments;
	int m_subSegments;
	float m_minval;
	float m_maxval;
	
	bool m_smooth;
	QPropertyAnimation m_animationCameraX;
	QPropertyAnimation m_animationCameraY;
	QPropertyAnimation m_animationCameraZoom;
	QPropertyAnimation m_animationCameraTarget;
	float m_defaultAngleX;
	float m_defaultAngleY;
	float m_defaultZoom;
	QVector3D m_defaultTarget;

	int m_cycleCount;
	float m_maxDeviation;

	QStringList m_cycles_1;
	QStringList m_cycles_2;

	QValueAxis3D *m_deviationAxis;
	QCategoryAxis3D *m_cyclesAxis_1;
	QCategoryAxis3D *m_cyclesAxis_2;

	QAbstractGraph3D* m_graph;
};

class CurrentControlTarget : public QObject
{
	Q_OBJECT
public:
	CurrentControlTarget(GraphModifierBase* graphModifier, GraphModifierBase* graphModifierStacked,
		QSlider* rotationSliderX, QSlider* rotationSliderY, QPushButton* labelButton, QPushButton *cameraButton,
		QPushButton *zoomToSelectedButton, 
		QCheckBox* backgroundCheckBox, QCheckBox* gridCheckBox, QCheckBox* seriesCheckBox, 
		QCheckBox* reverseValueAxisCheckBox, QCheckBox* reflectionCheckBox, QCheckBox *smoothCheckBox,
		QComboBox *rangeList, QFontComboBox *fontList, 
		QComboBox *selectionModeList, QComboBox *barStyleList, QComboBox *themeList, QComboBox *shadowQuality,
		QSlider* fontSizeSlider, QSlider* axisLabelRotationSlider,
		QCheckBox* axisTitlesVisibleCB, QCheckBox* axisTitlesFixedCB): m_activeModifier(NULL), m_oldModifier(NULL)
	{
		m_graphModifier = graphModifier;
		m_graphModifierStacked = graphModifierStacked;
		m_activeModifier = m_graphModifierStacked;
		m_rotationSliderX = rotationSliderX;
		m_rotationSliderY = rotationSliderY;
		m_labelButton = labelButton;
		m_cameraButton = cameraButton;
		m_zoomToSelectedButton = zoomToSelectedButton;

		m_backgroundCheckBox = backgroundCheckBox;
		m_gridCheckBox = gridCheckBox;
		m_seriesCheckBox = seriesCheckBox;
		m_reverseValueAxisCheckBox = reverseValueAxisCheckBox;
		m_reflectionCheckBox = reflectionCheckBox;
		m_smoothCheckBox = smoothCheckBox;
		m_axisTitlesVisibleCB = axisTitlesVisibleCB;
		m_axisTitlesFixedCB = axisTitlesFixedCB;
	
		m_rangeList = rangeList;
		m_fontList = fontList;
		m_selectionModeList = selectionModeList;
		m_barStyleList = barStyleList;
		m_themeList = themeList;
		m_shadowQuality = shadowQuality;

		m_fontSizeSlider = fontSizeSlider;
		m_axisLabelRotationSlider = axisLabelRotationSlider;
	}

	void switchTarget();

	public Q_SLOTS:
	void controlTargetChanged(int index)
	{
		if (index == 1)
		{
			m_oldModifier = m_activeModifier;
			m_activeModifier = m_graphModifierStacked;
			switchTarget();
		}
		else if (index == 0)
		{
			m_oldModifier = m_activeModifier;
			m_activeModifier = m_graphModifier;
			switchTarget();
		}
	}

private:
	GraphModifierBase* m_graphModifier;
	GraphModifierBase* m_graphModifierStacked;

	GraphModifierBase* m_activeModifier;
	GraphModifierBase* m_oldModifier;

	QSlider* m_rotationSliderX;
	QSlider* m_rotationSliderY;
	QPushButton* m_labelButton;
	QPushButton* m_cameraButton;
	QPushButton* m_zoomToSelectedButton;
	QCheckBox* m_backgroundCheckBox;
	QCheckBox* m_gridCheckBox;
	QCheckBox* m_seriesCheckBox;
	QCheckBox* m_reverseValueAxisCheckBox;
	QCheckBox* m_reflectionCheckBox;
	QCheckBox* m_smoothCheckBox;
	QComboBox* m_rangeList;
	QFontComboBox* m_fontList;
	QComboBox* m_selectionModeList;
	QComboBox* m_barStyleList;
	QComboBox* m_themeList;
	QComboBox* m_shadowQuality;
	QCheckBox* m_axisTitlesVisibleCB;
	QCheckBox* m_axisTitlesFixedCB;

	QSlider* m_fontSizeSlider;
	QSlider* m_axisLabelRotationSlider;
};

#endif
