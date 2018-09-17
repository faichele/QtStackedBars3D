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

#include "QScatterDataProxy3DPrivate.h"
#include "QScatterSeries3DPrivate.h"
#include "QAbstractAxis3DPrivate.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QScatterDataProxy3D
	* \inmodule QtDataVisualization
	* \brief The QScatterDataProxy3D class is the data proxy for 3D scatter graphs.
	* \since QtDataVisualization 1.0
	*
	* A scatter data proxy handles adding, inserting, changing, and removing data
	* items.
	*
	* QScatterDataProxy3D takes ownership of all
	* QtDataVisualization::QScatterDataArray and QScatterDataItem3D objects passed to
	* it.
	*
	* \sa {Qt Data Visualization Data Handling}
	*/

	/*!
	* \typedef QtDataVisualization::QScatterDataArray
	* \relates QScatterDataProxy3D
	*
	* A vector of \l {QScatterDataItem3D} objects.
	*/

	/*!
	* \qmltype ScatterDataProxy
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.0
	* \ingroup datavisualization_qml
	* \instantiates QScatterDataProxy3D
	* \inherits AbstractDataProxy
	* \brief The data proxy for 3D scatter graphs.
	*
	* This type handles adding, inserting, changing, and removing data items.
	*
	* This type is uncreatable, but contains properties that are exposed via subtypes.
	*
	* \sa ItemModelScatterDataProxy, {Qt Data Visualization Data Handling}
	*/

	/*!
	* \qmlproperty int ScatterDataProxy::itemCount
	* The number of items in the array.
	*/

	/*!
	* \qmlproperty Scatter3DSeries ScatterDataProxy::series
	*
	* The series this proxy is attached to.
	*/

	/*!
	* Constructs QScatterDataProxy3D with the given \a parent.
	*/
	QScatterDataProxy3D::QScatterDataProxy3D(QObject *parent) :
		QAbstractDataProxy3D(new QScatterDataProxy3DPrivate(this), parent)
	{
	}

	/*!
	* \internal
	*/
	QScatterDataProxy3D::QScatterDataProxy3D(QScatterDataProxy3DPrivate *d, QObject *parent) :
		QAbstractDataProxy3D(d, parent)
	{
	}

	/*!
	* Deletes the scatter data proxy.
	*/
	QScatterDataProxy3D::~QScatterDataProxy3D()
	{
	}

	/*!
	* \property QScatterDataProxy3D::series
	*
	* \brief The series this proxy is attached to.
	*/
	QScatterSeries3D *QScatterDataProxy3D::series() const
	{
		return static_cast<QScatterSeries3D *>(d_ptr->series());
	}

	/*!
	* Takes ownership of the array \a newArray. Clears the existing array if the
	* new array differs from it. If the arrays are the same, this function
	* just triggers the arrayReset() signal.
	*
	* Passing a null array deletes the old array and creates a new empty array.
	*/
	void QScatterDataProxy3D::resetArray(QScatterDataArray *newArray)
	{
		if (dptr()->m_dataArray != newArray)
			dptr()->resetArray(newArray);

		emit arrayReset();
		emit itemCountChanged(itemCount());
	}

	/*!
	* Replaces the item at the position \a index with the item \a item.
	*/
	void QScatterDataProxy3D::setItem(int index, const QScatterDataItem3D &item)
	{
		dptr()->setItem(index, item);
		emit itemsChanged(index, 1);
	}

	/*!
	* Replaces the items starting from the position \a index with the items
	* specified by \a items.
	*/
	void QScatterDataProxy3D::setItems(int index, const QScatterDataArray &items)
	{
		dptr()->setItems(index, items);
		emit itemsChanged(index, items.size());
	}

	/*!
	* Adds the item \a item to the end of the array.
	*
	* Returns the index of the added item.
	*/
	int QScatterDataProxy3D::addItem(const QScatterDataItem3D &item)
	{
		int addIndex = dptr()->addItem(item);
		emit itemsAdded(addIndex, 1);
		emit itemCountChanged(itemCount());
		return addIndex;
	}

	/*!
	* Adds the items specified by \a items to the end of the array.
	*
	* Returns the index of the first added item.
	*/
	int QScatterDataProxy3D::addItems(const QScatterDataArray &items)
	{
		int addIndex = dptr()->addItems(items);
		emit itemsAdded(addIndex, items.size());
		emit itemCountChanged(itemCount());
		return addIndex;
	}

	/*!
	* Inserts the item \a item to the position \a index. If the index is equal to
	* the data array size, the item is added to the array.
	*/
	void QScatterDataProxy3D::insertItem(int index, const QScatterDataItem3D &item)
	{
		dptr()->insertItem(index, item);
		emit itemsInserted(index, 1);
		emit itemCountChanged(itemCount());
	}

	/*!
	* Inserts the items specified by \a items to the position \a index. If the
	* index is equal to data array size, the items are added to the array.
	*/
	void QScatterDataProxy3D::insertItems(int index, const QScatterDataArray &items)
	{
		dptr()->insertItems(index, items);
		emit itemsInserted(index, items.size());
		emit itemCountChanged(itemCount());
	}

	/*!
	* Removes the number of items specified by \a removeCount starting at the
	* position \a index. Attempting to remove items past the end of
	* the array does nothing.
	*/
	void QScatterDataProxy3D::removeItems(int index, int removeCount)
	{
		if (index >= dptr()->m_dataArray->size())
			return;

		dptr()->removeItems(index, removeCount);
		emit itemsRemoved(index, removeCount);
		emit itemCountChanged(itemCount());
	}

	/*!
	* \property QScatterDataProxy3D::itemCount
	*
	* \brief The number of items in the array.
	*/
	int QScatterDataProxy3D::itemCount() const
	{
		return dptrc()->m_dataArray->size();
	}

	/*!
	* Returns the pointer to the data array.
	*/
	const QScatterDataArray *QScatterDataProxy3D::array() const
	{
		return dptrc()->m_dataArray;
	}

	/*!
	* Returns the pointer to the item at the index \a index. It is guaranteed to be
	* valid only until the next call that modifies data.
	*/
	const QScatterDataItem3D *QScatterDataProxy3D::itemAt(int index) const
	{
		return &dptrc()->m_dataArray->at(index);
	}

	/*!
	* \internal
	*/
	QScatterDataProxy3DPrivate *QScatterDataProxy3D::dptr()
	{
		return static_cast<QScatterDataProxy3DPrivate *>(d_ptr.data());
	}

	/*!
	* \internal
	*/
	const QScatterDataProxy3DPrivate *QScatterDataProxy3D::dptrc() const
	{
		return static_cast<const QScatterDataProxy3DPrivate *>(d_ptr.data());
	}

	/*!
	* \fn void QScatterDataProxy3D::arrayReset()
	*
	* This signal is emitted when the data array is reset.
	* If the contents of the whole array are changed without calling resetArray(),
	* this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QScatterDataProxy3D::itemsAdded(int startIndex, int count)
	*
	* This signal is emitted when the number of items specified by \a count is
	* added starting at the position \a startIndex.
	* If items are added to the array without calling addItem() or addItems(),
	* this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QScatterDataProxy3D::itemsChanged(int startIndex, int count)
	*
	* This signal is emitted when the number of items specified by \a count is
	* changed starting at the position \a startIndex.
	* If items are changed in the array without calling setItem() or setItems(),
	* this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QScatterDataProxy3D::itemsRemoved(int startIndex, int count)
	*
	* This signal is emitted when the number of rows specified by \a count is
	* removed starting at the position \a startIndex.
	* The index may be larger than the current array size if items are removed from
	* the end. If items are removed from the array without calling removeItems(),
	* this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QScatterDataProxy3D::itemsInserted(int startIndex, int count)
	*
	* This signal is emitted when the number of items specified by \a count is
	* inserted starting at the position \a startIndex.
	* If items are inserted into the array without calling insertItem() or
	* insertItems(), this signal needs to be emitted to update the graph.
	*/

	// QScatterDataProxy3DPrivate

	QScatterDataProxy3DPrivate::QScatterDataProxy3DPrivate(QScatterDataProxy3D *q)
		: QAbstractDataProxy3DPrivate(q, QAbstractDataProxy3D::DataTypeScatter),
		m_dataArray(new QScatterDataArray)
	{
	}

	QScatterDataProxy3DPrivate::~QScatterDataProxy3DPrivate()
	{
		m_dataArray->clear();
		delete m_dataArray;
	}

	void QScatterDataProxy3DPrivate::resetArray(QScatterDataArray *newArray)
	{
		if (!newArray)
			newArray = new QScatterDataArray;

		if (newArray != m_dataArray) {
			m_dataArray->clear();
			delete m_dataArray;
			m_dataArray = newArray;
		}
	}

	void QScatterDataProxy3DPrivate::setItem(int index, const QScatterDataItem3D &item)
	{
		Q_ASSERT(index >= 0 && index < m_dataArray->size());
		(*m_dataArray)[index] = item;
	}

	void QScatterDataProxy3DPrivate::setItems(int index, const QScatterDataArray &items)
	{
		Q_ASSERT(index >= 0 && (index + items.size()) <= m_dataArray->size());
		for (int i = 0; i < items.size(); i++)
			(*m_dataArray)[index++] = items[i];
	}

	int QScatterDataProxy3DPrivate::addItem(const QScatterDataItem3D &item)
	{
		int currentSize = m_dataArray->size();
		m_dataArray->append(item);
		return currentSize;
	}

	int QScatterDataProxy3DPrivate::addItems(const QScatterDataArray &items)
	{
		int currentSize = m_dataArray->size();
		(*m_dataArray) += items;
		return currentSize;
	}

	void QScatterDataProxy3DPrivate::insertItem(int index, const QScatterDataItem3D &item)
	{
		Q_ASSERT(index >= 0 && index <= m_dataArray->size());
		m_dataArray->insert(index, item);
	}

	void QScatterDataProxy3DPrivate::insertItems(int index, const QScatterDataArray &items)
	{
		Q_ASSERT(index >= 0 && index <= m_dataArray->size());
		for (int i = 0; i < items.size(); i++)
			m_dataArray->insert(index++, items.at(i));
	}

	void QScatterDataProxy3DPrivate::removeItems(int index, int removeCount)
	{
		Q_ASSERT(index >= 0);
		int maxRemoveCount = m_dataArray->size() - index;
		removeCount = qMin(removeCount, maxRemoveCount);
		m_dataArray->remove(index, removeCount);
	}

	void QScatterDataProxy3DPrivate::limitValues(QVector3D &minValues, QVector3D &maxValues,
		QAbstractAxis3D *axisX, QAbstractAxis3D *axisY,
		QAbstractAxis3D *axisZ) const
	{
		if (m_dataArray->isEmpty())
			return;

		const QVector3D &firstPos = m_dataArray->at(0).position();

		float minX = firstPos.x();
		float maxX = minX;
		float minY = firstPos.y();
		float maxY = minY;
		float minZ = firstPos.z();
		float maxZ = minZ;

		if (m_dataArray->size() > 1) {
			for (int i = 1; i < m_dataArray->size(); i++) {
				const QVector3D &pos = m_dataArray->at(i).position();

				float value = pos.x();
				if (qIsNaN(value) || qIsInf(value))
					continue;
				if (isValidValue(minX, value, axisX))
					minX = value;
				if (maxX < value)
					maxX = value;

				value = pos.y();
				if (qIsNaN(value) || qIsInf(value))
					continue;
				if (isValidValue(minY, value, axisY))
					minY = value;
				if (maxY < value)
					maxY = value;

				value = pos.z();
				if (qIsNaN(value) || qIsInf(value))
					continue;
				if (isValidValue(minZ, value, axisZ))
					minZ = value;
				if (maxZ < value)
					maxZ = value;
			}
		}

		minValues.setX(minX);
		minValues.setY(minY);
		minValues.setZ(minZ);

		maxValues.setX(maxX);
		maxValues.setY(maxY);
		maxValues.setZ(maxZ);
	}

	bool QScatterDataProxy3DPrivate::isValidValue(float axisValue, float value,
		QAbstractAxis3D *axis) const
	{
		return (axisValue > value && (value > 0.0f
			|| (value == 0.0f && axis->d_ptr->allowZero())
			|| (value < 0.0f && axis->d_ptr->allowNegatives())));
	}

	void QScatterDataProxy3DPrivate::setSeries(QAbstractSeries3D *series)
	{
		QAbstractDataProxy3DPrivate::setSeries(series);
		QScatterSeries3D *scatterSeries = static_cast<QScatterSeries3D *>(series);
		emit qptr()->seriesChanged(scatterSeries);
	}

	QScatterDataProxy3D *QScatterDataProxy3DPrivate::qptr()
	{
		return static_cast<QScatterDataProxy3D *>(q_ptr);
	}

}
