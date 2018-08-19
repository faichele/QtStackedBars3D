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

#ifndef Q_AXIS_3D_VALUE_FORMATTER_H
#define Q_AXIS_3D_VALUE_FORMATTER_H

#include <QVisualizationGlobals3D.h>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QVector>
#include <QtCore/QStringList>
#include <QtCore/QLocale>

namespace QtStackedBar3DVis
{

	class QValue3DAxisFormatterPrivate;
	class QValue3DAxis;

	class QAxisValueFormatter3D : public QObject
	{
		Q_OBJECT
	protected:
		explicit QAxisValueFormatter3D(QValue3DAxisFormatterPrivate *d, QObject *parent = Q_NULLPTR);
	public:
		explicit QAxisValueFormatter3D(QObject *parent = Q_NULLPTR);
		virtual ~QAxisValueFormatter3D();

	public:
		void setAllowNegatives(bool allow);
		bool allowNegatives() const;
		void setAllowZero(bool allow);
		bool allowZero() const;

		virtual QAxisValueFormatter3D *createNewInstance() const;
		virtual void recalculate();
		virtual QString stringForValue(qreal value, const QString &format) const;
		virtual float positionAt(float value) const;
		virtual float valueAt(float position) const;
		virtual void populateCopy(QAxisValueFormatter3D &copy) const;

		void markDirty(bool labelsChange = false);
		QValue3DAxis *axis() const;

		QVector<float> &gridPositions() const;
		QVector<float> &subGridPositions() const;
		QVector<float> &labelPositions() const;
		QStringList &labelStrings() const;

		void setLocale(const QLocale &locale);
		QLocale locale() const;

		QScopedPointer<QValue3DAxisFormatterPrivate> d_ptr;

	private:
		Q_DISABLE_COPY(QAxisValueFormatter3D)

			friend class QAbstractController3D;
		friend class QAbstractRenderer3D;
		friend class Bars3DRenderer;
		friend class Scatter3DRenderer;
		friend class Surface3DRenderer;
		friend class SurfaceObject;
		friend class QValue3DAxisFormatterPrivate;
		friend class QLogValue3DAxisFormatter;
		friend class QValue3DAxis;
		friend class QValue3DAxisPrivate;
		friend class AxisRenderCache;
		friend class QBar3DSeriesPrivate;
		friend class QScatter3DSeriesPrivate;
		friend class QSurface3DSeriesPrivate;
	};

}

#endif
