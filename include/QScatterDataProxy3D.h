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

#ifndef Q_SCATTER_DATA_PROXY_3D_H
#define Q_SCATTER_DATA_PROXY_3D_H

#include "QAbstractDataProxy3D.h"
#include "QScatterDataItem3D.h"

namespace QtStackedBar3DVis
{

	typedef QVector<QScatterDataItem3D> QScatterDataArray;

	class QScatterDataProxy3DPrivate;
	class QScatterSeries3D;

	class QT_STACKEDBAR3D_EXPORT QScatterDataProxy3D : public QAbstractDataProxy3D
	{
		Q_OBJECT

			Q_PROPERTY(int itemCount READ itemCount NOTIFY itemCountChanged)
			Q_PROPERTY(QScatterSeries3D *series READ series NOTIFY seriesChanged)

	public:
		explicit QScatterDataProxy3D(QObject *parent = Q_NULLPTR);
		virtual ~QScatterDataProxy3D();

		QScatterSeries3D *series() const;
		int itemCount() const;
		const QScatterDataArray *array() const;
		const QScatterDataItem3D *itemAt(int index) const;

		void resetArray(QScatterDataArray *newArray);

		void setItem(int index, const QScatterDataItem3D &item);
		void setItems(int index, const QScatterDataArray &items);

		int addItem(const QScatterDataItem3D &item);
		int addItems(const QScatterDataArray &items);

		void insertItem(int index, const QScatterDataItem3D &item);
		void insertItems(int index, const QScatterDataArray &items);

		void removeItems(int index, int removeCount);

	Q_SIGNALS:
		void arrayReset();
		void itemsAdded(int startIndex, int count);
		void itemsChanged(int startIndex, int count);
		void itemsRemoved(int startIndex, int count);
		void itemsInserted(int startIndex, int count);

		void itemCountChanged(int count);
		void seriesChanged(QScatterSeries3D *series);

	protected:
		explicit QScatterDataProxy3D(QScatterDataProxy3DPrivate *d, QObject *parent = Q_NULLPTR);
		QScatterDataProxy3DPrivate *dptr();
		const QScatterDataProxy3DPrivate *dptrc() const;

	private:
		Q_DISABLE_COPY(QScatterDataProxy3D)

			friend class QScatterController3D;
	};

}

#endif
