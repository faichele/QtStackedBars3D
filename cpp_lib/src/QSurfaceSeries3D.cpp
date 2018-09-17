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

#include "QSurfaceSeries3DPrivate.h"
#include "QSurfaceController3D.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QSurfaceSeries3D
	* \inmodule QtDataVisualization
	* \brief The QSurfaceSeries3D class represents a data series in a 3D surface
	* graph.
	* \since QtDataVisualization 1.0
	*
	* This class manages the series specific visual elements, as well as the series
	* data (via a data proxy).
	*
	* If no data proxy is set explicitly for the series, the series creates a default
	* proxy. Setting another proxy will destroy the existing proxy and all data added to it.
	*
	* The object mesh set via the QAbstractSeries3D::mesh property defines the selection
	* pointer shape in a surface series.
	*
	* QSurfaceSeries3D supports the following format tags for QAbstractSeries3D::setItemLabelFormat():
	* \table
	*   \row
	*     \li @xTitle    \li Title from x-axis
	*   \row
	*     \li @yTitle    \li Title from y-axis
	*   \row
	*     \li @zTitle    \li Title from z-axis
	*   \row
	*     \li @xLabel    \li Item value formatted using the format of the x-axis.
	*                        For more information, see
	*                        \l{QValueAxis3D::setLabelFormat()}.
	*   \row
	*     \li @yLabel    \li Item value formatted using the format of the y-axis.
	*                        For more information, see
	*                        \l{QValueAxis3D::setLabelFormat()}.
	*   \row
	*     \li @zLabel    \li Item value formatted using the format of the z-axis.
	*                        For more information, see
	*                        \l{QValueAxis3D::setLabelFormat()}.
	*   \row
	*     \li @seriesName \li Name of the series
	* \endtable
	*
	* For example:
	* \snippet doc_src_qtdatavisualization.cpp 1
	*
	* \sa {Qt Data Visualization Data Handling}
	*/

	/*!
	* \qmltype Surface3DSeries
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.0
	* \ingroup datavisualization_qml
	* \instantiates QSurfaceSeries3D
	* \inherits Abstract3DSeries
	* \brief Represents a data series in a 3D surface graph.
	*
	* This type manages the series specific visual elements, as well as the series
	* data (via a data proxy).
	*
	* For a more complete description, see QSurfaceSeries3D.
	*
	* \sa {Qt Data Visualization Data Handling}
	*/

	/*!
	* \qmlproperty SurfaceDataProxy Surface3DSeries::dataProxy
	*
	* The active data proxy. The series assumes ownership of any proxy set to
	* it and deletes any previously set proxy when a new one is added. The proxy cannot be null or
	* set to another series.
	*/

	/*!
	* \qmlproperty point Surface3DSeries::selectedPoint
	*
	* Sets the surface grid point in the position specified by a row and a column
	* in the data array of the series as selected.
	* Only one point can be selected at a time.
	*
	* To clear selection from this series, invalidSelectionPosition is set as the position.
	* If this series is added to a graph, the graph can adjust the selection according to user
	* interaction or if it becomes invalid.
	*
	* Removing rows from or inserting rows to the series before the row of the selected point
	* will adjust the selection so that the same point will stay selected.
	*
	* \sa AbstractGraph3D::clearSelection()
	*/

	/*!
	* \qmlproperty point Surface3DSeries::invalidSelectionPosition
	* A constant property providing an invalid selection position.
	* This position is set to the selectedPoint property to clear the selection
	* from this series.
	*
	* \sa AbstractGraph3D::clearSelection()
	*/

	/*!
	* \qmlproperty bool Surface3DSeries::flatShadingEnabled
	*
	* Sets surface flat shading to enabled. It is preset to \c true by default.
	* When disabled, the normals on the surface are interpolated making the edges look round.
	* When enabled, the normals are kept the same on a triangle making the color of the triangle solid.
	* This makes the data more readable from the model.
	* \note Flat shaded surfaces require at least GLSL version 1.2 with GL_EXT_gpu_shader4 extension.
	* The value of the flatShadingSupported property indicates whether flat shading
	* is supported at runtime.
	*/

	/*!
	* \qmlproperty bool Surface3DSeries::flatShadingSupported
	*
	* Indicates whether flat shading for surfaces is supported by the current system.
	* It requires at least GLSL version 1.2 with GL_EXT_gpu_shader4 extension.
	*
	* \note This read-only property is set to its correct value after the first
	* render pass. Until then it is always \c true.
	*/

	/*!
	* \qmlproperty DrawFlag Surface3DSeries::drawMode
	*
	* Sets the drawing mode to one of \l{QSurfaceSeries3D::DrawFlag}{Surface3DSeries.DrawFlag}.
	* Clearing all flags is not allowed.
	*/

	/*!
	* \qmlproperty string Surface3DSeries::textureFile
	*
	* The texture file name for the surface texture. To clear the texture, an empty
	* file name is set.
	*/


	/*!
	* \enum QSurfaceSeries3D::DrawFlag
	*
	* The drawing mode of the surface. Values of this enumeration can be combined
	* with the OR operator.
	*
	* \value DrawWireframe
	*        Only the grid is drawn.
	* \value DrawSurface
	*        Only the surface is drawn.
	* \value DrawSurfaceAndWireframe
	*        Both the surface and grid are drawn.
	*/

	/*!
	* Constructs a surface 3D series with the parent \a parent.
	*/
	QSurfaceSeries3D::QSurfaceSeries3D(QObject *parent) :
		QAbstractSeries3D(new QSurfaceSeries3DPrivate(this), parent)
	{
		// Default proxy
		dptr()->setDataProxy(new QSurfaceDataProxy3D);
	}

	/*!
	* Constructs a surface 3D series with the data proxy \a dataProxy and the
	* parent \a parent.
	*/
	QSurfaceSeries3D::QSurfaceSeries3D(QSurfaceDataProxy3D *dataProxy, QObject *parent) :
		QAbstractSeries3D(new QSurfaceSeries3DPrivate(this), parent)
	{
		dptr()->setDataProxy(dataProxy);
	}

	/*!
	* \internal
	*/
	QSurfaceSeries3D::QSurfaceSeries3D(QSurfaceSeries3DPrivate *d, QObject *parent) :
		QAbstractSeries3D(d, parent)
	{
	}

	/*!
	* Deletes the surface 3D series.
	*/
	QSurfaceSeries3D::~QSurfaceSeries3D()
	{
	}

	/*!
	* \property QSurfaceSeries3D::dataProxy
	*
	* \brief The active data proxy.
	*
	* The series assumes ownership of any proxy set to it and deletes any
	* previously set proxy when a new one is added. The proxy cannot be null or
	* set to another series.
	*/
	void QSurfaceSeries3D::setDataProxy(QSurfaceDataProxy3D *proxy)
	{
		d_ptr->setDataProxy(proxy);
	}

	QSurfaceDataProxy3D *QSurfaceSeries3D::dataProxy() const
	{
		return static_cast<QSurfaceDataProxy3D *>(d_ptr->dataProxy());
	}

	/*!
	* \property QSurfaceSeries3D::selectedPoint
	*
	* \brief The surface grid point that is selected in the series.
	*/

	/*!
	* Selects a surface grid point at the position \a position in the data array of
	* the series specified by a row and a column.
	*
	* Only one point can be selected at a time.
	*
	* To clear selection from this series, invalidSelectionPosition() is set as \a position.
	* If this series is added to a graph, the graph can adjust the selection according to user
	* interaction or if it becomes invalid.
	*
	* Removing rows from or inserting rows to the series before the row of the selected point
	* will adjust the selection so that the same point will stay selected.
	*
	* \sa QAbstractGraph3D::clearSelection()
	*/
	void QSurfaceSeries3D::setSelectedPoint(const QPoint &position)
	{
		// Don't do this in private to avoid loops, as that is used for callback from controller.
		if (d_ptr->m_controller)
			static_cast<QSurfaceController3D *>(d_ptr->m_controller)->setSelectedPoint(position, this, true);
		else
			dptr()->setSelectedPoint(position);
	}

	QPoint QSurfaceSeries3D::selectedPoint() const
	{
		return dptrc()->m_selectedPoint;
	}

	/*!
	* Returns the QPoint signifying an invalid selection position. This is set to
	* the selectedPoint property to clear the selection from this series.
	*
	* \sa QAbstractGraph3D::clearSelection()
	*/
	QPoint QSurfaceSeries3D::invalidSelectionPosition()
	{
		return QSurfaceController3D::invalidSelectionPosition();
	}

	/*!
	* \property QSurfaceSeries3D::flatShadingEnabled
	*
	* \brief Whether surface flat shading is enabled.
	*
	* Preset to \c true by default.
	*
	* When disabled, the normals on the surface are interpolated making the edges look round.
	* When enabled, the normals are kept the same on a triangle making the color of the triangle solid.
	* This makes the data more readable from the model.
	* \note Flat shaded surfaces require at least GLSL version 1.2 with GL_EXT_gpu_shader4 extension.
	* The value of the flatShadingSupported property indicates whether flat shading
	* is supported at runtime.
	*/
	void QSurfaceSeries3D::setFlatShadingEnabled(bool enabled)
	{
		if (dptr()->m_flatShadingEnabled != enabled) {
			dptr()->setFlatShadingEnabled(enabled);
			emit flatShadingEnabledChanged(enabled);
		}
	}

	bool QSurfaceSeries3D::isFlatShadingEnabled() const
	{
		return dptrc()->m_flatShadingEnabled;
	}

	/*!
	* \property QSurfaceSeries3D::flatShadingSupported
	*
	* \brief Whether surface flat shading is supported by the current system.
	*
	* Flat shading for surfaces requires at least GLSL version 1.2 with GL_EXT_gpu_shader4 extension.
	* If \c true, flat shading for surfaces is supported.
	* \note This read-only property is set to its correct value after the first
	* render pass. Until then it is always \c true.
	*/
	bool QSurfaceSeries3D::isFlatShadingSupported() const
	{
		if (d_ptr->m_controller)
			return static_cast<QSurfaceController3D *>(d_ptr->m_controller)->isFlatShadingSupported();
		else
			return true;
	}

	/*!
	* \property QSurfaceSeries3D::drawMode
	*
	* The drawing mode.
	*
	* Possible values are the values of DrawFlag. Clearing all flags is not allowed.
	*/
	void QSurfaceSeries3D::setDrawMode(DrawFlags mode)
	{
		if (dptr()->m_drawMode != mode) {
			dptr()->setDrawMode(mode);
			emit drawModeChanged(mode);
		}
	}

	QSurfaceSeries3D::DrawFlags QSurfaceSeries3D::drawMode() const
	{
		return dptrc()->m_drawMode;
	}

	/*!
	* \property QSurfaceSeries3D::texture
	*
	* \brief The texture for the surface as a QImage.
	*
	* Setting an empty QImage clears the texture.
	*/
	void QSurfaceSeries3D::setTexture(const QImage &texture)
	{
		if (dptr()->m_texture != texture) {
			dptr()->setTexture(texture);

			emit textureChanged(texture);
			dptr()->m_textureFile.clear();
		}
	}

	QImage QSurfaceSeries3D::texture() const
	{
		return dptrc()->m_texture;
	}

	/*!
	* \property QSurfaceSeries3D::textureFile
	*
	* \brief The texture for the surface as a file.
	*
	* Setting an empty file name clears the texture.
	*/
	void QSurfaceSeries3D::setTextureFile(const QString &filename)
	{
		if (dptr()->m_textureFile != filename) {
			if (filename.isEmpty()) {
				setTexture(QImage());
			}
			else {
				QImage image(filename);
				if (image.isNull()) {
					qWarning() << "Warning: Tried to set invalid image file as surface texture.";
					return;
				}
				setTexture(image);
			}

			dptr()->m_textureFile = filename;
			emit textureFileChanged(filename);
		}
	}

	QString QSurfaceSeries3D::textureFile() const
	{
		return dptrc()->m_textureFile;
	}

	/*!
	* \internal
	*/
	QSurfaceSeries3DPrivate *QSurfaceSeries3D::dptr()
	{
		return static_cast<QSurfaceSeries3DPrivate *>(d_ptr.data());
	}

	/*!
	* \internal
	*/
	const QSurfaceSeries3DPrivate *QSurfaceSeries3D::dptrc() const
	{
		return static_cast<const QSurfaceSeries3DPrivate *>(d_ptr.data());
	}

	// QSurfaceSeries3DPrivate

	QSurfaceSeries3DPrivate::QSurfaceSeries3DPrivate(QSurfaceSeries3D *q)
		: QAbstractSeries3DPrivate(q, QAbstractSeries3D::SeriesTypeSurface),
		m_selectedPoint(QSurfaceController3D::invalidSelectionPosition()),
		m_flatShadingEnabled(true),
		m_drawMode(QSurfaceSeries3D::DrawSurfaceAndWireframe)
	{
		m_itemLabelFormat = QStringLiteral("@xLabel, @yLabel, @zLabel");
		m_mesh = QAbstractSeries3D::MeshSphere;
	}

	QSurfaceSeries3DPrivate::~QSurfaceSeries3DPrivate()
	{
	}

	QSurfaceSeries3D *QSurfaceSeries3DPrivate::qptr()
	{
		return static_cast<QSurfaceSeries3D *>(q_ptr);
	}

	void QSurfaceSeries3DPrivate::setDataProxy(QAbstractDataProxy3D *proxy)
	{
		Q_ASSERT(proxy->type() == QAbstractDataProxy3D::DataTypeSurface);

		QAbstractSeries3DPrivate::setDataProxy(proxy);

		emit qptr()->dataProxyChanged(static_cast<QSurfaceDataProxy3D *>(proxy));
	}

	void QSurfaceSeries3DPrivate::connectControllerAndProxy(QAbstractController3D *newController)
	{
		QSurfaceDataProxy3D *surfaceDataProxy = static_cast<QSurfaceDataProxy3D *>(m_dataProxy);

		if (m_controller && surfaceDataProxy) {
			//Disconnect old controller/old proxy
			QObject::disconnect(surfaceDataProxy, 0, m_controller, 0);
			QObject::disconnect(q_ptr, 0, m_controller, 0);
		}

		if (newController && surfaceDataProxy) {
			QSurfaceController3D *controller = static_cast<QSurfaceController3D *>(newController);

			QObject::connect(surfaceDataProxy, &QSurfaceDataProxy3D::arrayReset, controller,
				&QSurfaceController3D::handleArrayReset);
			QObject::connect(surfaceDataProxy, &QSurfaceDataProxy3D::rowsAdded, controller,
				&QSurfaceController3D::handleRowsAdded);
			QObject::connect(surfaceDataProxy, &QSurfaceDataProxy3D::rowsChanged, controller,
				&QSurfaceController3D::handleRowsChanged);
			QObject::connect(surfaceDataProxy, &QSurfaceDataProxy3D::rowsRemoved, controller,
				&QSurfaceController3D::handleRowsRemoved);
			QObject::connect(surfaceDataProxy, &QSurfaceDataProxy3D::rowsInserted, controller,
				&QSurfaceController3D::handleRowsInserted);
			QObject::connect(surfaceDataProxy, &QSurfaceDataProxy3D::itemChanged, controller,
				&QSurfaceController3D::handleItemChanged);
			QObject::connect(qptr(), &QSurfaceSeries3D::dataProxyChanged, controller,
				&QSurfaceController3D::handleArrayReset);
		}
	}

	void QSurfaceSeries3DPrivate::createItemLabel()
	{
		static const QString xTitleTag(QStringLiteral("@xTitle"));
		static const QString yTitleTag(QStringLiteral("@yTitle"));
		static const QString zTitleTag(QStringLiteral("@zTitle"));
		static const QString xLabelTag(QStringLiteral("@xLabel"));
		static const QString yLabelTag(QStringLiteral("@yLabel"));
		static const QString zLabelTag(QStringLiteral("@zLabel"));
		static const QString seriesNameTag(QStringLiteral("@seriesName"));

		if (m_selectedPoint == QSurfaceSeries3D::invalidSelectionPosition()) {
			m_itemLabel = QString();
			return;
		}

		QValueAxis3D *axisX = static_cast<QValueAxis3D *>(m_controller->axisX());
		QValueAxis3D *axisY = static_cast<QValueAxis3D *>(m_controller->axisY());
		QValueAxis3D *axisZ = static_cast<QValueAxis3D *>(m_controller->axisZ());
		QVector3D selectedPosition = qptr()->dataProxy()->itemAt(m_selectedPoint)->position();

		m_itemLabel = m_itemLabelFormat;

		m_itemLabel.replace(xTitleTag, axisX->title());
		m_itemLabel.replace(yTitleTag, axisY->title());
		m_itemLabel.replace(zTitleTag, axisZ->title());

		if (m_itemLabel.contains(xLabelTag)) {
			QString valueLabelText = axisX->formatter()->stringForValue(
				qreal(selectedPosition.x()), axisX->labelFormat());
			m_itemLabel.replace(xLabelTag, valueLabelText);
		}
		if (m_itemLabel.contains(yLabelTag)) {
			QString valueLabelText = axisY->formatter()->stringForValue(
				qreal(selectedPosition.y()), axisY->labelFormat());
			m_itemLabel.replace(yLabelTag, valueLabelText);
		}
		if (m_itemLabel.contains(zLabelTag)) {
			QString valueLabelText = axisZ->formatter()->stringForValue(
				qreal(selectedPosition.z()), axisZ->labelFormat());
			m_itemLabel.replace(zLabelTag, valueLabelText);
		}
		m_itemLabel.replace(seriesNameTag, m_name);
	}

	void QSurfaceSeries3DPrivate::setSelectedPoint(const QPoint &position)
	{
		if (position != m_selectedPoint) {
			markItemLabelDirty();
			m_selectedPoint = position;
			emit qptr()->selectedPointChanged(m_selectedPoint);
		}
	}

	void QSurfaceSeries3DPrivate::setFlatShadingEnabled(bool enabled)
	{
		m_flatShadingEnabled = enabled;
		if (m_controller)
			m_controller->markSeriesVisualsDirty();
	}

	void QSurfaceSeries3DPrivate::setDrawMode(QSurfaceSeries3D::DrawFlags mode)
	{
		if (mode.testFlag(QSurfaceSeries3D::DrawWireframe)
			|| mode.testFlag(QSurfaceSeries3D::DrawSurface)) {
			m_drawMode = mode;
			if (m_controller)
				m_controller->markSeriesVisualsDirty();
		}
		else {
			qWarning("You may not clear all draw flags. Mode not changed.");
		}
	}

	void QSurfaceSeries3DPrivate::setTexture(const QImage &texture)
	{
		m_texture = texture;
		if (static_cast<QSurfaceController3D *>(m_controller))
			static_cast<QSurfaceController3D *>(m_controller)->updateSurfaceTexture(qptr());
	}

}
