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

#ifndef Q_SURFACE_OBJECT_3D_H
#define Q_SURFACE_OBJECT_3D_H

#include "QVisualizationGlobals3D.h"
#include "QAbstractObjectHelper3D.h"
#include "QSurfaceDataProxy3D.h"

#include <QtCore/QRect>

namespace QtStackedBar3DVis
{
	class QSurfaceRenderer3D;
	class QAxisRenderCache3D;

	class QT_STACKEDBAR3D_EXPORT QSurfaceObject3D : public QAbstractObjectHelper3D
	{
	public:
		enum SurfaceType {
			SurfaceSmooth,
			SurfaceFlat,
			Undefined
		};

		enum DataDimension {
			BothAscending = 0,
			XDescending = 1,
			ZDescending = 2,
			BothDescending = XDescending | ZDescending
		};
		Q_DECLARE_FLAGS(DataDimensions, DataDimension)

	public:
		QSurfaceObject3D(QSurfaceRenderer3D *renderer);
		virtual ~QSurfaceObject3D();

		void setUpData(const QSurfaceDataArray3D &dataArray, const QRect &space,
			bool changeGeometry, bool polar, bool flipXZ = false);
		void setUpSmoothData(const QSurfaceDataArray3D &dataArray, const QRect &space,
			bool changeGeometry, bool polar, bool flipXZ = false);
		void smoothUVs(const QSurfaceDataArray3D &dataArray, const QSurfaceDataArray3D &modelArray);
		void coarseUVs(const QSurfaceDataArray3D &dataArray, const QSurfaceDataArray3D &modelArray);
		void updateCoarseRow(const QSurfaceDataArray3D &dataArray, int rowIndex, bool polar);
		void updateSmoothRow(const QSurfaceDataArray3D &dataArray, int startRow, bool polar);
		void updateSmoothItem(const QSurfaceDataArray3D &dataArray, int row, int column, bool polar);
		void updateCoarseItem(const QSurfaceDataArray3D &dataArray, int row, int column, bool polar);
		void createSmoothIndices(int x, int y, int endX, int endY);
		void createCoarseSubSection(int x, int y, int columns, int rows);
		void createSmoothGridlineIndices(int x, int y, int endX, int endY);
		void createCoarseGridlineIndices(int x, int y, int endX, int endY);
		void uploadBuffers();
		GLuint gridElementBuf();
		GLuint uvBuf();
		GLuint gridIndexCount();
		QVector3D vertexAt(int column, int row);
		void clear();
		float minYValue() const { return m_minY; }
		float maxYValue() const { return m_maxY; }
		inline void activateSurfaceTexture(bool value) { m_returnTextureBuffer = value; }

	private:
		void createCoarseIndices(GLint *indices, int &p, int row, int upperRow, int j);
		void createNormals(int &p, int row, int upperRow, int j);
		void createSmoothNormalBodyLine(int &totalIndex, int column);
		void createSmoothNormalUpperLine(int &totalIndex);
		QVector3D createSmoothNormalBodyLineItem(int x, int y);
		QVector3D createSmoothNormalUpperLineItem(int x, int y);
		QVector3D normal(const QVector3D &a, const QVector3D &b, const QVector3D &c);
		void createBuffers(const QVector<QVector3D> &vertices, const QVector<QVector2D> &uvs,
			const QVector<QVector3D> &normals, const GLint *indices);
		void checkDirections(const QSurfaceDataArray3D &array);
		inline void getNormalizedVertex(const QSurfaceDataItem3D &data, QVector3D &vertex, bool polar,
			bool flipXZ);

	private:
		SurfaceType m_surfaceType;
		int m_columns;
		int m_rows;
		GLuint m_gridElementbuffer;
		GLuint m_gridIndexCount;
		QVector<QVector3D> m_vertices;
		QVector<QVector3D> m_normals;
		// Caches are not owned
		QAxisRenderCache3D &m_axisCacheX;
		QAxisRenderCache3D &m_axisCacheY;
		QAxisRenderCache3D &m_axisCacheZ;
		QSurfaceRenderer3D *m_renderer;
		float m_minY;
		float m_maxY;
		GLuint m_uvTextureBuffer;
		bool m_returnTextureBuffer;
		QSurfaceObject3D::DataDimensions m_dataDimension;
		QSurfaceObject3D::DataDimensions m_oldDataDimension;
	};
}

#endif