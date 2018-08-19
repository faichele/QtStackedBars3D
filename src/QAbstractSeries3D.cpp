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

#include "QAbstractSeries3DPrivate.h"
#include "QAbstractDataProxy3DPrivate.h"
#include "QAbstractController3D.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QAbstractSeries3D
	* \inmodule QtDataVisualization
	* \brief The QAbstractSeries3D class is a base class for all data series.
	* \since QtDataVisualization 1.0
	*
	* There are inherited classes for each supported series type: QBarSeries3D,
	* QScatterSeries3D, and QSurfaceSeries3D.
	*
	* For more information, see \l{Qt Data Visualization Data Handling}.
	*/

	/*!
	* \class QAbstractSeriesChangeBitField3D
	* \internal
	*/

	/*!
	* \class QAbstract3DSeriesThemeOverrideBitField
	* \internal
	*/

	/*!
	* \qmltype Abstract3DSeries
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.0
	* \ingroup datavisualization_qml
	* \instantiates QAbstractSeries3D
	* \brief A base type for all data series.
	*
	* This type is uncreatable, but contains properties that are exposed via the
	* following subtypes: Bar3DSeries, Scatter3DSeries, and Surface3DSeries.
	*
	* For more information, see \l{Qt Data Visualization Data Handling}.
	*/

	/*!
	* \enum QAbstractSeries3D::SeriesType
	*
	* Type of the series.
	*
	* \value SeriesTypeNone
	*        No series type.
	* \value SeriesTypeBar
	*        Series type for QBars3D.
	* \value SeriesTypeScatter
	*        Series type for Q3DScatter.
	* \value SeriesTypeSurface
	*        Series type for Q3DSurface.
	*/

	/*!
	*  \enum QAbstractSeries3D::Mesh
	*
	*  Predefined mesh types. All styles are not usable with all visualization types.
	*
	*  \value MeshUserDefined
	*         User defined mesh, set via QAbstractSeries3D::userDefinedMesh property.
	*  \value MeshBar
	*         Basic rectangular bar.
	*  \value MeshCube
	*         Basic cube.
	*  \value MeshPyramid
	*         Four-sided pyramid.
	*  \value MeshCone
	*         Basic cone.
	*  \value MeshCylinder
	*         Basic cylinder.
	*  \value MeshBevelBar
	*         Slightly beveled (rounded) rectangular bar.
	*  \value MeshBevelCube
	*         Slightly beveled (rounded) cube.
	*  \value MeshSphere
	*         Sphere.
	*  \value MeshMinimal
	*         The minimal 3D mesh: a triangular pyramid. Usable only with Q3DScatter.
	*  \value MeshArrow
	*         Arrow pointing upwards.
	*  \value MeshPoint
	*         2D point. Usable only with Q3DScatter.
	*         Shadows do not affect this style. Color style QTheme3D::ColorStyleObjectGradient
	*         is not supported by this style.
	*/

	/*!
	* \qmlproperty Abstract3DSeries.SeriesType Abstract3DSeries::type
	* The type of the series. One of the QAbstractSeries3D::SeriesType values.
	*
	*/

	/*!
	* \qmlproperty string Abstract3DSeries::itemLabelFormat
	*
	* The label format for data items in this series. This format is used for single item labels,
	* for example, when an item is selected. How the format is interpreted depends
	* on series type: Bar3DSeries, Scatter3DSeries, Surface3DSeries.
	*/

	/*!
	* \qmlproperty bool Abstract3DSeries::visible
	* Sets the visibility of the series. If \c false, the series is not rendered.
	*/

	/*!
	* \qmlproperty Abstract3DSeries.Mesh Abstract3DSeries::mesh
	*
	* Sets the mesh of the items in the series, or the selection pointer in case of
	* Surface3DSeries. If the mesh is \l{QAbstractSeries3D::MeshUserDefined}{Abstract3DSeries.MeshUserDefined},
	* then the userDefinedMesh property must also be set for items to render properly.
	* The default value depends on the graph type.
	*
	* \sa QAbstractSeries3D::Mesh
	*/

	/*!
	* \qmlproperty bool Abstract3DSeries::meshSmooth
	*
	* If \c true, smooth versions of predefined meshes set via the \l mesh property are used.
	* This property does not affect custom meshes used when the mesh is set to
	* \l{QAbstractSeries3D::MeshUserDefined}{Abstract3DSeries.MeshUserDefined}.
	* Defaults to \c{false}.
	*/

	/*!
	* \qmlproperty quaternion Abstract3DSeries::meshRotation
	*
	* Sets the mesh rotation that is applied to all items of the series.
	* The rotation should be a normalized quaternion.
	* For those series types that support item specific rotation, the rotations are
	* multiplied together.
	* Bar3DSeries ignores any rotation that is not around the y-axis.
	* Surface3DSeries applies the rotation only to the selection pointer.
	* Defaults to no rotation.
	*/

	/*!
	* \qmlproperty string Abstract3DSeries::userDefinedMesh
	*
	* Sets the filename for a user defined custom mesh for objects that is used when \l mesh
	* is \l{QAbstractSeries3D::MeshUserDefined}{Abstract3DSeries.MeshUserDefined}.
	* \note The file needs to be in the Wavefront OBJ format and include
	* vertices, normals, and UVs. It also needs to be in triangles.
	*/

	/*!
	* \qmlproperty Theme3D.ColorStyle Abstract3DSeries::colorStyle
	*
	* Sets the color style for the series.
	*
	* \sa {Theme3D::colorStyle}{Theme3D.colorStyle}
	*/

	/*!
	* \qmlproperty color Abstract3DSeries::baseColor
	*
	* Sets the base color of the series.
	*
	* \sa colorStyle, {Theme3D::baseColors}{Theme3D.baseColors}
	*/

	/*!
	* \qmlproperty ColorGradient Abstract3DSeries::baseGradient
	*
	* Sets the base gradient of the series.
	*
	* \sa colorStyle, {Theme3D::baseGradients}{Theme3D.baseGradients}
	*/

	/*!
	* \qmlproperty color Abstract3DSeries::singleHighlightColor
	*
	* Sets the single item highlight color of the series.
	*
	* \sa colorStyle, {Theme3D::singleHighlightColor}{Theme3D.singleHighlightColor}
	*/

	/*!
	* \qmlproperty ColorGradient Abstract3DSeries::singleHighlightGradient
	*
	* Sets the single item highlight gradient of the series.
	*
	* \sa colorStyle, {Theme3D::singleHighlightGradient}{Theme3D.singleHighlightGradient}
	*/

	/*!
	* \qmlproperty color Abstract3DSeries::multiHighlightColor
	*
	* Sets the multiple item highlight color of the series.
	*
	* \sa colorStyle, {Theme3D::multiHighlightColor}{Theme3D.multiHighlightColor}
	*/

	/*!
	* \qmlproperty ColorGradient Abstract3DSeries::multiHighlightGradient
	*
	* Sets the multiple item highlight gradient of the series.
	*
	* \sa colorStyle, {Theme3D::multiHighlightGradient}{Theme3D.multiHighlightGradient}
	*/

	/*!
	* \qmlproperty string Abstract3DSeries::name
	*
	* The series name.
	* It can be used in item label format with the tag \c{@seriesName}.
	*
	* \sa itemLabelFormat
	*/

	/*!
	* \qmlproperty string Abstract3DSeries::itemLabel
	* \since QtDataVisualization 1.1
	*
	* The formatted item label. If there is no selected item or the selected item is not
	* visible, returns an empty string.
	*
	* \sa itemLabelFormat
	*/

	/*!
	* \qmlproperty bool Abstract3DSeries::itemLabelVisible
	* \since QtDataVisualization 1.1
	*
	* If \c true, item labels are drawn as floating labels in the graph. Otherwise,
	* item labels are not drawn. To show the item label in an external control,
	* this property is set to \c false. Defaults to \c true.
	*
	* \sa itemLabelFormat, itemLabel
	*/

	/*!
	* \qmlmethod void Abstract3DSeries::setMeshAxisAndAngle(vector3d axis, real angle)
	*
	* A convenience function to construct a mesh rotation quaternion from \a axis
	* and \a angle.
	*
	* \sa meshRotation
	*/

	/*!
	* \internal
	*/
	QAbstractSeries3D::QAbstractSeries3D(QAbstractSeries3DPrivate *d, QObject *parent) :
		QObject(parent),
		d_ptr(d)
	{
	}

	/*!
	* Deletes the abstract 3D series.
	*/
	QAbstractSeries3D::~QAbstractSeries3D()
	{
	}

	/*!
	* \property QAbstractSeries3D::type
	*
	* \brief The type of the series.
	*/
	QAbstractSeries3D::SeriesType QAbstractSeries3D::type() const
	{
		return d_ptr->m_type;
	}

	/*!
	* \property QAbstractSeries3D::itemLabelFormat
	*
	* \brief The label format for data items in this series.
	*
	* This format is used for single item labels,
	* for example, when an item is selected. How the format is interpreted depends
	* on series type: QBarSeries3D, QScatterSeries3D, QSurfaceSeries3D.
	*/
	void QAbstractSeries3D::setItemLabelFormat(const QString &format)
	{
		if (d_ptr->m_itemLabelFormat != format) {
			d_ptr->setItemLabelFormat(format);
			emit itemLabelFormatChanged(format);
		}
	}

	QString QAbstractSeries3D::itemLabelFormat() const
	{
		return d_ptr->m_itemLabelFormat;
	}

	/*!
	* \property QAbstractSeries3D::visible
	*
	* \brief The visibility of the series.
	*
	* If this property is \c false, the series is not rendered.
	* Defaults to \c{true}.
	*/
	void QAbstractSeries3D::setVisible(bool visible)
	{
		if (d_ptr->m_visible != visible) {
			d_ptr->setVisible(visible);
			emit visibilityChanged(visible);
		}
	}

	bool QAbstractSeries3D::isVisible() const
	{
		return d_ptr->m_visible;
	}

	/*!
	* \property QAbstractSeries3D::mesh
	*
	* \brief The mesh of the items in the series.
	*
	* For QSurfaceSeries3D, this property holds the selection pointer.
	*
	* If the mesh is MeshUserDefined, then the userDefinedMesh property
	* must also be set for items to render properly. The default value depends on the graph type.
	*/
	void QAbstractSeries3D::setMesh(QAbstractSeries3D::Mesh mesh)
	{
		if ((mesh == QAbstractSeries3D::MeshPoint || mesh == QAbstractSeries3D::MeshMinimal
			|| mesh == QAbstractSeries3D::MeshArrow)
			&& type() != QAbstractSeries3D::SeriesTypeScatter) {
			qWarning() << "Specified style is only supported for QScatterSeries3D.";
		}
		else if (d_ptr->m_mesh != mesh) {
			d_ptr->setMesh(mesh);
			emit meshChanged(mesh);
		}
	}

	QAbstractSeries3D::Mesh QAbstractSeries3D::mesh() const
	{
		return d_ptr->m_mesh;
	}

	/*!
	* \property QAbstractSeries3D::meshSmooth
	*
	* \brief Whether smooth versions of predefined meshes are used.
	*
	* If \c true, smooth versions set via the \l mesh property are used.
	* This property does not affect custom meshes used when the mesh is set to
	* MeshUserDefined. Defaults to \c{false}.
	*/
	void QAbstractSeries3D::setMeshSmooth(bool enable)
	{
		if (d_ptr->m_meshSmooth != enable) {
			d_ptr->setMeshSmooth(enable);
			emit meshSmoothChanged(enable);
		}
	}

	bool QAbstractSeries3D::isMeshSmooth() const
	{
		return d_ptr->m_meshSmooth;
	}

	/*!
	* \property QAbstractSeries3D::meshRotation
	*
	* \brief The mesh rotation that is applied to all items of the series.
	*
	* The rotation should be a normalized QQuaternion.
	* For those series types that support item specific rotation, the rotations are
	* multiplied together.
	* QBarSeries3D ignores any rotation that is not around the y-axis.
	* QSurfaceSeries3D applies the rotation only to the selection pointer.
	* Defaults to no rotation.
	*/
	void QAbstractSeries3D::setMeshRotation(const QQuaternion &rotation)
	{
		if (d_ptr->m_meshRotation != rotation) {
			d_ptr->setMeshRotation(rotation);
			emit meshRotationChanged(rotation);
		}
	}

	QQuaternion QAbstractSeries3D::meshRotation() const
	{
		return d_ptr->m_meshRotation;
	}

	/*!
	* A convenience function to construct a mesh rotation quaternion from
	* \a axis and \a angle.
	*
	* \sa meshRotation
	*/
	void QAbstractSeries3D::setMeshAxisAndAngle(const QVector3D &axis, float angle)
	{
		setMeshRotation(QQuaternion::fromAxisAndAngle(axis, angle));
	}

	/*!
	* \property QAbstractSeries3D::userDefinedMesh
	*
	* \brief The filename for a user defined custom mesh for objects.
	*
	* The custom mesh is used when \l mesh is MeshUserDefined.
	* \note The file needs to be in the Wavefront OBJ format and include
	* vertices, normals, and UVs. It also needs to be in triangles.
	*/
	void QAbstractSeries3D::setUserDefinedMesh(const QString &fileName)
	{
		if (d_ptr->m_userDefinedMesh != fileName) {
			d_ptr->setUserDefinedMesh(fileName);
			emit userDefinedMeshChanged(fileName);
		}
	}

	QString QAbstractSeries3D::userDefinedMesh() const
	{
		return d_ptr->m_userDefinedMesh;
	}

	/*!
	* \property QAbstractSeries3D::colorStyle
	*
	* \brief The color style for the series.
	*
	* \sa QTheme3D::ColorStyle
	*/
	void QAbstractSeries3D::setColorStyle(QTheme3D::ColorStyle style)
	{
		if (d_ptr->m_colorStyle != style) {
			d_ptr->setColorStyle(style);
			emit colorStyleChanged(style);
		}
		d_ptr->m_themeTracker.colorStyleOverride = true;
	}

	QTheme3D::ColorStyle QAbstractSeries3D::colorStyle() const
	{
		return d_ptr->m_colorStyle;
	}

	/*!
	* \property QAbstractSeries3D::baseColor
	*
	* \brief The base color of the series.
	*
	* \sa colorStyle, QTheme3D::baseColors
	*/
	void QAbstractSeries3D::setBaseColor(const QColor &color)
	{
		if (d_ptr->m_baseColor != color) {
			d_ptr->setBaseColor(color);
			emit baseColorChanged(color);
		}
		d_ptr->m_themeTracker.baseColorOverride = true;
	}

	QColor QAbstractSeries3D::baseColor() const
	{
		return d_ptr->m_baseColor;
	}

	/*!
	* \property QAbstractSeries3D::baseGradient
	*
	* \brief The base gradient of the series.
	*
	* \sa colorStyle, QTheme3D::baseGradients
	*/
	void QAbstractSeries3D::setBaseGradient(const QLinearGradient &gradient)
	{
		if (d_ptr->m_baseGradient != gradient) {
			d_ptr->setBaseGradient(gradient);
			emit baseGradientChanged(gradient);
		}
		d_ptr->m_themeTracker.baseGradientOverride = true;
	}

	QLinearGradient QAbstractSeries3D::baseGradient() const
	{
		return d_ptr->m_baseGradient;
	}

	/*!
	* \property QAbstractSeries3D::singleHighlightColor
	*
	* \brief The single item highlight color of the series.
	*
	* \sa colorStyle, QTheme3D::singleHighlightColor
	*/
	void QAbstractSeries3D::setSingleHighlightColor(const QColor &color)
	{
		if (d_ptr->m_singleHighlightColor != color) {
			d_ptr->setSingleHighlightColor(color);
			emit singleHighlightColorChanged(color);
		}
		d_ptr->m_themeTracker.singleHighlightColorOverride = true;
	}

	QColor QAbstractSeries3D::singleHighlightColor() const
	{
		return d_ptr->m_singleHighlightColor;
	}

	/*!
	* \property QAbstractSeries3D::singleHighlightGradient
	*
	* \brief The single item highlight gradient of the series.
	*
	* \sa colorStyle, QTheme3D::singleHighlightGradient
	*/
	void QAbstractSeries3D::setSingleHighlightGradient(const QLinearGradient &gradient)
	{
		if (d_ptr->m_singleHighlightGradient != gradient) {
			d_ptr->setSingleHighlightGradient(gradient);
			emit singleHighlightGradientChanged(gradient);
		}
		d_ptr->m_themeTracker.singleHighlightGradientOverride = true;
	}

	QLinearGradient QAbstractSeries3D::singleHighlightGradient() const
	{
		return d_ptr->m_singleHighlightGradient;
	}

	/*!
	* \property QAbstractSeries3D::multiHighlightColor
	*
	* \brief The multiple item highlight color of the series.
	*
	* \sa colorStyle, QTheme3D::multiHighlightColor
	*/
	void QAbstractSeries3D::setMultiHighlightColor(const QColor &color)
	{
		if (d_ptr->m_multiHighlightColor != color) {
			d_ptr->setMultiHighlightColor(color);
			emit multiHighlightColorChanged(color);
		}
		d_ptr->m_themeTracker.multiHighlightColorOverride = true;
	}

	QColor QAbstractSeries3D::multiHighlightColor() const
	{
		return d_ptr->m_multiHighlightColor;
	}

	/*!
	* \property QAbstractSeries3D::multiHighlightGradient
	*
	* \brief The multiple item highlight gradient of the series.
	*
	* \sa colorStyle, QTheme3D::multiHighlightGradient
	*/
	void QAbstractSeries3D::setMultiHighlightGradient(const QLinearGradient &gradient)
	{
		if (d_ptr->m_multiHighlightGradient != gradient) {
			d_ptr->setMultiHighlightGradient(gradient);
			emit multiHighlightGradientChanged(gradient);
		}
		d_ptr->m_themeTracker.multiHighlightGradientOverride = true;
	}

	QLinearGradient QAbstractSeries3D::multiHighlightGradient() const
	{
		return d_ptr->m_multiHighlightGradient;
	}

	/*!
	* \property QAbstractSeries3D::name
	*
	* \brief The series name.
	*
	* The series name can be used in item label format with the tag \c{@seriesName}.
	*
	* \sa itemLabelFormat
	*/
	void QAbstractSeries3D::setName(const QString &name)
	{
		if (d_ptr->m_name != name) {
			d_ptr->setName(name);
			emit nameChanged(name);
		}
	}

	QString QAbstractSeries3D::name() const
	{
		return d_ptr->m_name;
	}

	/*!
	* \property QAbstractSeries3D::itemLabel
	* \since QtDataVisualization 1.1
	*
	* \brief The formatted item label.
	*
	* If there is no selected item or the selected item is not
	* visible, returns an empty string.
	*
	* \sa itemLabelFormat
	*/
	QString QAbstractSeries3D::itemLabel() const
	{
		return d_ptr->itemLabel();
	}

	/*!
	* \property QAbstractSeries3D::itemLabelVisible
	* \since QtDataVisualization 1.1
	*
	* \brief The visibility of item labels in the graph.
	*
	* If \c true, item labels are drawn as floating labels in the graph. Otherwise,
	* item labels are not drawn. To show the item label in an external control,
	* this property is set to \c false. Defaults to \c true.
	*
	* \sa itemLabelFormat, itemLabel
	*/
	void QAbstractSeries3D::setItemLabelVisible(bool visible)
	{
		if (d_ptr->m_itemLabelVisible != visible) {
			d_ptr->setItemLabelVisible(visible);
			emit itemLabelVisibilityChanged(visible);
		}
	}

	bool QAbstractSeries3D::isItemLabelVisible() const
	{
		return d_ptr->m_itemLabelVisible;
	}

	// QAbstractSeries3DPrivate

	QAbstractSeries3DPrivate::QAbstractSeries3DPrivate(QAbstractSeries3D *q,
		QAbstractSeries3D::SeriesType type)
		: QObject(0),
		q_ptr(q),
		m_type(type),
		m_dataProxy(0),
		m_visible(true),
		m_controller(0),
		m_mesh(QAbstractSeries3D::MeshCube),
		m_meshSmooth(false),
		m_colorStyle(QTheme3D::ColorStyleUniform),
		m_baseColor(Qt::black),
		m_singleHighlightColor(Qt::black),
		m_multiHighlightColor(Qt::black),
		m_itemLabelDirty(true),
		m_itemLabelVisible(true)
	{
	}

	QAbstractSeries3DPrivate::~QAbstractSeries3DPrivate()
	{
	}

	QAbstractDataProxy3D *QAbstractSeries3DPrivate::dataProxy() const
	{
		return m_dataProxy;
	}

	void QAbstractSeries3DPrivate::setDataProxy(QAbstractDataProxy3D *proxy)
	{
		Q_ASSERT(proxy && proxy != m_dataProxy && !proxy->d_ptr->series());

		delete m_dataProxy;
		m_dataProxy = proxy;

		proxy->d_ptr->setSeries(q_ptr); // also sets parent

		if (m_controller) {
			connectControllerAndProxy(m_controller);
			m_controller->markDataDirty();
		}
	}

	void QAbstractSeries3DPrivate::setController(QAbstractController3D *controller)
	{
		connectControllerAndProxy(controller);
		m_controller = controller;
		q_ptr->setParent(controller);
		markItemLabelDirty();
	}

	void QAbstractSeries3DPrivate::setItemLabelFormat(const QString &format)
	{
		m_itemLabelFormat = format;
		markItemLabelDirty();
	}

	void QAbstractSeries3DPrivate::setVisible(bool visible)
	{
		m_visible = visible;
		markItemLabelDirty();
	}

	void QAbstractSeries3DPrivate::setMesh(QAbstractSeries3D::Mesh mesh)
	{
		m_mesh = mesh;
		m_changeTracker.meshChanged = true;
		if (m_controller) {
			m_controller->markSeriesVisualsDirty();

			if (m_controller->optimizationHints().testFlag(QAbstractGraph3D::OptimizationStatic))
				m_controller->markDataDirty();
		}
	}

	void QAbstractSeries3DPrivate::setMeshSmooth(bool enable)
	{
		m_meshSmooth = enable;
		m_changeTracker.meshSmoothChanged = true;
		if (m_controller) {
			m_controller->markSeriesVisualsDirty();

			if (m_controller->optimizationHints().testFlag(QAbstractGraph3D::OptimizationStatic))
				m_controller->markDataDirty();
		}
	}

	void QAbstractSeries3DPrivate::setMeshRotation(const QQuaternion &rotation)
	{
		m_meshRotation = rotation;
		m_changeTracker.meshRotationChanged = true;
		if (m_controller) {
			m_controller->markSeriesVisualsDirty();

			if (m_controller->optimizationHints().testFlag(QAbstractGraph3D::OptimizationStatic))
				m_controller->markDataDirty();
		}
	}

	void QAbstractSeries3DPrivate::setUserDefinedMesh(const QString &meshFile)
	{
		m_userDefinedMesh = meshFile;
		m_changeTracker.userDefinedMeshChanged = true;
		if (m_controller) {
			m_controller->markSeriesVisualsDirty();

			if (m_controller->optimizationHints().testFlag(QAbstractGraph3D::OptimizationStatic))
				m_controller->markDataDirty();
		}
	}

	void QAbstractSeries3DPrivate::setColorStyle(QTheme3D::ColorStyle style)
	{
		m_colorStyle = style;
		m_changeTracker.colorStyleChanged = true;
		if (m_controller)
			m_controller->markSeriesVisualsDirty();
	}

	void QAbstractSeries3DPrivate::setBaseColor(const QColor &color)
	{
		m_baseColor = color;
		m_changeTracker.baseColorChanged = true;
		if (m_controller)
			m_controller->markSeriesVisualsDirty();
	}

	void QAbstractSeries3DPrivate::setBaseGradient(const QLinearGradient &gradient)
	{
		m_baseGradient = gradient;
		m_changeTracker.baseGradientChanged = true;
		if (m_controller)
			m_controller->markSeriesVisualsDirty();
	}

	void QAbstractSeries3DPrivate::setSingleHighlightColor(const QColor &color)
	{
		m_singleHighlightColor = color;
		m_changeTracker.singleHighlightColorChanged = true;
		if (m_controller)
			m_controller->markSeriesVisualsDirty();
	}

	void QAbstractSeries3DPrivate::setSingleHighlightGradient(const QLinearGradient &gradient)
	{
		m_singleHighlightGradient = gradient;
		m_changeTracker.singleHighlightGradientChanged = true;
		if (m_controller)
			m_controller->markSeriesVisualsDirty();
	}

	void QAbstractSeries3DPrivate::setMultiHighlightColor(const QColor &color)
	{
		m_multiHighlightColor = color;
		m_changeTracker.multiHighlightColorChanged = true;
		if (m_controller)
			m_controller->markSeriesVisualsDirty();
	}

	void QAbstractSeries3DPrivate::setMultiHighlightGradient(const QLinearGradient &gradient)
	{
		m_multiHighlightGradient = gradient;
		m_changeTracker.multiHighlightGradientChanged = true;
		if (m_controller)
			m_controller->markSeriesVisualsDirty();
	}

	void QAbstractSeries3DPrivate::setName(const QString &name)
	{
		m_name = name;
		markItemLabelDirty();
		m_changeTracker.nameChanged = true;
	}

	void QAbstractSeries3DPrivate::resetToTheme(const QTheme3D &theme, int seriesIndex, bool force)
	{
		int themeIndex = seriesIndex;
		if (force || !m_themeTracker.colorStyleOverride) {
			q_ptr->setColorStyle(theme.colorStyle());
			m_themeTracker.colorStyleOverride = false;
		}
		if (force || !m_themeTracker.baseColorOverride) {
			if (theme.baseColors().size() <= seriesIndex)
				themeIndex = seriesIndex % theme.baseColors().size();
			q_ptr->setBaseColor(theme.baseColors().at(themeIndex));
			m_themeTracker.baseColorOverride = false;
		}
		if (force || !m_themeTracker.baseGradientOverride) {
			if (theme.baseGradients().size() <= seriesIndex)
				themeIndex = seriesIndex % theme.baseGradients().size();
			q_ptr->setBaseGradient(theme.baseGradients().at(themeIndex));
			m_themeTracker.baseGradientOverride = false;
		}
		if (force || !m_themeTracker.singleHighlightColorOverride) {
			q_ptr->setSingleHighlightColor(theme.singleHighlightColor());
			m_themeTracker.singleHighlightColorOverride = false;
		}
		if (force || !m_themeTracker.singleHighlightGradientOverride) {
			q_ptr->setSingleHighlightGradient(theme.singleHighlightGradient());
			m_themeTracker.singleHighlightGradientOverride = false;
		}
		if (force || !m_themeTracker.multiHighlightColorOverride) {
			q_ptr->setMultiHighlightColor(theme.multiHighlightColor());
			m_themeTracker.multiHighlightColorOverride = false;
		}
		if (force || !m_themeTracker.multiHighlightGradientOverride) {
			q_ptr->setMultiHighlightGradient(theme.multiHighlightGradient());
			m_themeTracker.multiHighlightGradientOverride = false;
		}
	}

	QString QAbstractSeries3DPrivate::itemLabel()
	{
		if (m_itemLabelDirty) {
			QString oldLabel = m_itemLabel;
			if (m_controller && m_visible)
				createItemLabel();
			else
				m_itemLabel = QString();
			m_itemLabelDirty = false;

			if (oldLabel != m_itemLabel)
				emit q_ptr->itemLabelChanged(m_itemLabel);
		}

		return m_itemLabel;
	}

	void QAbstractSeries3DPrivate::markItemLabelDirty()
	{
		m_itemLabelDirty = true;
		m_changeTracker.itemLabelChanged = true;
		if (m_controller)
			m_controller->markSeriesVisualsDirty();
	}

	void QAbstractSeries3DPrivate::setItemLabelVisible(bool visible)
	{
		m_itemLabelVisible = visible;
		markItemLabelDirty();
		m_changeTracker.itemLabelVisibilityChanged = true;
	}
}
