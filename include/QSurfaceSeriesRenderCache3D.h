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

#ifndef SURFACESERIESRENDERCACHE_P_H
#define SURFACESERIESRENDERCACHE_P_H

#include "QVisualizationGlobals3D.h"
#include "QSeriesRenderCache3D.h"
#include "QSurfaceSeries3D.h"
#include "QSurfaceObject3D.h"
#include "QSelectionPointer3D.h"

#include <QtGui/QMatrix4x4>

namespace QtStackedBar3DVis
{
	class QSurfaceRenderer3D;

	class QT_STACKEDBAR3D_EXPORT QSurfaceSeriesRenderCache3D : public QSeriesRenderCache3D
	{
	public:
		QSurfaceSeriesRenderCache3D(QAbstractSeries3D *series, QSurfaceRenderer3D *renderer);
		virtual ~QSurfaceSeriesRenderCache3D();

		virtual void populate(bool newSeries);
		virtual void cleanup(QTextureHelper3D *texHelper);

		inline bool surfaceVisible() const { return m_surfaceVisible; }
		inline bool surfaceGridVisible() const { return m_surfaceGridVisible; }
		inline bool isFlatShadingEnabled() const { return m_surfaceFlatShading; }
		inline void setFlatShadingEnabled(bool enabled) { m_surfaceFlatShading = enabled; }
		inline void setFlatChangeAllowed(bool allowed) { m_flatChangeAllowed = allowed; }
		inline QSurfaceObject3D *surfaceObject() { return m_surfaceObj; }
		inline QSurfaceObject3D *sliceSurfaceObject() { return m_sliceSurfaceObj; }
		inline const QRect &sampleSpace() const { return m_sampleSpace; }
		inline void setSampleSpace(const QRect &sampleSpace) { m_sampleSpace = sampleSpace; }
		inline QSurfaceSeries3D *series() const { return static_cast<QSurfaceSeries3D *>(m_series); }
		inline QSurfaceDataArray3D &dataArray() { return m_dataArray; }
		inline QSurfaceDataArray3D &sliceDataArray() { return m_sliceDataArray; }
		inline bool renderable() const {
			return m_visible && (m_surfaceVisible ||
				m_surfaceGridVisible);
		}
		inline void setSelectionTexture(GLuint texture) { m_selectionTexture = texture; }
		inline GLuint selectionTexture() const { return m_selectionTexture; }
		inline void setSelectionIdRange(uint start, uint end) {
			m_selectionIdStart = start;
			m_selectionIdEnd = end;
		}
		inline uint selectionIdStart() const { return m_selectionIdStart; }
		inline bool isWithinIdRange(uint selection) const {
			return selection >= m_selectionIdStart &&
				selection <= m_selectionIdEnd;
		}
		inline bool isFlatStatusDirty() const { return m_flatStatusDirty; }
		inline void setFlatStatusDirty(bool status) { m_flatStatusDirty = status; }
		inline void setMVPMatrix(const QMatrix4x4 &matrix) { m_MVPMatrix = matrix; }
		inline const QMatrix4x4 &MVPMatrix() { return m_MVPMatrix; }

		inline void setSliceSelectionPointer(QSelectionPointer3D *pointer) { m_sliceSelectionPointer = pointer; }
		inline QSelectionPointer3D *sliceSelectionPointer() const { return m_sliceSelectionPointer; }
		inline void setMainSelectionPointer(QSelectionPointer3D *pointer) { m_mainSelectionPointer = pointer; }
		inline QSelectionPointer3D *mainSelectionPointer() const { return m_mainSelectionPointer; }

		inline void setSlicePointerActivity(bool activity) { m_slicePointerActive = activity; }
		inline bool slicePointerActive() const { return m_slicePointerActive; }
		inline void setMainPointerActivity(bool activity) { m_mainPointerActive = activity; }
		inline bool mainPointerActive() const { return m_mainPointerActive; }
		inline void setSurfaceTexture(GLuint texture) { m_surfaceTexture = texture; }
		inline GLuint surfaceTexture() const { return m_surfaceTexture; }

	protected:
		bool m_surfaceVisible;
		bool m_surfaceGridVisible;
		bool m_surfaceFlatShading;
		QSurfaceObject3D *m_surfaceObj;
		QSurfaceObject3D *m_sliceSurfaceObj;
		QRect m_sampleSpace;
		QSurfaceDataArray3D m_dataArray;
		QSurfaceDataArray3D m_sliceDataArray;
		GLuint m_selectionTexture;
		uint m_selectionIdStart;
		uint m_selectionIdEnd;
		bool m_flatChangeAllowed;
		bool m_flatStatusDirty;
		QMatrix4x4 m_MVPMatrix;
		QSelectionPointer3D *m_sliceSelectionPointer;
		QSelectionPointer3D *m_mainSelectionPointer;
		bool m_slicePointerActive;
		bool m_mainPointerActive;
		GLuint m_surfaceTexture;
	};
}

#endif
