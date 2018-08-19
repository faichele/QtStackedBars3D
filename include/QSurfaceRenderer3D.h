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

#ifndef SURFACE3DRENDERER_P_H
#define SURFACE3DRENDERER_P_H

#include "QVisualizationGlobals3D.h"
#include "QSurfaceController3D.h"
#include "QAbstractRenderer3D.h"
#include "QSurfaceSeriesRenderCache3D.h"

namespace QtStackedBar3DVis
{

	class QShaderHelper3D;
	class QScene3D;

	class QT_STACKEDBAR3D_EXPORT QSurfaceRenderer3D : public QAbstractRenderer3D
	{
		Q_OBJECT

	private:
		bool m_cachedIsSlicingActivated;

		// Internal attributes purely related to how the scene is drawn with GL.
		QShaderHelper3D *m_depthShader;
		QShaderHelper3D *m_backgroundShader;
		QShaderHelper3D *m_surfaceFlatShader;
		QShaderHelper3D *m_surfaceSmoothShader;
		QShaderHelper3D *m_surfaceTexturedSmoothShader;
		QShaderHelper3D *m_surfaceTexturedFlatShader;
		QShaderHelper3D *m_surfaceGridShader;
		QShaderHelper3D *m_surfaceSliceFlatShader;
		QShaderHelper3D *m_surfaceSliceSmoothShader;
		QShaderHelper3D *m_selectionShader;
		float m_heightNormalizer;
		float m_scaleX;
		float m_scaleY;
		float m_scaleZ;
		GLuint m_depthFrameBuffer;
		GLuint m_selectionFrameBuffer;
		GLuint m_selectionDepthBuffer;
		GLuint m_selectionResultTexture;
		GLfloat m_shadowQualityToShader;
		bool m_flatSupported;
		bool m_selectionActive;
		QAbstractRenderItem3D m_dummyRenderItem;
		GLint m_shadowQualityMultiplier;
		QPoint m_selectedPoint;
		QSurfaceSeries3D *m_selectedSeries;
		QPoint m_clickedPosition;
		bool m_selectionTexturesDirty;
		GLuint m_noShadowTexture;
		bool m_flipHorizontalGrid;

	public:
		explicit QSurfaceRenderer3D(QSurfaceController3D *controller);
		~QSurfaceRenderer3D();

		void updateData();
		void updateSeries(const QList<QAbstractSeries3D *> &seriesList);
		void updateSurfaceTextures(QVector<QSurfaceSeries3D *> seriesList);
		QSeriesRenderCache3D *createNewCache(QAbstractSeries3D *series);
		void cleanCache(QSeriesRenderCache3D *cache);
		void updateSelectionMode(QAbstractGraph3D::SelectionFlags mode);
		void updateRows(const QVector<QSurfaceController3D::ChangeRow> &rows);
		void updateItems(const QVector<QSurfaceController3D::ChangeItem> &points);
		void updateScene(QScene3D *scene);
		void updateSlicingActive(bool isSlicing);
		void updateSelectedPoint(const QPoint &position, QSurfaceSeries3D *series);
		void updateFlipHorizontalGrid(bool flip);
		inline QPoint clickedPosition() const { return m_clickedPosition; }
		void resetClickedStatus();
		QVector3D convertPositionToTranslation(const QVector3D &position, bool isAbsolute);
		void updateAxisLabels(QAbstractAxis3D::AxisOrientation orientation,
			const QStringList &labels);
		void updateAxisTitleVisibility(QAbstractAxis3D::AxisOrientation orientation,
			bool visible);
		void updateMargin(float margin);

		void render(GLuint defaultFboHandle = 0);

	protected:
		void initializeOpenGL();
		virtual void fixCameraTarget(QVector3D &target);
		virtual void getVisibleItemBounds(QVector3D &minBounds, QVector3D &maxBounds);

	Q_SIGNALS:
		void flatShadingSupportedChanged(bool supported);

	private:
		void checkFlatSupport(QSurfaceSeriesRenderCache3D *cache);
		void updateObjects(QSurfaceSeriesRenderCache3D *cache, bool dimensionChanged);
		void updateSliceDataModel(const QPoint &point);
		QPoint mapCoordsToSampleSpace(QSurfaceSeriesRenderCache3D *cache, const QPointF &coords);
		void findMatchingRow(float z, int &sample, int direction, QSurfaceDataArray3D &dataArray);
		void findMatchingColumn(float x, int &sample, int direction, QSurfaceDataArray3D &dataArray);
		void updateSliceObject(QSurfaceSeriesRenderCache3D *cache, const QPoint &point);
		void updateShadowQuality(QAbstractGraph3D::ShadowQuality quality);
		void updateTextures();
		void initShaders(const QString &vertexShader, const QString &fragmentShader);
		QRect calculateSampleRect(const QSurfaceDataArray3D &array);
		void loadBackgroundMesh();

		void drawSlicedScene();
		void drawScene(GLuint defaultFboHandle);
		void drawLabels(bool drawSelection, const QCamera3D *activeCamera,
			const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix);

		void calculateSceneScalingFactors();
		void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
		void initSelectionShaders();
		void initSurfaceShaders();
		void initSelectionBuffer();
		void initDepthShader();
		void updateSelectionTextures();
		void createSelectionTexture(QSurfaceSeriesRenderCache3D *cache, uint &lastSelectionId);
		void idToRGBA(uint id, uchar *r, uchar *g, uchar *b, uchar *a);
		void fillIdCorner(uchar *p, uchar r, uchar g, uchar b, uchar a);
		void surfacePointSelected(const QPoint &point);
		void updateSelectionPoint(QSurfaceSeriesRenderCache3D *cache, const QPoint &point, bool label);
		QPoint selectionIdToSurfacePoint(uint id);
		void updateDepthBuffer();
		void emitSelectedPointChanged(QPoint position);

		Q_DISABLE_COPY(QSurfaceRenderer3D)

			friend class QSurfaceObject3D;
	};

}

#endif
