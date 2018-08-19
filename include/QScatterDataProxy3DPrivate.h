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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Q_SCATTER_DATA_PROXY_3D_PRIVATE_H
#define Q_SCATTER_DATA_PROXY_3D_PRIVATE_H

#include "QScatterDataProxy3D.h"
#include "QAbstractDataProxy3DPrivate.h"
#include "QScatterDataItem3D.h"

namespace QtStackedBar3DVis
{

	class QAbstractAxis3D;

	class QScatterDataProxy3DPrivate : public QAbstractDataProxy3DPrivate
	{
		Q_OBJECT
	public:
		QScatterDataProxy3DPrivate(QScatterDataProxy3D *q);
		virtual ~QScatterDataProxy3DPrivate();

		void resetArray(QScatterDataArray *newArray);
		void setItem(int index, const QScatterDataItem3D &item);
		void setItems(int index, const QScatterDataArray &items);
		int addItem(const QScatterDataItem3D &item);
		int addItems(const QScatterDataArray &items);
		void insertItem(int index, const QScatterDataItem3D &item);
		void insertItems(int index, const QScatterDataArray &items);
		void removeItems(int index, int removeCount);
		void limitValues(QVector3D &minValues, QVector3D &maxValues, QAbstractAxis3D *axisX,
			QAbstractAxis3D *axisY, QAbstractAxis3D *axisZ) const;
		bool isValidValue(float axisValue, float value, QAbstractAxis3D *axis) const;

		virtual void setSeries(QAbstractSeries3D *series);
	private:
		QScatterDataProxy3D *qptr();
		QScatterDataArray *m_dataArray;

		friend class QScatterDataProxy3D;
	};

}

#endif
