#ifndef Q_ABSTRACT_3D_CONTROLLER_H
#define Q_ABSTRACT_3D_CONTROLLER_H

#include "QVisualizationGlobals3D.h"
#include "QAbstractAxis3D.h"
#include "QCategoryAxis3D.h"
#include "QValueAxis3D.h"
#include "QDrawer3D.h"

#include "QAbstractInputhandler3D.h"
#include <QAbstractGraph3D.h>

#include "QScene3DPrivate.h"

#include "QCustomItem3D.h"

#include <QtGui/QLinearGradient>
#include <QtCore/QTime>
#include <QtCore/QLocale>
#include <QtCore/QMutex>

QT_FORWARD_DECLARE_CLASS(QOpenGLFramebufferObject)

namespace QtStackedBar3DVis
{

	class QAbstractRenderer3D;
	class QAbstractSeries3D;
	class QThemeManager3D;

	struct QAbstractChangeBitField3D {
		bool themeChanged : 1;
		bool shadowQualityChanged : 1;
		bool selectionModeChanged : 1;
		bool optimizationHintChanged : 1;
		bool axisXTypeChanged : 1;
		bool axisYTypeChanged : 1;
		bool axisZTypeChanged : 1;
		bool axisXTitleChanged : 1;
		bool axisYTitleChanged : 1;
		bool axisZTitleChanged : 1;
		bool axisXLabelsChanged : 1;
		bool axisYLabelsChanged : 1;
		bool axisZLabelsChanged : 1;
		bool axisXRangeChanged : 1;
		bool axisYRangeChanged : 1;
		bool axisZRangeChanged : 1;
		bool axisXSegmentCountChanged : 1;
		bool axisYSegmentCountChanged : 1;
		bool axisZSegmentCountChanged : 1;
		bool axisXSubSegmentCountChanged : 1;
		bool axisYSubSegmentCountChanged : 1;
		bool axisZSubSegmentCountChanged : 1;
		bool axisXLabelFormatChanged : 1;
		bool axisYLabelFormatChanged : 1;
		bool axisZLabelFormatChanged : 1;
		bool axisXReversedChanged : 1;
		bool axisYReversedChanged : 1;
		bool axisZReversedChanged : 1;
		bool axisXFormatterChanged : 1;
		bool axisYFormatterChanged : 1;
		bool axisZFormatterChanged : 1;
		bool projectionChanged : 1;
		bool axisXLabelAutoRotationChanged : 1;
		bool axisYLabelAutoRotationChanged : 1;
		bool axisZLabelAutoRotationChanged : 1;
		bool aspectRatioChanged : 1;
		bool horizontalAspectRatioChanged : 1;
		bool axisXTitleVisibilityChanged : 1;
		bool axisYTitleVisibilityChanged : 1;
		bool axisZTitleVisibilityChanged : 1;
		bool axisXTitleFixedChanged : 1;
		bool axisYTitleFixedChanged : 1;
		bool axisZTitleFixedChanged : 1;
		bool polarChanged : 1;
		bool radialLabelOffsetChanged : 1;
		bool reflectionChanged : 1;
		bool reflectivityChanged : 1;
		bool marginChanged : 1;

		QAbstractChangeBitField3D() :
			themeChanged(true),
			shadowQualityChanged(true),
			selectionModeChanged(true),
			optimizationHintChanged(true),
			axisXTypeChanged(true),
			axisYTypeChanged(true),
			axisZTypeChanged(true),
			axisXTitleChanged(true),
			axisYTitleChanged(true),
			axisZTitleChanged(true),
			axisXLabelsChanged(true),
			axisYLabelsChanged(true),
			axisZLabelsChanged(true),
			axisXRangeChanged(true),
			axisYRangeChanged(true),
			axisZRangeChanged(true),
			axisXSegmentCountChanged(true),
			axisYSegmentCountChanged(true),
			axisZSegmentCountChanged(true),
			axisXSubSegmentCountChanged(true),
			axisYSubSegmentCountChanged(true),
			axisZSubSegmentCountChanged(true),
			axisXLabelFormatChanged(true),
			axisYLabelFormatChanged(true),
			axisZLabelFormatChanged(true),
			axisXReversedChanged(true),
			axisYReversedChanged(true),
			axisZReversedChanged(true),
			axisXFormatterChanged(true),
			axisYFormatterChanged(true),
			axisZFormatterChanged(true),
			projectionChanged(true),
			axisXLabelAutoRotationChanged(true),
			axisYLabelAutoRotationChanged(true),
			axisZLabelAutoRotationChanged(true),
			aspectRatioChanged(true),
			horizontalAspectRatioChanged(true),
			axisXTitleVisibilityChanged(true),
			axisYTitleVisibilityChanged(true),
			axisZTitleVisibilityChanged(true),
			axisXTitleFixedChanged(true),
			axisYTitleFixedChanged(true),
			axisZTitleFixedChanged(true),
			polarChanged(true),
			radialLabelOffsetChanged(true),
			reflectionChanged(true),
			reflectivityChanged(true),
			marginChanged(true)
		{
		}
	};

	class QAbstractController3DPrivate;

	class QT_STACKEDBAR3D_EXPORT QAbstractController3D : public QObject
	{
		Q_OBJECT

	public:
		enum SelectionType {
			SelectionNone = 0,
			SelectionItem,
			SelectionRow,
			SelectionColumn
		};

	private:
		QAbstractChangeBitField3D m_changeTracker;
		
		QAbstractGraph3D::SelectionFlags m_selectionMode;
		QAbstractGraph3D::ShadowQuality m_shadowQuality;
		bool m_useOrthoProjection;
		qreal m_aspectRatio;
		qreal m_horizontalAspectRatio;
		QAbstractGraph3D::OptimizationHints m_optimizationHints;
		bool m_reflectionEnabled;
		qreal m_reflectivity;
		QLocale m_locale;
		QVector3D m_queriedGraphPosition;

	protected:
		QScopedPointer<QAbstractController3DPrivate> d_ptr;

		QScene3D *m_scene;
		QList<QAbstractInputHandler3D *> m_inputHandlers; // List of all added input handlers
		QAbstractInputHandler3D *m_activeInputHandler;
		// Active axes
		QAbstractAxis3D *m_axisX;
		QAbstractAxis3D *m_axisY;
		QAbstractAxis3D *m_axisZ;

		QList<QAbstractAxis3D *> m_axes; // List of all added axes
		QAbstractRenderer3D *m_renderer;
		bool m_isDataDirty;
		bool m_isCustomDataDirty;
		bool m_isCustomItemDirty;
		bool m_isSeriesVisualsDirty;
		bool m_renderPending;
		bool m_isPolar;
		float m_radialLabelOffset;

		QList<QAbstractSeries3D *> m_seriesList;

		bool m_measureFps;
		QTime m_frameTimer;
		int m_numFrames;
		qreal m_currentFps;

		QVector<QAbstractSeries3D *> m_changedSeriesList;

		QList<QCustomItem3D *> m_customItems;

		QAbstractGraph3D::ElementType m_clickedType;
		int m_selectedLabelIndex;
		int m_selectedCustomItemIndex;
		qreal m_margin;

		QMutex m_renderMutex;

		explicit QAbstractController3D(QRect initialViewport, QScene3D *scene, QObject *parent = 0);

	public:
		virtual ~QAbstractController3D();

		inline bool isInitialized() { return (m_renderer != 0); }
		virtual void synchDataToRenderer();
		virtual void render(const GLuint defaultFboHandle = 0);
		virtual void initializeOpenGL() = 0;
		void setRenderer(QAbstractRenderer3D *renderer);

		virtual void addSeries(QAbstractSeries3D *series);
		virtual void insertSeries(int index, QAbstractSeries3D *series);
		virtual void removeSeries(QAbstractSeries3D *series);
		QList<QAbstractSeries3D *> seriesList();

		virtual void setAxisX(QAbstractAxis3D *axis);
		virtual QAbstractAxis3D *axisX() const;
		virtual void setAxisY(QAbstractAxis3D *axis);
		virtual QAbstractAxis3D *axisY() const;
		virtual void setAxisZ(QAbstractAxis3D *axis);
		virtual QAbstractAxis3D *axisZ() const;
		virtual void addAxis(QAbstractAxis3D *axis);
		virtual void releaseAxis(QAbstractAxis3D *axis);
		virtual QList<QAbstractAxis3D *> axes() const; // Omits default axes

		virtual void addInputHandler(QAbstractInputHandler3D *inputHandler);
		virtual void releaseInputHandler(QAbstractInputHandler3D *inputHandler);
		virtual void setActiveInputHandler(QAbstractInputHandler3D *inputHandler);
		virtual QAbstractInputHandler3D *activeInputHandler();
		virtual QList<QAbstractInputHandler3D *> inputHandlers() const;

		virtual void addTheme(QTheme3D *theme);
		virtual void releaseTheme(QTheme3D *theme);
		virtual void setActiveTheme(QTheme3D *theme, bool force = true);
		virtual QTheme3D *activeTheme() const;
		virtual QList<QTheme3D *> themes() const;

		virtual void setSelectionMode(QAbstractGraph3D::SelectionFlags mode);
		virtual QAbstractGraph3D::SelectionFlags selectionMode() const;

		virtual void setShadowQuality(QAbstractGraph3D::ShadowQuality quality);
		virtual void doSetShadowQuality(QAbstractGraph3D::ShadowQuality quality);
		virtual QAbstractGraph3D::ShadowQuality shadowQuality() const;
		virtual bool shadowsSupported() const;

		void setOptimizationHints(QAbstractGraph3D::OptimizationHints hints);
		QAbstractGraph3D::OptimizationHints optimizationHints() const;

		bool isSlicingActive() const;
		void setSlicingActive(bool isSlicing);

		QScene3D *scene();

		void markDataDirty();
		void markSeriesVisualsDirty();

		void requestRender(QOpenGLFramebufferObject *fbo);

		int addCustomItem(QCustomItem3D *item);
		void deleteCustomItems();
		void deleteCustomItem(QCustomItem3D *item);
		void deleteCustomItem(const QVector3D &position);
		void releaseCustomItem(QCustomItem3D *item);
		QList<QCustomItem3D *> customItems() const;

		int selectedLabelIndex() const;
		QAbstractAxis3D *selectedAxis() const;
		int selectedCustomItemIndex() const;
		QCustomItem3D *selectedCustomItem() const;

		void setOrthoProjection(bool enable);
		bool isOrthoProjection() const;

		void setMeasureFps(bool enable);
		inline bool measureFps() const { return m_measureFps; }
		inline qreal currentFps() const { return m_currentFps; }

		QAbstractGraph3D::ElementType selectedElement() const;

		void setAspectRatio(qreal ratio);
		qreal aspectRatio();
		void setHorizontalAspectRatio(qreal ratio);
		qreal horizontalAspectRatio() const;

		void setReflection(bool enable);
		bool reflection() const;
		void setReflectivity(qreal reflectivity);
		qreal reflectivity() const;

		void setPolar(bool enable);
		bool isPolar() const;
		void setRadialLabelOffset(float offset);
		float radialLabelOffset() const;

		void setLocale(const QLocale &locale);
		QLocale locale() const;

		QVector3D queriedGraphPosition() const;

		void setMargin(qreal margin);
		qreal margin() const;

		void emitNeedRender();

		virtual void clearSelection() = 0;

		virtual void mouseDoubleClickEvent(QMouseEvent *event);
		virtual void touchEvent(QTouchEvent *event);
		virtual void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
		virtual void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
		virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
#if QT_CONFIG(wheelevent)
		virtual void wheelEvent(QWheelEvent *event);
#endif

		virtual void handleAxisTitleChangedBySender(QObject *sender);
		virtual void handleAxisLabelsChangedBySender(QObject *sender);
		virtual void handleAxisRangeChangedBySender(QObject *sender);
		virtual void handleAxisSegmentCountChangedBySender(QObject *sender);
		virtual void handleAxisSubSegmentCountChangedBySender(QObject *sender);
		virtual void handleAxisAutoAdjustRangeChangedInOrientation(
			QAbstractAxis3D::AxisOrientation orientation, bool autoAdjust) = 0;
		virtual void handleAxisLabelFormatChangedBySender(QObject *sender);
		virtual void handleAxisReversedChangedBySender(QObject *sender);
		virtual void handleAxisFormatterDirtyBySender(QObject *sender);
		virtual void handleAxisLabelAutoRotationChangedBySender(QObject *sender);
		virtual void handleAxisTitleVisibilityChangedBySender(QObject *sender);
		virtual void handleAxisTitleFixedChangedBySender(QObject *sender);
		virtual void handleSeriesVisibilityChangedBySender(QObject *sender);
		virtual void handlePendingClick();
		virtual void handlePendingGraphPositionQuery();
		virtual void adjustAxisRanges() = 0;

		void markSeriesItemLabelsDirty();
		bool isOpenGLES() const;

		public Q_SLOTS:
		void destroyRenderer();

		void handleAxisTitleChanged(const QString &title);
		void handleAxisLabelsChanged();
		void handleAxisRangeChanged(float min, float max);
		void handleAxisSegmentCountChanged(int count);
		void handleAxisSubSegmentCountChanged(int count);
		void handleAxisAutoAdjustRangeChanged(bool autoAdjust);
		void handleAxisLabelFormatChanged(const QString &format);
		void handleAxisReversedChanged(bool enable);
		void handleAxisFormatterDirty();
		void handleAxisLabelAutoRotationChanged(float angle);
		void handleAxisTitleVisibilityChanged(bool visible);
		void handleAxisTitleFixedChanged(bool fixed);
		void handleInputViewChanged(QAbstractInputHandler3D::InputView view);
		void handleInputPositionChanged(const QPoint &position);
		void handleSeriesVisibilityChanged(bool visible);

		void handleThemeColorStyleChanged(QTheme3D::ColorStyle style);
		void handleThemeBaseColorsChanged(const QList<QColor> &color);
		void handleThemeBaseGradientsChanged(const QList<QLinearGradient> &gradient);
		void handleThemeSingleHighlightColorChanged(const QColor &color);
		void handleThemeSingleHighlightGradientChanged(const QLinearGradient &gradient);
		void handleThemeMultiHighlightColorChanged(const QColor &color);
		void handleThemeMultiHighlightGradientChanged(const QLinearGradient &gradient);
		void handleThemeTypeChanged(QTheme3D::Theme theme);

		// Renderer callback handlers
		void handleRequestShadowQuality(QAbstractGraph3D::ShadowQuality quality);

		void updateCustomItem();

	Q_SIGNALS:
		void shadowQualityChanged(QAbstractGraph3D::ShadowQuality quality);
		void activeInputHandlerChanged(QAbstractInputHandler3D *inputHandler);
		void activeThemeChanged(QTheme3D *activeTheme);
		void selectionModeChanged(QAbstractGraph3D::SelectionFlags mode);
		void needRender();
		void axisXChanged(QAbstractAxis3D *axis);
		void axisYChanged(QAbstractAxis3D *axis);
		void axisZChanged(QAbstractAxis3D *axis);
		void elementSelected(QAbstractGraph3D::ElementType type);
		void measureFpsChanged(bool enabled);
		void currentFpsChanged(qreal fps);
		void orthoProjectionChanged(bool enabled);
		void aspectRatioChanged(qreal ratio);
		void horizontalAspectRatioChanged(qreal ratio);
		void optimizationHintsChanged(QAbstractGraph3D::OptimizationHints hints);
		void polarChanged(bool enabled);
		void radialLabelOffsetChanged(float offset);
		void reflectionChanged(bool enabled);
		void reflectivityChanged(qreal reflectivity);
		void localeChanged(const QLocale &locale);
		void queriedGraphPositionChanged(const QVector3D &data);
		void marginChanged(qreal margin);

	protected:
		virtual QAbstractAxis3D *createDefaultAxis(QAbstractAxis3D::AxisOrientation orientation);
		QValueAxis3D *createDefaultValueAxis();
		QCategoryAxis3D *createDefaultCategoryAxis();
		virtual void startRecordingRemovesAndInserts();

	private:
		void setAxisHelper(QAbstractAxis3D::AxisOrientation orientation, QAbstractAxis3D *axis,
			QAbstractAxis3D **axisPtr);

		friend class AbstractDeclarative;
		friend class QBarsController3D;
		friend class QStackedBarsController3D;
		friend class QAbstractGraph3DPrivate;
	};

}

#endif