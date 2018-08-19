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

#ifndef Q_SURFACE_DATAP_ROXY_H
#define Q_SURFACE_DATAP_ROXY_H

#include <QAbstractDataProxy3D.h>
#include <QSurfaceDataItem3D.h>

namespace QtStackedBar3DVis
{
	typedef QVector<QSurfaceDataItem3D> QSurfaceDataRow3D;
	typedef QList<QSurfaceDataRow3D *> QSurfaceDataArray3D;

	class QSurfaceDataProxy3DPrivate;
	class QSurfaceSeries3D;

	class QT_STACKEDBAR3D_EXPORT QSurfaceDataProxy3D : public QAbstractDataProxy3D
	{
		Q_OBJECT

			Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
			Q_PROPERTY(int columnCount READ columnCount NOTIFY columnCountChanged)
			Q_PROPERTY(QSurfaceSeries3D *series READ series NOTIFY seriesChanged)

	public:
		explicit QSurfaceDataProxy3D(QObject *parent = Q_NULLPTR);
		virtual ~QSurfaceDataProxy3D();

		QSurfaceSeries3D *series() const;
		int rowCount() const;
		int columnCount() const;
		const QSurfaceDataArray3D *array() const;
		const QSurfaceDataItem3D *itemAt(int rowIndex, int columnIndex) const;
		const QSurfaceDataItem3D *itemAt(const QPoint &position) const;

		void resetArray(QSurfaceDataArray3D *newArray);

		void setRow(int rowIndex, QSurfaceDataRow3D *row);
		void setRows(int rowIndex, const QSurfaceDataArray3D &rows);

		void setItem(int rowIndex, int columnIndex, const QSurfaceDataItem3D &item);
		void setItem(const QPoint &position, const QSurfaceDataItem3D &item);

		int addRow(QSurfaceDataRow3D *row);
		int addRows(const QSurfaceDataArray3D &rows);

		void insertRow(int rowIndex, QSurfaceDataRow3D *row);
		void insertRows(int rowIndex, const QSurfaceDataArray3D &rows);

		void removeRows(int rowIndex, int removeCount);

	Q_SIGNALS:
		void arrayReset();
		void rowsAdded(int startIndex, int count);
		void rowsChanged(int startIndex, int count);
		void rowsRemoved(int startIndex, int count);
		void rowsInserted(int startIndex, int count);
		void itemChanged(int rowIndex, int columnIndex);

		void rowCountChanged(int count);
		void columnCountChanged(int count);
		void seriesChanged(QSurfaceSeries3D *series);

	protected:
		explicit QSurfaceDataProxy3D(QSurfaceDataProxy3DPrivate *d, QObject *parent = Q_NULLPTR);
		QSurfaceDataProxy3DPrivate *dptr();
		const QSurfaceDataProxy3DPrivate *dptrc() const;

	private:
		Q_DISABLE_COPY(QSurfaceDataProxy3D)

			friend class QSurfaceController3D;
	};

}

#endif
