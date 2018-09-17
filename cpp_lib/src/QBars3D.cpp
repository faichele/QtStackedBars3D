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

#include "QBars3DPrivate.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QBars3D
	* \inmodule QtDataVisualization
	* \brief The QBars3D class provides methods for rendering 3D bar graphs.
	* \since QtDataVisualization 1.0
	*
	* This class enables developers to render bar graphs in 3D and to view them by rotating the scene
	* freely. Rotation is done by holding down the right mouse button and moving the mouse. Zooming
	* is done by mouse wheel. Selection, if enabled, is done by left mouse button. The scene can be
	* reset to default camera view by clicking mouse wheel. In touch devices rotation is done
	* by tap-and-move, selection by tap-and-hold and zoom by pinch.
	*
	* If no axes are set explicitly to QBars3D, temporary default axes with no labels are created.
	* These default axes can be modified via axis accessors, but as soon any axis is set explicitly
	* for the orientation, the default axis for that orientation is destroyed.
	*
	* QBars3D supports more than one series visible at the same time. It is not necessary for all series
	* to have the same amount of rows and columns.
	* Row and column labels are taken from the first added series, unless explicitly defined to
	* row and column axes.
	*
	* \section1 How to construct a minimal QBars3D graph
	*
	* First, construct an instance of QBars3D. Since we are running the graph as top level window
	* in this example, we need to clear the \c Qt::FramelessWindowHint flag, which gets set by
	* default:
	*
	* \snippet doc_src_q3dbars_construction.cpp 4
	*
	* After constructing QBars3D, you can set the data window by changing the range on the row and
	* column axes. It is not mandatory, as data window will default to showing all of the data in
	* the series. If the amount of data is large, it is usually preferable to show just a
	* portion of it. For the example, let's set the data window to show first five rows and columns:
	*
	* \snippet doc_src_q3dbars_construction.cpp 0
	*
	* Now QBars3D is ready to receive data to be rendered. Create a series with one row of 5 values:
	*
	* \snippet doc_src_q3dbars_construction.cpp 1
	*
	* \note We set the data window to 5 x 5, but we are adding only one row of data. This is ok,
	* the rest of the rows will just be blank.
	*
	* Finally you will need to set it visible:
	*
	* \snippet doc_src_q3dbars_construction.cpp 2
	*
	* The complete code needed to create and display this graph is:
	*
	* \snippet doc_src_q3dbars_construction.cpp 3
	*
	* And this is what those few lines of code produce:
	*
	* \image q3dbars-minimal.png
	*
	* The scene can be rotated, zoomed into, and a bar can be selected to view its value,
	* but no other interaction is included in this minimal code example. You can learn more by
	* familiarizing yourself with the examples provided, like the \l{Bars Example}.
	*
	* \sa Q3DScatter, Q3DSurface, {Qt Data Visualization C++ Classes}
	*/

	/*!
	* Constructs a new 3D bar graph with optional \a parent window
	* and surface \a format.
	*/
	QBars3D::QBars3D(const QSurfaceFormat *format, QWindow *parent)
		: QAbstractGraph3D(new QBars3DPrivate(this), format, parent)
	{
		if (!dptr()->m_initialized)
			return;

		dptr()->m_shared = new QBarsController3D(geometry());
		d_ptr->setVisualController(dptr()->m_shared);
		dptr()->m_shared->initializeOpenGL();
		QObject::connect(dptr()->m_shared, &QBarsController3D::primarySeriesChanged,
			this, &QBars3D::primarySeriesChanged);
		QObject::connect(dptr()->m_shared, &QBarsController3D::selectedSeriesChanged,
			this, &QBars3D::selectedSeriesChanged);
	}

	/*!
	* Destroys the 3D bar graph.
	*/
	QBars3D::~QBars3D()
	{
	}

	/*!
	* \property QBars3D::primarySeries
	*
	* \brief The primary series of the graph.
	*/

	/*!
	* Sets \a series as the primary series of the graph. The primary series
	* determines the row and column axis labels when the labels are not explicitly
	* set to the axes.
	*
	* If the specified series is not yet added to the graph, setting it as the
	* primary series will also implicitly add it to the graph.
	*
	* If the primary series itself is removed from the graph, this property
	* resets to default.
	*
	* If \a series is null, this property resets to default.
	* Defaults to the first added series or zero if no series are added to the graph.
	*/
	void QBars3D::setPrimarySeries(QBarSeries3D *series)
	{
		dptr()->m_shared->setPrimarySeries(series);
	}

	QBarSeries3D *QBars3D::primarySeries() const
	{
		return dptrc()->m_shared->primarySeries();
	}

	/*!
	* Adds the \a series to the graph. A graph can contain multiple series, but only one set of axes,
	* so the rows and columns of all series must match for the visualized data to be meaningful.
	* If the graph has multiple visible series, only the primary series will
	* generate the row or column labels on the axes in cases where the labels are not explicitly set
	* to the axes. If the newly added series has specified a selected bar, it will be highlighted and
	* any existing selection will be cleared. Only one added series can have an active selection.
	*
	* /sa seriesList(), primarySeries
	*/
	void QBars3D::addSeries(QBarSeries3D *series)
	{
		dptr()->m_shared->addSeries(series);
	}

	/*!
	* Removes the \a series from the graph.
	*/
	void QBars3D::removeSeries(QBarSeries3D *series)
	{
		dptr()->m_shared->removeSeries(series);
	}

	/*!
	* Inserts the \a series into the position \a index in the series list.
	* If the \a series has already been added to the list, it is moved to the
	* new \a index.
	* \note When moving a series to a new \a index that is after its old index,
	* the new position in list is calculated as if the series was still in its old
	* index, so the final index is actually the \a index decremented by one.
	*
	* \sa addSeries(), seriesList()
	*/
	void QBars3D::insertSeries(int index, QBarSeries3D *series)
	{
		dptr()->m_shared->insertSeries(index, series);
	}

	/*!
	* Returns the list of series added to this graph.
	*/
	QList<QBarSeries3D *> QBars3D::seriesList() const
	{
		return dptrc()->m_shared->barSeriesList();
	}

	/*!
	* \property QBars3D::multiSeriesUniform
	*
	* \brief Whether bars are to be scaled with proportions set to a single series
	* bar even if there are multiple series displayed.
	*
	* If set to \c {true}, \l{barSpacing}{bar spacing} will be correctly applied
	* only to the X-axis. Preset to \c false by default.
	*/
	void QBars3D::setMultiSeriesUniform(bool uniform)
	{
		if (uniform != isMultiSeriesUniform()) {
			dptr()->m_shared->setMultiSeriesScaling(uniform);
			emit multiSeriesUniformChanged(uniform);
		}
	}

	bool QBars3D::isMultiSeriesUniform() const
	{
		return dptrc()->m_shared->multiSeriesScaling();
	}

	/*!
	* \property QBars3D::barThickness
	*
	* \brief The bar thickness ratio between the X and Z dimensions.
	*
	* The value \c 1.0 means that the bars are as wide as they are deep, whereas
	*\c 0.5 makes them twice as deep as they are wide. Preset to \c 1.0 by default.
	*/
	void QBars3D::setBarThickness(float thicknessRatio)
	{
		if (thicknessRatio != barThickness()) {
			dptr()->m_shared->setBarSpecs(GLfloat(thicknessRatio), barSpacing(),
				isBarSpacingRelative());
			emit barThicknessChanged(thicknessRatio);
		}
	}

	float QBars3D::barThickness() const
	{
		return dptrc()->m_shared->barThickness();
	}

	/*!
	* \property QBars3D::barSpacing
	*
	* \brief Bar spacing in the X and Z dimensions.
	*
	* Preset to \c {(1.0, 1.0)} by default. Spacing is affected by the
	* barSpacingRelative property.
	*
	* \sa barSpacingRelative, multiSeriesUniform
	*/
	void QBars3D::setBarSpacing(const QSizeF &spacing)
	{
		if (spacing != barSpacing()) {
			dptr()->m_shared->setBarSpecs(GLfloat(barThickness()), spacing, isBarSpacingRelative());
			emit barSpacingChanged(spacing);
		}
	}

	QSizeF QBars3D::barSpacing() const
	{
		return dptrc()->m_shared->barSpacing();
	}

	/*!
	* \property QBars3D::barSpacingRelative
	*
	* \brief Whether spacing is absolute or relative to bar thickness.
	*
	* If it is \c true, the value of \c 0.0 means that the bars are placed
	* side-to-side, \c 1.0 means that a space as wide as the thickness of one bar
	* is left between the bars, and so on. Preset to \c true.
	*/
	void QBars3D::setBarSpacingRelative(bool relative)
	{
		if (relative != isBarSpacingRelative()) {
			dptr()->m_shared->setBarSpecs(GLfloat(barThickness()), barSpacing(), relative);
			emit barSpacingRelativeChanged(relative);
		}
	}

	bool QBars3D::isBarSpacingRelative() const
	{
		return dptrc()->m_shared->isBarSpecRelative();
	}

	/*!
	* \property QBars3D::rowAxis
	*
	* \brief The axis attached to the active row.
	*/

	/*!
	* Sets the axis of the active row to \a axis. Implicitly calls addAxis() to
	* transfer the ownership of the axis to this graph.
	*
	* If \a axis is null, a temporary default axis with no labels is created.
	* This temporary axis is destroyed if another axis is set explicitly to the
	* same orientation.
	*
	* \sa addAxis(), releaseAxis()
	*/
	void QBars3D::setRowAxis(QCategoryAxis3D *axis)
	{
		dptr()->m_shared->setAxisZ(axis);
	}

	QCategoryAxis3D *QBars3D::rowAxis() const
	{
		return static_cast<QCategoryAxis3D *>(dptrc()->m_shared->axisZ());
	}

	/*!
	* \property QBars3D::columnAxis
	*
	* \brief The axis attached to the active column.
	*/

	/*!
	* Sets the axis of the active column to \a axis. Implicitly calls addAxis() to
	* transfer the ownership of the axis to this graph.
	*
	* If \a axis is null, a temporary default axis with no labels is created.
	* This temporary axis is destroyed if another axis is set explicitly to the
	* same orientation.
	*
	* \sa addAxis(), releaseAxis()
	*/
	void QBars3D::setColumnAxis(QCategoryAxis3D *axis)
	{
		dptr()->m_shared->setAxisX(axis);
	}

	QCategoryAxis3D *QBars3D::columnAxis() const
	{
		return static_cast<QCategoryAxis3D *>(dptrc()->m_shared->axisX());
	}

	/*!
	* \property QBars3D::valueAxis
	*
	* Sets the active value axis (the Y-axis) to \a axis. Implicitly calls
	* addAxis() to transfer the ownership of \a axis to this graph.
	*
	* If \a axis is null, a temporary default axis with no labels and
	* an automatically adjusting range is created.
	* This temporary axis is destroyed if another axis is set explicitly to the
	* same orientation.
	*
	* \sa addAxis(), releaseAxis()
	*/
	void QBars3D::setValueAxis(QValueAxis3D *axis)
	{
		dptr()->m_shared->setAxisY(axis);
	}

	QValueAxis3D *QBars3D::valueAxis() const
	{
		return static_cast<QValueAxis3D *>(dptrc()->m_shared->axisY());
	}

	/*!
	* \property QBars3D::selectedSeries
	*
	* \brief The selected series or a null value.
	*
	* If selectionMode has the \c SelectionMultiSeries flag set, this
	* property holds the series that owns the selected bar.
	*/
	QBarSeries3D *QBars3D::selectedSeries() const
	{
		return dptrc()->m_shared->selectedSeries();
	}

	/*!
	* \property QBars3D::floorLevel
	*
	* \brief The floor level for the bar graph in Y-axis data coordinates.
	*
	* The actual floor level will be restricted by the Y-axis minimum and maximum
	* values.
	* Defaults to zero.
	*/
	void QBars3D::setFloorLevel(float level)
	{
		if (level != floorLevel()) {
			dptr()->m_shared->setFloorLevel(level);
			emit floorLevelChanged(level);
		}
	}

	float QBars3D::floorLevel() const
	{
		return dptrc()->m_shared->floorLevel();
	}

	/*!
	* Adds \a axis to the graph. The axes added via addAxis are not yet taken to use,
	* addAxis is simply used to give the ownership of the \a axis to the graph.
	* The \a axis must not be null or added to another graph.
	*
	* \sa releaseAxis(), setValueAxis(), setRowAxis(), setColumnAxis()
	*/
	void QBars3D::addAxis(QAbstractAxis3D *axis)
	{
		dptr()->m_shared->addAxis(axis);
	}

	/*!
	* Releases the ownership of the \a axis back to the caller, if it is added to this graph.
	* If the released \a axis is in use, a new default axis will be created and set active.
	*
	* If the default axis is released and added back later, it behaves as any other axis would.
	*
	* \sa addAxis(), setValueAxis(), setRowAxis(), setColumnAxis()
	*/
	void QBars3D::releaseAxis(QAbstractAxis3D *axis)
	{
		dptr()->m_shared->releaseAxis(axis);
	}

	/*!
	* Returns the list of all added axes.
	*
	* \sa addAxis()
	*/
	QList<QAbstractAxis3D *> QBars3D::axes() const
	{
		return dptrc()->m_shared->axes();
	}

	QBars3DPrivate *QBars3D::dptr()
	{
		return static_cast<QBars3DPrivate *>(d_ptr.data());
	}

	const QBars3DPrivate *QBars3D::dptrc() const
	{
		return static_cast<const QBars3DPrivate *>(d_ptr.data());
	}

	QBars3DPrivate::QBars3DPrivate(QBars3D *q)
		: QAbstractGraph3DPrivate(q),
		m_shared(0)
	{
	}

	QBars3DPrivate::~QBars3DPrivate()
	{
	}

	void QBars3DPrivate::handleAxisXChanged(QAbstractAxis3D *axis)
	{
		emit qptr()->columnAxisChanged(static_cast<QCategoryAxis3D *>(axis));
	}

	void QBars3DPrivate::handleAxisYChanged(QAbstractAxis3D *axis)
	{
		emit qptr()->valueAxisChanged(static_cast<QValueAxis3D *>(axis));
	}

	void QBars3DPrivate::handleAxisZChanged(QAbstractAxis3D *axis)
	{
		emit qptr()->rowAxisChanged(static_cast<QCategoryAxis3D *>(axis));
	}

	QBars3D *QBars3DPrivate::qptr()
	{
		return static_cast<QBars3D *>(q_ptr);
	}

}
