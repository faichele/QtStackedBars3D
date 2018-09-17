#ifndef Q_STACKED_BAR_RENDERER_3D_H
#define Q_STACKED_BAR_RENDERER_3D_H

#include "QVisualizationGlobals3D.h"
#include "QStackedBarsController3D.h"
#include "QAbstractRenderer3D.h"
#include "QStackedBarRenderItem3D.h"
#include "QStackedBarDataProxy3D.h"

#include <QtCore/QPoint>
#include <QtCore/QSize>

namespace QtStackedBar3DVis
{

	class QShaderHelper3D;
	class QLabelItem3D;
	class QScene3D;
	class QStackedBarSeriesRenderCache3D;

	class QT_STACKEDBAR3D_EXPORT QStackedBarsRenderer3D : public QAbstractRenderer3D
	{
		Q_OBJECT

	private:
		// Cached state based on emitted signals from the controller
		QSizeF m_cachedBarThickness;
		QSizeF m_cachedBarSpacing;
		bool m_cachedIsSlicingActivated;
		int m_cachedRowCount;
		int m_cachedColumnCount;

		// Internal state
		QStackedBarRenderItem3D *m_selectedBar; // points to renderitem array
		QAxisRenderCache3D *m_sliceCache; // not owned
		const QLabelItem3D *m_sliceTitleItem; // not owned
		bool m_updateLabels;
		QShaderHelper3D *m_barShader;
		QShaderHelper3D *m_barGradientShader;
		QShaderHelper3D *m_depthShader;
		QShaderHelper3D *m_selectionShader;
		QShaderHelper3D *m_backgroundShader;
		GLuint m_bgrTexture;
		GLuint m_selectionTexture;
		GLuint m_depthFrameBuffer;
		GLuint m_selectionFrameBuffer;
		GLuint m_selectionDepthBuffer;
		GLfloat m_shadowQualityToShader;
		GLint m_shadowQualityMultiplier;
		GLfloat m_heightNormalizer;
		GLfloat m_gradientFraction;
		GLfloat m_backgroundAdjustment;
		GLfloat m_rowWidth;
		GLfloat m_columnDepth;
		GLfloat m_maxDimension;
		GLfloat m_scaleX;
		GLfloat m_scaleZ;
		GLfloat m_scaleFactor;
		GLfloat m_maxSceneSize;
		QPoint m_visualSelectedBarPos;
		QPoint m_selectedBarPos;
		QStackedBarSeriesRenderCache3D *m_selectedSeriesCache;
		QStackedBarRenderItem3D m_dummyBarRenderItem;
		bool m_noZeroInRange;
		float m_seriesScaleX;
		float m_seriesScaleZ;
		float m_seriesStep;
		float m_seriesStart;
		QPoint m_clickedPosition;
		bool m_keepSeriesUniform;
		bool m_haveUniformColorSeries;
		bool m_haveGradientSeries;
		float m_zeroPosition;
		float m_xScaleFactor;
		float m_zScaleFactor;
		float m_floorLevel;
		float m_actualFloorLevel;

	public:
		explicit QStackedBarsRenderer3D(QStackedBarsController3D *controller);
		~QStackedBarsRenderer3D();

		void updateData();
		void updateSeries(const QList<QAbstractSeries3D *> &seriesList);
		QSeriesRenderCache3D *createNewCache(QAbstractSeries3D *series);
		void updateRows(const QVector<QStackedBarsController3D::ChangeRow> &rows);
		void updateItems(const QVector<QStackedBarsController3D::ChangeItem> &items);
		void updateScene(QScene3D *scene);
		void render(GLuint defaultFboHandle = 0);

		QVector3D convertPositionToTranslation(const QVector3D &position, bool isAbsolute);

		void updateAspectRatio(float ratio);
		void updateFloorLevel(float level);
		void updateMargin(float margin);

	protected:
		virtual void initializeOpenGL();
		virtual void fixCameraTarget(QVector3D &target);
		virtual void getVisibleItemBounds(QVector3D &minBounds, QVector3D &maxBounds);

		public Q_SLOTS:
		void updateMultiSeriesScaling(bool uniform);
		void updateBarSpecs(GLfloat thicknessRatio = 1.0f,
			const QSizeF &spacing = QSizeF(1.0, 1.0),
			bool relative = true);
		void updateSlicingActive(bool isSlicing);
		void updateSelectedBar(const QPoint &position, QStackedBarSeries3D *series);
		inline QPoint clickedPosition() const { return m_clickedPosition; }
		void resetClickedStatus();

		// Overloaded from abstract renderer
		virtual void updateAxisRange(QAbstractAxis3D::AxisOrientation orientation, float min,
			float max);
		virtual void updateAxisReversed(QAbstractAxis3D::AxisOrientation orientation,
			bool enable);

	private:
		virtual void initShaders(const QString &vertexShader, const QString &fragmentShader);
		virtual void initGradientShaders(const QString &vertexShader, const QString &fragmentShader);
		virtual void updateShadowQuality(QAbstractGraph3D::ShadowQuality quality);
		virtual void updateTextures();
		virtual void fixMeshFileName(QString &fileName, QAbstractSeries3D::Mesh mesh);

		void drawSlicedScene();
		void drawScene(GLuint defaultFboHandle);
		void drawLabels(bool drawSelection, const QCamera3D *activeCamera,
			const QMatrix4x4 &viewMatrix, const QMatrix4x4 &projectionMatrix);

		bool drawBars(QStackedBarRenderItem3D **selectedBar, const QMatrix4x4 &depthProjectionViewMatrix,
			const QMatrix4x4 &projectionViewMatrix, const QMatrix4x4 &viewMatrix,
			GLint startRow, GLint stopRow, GLint stepRow,
			GLint startBar, GLint stopBar, GLint stepBar, GLfloat reflection = 1.0f);
		void drawBackground(GLfloat backgroundRotation, const QMatrix4x4 &depthProjectionViewMatrix,
			const QMatrix4x4 &projectionViewMatrix, const QMatrix4x4 &viewMatrix,
			bool reflectingDraw = false, bool drawingSelectionBuffer = false);
		void drawGridLines(const QMatrix4x4 &depthProjectionViewMatrix,
			const QMatrix4x4 &projectionViewMatrix,
			const QMatrix4x4 &viewMatrix);

		void loadBackgroundMesh();
		void initSelectionShader();
		void initBackgroundShaders(const QString &vertexShader, const QString &fragmentShader);
		void initSelectionBuffer();
		void initDepthShader();
		void updateDepthBuffer();
		void calculateSceneScalingFactors();
		void calculateHeightAdjustment();
		QAbstractController3D::SelectionType isSelected(int row, int bar,
			const QStackedBarSeriesRenderCache3D *cache);
		QPoint selectionColorToArrayPosition(const QVector4D &selectionColor);
		QStackedBarSeries3D *selectionColorToSeries(const QVector4D &selectionColor);

		inline void updateRenderRow(const QStackedBarDataRow3D *dataRow, QStackedBarRenderItemRow3D &renderRow);
		inline void updateRenderItem(const QStackedBarDataItem3D &dataItem, QStackedBarRenderItem3D &renderItem);

		// Q_DISABLE_COPY(QBarsRenderer3D)
	};
}
#endif