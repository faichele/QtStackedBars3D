#ifndef Q_STACKED_BAR_SERIES_3D_H
#define Q_STACKED_BAR_SERIES_3D_H

#include "QAbstractSeries3D.h"
#include "QStackedBarSeries3D.h"
#include "QStackedBarDataProxy3D.h"

#include <QtCore/QPoint>
#include <QtCore/QList>

namespace QtStackedBar3DVis
{
	class QStackedBarSeries3DPrivate;

	class QT_STACKEDBAR3D_EXPORT QStackedBarSeries3D : public QAbstractSeries3D
	{
		Q_OBJECT
			Q_PROPERTY(QStackedBarDataProxy3D *dataProxy READ dataProxy WRITE setDataProxy NOTIFY dataProxyChanged)
			Q_PROPERTY(QPoint selectedBar READ selectedBar WRITE setSelectedBar NOTIFY selectedBarChanged)
			Q_PROPERTY(float meshAngle READ meshAngle WRITE setMeshAngle NOTIFY meshAngleChanged)

	public:
		explicit QStackedBarSeries3D(QObject *parent = Q_NULLPTR);
		explicit QStackedBarSeries3D(QStackedBarDataProxy3D *dataProxy, QObject *parent = Q_NULLPTR);
		virtual ~QStackedBarSeries3D();

		void setDataProxy(QStackedBarDataProxy3D *proxy);
		QStackedBarDataProxy3D *dataProxy() const;

		void setSelectedBar(const QPoint &position);
		QPoint selectedBar() const;
		static QPoint invalidSelectionPosition();

		void setMeshAngle(float angle);
		float meshAngle() const;

		const QList<QColor>& sectionColors();
		void setSectionColors(const QList<QColor>& sectionColors);

        void setSectionColor(unsigned int index, unsigned int r, unsigned int g, unsigned int b, unsigned int a);

	Q_SIGNALS:
		void dataProxyChanged(QStackedBarDataProxy3D *proxy);
		void selectedBarChanged(const QPoint &position);
		void meshAngleChanged(float angle);

	protected:
		QStackedBarSeries3DPrivate *dptr();
		const QStackedBarSeries3DPrivate *dptrc() const;

	private:
		Q_DISABLE_COPY(QStackedBarSeries3D)

		friend class QStackedBarsController3D;
	};
}

#endif
