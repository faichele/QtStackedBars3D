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

#include "QSurfaceSeriesRenderCache3D.h"
#include "QSurfaceRenderer3D.h"
#include "QTextureHelper3D.h"

namespace QtStackedBar3DVis
{

	QSurfaceSeriesRenderCache3D::QSurfaceSeriesRenderCache3D(QAbstractSeries3D *series,
		QSurfaceRenderer3D *renderer)
		: QSeriesRenderCache3D(series, renderer),
		m_surfaceVisible(false),
		m_surfaceGridVisible(false),
		m_surfaceFlatShading(false),
		m_surfaceObj(new QSurfaceObject3D(renderer)),
		m_sliceSurfaceObj(new QSurfaceObject3D(renderer)),
		m_sampleSpace(QRect(0, 0, 0, 0)),
		m_selectionTexture(0),
		m_selectionIdStart(0),
		m_selectionIdEnd(0),
		m_flatChangeAllowed(true),
		m_flatStatusDirty(true),
		m_sliceSelectionPointer(0),
		m_mainSelectionPointer(0),
		m_slicePointerActive(false),
		m_mainPointerActive(false),
		m_surfaceTexture(0)
	{
	}

	QSurfaceSeriesRenderCache3D::~QSurfaceSeriesRenderCache3D()
	{
	}

	void QSurfaceSeriesRenderCache3D::populate(bool newSeries)
	{
		QSeriesRenderCache3D::populate(newSeries);

		QSurfaceSeries3D::DrawFlags drawMode = series()->drawMode();
		m_surfaceVisible = drawMode.testFlag(QSurfaceSeries3D::DrawSurface);
		m_surfaceGridVisible = drawMode.testFlag(QSurfaceSeries3D::DrawWireframe);
		if (m_flatChangeAllowed && m_surfaceFlatShading != series()->isFlatShadingEnabled()) {
			m_surfaceFlatShading = series()->isFlatShadingEnabled();
			m_flatStatusDirty = true;
		}
	}

	void QSurfaceSeriesRenderCache3D::cleanup(QTextureHelper3D *texHelper)
	{
		if (QOpenGLContext::currentContext()) {
			texHelper->deleteTexture(&m_selectionTexture);
			texHelper->deleteTexture(&m_surfaceTexture);
		}

		delete m_surfaceObj;
		delete m_sliceSurfaceObj;
		for (int i = 0; i < m_dataArray.size(); i++)
			delete m_dataArray.at(i);
		m_dataArray.clear();

		for (int i = 0; i < m_sliceDataArray.size(); i++)
			delete m_sliceDataArray.at(i);
		m_sliceDataArray.clear();

		delete m_sliceSelectionPointer;
		delete m_mainSelectionPointer;

		QSeriesRenderCache3D::cleanup(texHelper);
	}

}
