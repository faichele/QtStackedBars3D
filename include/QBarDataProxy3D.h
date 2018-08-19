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

#ifndef Q_BAR_DATA_PROXY_3D_H
#define Q_BAR_DATA_PROXY_3D_H

#include "QAbstractDataProxy3D.h"

#include <QBarDataItem3D.h>
#include <QtCore/QVector>
#include <QtCore/QStringList>

namespace QtStackedBar3DVis
{
	typedef QVector<QBarDataItem3D> QBarDataRow3D;
	typedef QList<QBarDataRow3D *> QBarDataArray3D;

	class QBarDataProxyPrivate;
	class QBarSeries3D;

	class QT_STACKEDBAR3D_EXPORT QBarDataProxy3D : public QAbstractDataProxy3D
	{
		Q_OBJECT

			Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)
			Q_PROPERTY(QStringList rowLabels READ rowLabels WRITE setRowLabels NOTIFY rowLabelsChanged)
			Q_PROPERTY(QStringList columnLabels READ columnLabels WRITE setColumnLabels NOTIFY columnLabelsChanged)
			Q_PROPERTY(QBarSeries3D *series READ series NOTIFY seriesChanged)
	public:
		explicit QBarDataProxy3D(QObject *parent = Q_NULLPTR);
		virtual ~QBarDataProxy3D();

		QBarSeries3D *series() const;
		int rowCount() const;

		QStringList rowLabels() const;
		void setRowLabels(const QStringList &labels);
		QStringList columnLabels() const;
		void setColumnLabels(const QStringList &labels);

		const QBarDataArray3D *array() const;
		const QBarDataRow3D *rowAt(int rowIndex) const;
		const QBarDataItem3D *itemAt(int rowIndex, int columnIndex) const;
		const QBarDataItem3D *itemAt(const QPoint &position) const;

		void resetArray();
		void resetArray(QBarDataArray3D *newArray);
		void resetArray(QBarDataArray3D *newArray, const QStringList &rowLabels,
			const QStringList &columnLabels);

		void setRow(int rowIndex, QBarDataRow3D *row);
		void setRow(int rowIndex, QBarDataRow3D *row, const QString &label);
		void setRows(int rowIndex, const QBarDataArray3D &rows);
		void setRows(int rowIndex, const QBarDataArray3D &rows, const QStringList &labels);

		void setItem(int rowIndex, int columnIndex, const QBarDataItem3D &item);
		void setItem(const QPoint &position, const QBarDataItem3D &item);

		int addRow(QBarDataRow3D *row);
		int addRow(QBarDataRow3D *row, const QString &label);
		int addRows(const QBarDataArray3D &rows);
		int addRows(const QBarDataArray3D &rows, const QStringList &labels);

		void insertRow(int rowIndex, QBarDataRow3D *row);
		void insertRow(int rowIndex, QBarDataRow3D *row, const QString &label);
		void insertRows(int rowIndex, const QBarDataArray3D &rows);
		void insertRows(int rowIndex, const QBarDataArray3D &rows, const QStringList &labels);

		void removeRows(int rowIndex, int removeCount, bool removeLabels = true);

	Q_SIGNALS:
		void arrayReset();
		void rowsAdded(int startIndex, int count);
		void rowsChanged(int startIndex, int count);
		void rowsRemoved(int startIndex, int count);
		void rowsInserted(int startIndex, int count);
		void itemChanged(int rowIndex, int columnIndex);

		void rowCountChanged(int count);
		void rowLabelsChanged();
		void columnLabelsChanged();
		void seriesChanged(QBarSeries3D *series);

	protected:
		explicit QBarDataProxy3D(QBarDataProxyPrivate *d, QObject *parent = Q_NULLPTR);
		QBarDataProxyPrivate *dptr();
		const QBarDataProxyPrivate *dptrc() const;

	private:
		Q_DISABLE_COPY(QBarDataProxy3D)

			friend class QBarsController3D;
	};

}

#endif
