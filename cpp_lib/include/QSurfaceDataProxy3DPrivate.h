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

#ifndef QSURFACEDATAPROXY_P_H
#define QSURFACEDATAPROXY_P_H

#include "QSurfaceDataProxy3D.h"
#include "QAbstractDataProxy3DPrivate.h"

namespace QtStackedBar3DVis
{

	class QAbstractAxis3D;

	class QSurfaceDataProxy3DPrivate : public QAbstractDataProxy3DPrivate
	{
		Q_OBJECT
	public:
		QSurfaceDataProxy3DPrivate(QSurfaceDataProxy3D *q);
		virtual ~QSurfaceDataProxy3DPrivate();

		void resetArray(QSurfaceDataArray3D *newArray);
		void setRow(int rowIndex, QSurfaceDataRow3D *row);
		void setRows(int rowIndex, const QSurfaceDataArray3D &rows);
		void setItem(int rowIndex, int columnIndex, const QSurfaceDataItem3D &item);
		int addRow(QSurfaceDataRow3D *row);
		int addRows(const QSurfaceDataArray3D &rows);
		void insertRow(int rowIndex, QSurfaceDataRow3D *row);
		void insertRows(int rowIndex, const QSurfaceDataArray3D &rows);
		void removeRows(int rowIndex, int removeCount);
		void limitValues(QVector3D &minValues, QVector3D &maxValues, QAbstractAxis3D *axisX,
			QAbstractAxis3D *axisY, QAbstractAxis3D *axisZ) const;
		bool isValidValue(float value, QAbstractAxis3D *axis) const;

		virtual void setSeries(QAbstractSeries3D *series);

	protected:
		QSurfaceDataArray3D *m_dataArray;

	private:
		QSurfaceDataProxy3D *qptr();
		void clearRow(int rowIndex);
		void clearArray();

		friend class QSurfaceDataProxy3D;
	};

}

#endif
