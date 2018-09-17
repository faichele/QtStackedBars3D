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

#ifndef Q_BAR_DATA_PROXY_3D_PRIVATE_H
#define Q_BAR_DATA_PROXY_3D_PRIVATE_H

#include "QStackedBarDataProxy3D.h"
#include "QAbstractDataProxy3DPrivate.h"

namespace QtStackedBar3DVis
{

	class QStackedBarDataProxy3DPrivate : public QAbstractDataProxy3DPrivate
	{
		Q_OBJECT
	public:
		QStackedBarDataProxy3DPrivate(QStackedBarDataProxy3D *q);
		virtual ~QStackedBarDataProxy3DPrivate();

		void resetArray(QStackedBarDataArray3D *newArray, const QStringList *rowLabels,
			const QStringList *columnLabels);
		void setRow(int rowIndex, QStackedBarDataRow3D *row, const QString *label);
		void setRows(int rowIndex, const QStackedBarDataArray3D &rows, const QStringList *labels);
		void setItem(int rowIndex, int columnIndex, const QStackedBarDataItem3D &item);
		int addRow(QStackedBarDataRow3D *row, const QString *label);
		int addRows(const QStackedBarDataArray3D &rows, const QStringList *labels);
		void insertRow(int rowIndex, QStackedBarDataRow3D *row, const QString *label);
		void insertRows(int rowIndex, const QStackedBarDataArray3D &rows, const QStringList *labels);
		void removeRows(int rowIndex, int removeCount, bool removeLabels);

		QPair<GLfloat, GLfloat> limitValues(int startRow, int startColumn, int rowCount,
			int columnCount) const;

		virtual void setSeries(QAbstractSeries3D *series);

		int sectionCount() const;

	private:
		QStackedBarDataProxy3D *qptr();
		void clearRow(int rowIndex);
		void clearArray();
		void fixRowLabels(int startIndex, int count, const QStringList &newLabels, bool isInsert);

		QStackedBarDataArray3D *m_dataArray;
		int m_sectionCount;
		QStringList m_rowLabels;
		QStringList m_columnLabels;

	private:
		friend class QStackedBarDataProxy3D;
	};

}

#endif
