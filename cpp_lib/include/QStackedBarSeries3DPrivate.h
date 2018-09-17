#ifndef Q_STACKED_BAR_SERIES_3D_PRIVATE_H
#define Q_STACKED_BAR_SERIES_3D_PRIVATE_H

#include "QAbstractSeries3DPrivate.h"
#include "QStackedBarSeries3D.h"
#include "QStackedBarDataProxy3D.h"
#include <QtCore/QPoint>
#include <QtCore/QList>

namespace QtStackedBar3DVis
{
	class QStackedBarSeries3DPrivate : public QAbstractSeries3DPrivate
	{
		Q_OBJECT
	public:
		QStackedBarSeries3DPrivate(QStackedBarSeries3D *q);
		virtual ~QStackedBarSeries3DPrivate();

		virtual void setDataProxy(QAbstractDataProxy3D* proxy);
		virtual void connectControllerAndProxy(QAbstractController3D *newController);
		virtual void createItemLabel();

		void handleMeshRotationChanged(const QQuaternion &rotation);

		void setSelectedBar(const QPoint &position);

		const QList<QColor>& sectionColors();
		void setSectionColors(const QList<QColor>& sectionColors);

		void connectSignals();

	private:
		QStackedBarSeries3D *qptr();

		QPoint m_selectedBar;
		QList<QColor> m_sectionColors;

	private:
		friend class QStackedBarSeries3D;
	};
}

#endif