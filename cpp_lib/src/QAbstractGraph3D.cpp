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

#include "QAbstractGraph3D.h"
#include "QAbstractGraph3DPrivate.h"
#include "QAbstractController3D.h"
#include "QAbstractInputHandler3DPrivate.h"
#include "QScene3DPrivate.h"
#include "QUtils3D.h"
#include "QUtilsGeneric.h"

#include <QtGui/QGuiApplication>
#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLPaintDevice>
#include <QtGui/QPainter>
#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QOffscreenSurface>
#if defined(Q_OS_OSX)
#include <qpa/qplatformnativeinterface.h>
#endif

namespace QtStackedBar3DVis
{

	/*!
	* \class QAbstractGraph3D
	* \inmodule QtDataVisualization
	* \brief The QAbstractGraph3D class provides a window and render loop for graphs.
	* \since QtDataVisualization 1.0
	*
	* This class subclasses a QWindow and provides render loop for graphs inheriting it.
	*
	* You should not need to use this class directly, but one of its subclasses instead.
	*
	* Anti-aliasing is turned on by default on C++, except in OpenGL ES2
	* environments, where anti-aliasing is not supported by Qt Data Visualization.
	* To specify non-default anti-aliasing for a graph, give a custom surface format as
	* a constructor parameter. You can use the convenience function \c QtDataVisualization::qDefaultSurfaceFormat()
	* to create the surface format object.
	*
	* \note QAbstractGraph3D sets window flag \c Qt::FramelessWindowHint on by default. If you want to display
	* graph windows as standalone windows with regular window frame, clear this flag after constructing
	* the graph. For example:
	*
	* \code
	*  QBars3D *graphWindow = new QBars3D;
	*  graphWindow->setFlags(graphWindow->flags() ^ Qt::FramelessWindowHint);
	* \endcode
	*
	* \sa QBars3D, Q3DScatter, Q3DSurface, {Qt Data Visualization C++ Classes}
	*/

	/*!
	\enum QAbstractGraph3D::SelectionFlag

	Item selection modes. Values of this enumeration can be combined with OR operator.

	\value SelectionNone
	Selection mode disabled.
	\value SelectionItem
	Selection highlights a single item.
	\value SelectionRow
	Selection highlights a single row.
	\value SelectionItemAndRow
	Combination flag for highlighting both item and row with different colors.
	\value SelectionColumn
	Selection highlights a single column.
	\value SelectionItemAndColumn
	Combination flag for highlighting both item and column with different colors.
	\value SelectionRowAndColumn
	Combination flag for highlighting both row and column.
	\value SelectionItemRowAndColumn
	Combination flag for highlighting item, row, and column.
	\value SelectionSlice
	Setting this mode flag indicates that the graph should take care of the slice view handling
	automatically. If you wish to control the slice view yourself via QScene3D, do not set this
	flag. When setting this mode flag, either \c SelectionRow or \c SelectionColumn must also
	be set, but not both. Slicing is supported by QBars3D and Q3DSurface only.
	When this flag is set, slice mode is entered in the following situations:
	\list
	\li When selection is changed explicitly via series API to a visible item
	\li When selection is changed by clicking on the graph
	\li When the selection mode changes and the selected item is visible
	\endlist
	\value SelectionMultiSeries
	Setting this mode means that items for all series at same position are highlighted, instead
	of just the selected item. The actual selection in the other series doesn't change.
	Multi-series selection is not supported for Q3DScatter.
	*/

	/*!
	\enum QAbstractGraph3D::ShadowQuality

	Quality of shadows.

	\value ShadowQualityNone
	Shadows are disabled.
	\value ShadowQualityLow
	Shadows are rendered in low quality.
	\value ShadowQualityMedium
	Shadows are rendered in medium quality.
	\value ShadowQualityHigh
	Shadows are rendered in high quality.
	\value ShadowQualitySoftLow
	Shadows are rendered in low quality with softened edges.
	\value ShadowQualitySoftMedium
	Shadows are rendered in medium quality with softened edges.
	\value ShadowQualitySoftHigh
	Shadows are rendered in high quality with softened edges.
	*/

	/*!
	\enum QAbstractGraph3D::ElementType
	\since QtDataVisualization 1.1

	Type of an element in the graph.

	\value ElementNone
	No defined element.
	\value ElementSeries
	A series (that is, an item in a series).
	\value ElementAxisXLabel
	The x-axis label.
	\value ElementAxisYLabel
	The y-axis label.
	\value ElementAxisZLabel
	The z-axis label.
	\value ElementCustomItem
	A custom item.
	*/

	/*!
	\enum QAbstractGraph3D::OptimizationHint
	\since Qt Data Visualization 1.1

	The optimization hint for rendering.

	\value OptimizationDefault
	Provides the full feature set at a reasonable performance.
	\value OptimizationStatic
	Optimizes the rendering of static data sets at the expense of some features.
	*/

	/*!
	* \internal
	*/
	QAbstractGraph3D::QAbstractGraph3D(QAbstractGraph3DPrivate *d, const QSurfaceFormat *format,
		QWindow *parent)
		: QWindow(parent),
		d_ptr(d)
	{
		qRegisterMetaType<QAbstractGraph3D::ShadowQuality>("QAbstractGraph3D::ShadowQuality");
		qRegisterMetaType<QAbstractGraph3D::ElementType>("QAbstractGraph3D::ElementType");

		// Default to frameless window, as typically graphs are not toplevel
		setFlags(flags() | Qt::FramelessWindowHint);

		QSurfaceFormat surfaceFormat;
		if (format) {
			surfaceFormat = *format;
			// Make sure renderable type is correct
			surfaceFormat.setRenderableType(QSurfaceFormat::DefaultRenderableType);
		}
		else {
			surfaceFormat = qDefaultSurfaceFormat();
		}

		d_ptr->m_context = new QOpenGLContext(this);
		setSurfaceType(QWindow::OpenGLSurface);
		setFormat(surfaceFormat);

		create();

		d_ptr->m_context->setFormat(requestedFormat());
		d_ptr->m_context->create();
		bool makeSuccess = d_ptr->m_context->makeCurrent(this);

		// If we fail to get context, just abort
		if (!makeSuccess || !QOpenGLContext::currentContext())
			return;

		initializeOpenGLFunctions();

		const GLubyte *shaderVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	#ifndef QT_NO_DEBUG
		const GLubyte *openGLVersion = glGetString(GL_VERSION);
		qDebug() << "OpenGL version:" << (const char *)openGLVersion;
		qDebug() << "GLSL version:" << (const char *)shaderVersion;
	#endif

		if (!QUtils3D::isOpenGLES()) {
			// If we have real OpenGL, GLSL version must be 1.2 or over. Quit if not.
			QStringList splitversionstr =
				QString::fromLatin1((const char *)shaderVersion).split(QChar::fromLatin1(' '));
			if (splitversionstr[0].toFloat() < 1.2)
				qFatal("GLSL version must be 1.20 or higher. Try installing latest display drivers.");
		}

		d_ptr->m_initialized = true;

		d_ptr->renderLater();

	#if defined(Q_OS_OSX)
		// Enable touch events for Mac touchpads
		typedef void * (*EnableTouch)(QWindow*, bool);
		EnableTouch enableTouch =
			(EnableTouch)QGuiApplication::platformNativeInterface()->nativeResourceFunctionForIntegration("registertouchwindow");
		if (enableTouch)
			enableTouch(this, true);
	#endif
	}

	/*!
	* Destroys QAbstractGraph3D.
	*/
	QAbstractGraph3D::~QAbstractGraph3D()
	{
	}

	/*!
	* Adds the given \a inputHandler to the graph. The input handlers added via addInputHandler
	* are not taken in to use directly. Only the ownership of the \a inputHandler is given to the graph.
	* The \a inputHandler must not be null or already added to another graph.
	*
	* \sa releaseInputHandler(), setActiveInputHandler()
	*/
	void QAbstractGraph3D::addInputHandler(QAbstractInputHandler3D *inputHandler)
	{
		d_ptr->m_visualController->addInputHandler(inputHandler);
	}

	/*!
	* Releases the ownership of the \a inputHandler back to the caller, if it was added to this graph.
	* If the released \a inputHandler is in use there will be no input handler active after this call.
	*
	* If the default input handler is released and added back later, it behaves as any other input handler would.
	*
	* \sa addInputHandler(), setActiveInputHandler()
	*/
	void QAbstractGraph3D::releaseInputHandler(QAbstractInputHandler3D *inputHandler)
	{
		d_ptr->m_visualController->releaseInputHandler(inputHandler);
	}

	/*!
	* \property QAbstractGraph3D::activeInputHandler
	*
	* \brief The active input handler used in the graph.
	*/

	/*!
	* Sets \a inputHandler as the active input handler used in the graph.
	* Implicitly calls addInputHandler() to transfer ownership of \a inputHandler
	* to this graph.
	*
	* If \a inputHandler is null, no input handler will be active after this call.
	*
	* \sa addInputHandler(), releaseInputHandler()
	*/
	void QAbstractGraph3D::setActiveInputHandler(QAbstractInputHandler3D *inputHandler)
	{
		d_ptr->m_visualController->setActiveInputHandler(inputHandler);
	}

	QAbstractInputHandler3D *QAbstractGraph3D::activeInputHandler() const
	{
		return d_ptr->m_visualController->activeInputHandler();
	}

	/*!
	* Returns the list of all added input handlers.
	*
	* \sa addInputHandler()
	*/
	QList<QAbstractInputHandler3D *> QAbstractGraph3D::inputHandlers() const
	{
		return d_ptr->m_visualController->inputHandlers();
	}

	/*!
	* Adds the given \a theme to the graph. The themes added via addTheme are not taken in to use
	* directly. Only the ownership of the theme is given to the graph.
	* The \a theme must not be null or already added to another graph.
	*
	* \sa releaseTheme(), setActiveTheme()
	*/
	void QAbstractGraph3D::addTheme(QTheme3D *theme)
	{
		d_ptr->m_visualController->addTheme(theme);
	}

	/*!
	* Releases the ownership of the \a theme back to the caller, if it was added to this graph.
	* If the released \a theme is in use, a new default theme will be created and set active.
	*
	* If the default theme is released and added back later, it behaves as any other theme would.
	*
	* \sa addTheme(), setActiveTheme()
	*/
	void QAbstractGraph3D::releaseTheme(QTheme3D *theme)
	{
		d_ptr->m_visualController->releaseTheme(theme);
	}

	/*!
	* \property QAbstractGraph3D::activeTheme
	*
	* \brief The active theme of the graph.
	*/

	/*!
	* Sets \a theme as the active theme to be used for the graph. Implicitly calls
	* addTheme() to transfer the ownership of the theme to this graph.
	*
	* If \a theme is null, a temporary default theme is created. This temporary theme is destroyed
	* if any theme is explicitly set later.
	* Properties of the theme can be modified even after setting it, and the modifications take
	* effect immediately.
	*/
	void QAbstractGraph3D::setActiveTheme(QTheme3D *theme)
	{
		d_ptr->m_visualController->setActiveTheme(theme);
	}


	QTheme3D *QAbstractGraph3D::activeTheme() const
	{
		return d_ptr->m_visualController->activeTheme();
	}

	/*!
	* Returns the list of all added themes.
	*
	* \sa addTheme()
	*/
	QList<QTheme3D *> QAbstractGraph3D::themes() const
	{
		return d_ptr->m_visualController->themes();
	}

	/*!
	* \property QAbstractGraph3D::selectionMode
	*
	* \brief Item selection mode.
	*
	* A combination of SelectionFlags. By default, \c SelectionItem.
	* Different graph types support different selection modes.
	*
	* \sa SelectionFlags
	*/
	void QAbstractGraph3D::setSelectionMode(SelectionFlags mode)
	{
		d_ptr->m_visualController->setSelectionMode(mode);
	}

	QAbstractGraph3D::SelectionFlags QAbstractGraph3D::selectionMode() const
	{
		return d_ptr->m_visualController->selectionMode();
	}

	/*!
	* \property QAbstractGraph3D::shadowQuality
	*
	* \brief The quality of the shadow.
	*
	* One of the ShadowQuality enum values. By default, \c ShadowQualityMedium.
	*
	* \note If setting the shadow quality to a certain level fails, the level is lowered
	* until it is successfully set. The \c shadowQualityChanged signal is emitted each time
	* a change is made.
	*
	* \sa ShadowQuality
	*/
	void QAbstractGraph3D::setShadowQuality(ShadowQuality quality)
	{
		d_ptr->m_visualController->setShadowQuality(quality);
	}

	QAbstractGraph3D::ShadowQuality QAbstractGraph3D::shadowQuality() const
	{
		return d_ptr->m_visualController->shadowQuality();
	}

	/*!
	* Returns \c true if shadows are supported with the current configuration.
	* OpenGL ES2 configurations do not support shadows.
	*/
	bool QAbstractGraph3D::shadowsSupported() const
	{
		return d_ptr->m_visualController->shadowsSupported();
	}

	/*!
	* \property QAbstractGraph3D::scene
	*
	* \brief The QScene3D pointer that can be used to manipulate the scene and
	* access the scene elements, such as the active camera.
	*
	* This property is read-only.
	*/
	QScene3D *QAbstractGraph3D::scene() const
	{
		return d_ptr->m_visualController->scene();
	}

	/*!
	* Clears selection from all attached series.
	*/
	void QAbstractGraph3D::clearSelection()
	{
		d_ptr->m_visualController->clearSelection();
	}

	/*!
	* Adds a QCustomItem3D \a item to the graph. Graph takes ownership of the added item.
	*
	* Returns the index to the added item if the add operation was successful, -1
	* if trying to add a null item, and the index of the item if trying to add an
	* already added item.
	*
	* \sa removeCustomItems(), removeCustomItem(), removeCustomItemAt(), customItems()
	*
	* \since QtDataVisualization 1.1
	*/
	int QAbstractGraph3D::addCustomItem(QCustomItem3D *item)
	{
		return d_ptr->m_visualController->addCustomItem(item);
	}

	/*!
	* Removes all custom items. Deletes the resources allocated to them.
	*
	* \since QtDataVisualization 1.1
	*/
	void QAbstractGraph3D::removeCustomItems()
	{
		d_ptr->m_visualController->deleteCustomItems();
	}

	/*!
	* Removes the custom \a {item}. Deletes the resources allocated to it.
	*
	* \since QtDataVisualization 1.1
	*/
	void QAbstractGraph3D::removeCustomItem(QCustomItem3D *item)
	{
		d_ptr->m_visualController->deleteCustomItem(item);
	}

	/*!
	* Removes all custom items at \a {position}. Deletes the resources allocated to them.
	*
	* \since QtDataVisualization 1.1
	*/
	void QAbstractGraph3D::removeCustomItemAt(const QVector3D &position)
	{
		d_ptr->m_visualController->deleteCustomItem(position);
	}

	/*!
	* Gets ownership of given \a item back and removes the \a item from the graph.
	*
	* \since QtDataVisualization 1.1
	*
	* \note If the same item is added back to the graph, the texture or the texture file needs to be
	* re-set.
	*
	* \sa QCustomItem3D::setTextureImage(), QCustomItem3D::setTextureFile()
	*/
	void QAbstractGraph3D::releaseCustomItem(QCustomItem3D *item)
	{
		return d_ptr->m_visualController->releaseCustomItem(item);
	}

	/*!
	* Returns the list of all added custom items.
	* \since QtDataVisualization 1.2
	* \sa addCustomItem()
	*/
	QList<QCustomItem3D *> QAbstractGraph3D::customItems() const
	{
		return d_ptr->m_visualController->customItems();
	}

	/*!
	* Can be used to query the index of the selected label after receiving \c selectedElementChanged
	* signal with any label type. Selection is valid until the next \c selectedElementChanged signal.
	*
	* Returns the index of the selected label, or -1.
	*
	* \since QtDataVisualization 1.1
	*
	* \sa selectedElement
	*/
	int QAbstractGraph3D::selectedLabelIndex() const
	{
		return d_ptr->m_visualController->selectedLabelIndex();
	}

	/*!
	* Can be used to get the selected axis after receiving \c selectedElementChanged signal with any label
	* type. Selection is valid until the next \c selectedElementChanged signal.
	*
	* Returns the pointer to the selected axis, or null.
	*
	* \since QtDataVisualization 1.1
	*
	* \sa selectedElement
	*/
	QAbstractAxis3D *QAbstractGraph3D::selectedAxis() const
	{
		return d_ptr->m_visualController->selectedAxis();
	}

	/*!
	* Can be used to query the index of the selected custom item after receiving \c selectedElementChanged
	* signal with QAbstractGraph3D::ElementCustomItem type. Selection is valid until the next
	* \c selectedElementChanged signal.
	*
	* Returns the index of the selected custom item, or -1.
	*
	* \since QtDataVisualization 1.1
	*
	* \sa selectedElement
	*/
	int QAbstractGraph3D::selectedCustomItemIndex() const
	{
		return d_ptr->m_visualController->selectedCustomItemIndex();
	}

	/*!
	* Can be used to get the selected custom item after receiving \c selectedElementChanged signal with
	* QAbstractGraph3D::ElementCustomItem type. Ownership of the item remains with the graph.
	* Selection is valid until the next \c selectedElementChanged signal.
	*
	* Returns the pointer to the selected custom item, or null.
	*
	* \since QtDataVisualization 1.1
	*
	* \sa selectedElement
	*/
	QCustomItem3D *QAbstractGraph3D::selectedCustomItem() const
	{
		return d_ptr->m_visualController->selectedCustomItem();
	}

	/*!
	* \property QAbstractGraph3D::selectedElement
	*
	* \brief The element selected in the graph.
	*
	* This property can be used to query the selected element type. The type is
	* valid until a new selection is made in the graph and the
	* \c selectedElementChanged signal is emitted.
	*
	* The signal can be used for example for implementing custom input handlers, as
	* demonstrated by the \l {Axis Range Dragging With Labels Example}.
	*
	* \sa selectedLabelIndex(), selectedAxis(), selectedCustomItemIndex(), selectedCustomItem(),
	* QBars3D::selectedSeries(), Q3DScatter::selectedSeries(), Q3DSurface::selectedSeries(),
	* QScene3D::setSelectionQueryPosition()
	*
	* \since QtDataVisualization 1.1
	*/
	QAbstractGraph3D::ElementType QAbstractGraph3D::selectedElement() const
	{
		return d_ptr->m_visualController->selectedElement();
	}

	/*!
	* Renders current frame to an image of \a imageSize. Default size is the window size. Image is
	* rendered with antialiasing level given in \a msaaSamples. Default level is \c{0}.
	*
	* \since QtDataVisualization 1.1
	*
	* Returns the rendered image.
	*
	* \note OpenGL ES2 does not support anitialiasing, so \a msaaSamples is always forced to \c{0}.
	*/
	QImage QAbstractGraph3D::renderToImage(int msaaSamples, const QSize &imageSize)
	{
		QSize renderSize = imageSize;
		if (renderSize.isEmpty())
			renderSize = size();
		return d_ptr->renderToImage(msaaSamples, renderSize);
	}

	/*!
	* \property QAbstractGraph3D::measureFps
	* \since QtDataVisualization 1.1
	*
	* \brief Whether rendering is done continuously instead of on demand.
	*
	* If \c {true}, rendering is continuous and the value of the currentFps
	* property is updated. Defaults to \c{false}.
	*
	* \sa currentFps
	*/
	void QAbstractGraph3D::setMeasureFps(bool enable)
	{
		d_ptr->m_visualController->setMeasureFps(enable);
	}

	bool QAbstractGraph3D::measureFps() const
	{
		return d_ptr->m_visualController->measureFps();
	}

	/*!
	* \property QAbstractGraph3D::currentFps
	* \since QtDataVisualization 1.1
	*
	* \brief The rendering results for the last second.
	*
	* The results are stored in this read-only property when FPS measuring is
	* enabled. It takes at least a second before this value is updated after
	* measuring is activated.
	*
	* \sa measureFps
	*/
	qreal QAbstractGraph3D::currentFps() const
	{
		return d_ptr->m_visualController->currentFps();
	}

	/*!
	* \property QAbstractGraph3D::orthoProjection
	* \since QtDataVisualization 1.1
	*
	* \brief Whether orthographic projection is used for displaying the graph.
	*
	* If \c {true}, ortographic projection is used to create 2D graphs by replacing
	* the default input handler with one that does not allow rotating the graph and
	* by setting the camera to view the graph
	* directly from the side or from the top. Also, axis labels typically need to be rotated when
	* viewing the graph from the sides.
	* Defaults to \c{false}.
	* \note Shadows will be disabled when set to \c{true}.
	*
	* \sa QAbstract3DAxis::labelAutoRotation, Q3DCamera::cameraPreset
	*/
	void QAbstractGraph3D::setOrthoProjection(bool enable)
	{
		d_ptr->m_visualController->setOrthoProjection(enable);
	}

	bool QAbstractGraph3D::isOrthoProjection() const
	{
		return d_ptr->m_visualController->isOrthoProjection();
	}

	/*!
	* \property QAbstractGraph3D::aspectRatio
	* \since QtDataVisualization 1.1
	*
	* \brief The ratio of the graph scaling between the longest axis on the
	* horizontal plane and the y-axis.
	*
	* Defaults to \c{2.0}.
	*
	* \note Has no effect on QBars3D.
	*
	* \sa horizontalAspectRatio
	*/
	void QAbstractGraph3D::setAspectRatio(qreal ratio)
	{
		d_ptr->m_visualController->setAspectRatio(ratio);
	}

	qreal QAbstractGraph3D::aspectRatio() const
	{
		return d_ptr->m_visualController->aspectRatio();
	}

	/*!
	* \property QAbstractGraph3D::optimizationHints
	*
	* \brief Whether the default or static mode is used for rendering optimization.
	*
	* The default mode provides the full feature set at a reasonable level of
	* performance. The static mode optimizes graph rendering and is ideal for
	* large non-changing data sets. It is slower with dynamic data changes and item rotations.
	* Selection is not optimized, so using the static mode with massive data sets is not advisable.
	* Static optimization works only on scatter graphs.
	* Defaults to \l{OptimizationDefault}.
	*
	* \note On some environments, large graphs using static optimization may not render, because
	* all of the items are rendered using a single draw call, and different graphics drivers
	* support different maximum vertice counts per call.
	* This is mostly an issue on 32bit and OpenGL ES2 platforms.
	* To work around this issue, choose an item mesh with a low vertex count or use
	* the point mesh.
	*
	* \sa QAbstract3DSeries::mesh
	*/
	void QAbstractGraph3D::setOptimizationHints(OptimizationHints hints)
	{
		d_ptr->m_visualController->setOptimizationHints(hints);
	}

	QAbstractGraph3D::OptimizationHints QAbstractGraph3D::optimizationHints() const
	{
		return d_ptr->m_visualController->optimizationHints();
	}

	/*!
	* \property QAbstractGraph3D::polar
	* \since QtDataVisualization 1.2
	*
	* \brief Whether horizontal axes are changed into polar axes.
	*
	* If \c {true}, the x-axis becomes the angular axis and the z-axis becomes the
	* radial axis.
	* Polar mode is not available for bar graphs.
	*
	* Defaults to \c{false}.
	*
	* \sa orthoProjection, radialLabelOffset
	*/
	void QAbstractGraph3D::setPolar(bool enable)
	{
		d_ptr->m_visualController->setPolar(enable);
	}

	bool QAbstractGraph3D::isPolar() const
	{
		return d_ptr->m_visualController->isPolar();
	}

	/*!
	* \property QAbstractGraph3D::radialLabelOffset
	* \since QtDataVisualization 1.2
	*
	* \brief The normalized horizontal offset for the axis labels of the radial
	* polar axis.
	*
	* The value \c 0.0 indicates that the labels should be drawn next to the 0-angle
	* angular axis grid line. The value \c 1.0 indicates that the labels are drawn
	* in their usual place at the edge of the graph background. Defaults to \c 1.0.
	*
	* This property is ignored if the \l polar property value is \c{false}.
	*
	* \sa polar
	*/
	void QAbstractGraph3D::setRadialLabelOffset(float offset)
	{
		d_ptr->m_visualController->setRadialLabelOffset(offset);
	}

	float QAbstractGraph3D::radialLabelOffset() const
	{
		return d_ptr->m_visualController->radialLabelOffset();
	}

	/*!
	* \property QAbstractGraph3D::horizontalAspectRatio
	* \since QtDataVisualization 1.2
	*
	* \brief The ratio of the graph scaling between the x-axis and z-axis.
	*
	* The value of \c 0.0 indicates automatic scaling according to axis ranges.
	* Defaults to \c{0.0}.
	*
	* Has no effect on QBars3D, which handles scaling on the horizontal plane via
	* the \l{QBars3D::barThickness}{barThickness} and \l{QBars3D::barSpacing}{barSpacing} properties.
	* Polar graphs also ignore this property.
	*
	* \sa aspectRatio, polar, QBars3D::barThickness, QBars3D::barSpacing
	*/
	void QAbstractGraph3D::setHorizontalAspectRatio(qreal ratio)
	{
		d_ptr->m_visualController->setHorizontalAspectRatio(ratio);
	}

	qreal QAbstractGraph3D::horizontalAspectRatio() const
	{
		return d_ptr->m_visualController->horizontalAspectRatio();
	}

	/*!
	* \property QAbstractGraph3D::reflection
	* \since QtDataVisualization 1.2
	*
	* \brief Whether floor reflections are on or off.
	*
	* Defaults to \c{false}.
	*
	* Affects only QBars3D. However, in QBars3D graphs holding both positive and
	* negative values, reflections are not supported for custom items that
	* intersect the floor plane. In that case, reflections should be turned off
	* to avoid incorrect rendering.
	*
	* If using a custom surface format, the stencil buffer needs to be defined
	* (QSurfaceFormat::setStencilBufferSize()) for reflections to work.
	*
	* \sa reflectivity
	*/
	void QAbstractGraph3D::setReflection(bool enable)
	{
		d_ptr->m_visualController->setReflection(enable);
	}

	bool QAbstractGraph3D::isReflection() const
	{
		return d_ptr->m_visualController->reflection();
	}

	/*!
	* \property QAbstractGraph3D::reflectivity
	* \since QtDataVisualization 1.2
	*
	* \brief Floor reflectivity.
	*
	* Larger numbers make the floor more reflective. The valid range is \c{[0...1]}.
	* Defaults to \c{0.5}.
	*
	* \note Affects only QBars3D.
	*
	* \sa reflection
	*/
	void QAbstractGraph3D::setReflectivity(qreal reflectivity)
	{
		d_ptr->m_visualController->setReflectivity(reflectivity);
	}

	qreal QAbstractGraph3D::reflectivity() const
	{
		return d_ptr->m_visualController->reflectivity();
	}

	/*!
	* \property QAbstractGraph3D::locale
	* \since QtDataVisualization 1.2
	*
	* \brief The locale used for formatting various numeric labels.
	*
	* Defaults to the \c{"C"} locale.
	*
	* \sa QValue3DAxis::labelFormat
	*/
	void QAbstractGraph3D::setLocale(const QLocale &locale)
	{
		d_ptr->m_visualController->setLocale(locale);
	}

	QLocale QAbstractGraph3D::locale() const
	{
		return d_ptr->m_visualController->locale();
	}

	/*!
	* \property QAbstractGraph3D::queriedGraphPosition
	* \since QtDataVisualization 1.2
	*
	* \brief The latest queried graph position values along each axis.
	*
	* This read-only property contains the results from
	* QScene3D::graphPositionQuery. The values are normalized to the range \c{[-1, 1]}.
	* If the queried position was outside the graph bounds, the values
	* will not reflect the real position, but will instead indicate an undefined position outside
	* the range \c{[-1, 1]}. The value will be undefined until a query is made.
	*
	* There is no single correct 3D coordinate to match a particular screen position, so to be
	* consistent, the queries are always done against the inner sides of an invisible box surrounding
	* the graph.
	*
	* \note Bar graphs only allow querying graph position at the graph floor level,
	* so the y-value is always zero for bar graphs and the valid queries can be only made at
	* screen positions that contain the floor of the graph.
	*
	* \sa QScene3D::graphPositionQuery
	*/
	QVector3D QAbstractGraph3D::queriedGraphPosition() const
	{
		return d_ptr->m_visualController->queriedGraphPosition();
	}

	/*!
	* \property QAbstractGraph3D::margin
	* \since QtDataVisualization 1.2
	*
	* \brief The absolute value used for the space left between the edge of the
	* plottable graph area and the edge of the graph background.
	*
	* If the margin value is negative, the margins are determined automatically and can vary according
	* to the size of the items in the series and the type of the graph.
	* The value is interpreted as a fraction of the y-axis range if the graph
	* aspect ratios have not beed changed from the default values.
	* Defaults to \c{-1.0}.
	*
	* \note Setting a smaller margin for a scatter graph than the automatically
	* determined margin can cause the scatter items at the edges of the graph to
	* overlap with the graph background.
	*
	* \note On scatter and surface graphs, if the margin is small in comparison to the axis label
	* size, the positions of the edge labels of the axes are adjusted to avoid overlap with
	* the edge labels of the neighboring axes.
	*/
	void QAbstractGraph3D::setMargin(qreal margin)
	{
		d_ptr->m_visualController->setMargin(margin);
	}

	qreal QAbstractGraph3D::margin() const
	{
		return d_ptr->m_visualController->margin();
	}

	/*!
	* Returns \c{true} if the OpenGL context of the graph has been successfully initialized.
	* Trying to use a graph when the context initialization has failed typically results in a crash.
	* A common reason for a context initialization failure is lack of sufficient platform support
	* for OpenGL.
	*/
	bool QAbstractGraph3D::hasContext() const
	{
		if (d_ptr->m_initialized)
			return true;
		else
			return false;
	}

	/*!
	* \internal
	*/
	bool QAbstractGraph3D::event(QEvent *event)
	{
		switch (event->type()) {
		case QEvent::UpdateRequest:
			d_ptr->renderNow();
			return true;
		case QEvent::TouchBegin:
		case QEvent::TouchCancel:
		case QEvent::TouchUpdate:
		case QEvent::TouchEnd:
			d_ptr->m_visualController->touchEvent(static_cast<QTouchEvent *>(event));
			return true;
		default:
			return QWindow::event(event);
		}
	}

	/*!
	* \internal
	*/
	void QAbstractGraph3D::resizeEvent(QResizeEvent *event)
	{
		Q_UNUSED(event);

		if (d_ptr->m_visualController) {
			QScene3D *scene = d_ptr->m_visualController->scene();
			scene->d_ptr->setWindowSize(QSize(width(), height()));
			scene->d_ptr->setViewport(QRect(0, 0, width(), height()));
		}
	}

	/*!
	* \internal
	*/
	void QAbstractGraph3D::exposeEvent(QExposeEvent *event)
	{
		Q_UNUSED(event);

		if (isExposed())
			d_ptr->renderNow();
	}

	/*!
	* \internal
	*/
	void QAbstractGraph3D::mouseDoubleClickEvent(QMouseEvent *event)
	{
		d_ptr->m_visualController->mouseDoubleClickEvent(event);
	}

	/*!
	* \internal
	*/
	void QAbstractGraph3D::touchEvent(QTouchEvent *event)
	{
		d_ptr->m_visualController->touchEvent(event);
	}

	/*!
	* \internal
	*/
	void QAbstractGraph3D::mousePressEvent(QMouseEvent *event)
	{
		d_ptr->m_visualController->mousePressEvent(event, event->pos());
	}

	/*!
	* \internal
	*/
	void QAbstractGraph3D::mouseReleaseEvent(QMouseEvent *event)
	{
		d_ptr->m_visualController->mouseReleaseEvent(event, event->pos());
	}

	/*!
	* \internal
	*/
	void QAbstractGraph3D::mouseMoveEvent(QMouseEvent *event)
	{
		d_ptr->m_visualController->mouseMoveEvent(event, event->pos());
	}

	#ifdef Q_WS_WIN
	#if QT_CONFIG(wheelevent)
        #endif
	/*!
	* \internal
	*/
	void QAbstractGraph3D::wheelEvent(QWheelEvent *event)
	{
		d_ptr->m_visualController->wheelEvent(event);
	}
	#ifdef Q_WS_WIN
	#endif
        #endif

	QAbstractGraph3DPrivate::QAbstractGraph3DPrivate(QAbstractGraph3D *q)
		: QObject(0),
		q_ptr(q),
		m_updatePending(false),
		m_visualController(0),
		m_devicePixelRatio(1.f),
		m_offscreenSurface(0),
		m_initialized(false)
	{
	}

	QAbstractGraph3DPrivate::~QAbstractGraph3DPrivate()
	{
		if (m_offscreenSurface) {
			m_offscreenSurface->destroy();
			delete m_offscreenSurface;
		}
		if (m_context)
			m_context->makeCurrent(q_ptr);

		delete m_visualController;
	}

	void QAbstractGraph3DPrivate::setVisualController(QAbstractController3D *controller)
	{
		m_visualController = controller;

		QObject::connect(m_visualController, &QAbstractController3D::activeInputHandlerChanged, q_ptr,
			&QAbstractGraph3D::activeInputHandlerChanged);
		QObject::connect(m_visualController, &QAbstractController3D::activeThemeChanged, q_ptr,
			&QAbstractGraph3D::activeThemeChanged);
		QObject::connect(m_visualController, &QAbstractController3D::selectionModeChanged, q_ptr,
			&QAbstractGraph3D::selectionModeChanged);
		QObject::connect(m_visualController, &QAbstractController3D::shadowQualityChanged, q_ptr,
			&QAbstractGraph3D::shadowQualityChanged);
		QObject::connect(m_visualController, &QAbstractController3D::optimizationHintsChanged, q_ptr,
			&QAbstractGraph3D::optimizationHintsChanged);
		QObject::connect(m_visualController, &QAbstractController3D::elementSelected, q_ptr,
			&QAbstractGraph3D::selectedElementChanged);

		QObject::connect(m_visualController, &QAbstractController3D::needRender, this,
			&QAbstractGraph3DPrivate::renderLater);

		QObject::connect(m_visualController, &QAbstractController3D::axisXChanged, this,
			&QAbstractGraph3DPrivate::handleAxisXChanged);
		QObject::connect(m_visualController, &QAbstractController3D::axisYChanged, this,
			&QAbstractGraph3DPrivate::handleAxisYChanged);
		QObject::connect(m_visualController, &QAbstractController3D::axisZChanged, this,
			&QAbstractGraph3DPrivate::handleAxisZChanged);

		QObject::connect(m_visualController, &QAbstractController3D::measureFpsChanged, q_ptr,
			&QAbstractGraph3D::measureFpsChanged);
		QObject::connect(m_visualController, &QAbstractController3D::currentFpsChanged, q_ptr,
			&QAbstractGraph3D::currentFpsChanged);

		QObject::connect(m_visualController, &QAbstractController3D::orthoProjectionChanged, q_ptr,
			&QAbstractGraph3D::orthoProjectionChanged);

		QObject::connect(m_visualController, &QAbstractController3D::aspectRatioChanged, q_ptr,
			&QAbstractGraph3D::aspectRatioChanged);
		QObject::connect(m_visualController, &QAbstractController3D::polarChanged, q_ptr,
			&QAbstractGraph3D::polarChanged);
		QObject::connect(m_visualController, &QAbstractController3D::radialLabelOffsetChanged, q_ptr,
			&QAbstractGraph3D::radialLabelOffsetChanged);
		QObject::connect(m_visualController, &QAbstractController3D::horizontalAspectRatioChanged, q_ptr,
			&QAbstractGraph3D::horizontalAspectRatioChanged);

		QObject::connect(m_visualController, &QAbstractController3D::reflectionChanged, q_ptr,
			&QAbstractGraph3D::reflectionChanged);
		QObject::connect(m_visualController, &QAbstractController3D::reflectivityChanged, q_ptr,
			&QAbstractGraph3D::reflectivityChanged);
		QObject::connect(m_visualController, &QAbstractController3D::localeChanged, q_ptr,
			&QAbstractGraph3D::localeChanged);
		QObject::connect(m_visualController, &QAbstractController3D::queriedGraphPositionChanged, q_ptr,
			&QAbstractGraph3D::queriedGraphPositionChanged);
		QObject::connect(m_visualController, &QAbstractController3D::marginChanged, q_ptr,
			&QAbstractGraph3D::marginChanged);
	}

	void QAbstractGraph3DPrivate::handleDevicePixelRatioChange()
	{
		if (q_ptr->devicePixelRatio() == m_devicePixelRatio || !m_visualController)
			return;

		m_devicePixelRatio = q_ptr->devicePixelRatio();
		m_visualController->scene()->setDevicePixelRatio(m_devicePixelRatio);
	}

	void QAbstractGraph3DPrivate::render()
	{
		handleDevicePixelRatioChange();
		m_visualController->synchDataToRenderer();
		m_visualController->render();
	}

	void QAbstractGraph3DPrivate::renderLater()
	{
		if (!m_updatePending) {
			m_updatePending = true;
			QCoreApplication::postEvent(q_ptr, new QEvent(QEvent::UpdateRequest));
		}
	}

	void QAbstractGraph3DPrivate::renderNow()
	{
		if (!q_ptr->isExposed())
			return;

		m_updatePending = false;

		m_context->makeCurrent(q_ptr);

		render();

		m_context->swapBuffers(q_ptr);
	}

	QImage QAbstractGraph3DPrivate::renderToImage(int msaaSamples, const QSize &imageSize)
	{
		QImage image;
		QOpenGLFramebufferObject *fbo;
		QOpenGLFramebufferObjectFormat fboFormat;
		if (!m_offscreenSurface) {
			// Create an offscreen surface for rendering to images without rendering on screen
			m_offscreenSurface = new QOffscreenSurface(q_ptr->screen());
			m_offscreenSurface->setFormat(q_ptr->requestedFormat());
			m_offscreenSurface->create();
		}
		// Render the wanted frame offscreen
		m_context->makeCurrent(m_offscreenSurface);
		fboFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
		if (!QUtils3D::isOpenGLES()) {
			fboFormat.setInternalTextureFormat(GL_RGB);
			fboFormat.setSamples(msaaSamples);
		}
		fbo = new QOpenGLFramebufferObject(imageSize, fboFormat);
		if (fbo->isValid()) {
			QRect originalViewport = m_visualController->m_scene->viewport();
			m_visualController->m_scene->d_ptr->setWindowSize(imageSize);
			m_visualController->m_scene->d_ptr->setViewport(QRect(0, 0,
				imageSize.width(),
				imageSize.height()));
			m_visualController->synchDataToRenderer();
			fbo->bind();
			m_visualController->requestRender(fbo);
			image = fbo->toImage();
			fbo->release();
			m_visualController->m_scene->d_ptr->setWindowSize(originalViewport.size());
			m_visualController->m_scene->d_ptr->setViewport(originalViewport);
		}
		delete fbo;
		m_context->makeCurrent(q_ptr);

		return image;
	}

}
