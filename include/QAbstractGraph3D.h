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

#ifndef Q_ABSTRACT_GRAPH_3D_H
#define Q_ABSTRACT_GRAPH_3D_H

#include "QVisualizationGlobals3D.h"
#include "QTheme3D.h"
#include "QScene3D.h"
#include "QAbstractInputHandler3D.h"

#include <QtGui/QWindow>
#include <QtGui/QOpenGLFunctions>
#include <QtCore/QLocale>

namespace QtStackedBar3DVis
{

	class QAbstractGraph3DPrivate;
	class QCustomItem3D;
	class QAbstractAxis3D;

	class QT_STACKEDBAR3D_EXPORT QAbstractGraph3D : public QWindow, protected QOpenGLFunctions
	{
		Q_OBJECT
			Q_ENUMS(ShadowQuality)
			Q_ENUMS(ElementType)
			Q_FLAGS(SelectionFlag SelectionFlags)
			Q_FLAGS(OptimizationHint OptimizationHints)
			Q_PROPERTY(QAbstractInputHandler3D* activeInputHandler READ activeInputHandler WRITE setActiveInputHandler NOTIFY activeInputHandlerChanged)
			Q_PROPERTY(QTheme3D* activeTheme READ activeTheme WRITE setActiveTheme NOTIFY activeThemeChanged)
			Q_PROPERTY(SelectionFlags selectionMode READ selectionMode WRITE setSelectionMode NOTIFY selectionModeChanged)
			Q_PROPERTY(ShadowQuality shadowQuality READ shadowQuality WRITE setShadowQuality NOTIFY shadowQualityChanged)
			Q_PROPERTY(QScene3D* scene READ scene)
			Q_PROPERTY(bool measureFps READ measureFps WRITE setMeasureFps NOTIFY measureFpsChanged)
			Q_PROPERTY(qreal currentFps READ currentFps NOTIFY currentFpsChanged)
			Q_PROPERTY(bool orthoProjection READ isOrthoProjection WRITE setOrthoProjection NOTIFY orthoProjectionChanged)
			Q_PROPERTY(ElementType selectedElement READ selectedElement NOTIFY selectedElementChanged)
			Q_PROPERTY(qreal aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
			Q_PROPERTY(OptimizationHints optimizationHints READ optimizationHints WRITE setOptimizationHints NOTIFY optimizationHintsChanged)
			Q_PROPERTY(bool polar READ isPolar WRITE setPolar NOTIFY polarChanged)
			Q_PROPERTY(float radialLabelOffset READ radialLabelOffset WRITE setRadialLabelOffset NOTIFY radialLabelOffsetChanged)
			Q_PROPERTY(qreal horizontalAspectRatio READ horizontalAspectRatio WRITE setHorizontalAspectRatio NOTIFY horizontalAspectRatioChanged)
			Q_PROPERTY(bool reflection READ isReflection WRITE setReflection NOTIFY reflectionChanged)
			Q_PROPERTY(qreal reflectivity READ reflectivity WRITE setReflectivity NOTIFY reflectivityChanged)
			Q_PROPERTY(QLocale locale READ locale WRITE setLocale NOTIFY localeChanged)
			Q_PROPERTY(QVector3D queriedGraphPosition READ queriedGraphPosition NOTIFY queriedGraphPositionChanged)
			Q_PROPERTY(qreal margin READ margin WRITE setMargin NOTIFY marginChanged)

	protected:
		explicit QAbstractGraph3D(QAbstractGraph3DPrivate *d, const QSurfaceFormat *format,
			QWindow *parent = Q_NULLPTR);

	public:
		enum SelectionFlag {
			SelectionNone = 0,
			SelectionItem = 1,
			SelectionRow = 2,
			SelectionItemAndRow = SelectionItem | SelectionRow,
			SelectionColumn = 4,
			SelectionItemAndColumn = SelectionItem | SelectionColumn,
			SelectionRowAndColumn = SelectionRow | SelectionColumn,
			SelectionItemRowAndColumn = SelectionItem | SelectionRow | SelectionColumn,
			SelectionSlice = 8,
			SelectionMultiSeries = 16
		};
		Q_DECLARE_FLAGS(SelectionFlags, SelectionFlag)

			enum ShadowQuality {
			ShadowQualityNone = 0,
			ShadowQualityLow,
			ShadowQualityMedium,
			ShadowQualityHigh,
			ShadowQualitySoftLow,
			ShadowQualitySoftMedium,
			ShadowQualitySoftHigh
		};

		enum ElementType {
			ElementNone = 0,
			ElementSeries,
			ElementAxisXLabel,
			ElementAxisYLabel,
			ElementAxisZLabel,
			ElementCustomItem
		};

		enum OptimizationHint {
			OptimizationDefault = 0,
			OptimizationStatic = 1
		};
		Q_DECLARE_FLAGS(OptimizationHints, OptimizationHint)

	public:
		virtual ~QAbstractGraph3D();

		void addInputHandler(QAbstractInputHandler3D *inputHandler);
		void releaseInputHandler(QAbstractInputHandler3D *inputHandler);
		void setActiveInputHandler(QAbstractInputHandler3D *inputHandler);
		QAbstractInputHandler3D *activeInputHandler() const;
		QList<QAbstractInputHandler3D *> inputHandlers() const;

		void addTheme(QTheme3D *theme);
		void releaseTheme(QTheme3D *theme);
		void setActiveTheme(QTheme3D *theme);
		QTheme3D *activeTheme() const;
		QList<QTheme3D *> themes() const;

		void setSelectionMode(SelectionFlags mode);
		SelectionFlags selectionMode() const;

		void setShadowQuality(ShadowQuality quality);
		ShadowQuality shadowQuality() const;
		virtual bool shadowsSupported() const;

		QScene3D *scene() const;

		void clearSelection();

		int addCustomItem(QCustomItem3D *item);
		void removeCustomItems();
		void removeCustomItem(QCustomItem3D *item);
		void removeCustomItemAt(const QVector3D &position);
		void releaseCustomItem(QCustomItem3D *item);
		QList<QCustomItem3D *> customItems() const;

		int selectedLabelIndex() const;
		QAbstractAxis3D *selectedAxis() const;

		int selectedCustomItemIndex() const;
		QCustomItem3D *selectedCustomItem() const;

		QImage renderToImage(int msaaSamples = 0, const QSize &imageSize = QSize());

		void setMeasureFps(bool enable);
		bool measureFps() const;
		qreal currentFps() const;

		void setOrthoProjection(bool enable);
		bool isOrthoProjection() const;

		ElementType selectedElement() const;

		void setAspectRatio(qreal ratio);
		qreal aspectRatio() const;

		void setOptimizationHints(OptimizationHints hints);
		OptimizationHints optimizationHints() const;

		void setPolar(bool enable);
		bool isPolar() const;

		void setRadialLabelOffset(float offset);
		float radialLabelOffset() const;

		void setHorizontalAspectRatio(qreal ratio);
		qreal horizontalAspectRatio() const;

		void setReflection(bool enable);
		bool isReflection() const;

		void setReflectivity(qreal reflectivity);
		qreal reflectivity() const;

		void setLocale(const QLocale &locale);
		QLocale locale() const;

		QVector3D queriedGraphPosition() const;

		void setMargin(qreal margin);
		qreal margin() const;

		bool hasContext() const;

	protected:
		bool event(QEvent *event);
		void resizeEvent(QResizeEvent *event);
		void exposeEvent(QExposeEvent *event);

		void mouseDoubleClickEvent(QMouseEvent *event);
		void touchEvent(QTouchEvent *event);
		void mousePressEvent(QMouseEvent *event);
		void mouseReleaseEvent(QMouseEvent *event);
		void mouseMoveEvent(QMouseEvent *event);
#if QT_CONFIG(wheelevent)
		void wheelEvent(QWheelEvent *event);
#endif

	Q_SIGNALS:
		void activeInputHandlerChanged(QAbstractInputHandler3D *inputHandler);
		void activeThemeChanged(QTheme3D *theme);
		void selectionModeChanged(QAbstractGraph3D::SelectionFlags mode);
		void shadowQualityChanged(QAbstractGraph3D::ShadowQuality quality);
		void selectedElementChanged(QAbstractGraph3D::ElementType type);
		void measureFpsChanged(bool enabled);
		void currentFpsChanged(qreal fps);
		void orthoProjectionChanged(bool enabled);
		void aspectRatioChanged(qreal ratio);
		void optimizationHintsChanged(QAbstractGraph3D::OptimizationHints hints);
		void polarChanged(bool enabled);
		void radialLabelOffsetChanged(float offset);
		void horizontalAspectRatioChanged(qreal ratio);
		void reflectionChanged(bool enabled);
		void reflectivityChanged(qreal reflectivity);
		void localeChanged(const QLocale &locale);
		void queriedGraphPositionChanged(const QVector3D &data);
		void marginChanged(qreal margin);

	private:
		Q_DISABLE_COPY(QAbstractGraph3D)
			QScopedPointer<QAbstractGraph3DPrivate> d_ptr;

		friend class QBars3D;
		friend class QStackedBars3D;
		friend class Q3DScatter;
		friend class Q3DSurface;
	};
	Q_DECLARE_OPERATORS_FOR_FLAGS(QAbstractGraph3D::SelectionFlags)
	Q_DECLARE_OPERATORS_FOR_FLAGS(QAbstractGraph3D::OptimizationHints)

}

#endif
