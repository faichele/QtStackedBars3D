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

#include "QAbstractSeries3D.h"
#include "QScatterSeriesRenderCache3D.h"
#include "QScatterObjectBufferHelper3D.h"
#include "QScatterPointBufferHelper3D.h"

namespace QtStackedBar3DVis
{

	QScatterSeriesRenderCache3D::QScatterSeriesRenderCache3D(QAbstractSeries3D *series,
		QAbstractRenderer3D *renderer)
		: QSeriesRenderCache3D(series, renderer),
		m_itemSize(0.0f),
		m_selectionIndexOffset(0),
		m_staticBufferDirty(false),
		m_oldRenderArraySize(0),
		m_oldMeshFileName(QString()),
		m_scatterBufferObj(0),
		m_scatterBufferPoints(0),
		m_visibilityChanged(false)
	{
	}

	QScatterSeriesRenderCache3D::~QScatterSeriesRenderCache3D()
	{
		delete m_scatterBufferObj;
		delete m_scatterBufferPoints;
	}

	void QScatterSeriesRenderCache3D::cleanup(QTextureHelper3D *texHelper)
	{
		m_renderArray.clear();

		QSeriesRenderCache3D::cleanup(texHelper);
	}

}
