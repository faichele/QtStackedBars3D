#ifndef QABSTRACT3DRENDERER_P_H
#define QABSTRACT3DRENDERER_P_H

#include <QtGui/QOpenGLFunctions>
#if !defined(QT_OPENGL_ES_2)
#  include <QtGui/QOpenGLFunctions_2_1>
#endif
#include "QVisualizationGlobals3D.h"
#include "QAbstractController3D.h"
#include "QAxisRenderCache3D.h"
#include "QSeriesRenderCache3D.h"
#include "QCustomRenderItem3D.h"

QT_FORWARD_DECLARE_CLASS(QOffscreenSurface)

namespace QtStackedBar3DVis
{

	class QTextureHelper3D;
	class Theme;
	class QDrawer3D;

	class QT_STACKEDBAR3D_EXPORT QAbstractRenderer3D : public QObject, protected QOpenGLFunctions
	{
		Q_OBJECT

	protected:
		enum SelectionState {
			SelectNone = 0,
			SelectOnScene,
			SelectOnOverview,
			SelectOnSlice
		};

		enum RenderingState {
			RenderingNormal = 0,
			RenderingSelection,
			RenderingDepth
		};

	public:
		virtual ~QAbstractRenderer3D();

		virtual void updateData() = 0;
		virtual void updateSeries(const QList<QAbstractSeries3D *> &seriesList);
		virtual void updateCustomData(const QList<QCustomItem3D *> &customItems);
		virtual void updateCustomItems();
		virtual void updateCustomItemPositions();
		virtual QSeriesRenderCache3D *createNewCache(QAbstractSeries3D *series);
		virtual void cleanCache(QSeriesRenderCache3D *cache);
		virtual void render(GLuint defaultFboHandle);

		virtual void updateTheme(QTheme3D *theme);
		virtual void updateSelectionMode(QAbstractGraph3D::SelectionFlags newMode);
		virtual void updateOptimizationHint(QAbstractGraph3D::OptimizationHints hint);
		virtual void updateScene(QScene3D *scene);
		virtual void updateTextures();
		virtual void initSelectionBuffer() = 0;
		virtual void updateSelectionState(SelectionState state);

		virtual void updateDepthBuffer() = 0;
		virtual void updateShadowQuality(QAbstractGraph3D::ShadowQuality quality) = 0;
		virtual void initShaders(const QString &vertexShader, const QString &fragmentShader) = 0;
		virtual void initGradientShaders(const QString &vertexShader, const QString &fragmentShader);
		virtual void initStaticSelectedItemShaders(const QString &vertexShader,
			const QString &fragmentShader,
			const QString &gradientVertexShader,
			const QString &gradientFragmentShader);
		virtual void initBackgroundShaders(const QString &vertexShader,
			const QString &fragmentShader) = 0;
		virtual void initCustomItemShaders(const QString &vertexShader,
			const QString &fragmentShader);
		virtual void initVolumeTextureShaders(const QString &vertexShader,
			const QString &fragmentShader,
			const QString &fragmentLowDefShader,
			const QString &sliceShader,
			const QString &sliceFrameVertexShader,
			const QString &sliceFrameShader);
		virtual void initLabelShaders(const QString &vertexShader, const QString &fragmentShader);
		virtual void initCursorPositionShaders(const QString &vertexShader,
			const QString &fragmentShader);
		virtual void initCursorPositionBuffer();

		virtual void updateAxisType(QAbstractAxis3D::AxisOrientation orientation,
			QAbstractAxis3D::AxisType type);
		virtual void updateAxisTitle(QAbstractAxis3D::AxisOrientation orientation,
			const QString &title);
		virtual void updateAxisLabels(QAbstractAxis3D::AxisOrientation orientation,
			const QStringList &labels);
		virtual void updateAxisRange(QAbstractAxis3D::AxisOrientation orientation, float min,
			float max);
		virtual void updateAxisSegmentCount(QAbstractAxis3D::AxisOrientation orientation, int count);
		virtual void updateAxisSubSegmentCount(QAbstractAxis3D::AxisOrientation orientation,
			int count);
		virtual void updateAxisLabelFormat(QAbstractAxis3D::AxisOrientation orientation,
			const QString &format);
		virtual void updateAxisReversed(QAbstractAxis3D::AxisOrientation orientation,
			bool enable);
		virtual void updateAxisFormatter(QAbstractAxis3D::AxisOrientation orientation,
			QAxisValueFormatter3D *formatter);
		virtual void updateAxisLabelAutoRotation(QAbstractAxis3D::AxisOrientation orientation,
			float angle);
		virtual void updateAxisTitleVisibility(QAbstractAxis3D::AxisOrientation orientation,
			bool visible);
		virtual void updateAxisTitleFixed(QAbstractAxis3D::AxisOrientation orientation,
			bool fixed);
		virtual void modifiedSeriesList(const QVector<QAbstractSeries3D *> &seriesList);

		virtual void fixMeshFileName(QString &fileName, QAbstractSeries3D::Mesh mesh);

		virtual QCustomRenderItem3D *addCustomItem(QCustomItem3D *item);
		virtual void updateCustomItem(QCustomRenderItem3D *renderItem);

		virtual void updateAspectRatio(float ratio);
		virtual void updateHorizontalAspectRatio(float ratio);
		virtual void updatePolar(bool enable);
		virtual void updateRadialLabelOffset(float offset);
		virtual void updateMargin(float margin);

		virtual QVector3D convertPositionToTranslation(const QVector3D &position,
			bool isAbsolute) = 0;

		void generateBaseColorTexture(const QColor &color, GLuint *texture);
		void fixGradientAndGenerateTexture(QLinearGradient *gradient, GLuint *gradientTexture);

		inline bool isClickQueryResolved() const { return m_clickResolved; }
		inline void clearClickQueryResolved() { m_clickResolved = false; }
		inline QPoint cachedClickQuery() const { return m_cachedScene->selectionQueryPosition(); }
		inline QAbstractSeries3D *clickedSeries() const { return m_clickedSeries; }
		inline QAbstractGraph3D::ElementType clickedType() { return m_clickedType; }
		inline bool isGraphPositionQueryResolved() const { return m_graphPositionQueryResolved; }
		inline void clearGraphPositionQueryResolved() { m_graphPositionQueryResolved = false; }
		inline QVector3D queriedGraphPosition() const { return m_queriedGraphPosition; }
		inline QPoint cachedGraphPositionQuery() const { return m_cachedScene->graphPositionQuery(); }

		QLabelItem3D &selectionLabelItem();
		void setSelectionLabel(const QString &label);
		QString &selectionLabel();

		void drawCustomItems(RenderingState state, QShaderHelper3D *regularShader,
			const QMatrix4x4 &viewMatrix,
			const QMatrix4x4 &projectionViewMatrix,
			const QMatrix4x4 &depthProjectionViewMatrix,
			GLuint depthTexture, GLfloat shadowQuality, GLfloat reflection = 1.0f);

		QVector4D indexToSelectionColor(GLint index);
		void calculatePolarXZ(const QVector3D &dataPos, float &x, float &z) const;

	Q_SIGNALS:
		void needRender(); // Emit this if something in renderer causes need for another render pass.
		void requestShadowQuality(QAbstractGraph3D::ShadowQuality quality); // For automatic quality adjustments

	protected:
		QAbstractRenderer3D(QAbstractController3D *controller);

		virtual void initializeOpenGL();

		void reInitShaders();
		virtual void handleShadowQualityChange();
		virtual void handleResize();

		QAxisRenderCache3D &axisCacheForOrientation(QAbstractAxis3D::AxisOrientation orientation);

		virtual void lowerShadowQuality();

		void fixGradient(QLinearGradient *gradient, GLuint *gradientTexture);

		void calculateZoomLevel();
		void drawAxisTitleY(const QVector3D &sideLabelRotation, const QVector3D &backLabelRotation,
			const QVector3D &sideLabelTrans, const QVector3D &backLabelTrans,
			const QQuaternion &totalSideRotation, const QQuaternion &totalBackRotation,
			QAbstractRenderItem3D &dummyItem, const QCamera3D *activeCamera,
			float labelsMaxWidth,
			const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix,
			QShaderHelper3D *shader);
		void drawAxisTitleX(const QVector3D &labelRotation, const QVector3D &labelTrans,
			const QQuaternion &totalRotation, QAbstractRenderItem3D &dummyItem,
			const QCamera3D *activeCamera, float labelsMaxWidth,
			const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix,
			QShaderHelper3D *shader, bool radial = false);
		void drawAxisTitleZ(const QVector3D &labelRotation, const QVector3D &labelTrans,
			const QQuaternion &totalRotation, QAbstractRenderItem3D &dummyItem,
			const QCamera3D *activeCamera, float labelsMaxWidth,
			const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix,
			QShaderHelper3D *shader);

		void loadGridLineMesh();
		void loadLabelMesh();
		void loadPositionMapperMesh();

		void drawRadialGrid(QShaderHelper3D *shader, float yFloorLinePos,
			const QMatrix4x4 &projectionViewMatrix, const QMatrix4x4 &depthMatrix);
		void drawAngularGrid(QShaderHelper3D *shader, float yFloorLinePos,
			const QMatrix4x4 &projectionViewMatrix, const QMatrix4x4 &depthMatrix);

		float calculatePolarBackgroundMargin();
		virtual void fixCameraTarget(QVector3D &target) = 0;
		void updateCameraViewport();

		void recalculateCustomItemScalingAndPos(QCustomRenderItem3D *item);
		virtual void getVisibleItemBounds(QVector3D &minBounds, QVector3D &maxBounds) = 0;
		void drawVolumeSliceFrame(const QCustomRenderItem3D *item, Qt::Axis axis,
			const QMatrix4x4 &projectionViewMatrix);
		void queriedGraphPosition(const QMatrix4x4 &projectionViewMatrix, const QVector3D &scaling,
			GLuint defaultFboHandle);

		void fixContextBeforeDelete();
		void restoreContextAfterDelete();

		bool m_hasNegativeValues;
		QTheme3D *m_cachedTheme;
		QDrawer3D *m_drawer;
		QRect m_viewport;
		QAbstractGraph3D::ShadowQuality m_cachedShadowQuality;
		GLfloat m_autoScaleAdjustment;

		QAbstractGraph3D::SelectionFlags m_cachedSelectionMode;
		QAbstractGraph3D::OptimizationHints m_cachedOptimizationHint;

		QAxisRenderCache3D m_axisCacheX;
		QAxisRenderCache3D m_axisCacheY;
		QAxisRenderCache3D m_axisCacheZ;
		QTextureHelper3D *m_textureHelper;
		GLuint m_depthTexture;

		QScene3D *m_cachedScene;
		bool m_selectionDirty;
		SelectionState m_selectionState;
		QPoint m_inputPosition;
		QHash<QAbstractSeries3D *, QSeriesRenderCache3D *> m_renderCacheList;
		CustomRenderItemArray m_customRenderCache;
		QRect m_primarySubViewport;
		QRect m_secondarySubViewport;
		float m_devicePixelRatio;
		bool m_selectionLabelDirty;
		bool m_clickResolved;
		bool m_graphPositionQueryPending;
		bool m_graphPositionQueryResolved;
		QAbstractSeries3D *m_clickedSeries;
		QAbstractGraph3D::ElementType m_clickedType;
		int m_selectedLabelIndex;
		int m_selectedCustomItemIndex;
		QVector3D m_queriedGraphPosition;
		QPoint m_graphPositionQuery;

		QString m_selectionLabel;
		QLabelItem3D *m_selectionLabelItem;
		int m_visibleSeriesCount;

		QShaderHelper3D *m_customItemShader;
		QShaderHelper3D *m_volumeTextureShader;
		QShaderHelper3D *m_volumeTextureLowDefShader;
		QShaderHelper3D *m_volumeTextureSliceShader;
		QShaderHelper3D *m_volumeSliceFrameShader;
		QShaderHelper3D *m_labelShader;
		QShaderHelper3D *m_cursorPositionShader;
		GLuint m_cursorPositionFrameBuffer;
		GLuint m_cursorPositionTexture;

		bool m_useOrthoProjection;
		bool m_xFlipped;
		bool m_yFlipped;
		bool m_zFlipped;
		bool m_yFlippedForGrid;

		QObjectHelper3D *m_backgroundObj; // Shared reference
		QObjectHelper3D *m_gridLineObj; // Shared reference
		QObjectHelper3D *m_labelObj; // Shared reference
		QObjectHelper3D *m_positionMapperObj; // Shared reference

		float m_graphAspectRatio;
		float m_graphHorizontalAspectRatio;
		bool m_polarGraph;
		float m_radialLabelOffset;
		float m_polarRadius;

		QQuaternion m_xRightAngleRotation;
		QQuaternion m_yRightAngleRotation;
		QQuaternion m_zRightAngleRotation;
		QQuaternion m_xRightAngleRotationNeg;
		QQuaternion m_yRightAngleRotationNeg;
		QQuaternion m_zRightAngleRotationNeg;
		QQuaternion m_xFlipRotation;
		QQuaternion m_zFlipRotation;

		float m_requestedMargin;
		float m_vBackgroundMargin;
		float m_hBackgroundMargin;
		float m_scaleXWithBackground;
		float m_scaleYWithBackground;
		float m_scaleZWithBackground;

		QVector3D m_oldCameraTarget;

		bool m_reflectionEnabled;
		qreal m_reflectivity;

		QLocale m_locale;
#if !defined(QT_OPENGL_ES_2)
		QOpenGLFunctions_2_1 *m_funcs_2_1;  // Not owned
#endif
		QPointer<QOpenGLContext> m_context; // Not owned
		QOffscreenSurface *m_dummySurfaceAtDelete;
		bool m_isOpenGLES;

	private:
		friend class QAbstractController3D;
	};

}
#endif