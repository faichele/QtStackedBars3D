/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#include "QScatterSeries3DPrivate.h"
#include "QScatterController3D.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QScatterSeries3D
	* \inmodule QtDataVisualization
	* \brief The QScatterSeries3D class represents a data series in a 3D scatter
	* graph.
	* \since QtDataVisualization 1.0
	*
	* This class manages the series specific visual elements, as well as the series
	* data (via a data proxy).
	*
	* If no data proxy is set explicitly for the series, the series creates a default
	* proxy. Setting another proxy will destroy the existing proxy and all data added to it.
	*
	* QScatterSeries3D supports the following format tags for QAbstractSeries3D::setItemLabelFormat():
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
	* \qmltype Scatter3DSeries
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.0
	* \ingroup datavisualization_qml
	* \instantiates QScatterSeries3D
	* \inherits Abstract3DSeries
	* \brief Represents a data series in a 3D scatter graph.
	*
	* This type manages the series specific visual elements, as well as the series
	* data (via a data proxy).
	*
	* For a more complete description, see QScatterSeries3D.
	*
	* \sa {Qt Data Visualization Data Handling}
	*/

	/*!
	* \qmlproperty ScatterDataProxy Scatter3DSeries::dataProxy
	*
	* Sets the active data proxy. The series assumes ownership of any proxy set to
	* it and deletes any previously set proxy when a new one is added. The proxy
	* cannot be null or set to another series.
	*/

	/*!
	* \qmlproperty int Scatter3DSeries::selectedItem
	*
	* The item that is selected at the index in the data array of the series.
	* Only one item can be selected at a time.
	* To clear selection from this series, invalidSelectionIndex is set as the index.
	* If this series is added to a graph, the graph can adjust the selection according to user
	* interaction or if it becomes invalid. Selecting an item on another added series will also
	* clear the selection.
	* Removing items from or inserting items to the series before the selected item
	* will adjust the selection so that the same item will stay selected.
	*
	* \sa AbstractGraph3D::clearSelection()
	*/

	/*!
	* \qmlproperty float Scatter3DSeries::itemSize
	*
	* Sets the item size for the series. The size must be between \c 0.0 and
	* \c 1.0. Setting the size to \c 0.0 causes the item size to be automatically
	* scaled based on the total number of items in all the series for the graph.
	* The preset default is \c 0.0.
	*/

	/*!
	* \qmlproperty int Scatter3DSeries::invalidSelectionIndex
	* A constant property providing an invalid index for selection. This index is
	* set to the selectedItem property to clear the selection from this series.
	*
	* \sa AbstractGraph3D::clearSelection()
	*/

	/*!
	* Constructs a scatter 3D series with the parent \a parent.
	*/
	QScatterSeries3D::QScatterSeries3D(QObject *parent) :
		QAbstractSeries3D(new QScatterSeries3DPrivate(this), parent)
	{
		// Default proxy
		dptr()->setDataProxy(new QScatterDataProxy3D);
	}

	/*!
	* Constructs a scatter 3D series with the data proxy \a dataProxy and the
	* parent \a parent.
	*/
	QScatterSeries3D::QScatterSeries3D(QScatterDataProxy3D *dataProxy, QObject *parent) :
		QAbstractSeries3D(new QScatterSeries3DPrivate(this), parent)
	{
		dptr()->setDataProxy(dataProxy);
	}

	/*!
	* \internal
	*/
	QScatterSeries3D::QScatterSeries3D(QScatterSeries3DPrivate *d, QObject *parent) :
		QAbstractSeries3D(d, parent)
	{
	}

	/*!
	* Deletes the scatter 3D series.
	*/
	QScatterSeries3D::~QScatterSeries3D()
	{
	}

	/*!
	* \property QScatterSeries3D::dataProxy
	*
	* \brief The active data proxy.
	*/

	/*!
	* Sets the active data proxy for the series to \a proxy. The series assumes
	* ownership of any proxy set to it and deletes any previously set proxy when
	* a new one is added. The \a proxy argument cannot be null or set to another
	* series.
	*/
	void QScatterSeries3D::setDataProxy(QScatterDataProxy3D *proxy)
	{
		d_ptr->setDataProxy(proxy);
	}

	QScatterDataProxy3D *QScatterSeries3D::dataProxy() const
	{
		return static_cast<QScatterDataProxy3D *>(d_ptr->dataProxy());
	}

	/*!
	* \property QScatterSeries3D::selectedItem
	*
	* \brief The item that is selected in the series.
	*/

	/*!
	* Selects the item at the index \a index in the data array of the series.
	* Only one item can be selected at a time.
	*
	* To clear selection from this series, invalidSelectionIndex() is set as \a index.
	* If this series is added to a graph, the graph can adjust the selection according to user
	* interaction or if it becomes invalid. Selecting an item on another added series will also
	* clear the selection.
	*
	* Removing items from or inserting items to the series before the selected item
	* will adjust the selection so that the same item will stay selected.
	*
	* \sa QAbstractGraph3D::clearSelection()
	*/
	void QScatterSeries3D::setSelectedItem(int index)
	{
		// Don't do this in private to avoid loops, as that is used for callback from controller.
		if (d_ptr->m_controller)
			static_cast<QScatterController3D *>(d_ptr->m_controller)->setSelectedItem(index, this);
		else
			dptr()->setSelectedItem(index);
	}

	int QScatterSeries3D::selectedItem() const
	{
		return dptrc()->m_selectedItem;
	}

	/*!
	* \property QScatterSeries3D::itemSize
	*
	* \brief Item size for the series.
	*
	* The size must be between \c 0.0f and \c 1.0f. Setting the size to \c 0.0f
	* causes the item size to be automatically scaled based on the total number of
	* items in all the series for the graph.
	*
	* The preset default is \c 0.0f.
	*/
	void QScatterSeries3D::setItemSize(float size)
	{
		if (size < 0.0f || size > 1.0f) {
			qWarning("Invalid size. Valid range for itemSize is 0.0f...1.0f");
		}
		else if (size != dptr()->m_itemSize) {
			dptr()->setItemSize(size);
			emit itemSizeChanged(size);
		}
	}

	float QScatterSeries3D::itemSize() const
	{
		return dptrc()->m_itemSize;
	}

	/*!
	* Returns an invalid index for selection. This index is set to the selectedItem
	* property to clear the selection from this series.
	*
	* \sa QAbstractGraph3D::clearSelection()
	*/
	int QScatterSeries3D::invalidSelectionIndex()
	{
		return QScatterController3D::invalidSelectionIndex();
	}

	/*!
	* \internal
	*/
	QScatterSeries3DPrivate *QScatterSeries3D::dptr()
	{
		return static_cast<QScatterSeries3DPrivate *>(d_ptr.data());
	}

	/*!
	* \internal
	*/
	const QScatterSeries3DPrivate *QScatterSeries3D::dptrc() const
	{
		return static_cast<const QScatterSeries3DPrivate *>(d_ptr.data());
	}

	// QScatterSeries3DPrivate

	QScatterSeries3DPrivate::QScatterSeries3DPrivate(QScatterSeries3D *q)
		: QAbstractSeries3DPrivate(q, QAbstractSeries3D::SeriesTypeScatter),
		m_selectedItem(QScatterController3D::invalidSelectionIndex()),
		m_itemSize(0.0f)
	{
		m_itemLabelFormat = QStringLiteral("@xLabel, @yLabel, @zLabel");
		m_mesh = QAbstractSeries3D::MeshSphere;
	}

	QScatterSeries3DPrivate::~QScatterSeries3DPrivate()
	{
	}

	QScatterSeries3D *QScatterSeries3DPrivate::qptr()
	{
		return static_cast<QScatterSeries3D *>(q_ptr);
	}

	void QScatterSeries3DPrivate::setDataProxy(QAbstractDataProxy3D *proxy)
	{
		Q_ASSERT(proxy->type() == QAbstractDataProxy3D::DataTypeScatter);

		QAbstractSeries3DPrivate::setDataProxy(proxy);

		emit qptr()->dataProxyChanged(static_cast<QScatterDataProxy3D *>(proxy));
	}

	void QScatterSeries3DPrivate::connectControllerAndProxy(QAbstractController3D *newController)
	{
		QScatterDataProxy3D *scatterDataProxy = static_cast<QScatterDataProxy3D *>(m_dataProxy);

		if (m_controller && scatterDataProxy) {
			//Disconnect old controller/old proxy
			QObject::disconnect(scatterDataProxy, 0, m_controller, 0);
			QObject::disconnect(q_ptr, 0, m_controller, 0);
		}

		if (newController && scatterDataProxy) {
			QScatterController3D *controller = static_cast<QScatterController3D *>(newController);
			QObject::connect(scatterDataProxy, &QScatterDataProxy3D::arrayReset,
				controller, &QScatterController3D::handleArrayReset);
			QObject::connect(scatterDataProxy, &QScatterDataProxy3D::itemsAdded,
				controller, &QScatterController3D::handleItemsAdded);
			QObject::connect(scatterDataProxy, &QScatterDataProxy3D::itemsChanged,
				controller, &QScatterController3D::handleItemsChanged);
			QObject::connect(scatterDataProxy, &QScatterDataProxy3D::itemsRemoved,
				controller, &QScatterController3D::handleItemsRemoved);
			QObject::connect(scatterDataProxy, &QScatterDataProxy3D::itemsInserted,
				controller, &QScatterController3D::handleItemsInserted);
			QObject::connect(qptr(), &QScatterSeries3D::dataProxyChanged,
				controller, &QScatterController3D::handleArrayReset);
		}
	}

	void QScatterSeries3DPrivate::createItemLabel()
	{
		static const QString xTitleTag(QStringLiteral("@xTitle"));
		static const QString yTitleTag(QStringLiteral("@yTitle"));
		static const QString zTitleTag(QStringLiteral("@zTitle"));
		static const QString xLabelTag(QStringLiteral("@xLabel"));
		static const QString yLabelTag(QStringLiteral("@yLabel"));
		static const QString zLabelTag(QStringLiteral("@zLabel"));
		static const QString seriesNameTag(QStringLiteral("@seriesName"));

		if (m_selectedItem == QScatterSeries3D::invalidSelectionIndex()) {
			m_itemLabel = QString();
			return;
		}

		QValueAxis3D *axisX = static_cast<QValueAxis3D *>(m_controller->axisX());
		QValueAxis3D *axisY = static_cast<QValueAxis3D *>(m_controller->axisY());
		QValueAxis3D *axisZ = static_cast<QValueAxis3D *>(m_controller->axisZ());
		QVector3D selectedPosition = qptr()->dataProxy()->itemAt(m_selectedItem)->position();

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

	void QScatterSeries3DPrivate::setSelectedItem(int index)
	{
		if (index != m_selectedItem) {
			markItemLabelDirty();
			m_selectedItem = index;
			emit qptr()->selectedItemChanged(m_selectedItem);
		}
	}

	void QScatterSeries3DPrivate::setItemSize(float size)
	{
		m_itemSize = size;
		if (m_controller)
			m_controller->markSeriesVisualsDirty();
	}

}
