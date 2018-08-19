#ifndef GRAPH_MODIFIER_STACKED_H
#define GRAPH_MODIFIER_STACKED_H

#include "QStackedBars3D.h"
#include "QStackedBarDataProxy3D.h"

#include "QAbstractSeries3D.h"

#include <QtGui/QFont>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QPointer>
#include <QtCore/QPropertyAnimation>

#include <random>

#include "GraphModifierBase.h"

using namespace QtStackedBar3DVis;

class GraphModifierStacked : public GraphModifierBase
{
	Q_OBJECT
public:
	explicit GraphModifierStacked(QStackedBars3D *bargraph);
	~GraphModifierStacked();

	void resetCycleDeviationData();
	void setSmoothBars(int smooth);
	void setSeriesVisibility(int enabled);
	void setReverseValueAxis(int enabled);

public Q_SLOTS:
	void changeStyle(int style);
	void zoomToSelectedBar();

private:
	QStackedBars3D *m_graph;
	QStackedBarSeries3D *m_primarySeries;
	QStackedBarSeries3D *m_secondarySeries;
	QAbstractSeries3D::Mesh m_barMesh;
	
};

#endif
