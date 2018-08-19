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

#include "QStackedBarSeriesRenderCache3D.h"
#include "QAbstractSeries3DPrivate.h"
#include "QAbstractRenderer3D.h"
#include "QUtils3D.h"

namespace QtStackedBar3DVis
{

	QStackedBarSeriesRenderCache3D::QStackedBarSeriesRenderCache3D(QAbstractSeries3D *series,
		QAbstractRenderer3D *renderer)
		: QSeriesRenderCache3D(series, renderer),
		m_visualIndex(-1)
	{
	}

	QStackedBarSeriesRenderCache3D::~QStackedBarSeriesRenderCache3D()
	{
	}

	void QStackedBarSeriesRenderCache3D::populate(bool newSeries)
	{
		QSeriesRenderCache3D::populate(newSeries);
		QAbstractSeriesChangeBitField3D &changeTracker = m_series->d_ptr->m_changeTracker;

		if (newSeries || changeTracker.colorRangeChanged) 
		{
			QStackedBarSeries3D* series = this->series();
			const QList<QColor>& sectionColors = series->sectionColors();
			m_sectionColors.clear();
			for (int k = 0; k < sectionColors.size(); k++)
			{
				m_sectionColors.append(QUtils3D::vectorFromColor(sectionColors[k]));
			}
			
			changeTracker.colorRangeChanged = false;
		}
	}

	void QStackedBarSeriesRenderCache3D::cleanup(QTextureHelper3D *texHelper)
	{
		m_renderArray.clear();
		m_sliceArray.clear();

		QSeriesRenderCache3D::cleanup(texHelper);
	}
}
