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

#include "QSurfaceDataProxy3DPrivate.h"
#include "QSurfaceSeries3D.h"
#include "QAbstractAxis3DPrivate.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QSurfaceDataProxy3D
	* \inmodule QtDataVisualization
	* \brief The QSurfaceDataProxy3D class is the data proxy for a 3D surface graph.
	* \since QtDataVisualization 1.0
	*
	* A surface data proxy handles surface related data in rows. For this it
	* provides two auxiliary typedefs: QtDataVisualization::QSurfaceDataArray3D and
	* QtDataVisualization::QSurfaceDataRow3D. \c QSurfaceDataArray3D is a QList that
	* controls the rows. \c QSurfaceDataRow3D is a QVector that contains
	* QSurfaceDataItem3D objects. For more information about how to feed the data to
	* the proxy, see the sample code in the Q3DSurface documentation.
	*
	* All rows must have the same number of items.
	*
	* QSurfaceDataProxy3D takes ownership of all \c QSurfaceDataRow3D objects passed to
	* it, whether directly or in a \c QSurfaceDataArray3D container.
	* To use surface data row pointers to directly modify data after adding the
	* array to the proxy, the appropriate signal must be emitted to update the
	* graph.
	*
	* To make a sensible surface, the x-value of each successive item in all rows must be
	* either ascending or descending throughout the row.
	* Similarly, the z-value of each successive item in all columns must be either ascending or
	* descending throughout the column.
	*
	* \note Currently only surfaces with straight rows and columns are fully supported. Any row
	* with items that do not have the exact same z-value or any column with items
	* that do not have the exact same x-value may get clipped incorrectly if the
	* whole surface does not completely fit within the visible x-axis or z-axis
	* ranges.
	*
	* \note Surfaces with less than two rows or columns are not considered valid surfaces and will
	* not be rendered.
	*
	* \note On some environments, surfaces with a lot of visible vertices may not render, because
	* they exceed the per-draw vertex count supported by the graphics driver.
	* This is mostly an issue on 32-bit and OpenGL ES2 platforms.
	*
	* \sa {Qt Data Visualization Data Handling}
	*/

	/*!
	* \typedef QtDataVisualization::QSurfaceDataRow3D
	* \relates QSurfaceDataProxy3D
	*
	* A vector of \l {QSurfaceDataItem3D} objects.
	*/

	/*!
	* \typedef QtDataVisualization::QSurfaceDataArray3D
	* \relates QSurfaceDataProxy3D
	*
	* A list of pointers to \l {QSurfaceDataRow3D} objects.
	*/

	/*!
	* \qmltype SurfaceDataProxy
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.0
	* \ingroup datavisualization_qml
	* \instantiates QSurfaceDataProxy3D
	* \inherits AbstractDataProxy
	* \brief The data proxy for a 3D surface graph.
	*
	* This type handles surface data items. The data is arranged into rows and columns, and all rows must have
	* the same number of columns.
	*
	* This type is uncreatable, but contains properties that are exposed via subtypes.
	*
	* For a more complete description, see QSurfaceDataProxy3D.
	*
	* \sa ItemModelSurfaceDataProxy, {Qt Data Visualization Data Handling}
	*/

	/*!
	* \qmlproperty int SurfaceDataProxy::rowCount
	* The number of rows in the data array.
	*/

	/*!
	* \qmlproperty int SurfaceDataProxy::columnCount
	* The number of columns in the data array.
	*/

	/*!
	* \qmlproperty Surface3DSeries SurfaceDataProxy::series
	*
	* The series this proxy is attached to.
	*/

	/*!
	* Constructs QSurfaceDataProxy3D with the given \a parent.
	*/
	QSurfaceDataProxy3D::QSurfaceDataProxy3D(QObject *parent) :
		QAbstractDataProxy3D(new QSurfaceDataProxy3DPrivate(this), parent)
	{
	}

	/*!
	* \internal
	*/
	QSurfaceDataProxy3D::QSurfaceDataProxy3D(QSurfaceDataProxy3DPrivate *d, QObject *parent) :
		QAbstractDataProxy3D(d, parent)
	{
	}

	/*!
	* Deletes the surface data proxy.
	*/
	QSurfaceDataProxy3D::~QSurfaceDataProxy3D()
	{
	}

	/*!
	* \property QSurfaceDataProxy3D::series
	*
	*  \brief The series this proxy is attached to.
	*/
	QSurfaceSeries3D *QSurfaceDataProxy3D::series() const
	{
		return static_cast<QSurfaceSeries3D *>(d_ptr->series());
	}

	/*!
	* Takes ownership of the array \a newArray. Clears the existing array if the
	* new array differs from it. If the arrays are the same, this function
	* just triggers the arrayReset() signal.
	*
	* Passing a null array deletes the old array and creates a new empty array.
	* All rows in \a newArray must be of same length.
	*/
	void QSurfaceDataProxy3D::resetArray(QSurfaceDataArray3D *newArray)
	{
		if (dptr()->m_dataArray != newArray) {
			dptr()->resetArray(newArray);
		}
		emit arrayReset();
		emit rowCountChanged(rowCount());
		emit columnCountChanged(columnCount());
	}

	/*!
	* Changes an existing row by replacing the row at the position \a rowIndex
	* with the new row specified by \a row. The new row can be the same as the
	* existing row already stored at the \a rowIndex. The new row must have
	* the same number of columns as the row it is replacing.
	*/
	void QSurfaceDataProxy3D::setRow(int rowIndex, QSurfaceDataRow3D *row)
	{
		dptr()->setRow(rowIndex, row);
		emit rowsChanged(rowIndex, 1);
	}

	/*!
	* Changes existing rows by replacing the rows starting at the position
	* \a rowIndex with the new rows specifies by \a rows.
	* The rows in the \a rows array can be the same as the existing rows already
	* stored at the \a rowIndex. The new rows must have the same number of columns
	* as the rows they are replacing.
	*/
	void QSurfaceDataProxy3D::setRows(int rowIndex, const QSurfaceDataArray3D &rows)
	{
		dptr()->setRows(rowIndex, rows);
		emit rowsChanged(rowIndex, rows.size());
	}

	/*!
	* Changes a single item at the position specified by \a rowIndex and
	* \a columnIndex to the item \a item.
	*/
	void QSurfaceDataProxy3D::setItem(int rowIndex, int columnIndex, const QSurfaceDataItem3D &item)
	{
		dptr()->setItem(rowIndex, columnIndex, item);
		emit itemChanged(rowIndex, columnIndex);
	}

	/*!
	* Changes a single item at the position \a position to the item \a item.
	* The x-value of \a position indicates the row and the y-value indicates the
	* column.
	*/
	void QSurfaceDataProxy3D::setItem(const QPoint &position, const QSurfaceDataItem3D &item)
	{
		setItem(position.x(), position.y(), item);
	}

	/*!
	* Adds the new row \a row to the end of an array. The new row must have
	* the same number of columns as the rows in the initial array.
	*
	* Returns the index of the added row.
	*/
	int QSurfaceDataProxy3D::addRow(QSurfaceDataRow3D *row)
	{
		int addIndex = dptr()->addRow(row);
		emit rowsAdded(addIndex, 1);
		emit rowCountChanged(rowCount());
		return addIndex;
	}

	/*!
	* Adds new \a rows to the end of an array. The new rows must have the same
	* number of columns as the rows in the initial array.
	*
	* Returns the index of the first added row.
	*/
	int QSurfaceDataProxy3D::addRows(const QSurfaceDataArray3D &rows)
	{
		int addIndex = dptr()->addRows(rows);
		emit rowsAdded(addIndex, rows.size());
		emit rowCountChanged(rowCount());
		return addIndex;
	}

	/*!
	* Inserts the new row \a row into \a rowIndex.
	* If \a rowIndex is equal to the array size, the rows are added to the end of
	* the array. The new row must have the same number of columns as the rows in
	* the initial array.
	*/
	void QSurfaceDataProxy3D::insertRow(int rowIndex, QSurfaceDataRow3D *row)
	{
		dptr()->insertRow(rowIndex, row);
		emit rowsInserted(rowIndex, 1);
		emit rowCountChanged(rowCount());
	}

	/*!
	* Inserts new \a rows into \a rowIndex.
	* If \a rowIndex is equal to the array size, the rows are added to the end of
	* the array. The new \a rows must have the same number of columns as the rows
	* in the initial array.
	*/
	void QSurfaceDataProxy3D::insertRows(int rowIndex, const QSurfaceDataArray3D &rows)
	{
		dptr()->insertRows(rowIndex, rows);
		emit rowsInserted(rowIndex, rows.size());
		emit rowCountChanged(rowCount());
	}

	/*!
	* Removes the number of rows specified by \a removeCount starting at the
	* position \a rowIndex. Attempting to remove rows past the end of the
	* array does nothing.
	*/
	void QSurfaceDataProxy3D::removeRows(int rowIndex, int removeCount)
	{
		if (rowIndex < rowCount() && removeCount >= 1) {
			dptr()->removeRows(rowIndex, removeCount);
			emit rowsRemoved(rowIndex, removeCount);
			emit rowCountChanged(rowCount());
		}
	}

	/*!
	* Returns the pointer to the data array.
	*/
	const QSurfaceDataArray3D *QSurfaceDataProxy3D::array() const
	{
		return dptrc()->m_dataArray;
	}

	/*!
	* Returns the pointer to the item at the position specified by \a rowIndex and
	* \a columnIndex. It is guaranteed to be valid only
	* until the next call that modifies data.
	*/
	const QSurfaceDataItem3D *QSurfaceDataProxy3D::itemAt(int rowIndex, int columnIndex) const
	{
		const QSurfaceDataArray3D &dataArray = *dptrc()->m_dataArray;
		Q_ASSERT(rowIndex >= 0 && rowIndex < dataArray.size());
		const QSurfaceDataRow3D &dataRow = *dataArray[rowIndex];
		Q_ASSERT(columnIndex >= 0 && columnIndex < dataRow.size());
		return &dataRow.at(columnIndex);
	}

	/*!
	* Returns the pointer to the item at the position \a position. The x-value of
	* \a position indicates the row and the y-value indicates the column. The item
	* is guaranteed to be valid only until the next call that modifies data.
	*/
	const QSurfaceDataItem3D *QSurfaceDataProxy3D::itemAt(const QPoint &position) const
	{
		return itemAt(position.x(), position.y());
	}

	/*!
	* \property QSurfaceDataProxy3D::rowCount
	*
	* \brief The number of rows in the data array.
	*/
	int QSurfaceDataProxy3D::rowCount() const
	{
		return dptrc()->m_dataArray->size();
	}

	/*!
	* \property QSurfaceDataProxy3D::columnCount
	*
	* \brief The number of columns in the data array.
	*/
	int QSurfaceDataProxy3D::columnCount() const
	{
		if (dptrc()->m_dataArray->size() > 0)
			return dptrc()->m_dataArray->at(0)->size();
		else
			return 0;
	}

	/*!
	* \internal
	*/
	QSurfaceDataProxy3DPrivate *QSurfaceDataProxy3D::dptr()
	{
		return static_cast<QSurfaceDataProxy3DPrivate *>(d_ptr.data());
	}

	/*!
	* \internal
	*/
	const QSurfaceDataProxy3DPrivate *QSurfaceDataProxy3D::dptrc() const
	{
		return static_cast<const QSurfaceDataProxy3DPrivate *>(d_ptr.data());
	}

	/*!
	* \fn void QSurfaceDataProxy3D::arrayReset()
	*
	* This signal is emitted when the data array is reset.
	* If the contents of the whole array are changed without calling resetArray(),
	* this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QSurfaceDataProxy3D::rowsAdded(int startIndex, int count)
	*
	* This signal is emitted when the number of rows specified by \a count is
	* added starting at the position \a startIndex.
	* If rows are added to the array without calling addRow() or addRows(),
	* this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QSurfaceDataProxy3D::rowsChanged(int startIndex, int count)
	*
	* This signal is emitted when the number of rows specified by \a count is
	* changed starting at the position \a startIndex.
	* If rows are changed in the array without calling setRow() or setRows(),
	* this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QSurfaceDataProxy3D::rowsRemoved(int startIndex, int count)
	*
	* This signal is emitted when the number of rows specified by \a count is
	* removed starting at the position \a startIndex.
	*
	* The index is the current array size if the rows were removed from the end of
	* the array. If rows are removed from the array without calling removeRows(),
	* this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QSurfaceDataProxy3D::rowsInserted(int startIndex, int count)
	*
	* This signal is emitted when the number of rows specified by \a count is
	* inserted at the position \a startIndex.
	*
	* If rows are inserted into the array without calling insertRow() or
	* insertRows(), this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QSurfaceDataProxy3D::itemChanged(int rowIndex, int columnIndex)
	*
	* This signal is emitted when the item at the position specified by \a rowIndex
	* and \a columnIndex changes.
	* If the item is changed in the array without calling setItem(),
	* this signal needs to be emitted to update the graph.
	*/

	//  QSurfaceDataProxy3DPrivate

	QSurfaceDataProxy3DPrivate::QSurfaceDataProxy3DPrivate(QSurfaceDataProxy3D *q)
		: QAbstractDataProxy3DPrivate(q, QAbstractDataProxy3D::DataTypeSurface),
		m_dataArray(new QSurfaceDataArray3D)
	{
	}

	QSurfaceDataProxy3DPrivate::~QSurfaceDataProxy3DPrivate()
	{
		clearArray();
	}

	void QSurfaceDataProxy3DPrivate::resetArray(QSurfaceDataArray3D *newArray)
	{
		if (!newArray)
			newArray = new QSurfaceDataArray3D;

		if (newArray != m_dataArray) {
			clearArray();
			m_dataArray = newArray;
		}
	}

	void QSurfaceDataProxy3DPrivate::setRow(int rowIndex, QSurfaceDataRow3D *row)
	{
		Q_ASSERT(rowIndex >= 0 && rowIndex < m_dataArray->size());
		Q_ASSERT(m_dataArray->at(rowIndex)->size() == row->size());

		if (row != m_dataArray->at(rowIndex)) {
			clearRow(rowIndex);
			(*m_dataArray)[rowIndex] = row;
		}
	}

	void QSurfaceDataProxy3DPrivate::setRows(int rowIndex, const QSurfaceDataArray3D &rows)
	{
		QSurfaceDataArray3D &dataArray = *m_dataArray;
		Q_ASSERT(rowIndex >= 0 && (rowIndex + rows.size()) <= dataArray.size());

		for (int i = 0; i < rows.size(); i++) {
			Q_ASSERT(m_dataArray->at(rowIndex)->size() == rows.at(i)->size());
			if (rows.at(i) != dataArray.at(rowIndex)) {
				clearRow(rowIndex);
				dataArray[rowIndex] = rows.at(i);
			}
			rowIndex++;
		}
	}

	void QSurfaceDataProxy3DPrivate::setItem(int rowIndex, int columnIndex, const QSurfaceDataItem3D &item)
	{
		Q_ASSERT(rowIndex >= 0 && rowIndex < m_dataArray->size());
		QSurfaceDataRow3D &row = *(*m_dataArray)[rowIndex];
		Q_ASSERT(columnIndex < row.size());
		row[columnIndex] = item;
	}

	int QSurfaceDataProxy3DPrivate::addRow(QSurfaceDataRow3D *row)
	{
		Q_ASSERT(m_dataArray->at(0)->size() == row->size());
		int currentSize = m_dataArray->size();
		m_dataArray->append(row);
		return currentSize;
	}

	int QSurfaceDataProxy3DPrivate::addRows(const QSurfaceDataArray3D &rows)
	{
		int currentSize = m_dataArray->size();
		for (int i = 0; i < rows.size(); i++) {
			Q_ASSERT(m_dataArray->at(0)->size() == rows.at(i)->size());
			m_dataArray->append(rows.at(i));
		}
		return currentSize;
	}

	void QSurfaceDataProxy3DPrivate::insertRow(int rowIndex, QSurfaceDataRow3D *row)
	{
		Q_ASSERT(rowIndex >= 0 && rowIndex <= m_dataArray->size());
		Q_ASSERT(m_dataArray->at(0)->size() == row->size());
		m_dataArray->insert(rowIndex, row);
	}

	void QSurfaceDataProxy3DPrivate::insertRows(int rowIndex, const QSurfaceDataArray3D &rows)
	{
		Q_ASSERT(rowIndex >= 0 && rowIndex <= m_dataArray->size());

		for (int i = 0; i < rows.size(); i++) {
			Q_ASSERT(m_dataArray->at(0)->size() == rows.at(i)->size());
			m_dataArray->insert(rowIndex++, rows.at(i));
		}
	}

	void QSurfaceDataProxy3DPrivate::removeRows(int rowIndex, int removeCount)
	{
		Q_ASSERT(rowIndex >= 0);
		int maxRemoveCount = m_dataArray->size() - rowIndex;
		removeCount = qMin(removeCount, maxRemoveCount);
		for (int i = 0; i < removeCount; i++) {
			clearRow(rowIndex);
			m_dataArray->removeAt(rowIndex);
		}
	}

	QSurfaceDataProxy3D *QSurfaceDataProxy3DPrivate::qptr()
	{
		return static_cast<QSurfaceDataProxy3D *>(q_ptr);
	}

	void QSurfaceDataProxy3DPrivate::limitValues(QVector3D &minValues, QVector3D &maxValues,
		QAbstractAxis3D *axisX, QAbstractAxis3D *axisY,
		QAbstractAxis3D *axisZ) const
	{
		float min = 0.0f;
		float max = 0.0f;

		int rows = m_dataArray->size();
		int columns = 0;
		if (rows)
			columns = m_dataArray->at(0)->size();

		if (rows && columns) {
			min = m_dataArray->at(0)->at(0).y();
			max = m_dataArray->at(0)->at(0).y();
		}

		for (int i = 0; i < rows; i++) {
			QSurfaceDataRow3D *row = m_dataArray->at(i);
			if (row) {
				for (int j = 0; j < columns; j++) {
					float itemValue = m_dataArray->at(i)->at(j).y();
					if (qIsNaN(itemValue) || qIsInf(itemValue))
						continue;
					if (min > itemValue && isValidValue(itemValue, axisY))
						min = itemValue;
					if (max < itemValue)
						max = itemValue;
				}
			}
		}

		minValues.setY(min);
		maxValues.setY(max);

		if (columns) {
			// Have some defaults
			float xLow = m_dataArray->at(0)->at(0).x();
			float xHigh = m_dataArray->at(0)->last().x();
			float zLow = m_dataArray->at(0)->at(0).z();
			float zHigh = m_dataArray->last()->at(0).z();
			for (int i = 0; i < columns; i++) {
				float zItemValue = m_dataArray->at(0)->at(i).z();
				if (qIsNaN(zItemValue) || qIsInf(zItemValue))
					continue;
				else if (isValidValue(zItemValue, axisZ))
					zLow = qMin(zLow, zItemValue);
			}
			for (int i = 0; i < columns; i++) {
				float zItemValue = m_dataArray->last()->at(i).z();
				if (qIsNaN(zItemValue) || qIsInf(zItemValue))
					continue;
				else if (isValidValue(zItemValue, axisZ))
					zHigh = qMax(zHigh, zItemValue);
			}
			for (int i = 0; i < rows; i++) {
				float xItemValue = m_dataArray->at(i)->at(0).x();
				if (qIsNaN(xItemValue) || qIsInf(xItemValue))
					continue;
				else if (isValidValue(xItemValue, axisX))
					xLow = qMin(xLow, xItemValue);
			}
			for (int i = 0; i < rows; i++) {
				float xItemValue = m_dataArray->at(i)->last().x();
				if (qIsNaN(xItemValue) || qIsInf(xItemValue))
					continue;
				else if (isValidValue(xItemValue, axisX))
					xHigh = qMax(xHigh, xItemValue);
			}
			minValues.setX(xLow);
			minValues.setZ(zLow);
			maxValues.setX(xHigh);
			maxValues.setZ(zHigh);
		}
		else {
			minValues.setX(axisX->d_ptr->allowZero() ? 0.0f : 1.0f);
			minValues.setZ(axisZ->d_ptr->allowZero() ? 0.0f : 1.0f);
			maxValues.setX(axisX->d_ptr->allowZero() ? 0.0f : 1.0f);
			maxValues.setZ(axisZ->d_ptr->allowZero() ? 0.0f : 1.0f);
		}
	}

	bool QSurfaceDataProxy3DPrivate::isValidValue(float value, QAbstractAxis3D *axis) const
	{
		return (value > 0.0f || (value == 0.0f && axis->d_ptr->allowZero())
			|| (value < 0.0f && axis->d_ptr->allowNegatives()));
	}

	void QSurfaceDataProxy3DPrivate::clearRow(int rowIndex)
	{
		if (m_dataArray->at(rowIndex)) {
			delete m_dataArray->at(rowIndex);
			(*m_dataArray)[rowIndex] = 0;
		}
	}

	void QSurfaceDataProxy3DPrivate::clearArray()
	{
		for (int i = 0; i < m_dataArray->size(); i++)
			clearRow(i);
		m_dataArray->clear();
		delete m_dataArray;
	}

	void QSurfaceDataProxy3DPrivate::setSeries(QAbstractSeries3D *series)
	{
		QAbstractDataProxy3DPrivate::setSeries(series);
		QSurfaceSeries3D *surfaceSeries = static_cast<QSurfaceSeries3D *>(series);
		emit qptr()->seriesChanged(surfaceSeries);
	}

}
