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

#ifndef Q3DSCATTERRENDERER_P_H
#define Q3DSCATTERRENDERER_P_H

#include "QVisualizationGlobals3D.h"
#include "QScatterController3D.h"
#include "QAbstractRenderer3D.h"
#include "QScatterRenderItem3D.h"

QT_FORWARD_DECLARE_CLASS(QSizeF)

namespace QtStackedBar3DVis
{

	class QShaderHelper3D;
	class QScene3D;
	class QScatterSeriesRenderCache3D;
	class QScatterDataItem3D;

	class QT_STACKEDBAR3D_EXPORT QScatterRenderer3D : public QAbstractRenderer3D
	{
		Q_OBJECT

	private:
		// Internal state
		QScatterRenderItem3D *m_selectedItem; // points to renderitem array
		bool m_updateLabels;
		QShaderHelper3D *m_dotShader;
		QShaderHelper3D *m_dotGradientShader;
		QShaderHelper3D *m_staticSelectedItemGradientShader;
		QShaderHelper3D *m_staticSelectedItemShader;
		QShaderHelper3D *m_pointShader;
		QShaderHelper3D *m_depthShader;
		QShaderHelper3D *m_selectionShader;
		QShaderHelper3D *m_backgroundShader;
		QShaderHelper3D *m_staticGradientPointShader;
		GLuint m_bgrTexture;
		GLuint m_selectionTexture;
		GLuint m_depthFrameBuffer;
		GLuint m_selectionFrameBuffer;
		GLuint m_selectionDepthBuffer;
		GLfloat m_shadowQualityToShader;
		GLint m_shadowQualityMultiplier;
		float m_scaleX;
		float m_scaleY;
		float m_scaleZ;
		int m_selectedItemIndex;
		QScatterSeriesRenderCache3D *m_selectedSeriesCache;
		QScatterSeriesRenderCache3D *m_oldSelectedSeriesCache;
		GLfloat m_dotSizeScale;
		QScatterRenderItem3D m_dummyRenderItem;
		GLfloat m_maxItemSize;
		int m_clickedIndex;
		bool m_havePointSeries;
		bool m_haveMeshSeries;
		bool m_haveUniformColorMeshSeries;
		bool m_haveGradientMeshSeries;

	public:
		explicit QScatterRenderer3D(QScatterController3D *controller);
		~QScatterRenderer3D();

		void updateData();
		void updateSeries(const QList<QAbstractSeries3D *> &seriesList);
		QSeriesRenderCache3D *createNewCache(QAbstractSeries3D *series);
		void updateItems(const QVector<QScatterController3D::ChangeItem> &items);
		void updateScene(QScene3D *scene);
		void updateAxisLabels(QAbstractAxis3D::AxisOrientation orientation,
			const QStringList &labels);
		void updateAxisTitleVisibility(QAbstractAxis3D::AxisOrientation orientation,
			bool visible);
		void updateOptimizationHint(QAbstractGraph3D::OptimizationHints hint);
		void updateMargin(float margin);

		QVector3D convertPositionToTranslation(const QVector3D &position, bool isAbsolute);

		inline int clickedIndex() const { return m_clickedIndex; }
		void resetClickedStatus();

		void render(GLuint defaultFboHandle);

		public Q_SLOTS:
		void updateSelectedItem(int index, QScatterSeries3D *series);

	protected:
		virtual void initializeOpenGL();
		virtual void fixCameraTarget(QVector3D &target);
		virtual void getVisibleItemBounds(QVector3D &minBounds, QVector3D &maxBounds);

	private:
		virtual void initShaders(const QString &vertexShader, const QString &fragmentShader);
		virtual void initGradientShaders(const QString &vertexShader, const QString &fragmentShader);
		virtual void initStaticSelectedItemShaders(const QString &vertexShader,
			const QString &fragmentShader,
			const QString &gradientVertexShader,
			const QString &gradientFragmentShader);
		virtual void updateShadowQuality(QAbstractGraph3D::ShadowQuality quality);
		virtual void updateTextures();
		virtual void fixMeshFileName(QString &fileName, QAbstractSeries3D::Mesh mesh);

		void drawScene(GLuint defaultFboHandle);
		void drawLabels(bool drawSelection, const QCamera3D *activeCamera,
			const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix);

		void loadBackgroundMesh();
		void initSelectionShader();
		void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
		void initStaticPointShaders(const QString &vertexShader, const QString &fragmentShader);
		void initSelectionBuffer();
		void initDepthShader();
		void updateDepthBuffer();
		void initPointShader();
		void calculateTranslation(QScatterRenderItem3D &item);
		void calculateSceneScalingFactors();

		void selectionColorToSeriesAndIndex(const QVector4D &color, int &index,
			QAbstractSeries3D *&series);
		inline void updateRenderItem(const QScatterDataItem3D &dataItem, QScatterRenderItem3D &renderItem);

		Q_DISABLE_COPY(QScatterRenderer3D)
	};

}

#endif
