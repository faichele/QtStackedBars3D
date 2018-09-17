#ifndef GRAPH_MODIFIER_H
#define GRAPH_MODIFIER_H

#include "GraphModifierBase.h"
#include "QBars3D.h"
#include "QBarDataProxy3D.h"
#include "QAbstractSeries3D.h"

#include <QtGui/QFont>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QPointer>
#include <QtCore/QPropertyAnimation>

#include <random>

using namespace QtStackedBar3DVis;

class GraphModifier : public GraphModifierBase
{
	Q_OBJECT
public:
	explicit GraphModifier(QBars3D *bargraph);
	~GraphModifier();

	void resetCycleDeviationData();
	void setSmoothBars(int smooth);
	void setSeriesVisibility(int enabled);
	void setReverseValueAxis(int enabled);

public Q_SLOTS:
	void changeStyle(int style);
	void zoomToSelectedBar();

private:
	QBars3D *m_graph;
	
	QBarSeries3D *m_primarySeries;
	QBarSeries3D *m_secondarySeries;
	QAbstractSeries3D::Mesh m_barMesh;
};

#endif
