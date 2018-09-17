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

#ifndef BARSERIESRENDERCACHE_P_H
#define BARSERIESRENDERCACHE_P_H

#include "QVisualizationGlobals3D.h"
#include "QSeriesRenderCache3D.h"
#include "QBarSeries3D.h"
#include "QBarRenderItem3D.h"

namespace QtStackedBar3DVis
{

	class QT_STACKEDBAR3D_EXPORT QBarSeriesRenderCache3D : public QSeriesRenderCache3D
	{
	public:
		QBarSeriesRenderCache3D(QAbstractSeries3D *series, QAbstractRenderer3D *renderer);
		virtual ~QBarSeriesRenderCache3D();

		void cleanup(QTextureHelper3D *texHelper);

		inline BarRenderItemArray &renderArray() { return m_renderArray; }
		inline QBarSeries3D *series() const { return static_cast<QBarSeries3D *>(m_series); }
		inline QVector<BarRenderSliceItem> &sliceArray() { return m_sliceArray; }
		inline void setVisualIndex(int index) { m_visualIndex = index; }
		inline int visualIndex() { return m_visualIndex; }

	protected:
		BarRenderItemArray m_renderArray;
		QVector<BarRenderSliceItem> m_sliceArray;
		int m_visualIndex; // order of the series is relevant
	};

}

#endif
