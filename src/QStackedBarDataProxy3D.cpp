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

#include "QStackedBarDataProxy3DPrivate.h"
#include "QStackedBarSeries3D.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QStackedBarDataProxy3D
	* \inmodule QtDataVisualization
	* \brief The QStackedBarDataProxy3D class is the data proxy for a 3D bars graph.
	* \since QtDataVisualization 1.0
	*
	* A bar data proxy handles adding, inserting, changing, and removing rows of
	* data.
	*
	* The data array is a list of vectors (rows) of QStackedBarDataItem3D instances.
	* Each row can contain a different number of items or even be null.
	*
	* QStackedBarDataProxy3D takes ownership of all QtDataVisualization::QStackedBarDataRow3D objects
	* passed to it, whether directly or in a QtDataVisualization::QStackedBarDataArray3D container.
	* If bar data row pointers are used to directly modify data after adding the
	* array to the proxy, the appropriate signal must be emitted to update the
	* graph.
	*
	* QStackedBarDataProxy3D optionally keeps track of row and column labels, which QCategory3DAxis can utilize
	* to show axis labels. The row and column labels are stored in a separate array from the data and
	* row manipulation methods provide alternate versions that do not affect the row labels.
	* This enables the option of having row labels that relate to the position of the data in the
	* array rather than the data itself.
	*
	* \sa {Qt Data Visualization Data Handling}
	*/

	/*!
	* \typedef QtDataVisualization::QStackedBarDataRow3D
	* \relates QStackedBarDataProxy3D
	*
	* A vector of \l {QStackedBarDataItem3D} objects.
	*/

	/*!
	* \typedef QtDataVisualization::QStackedBarDataArray3D
	* \relates QStackedBarDataProxy3D
	*
	* A list of pointers to \l {QStackedBarDataRow3D} objects.
	*/

	/*!
	* \qmltype BarDataProxy
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.0
	* \ingroup datavisualization_qml
	* \instantiates QStackedBarDataProxy3D
	* \inherits AbstractDataProxy
	* \brief The data proxy for a 3D bars graph.
	*
	* This type handles adding, inserting, changing, and removing rows of data with Qt Quick 2.
	*
	* This type is uncreatable, but contains properties that are exposed via subtypes.
	*
	* For a more complete description, see QStackedBarDataProxy3D.
	*
	* \sa ItemModelBarDataProxy, {Qt Data Visualization Data Handling}
	*/

	/*!
	* \qmlproperty int BarDataProxy::rowCount
	* The number of rows in the array.
	*/

	/*!
	* \qmlproperty list BarDataProxy::rowLabels
	*
	* The optional row labels for the array. Indexes in this array match the row
	* indexes in the data array.
	* If the list is shorter than the number of rows, all rows will not get labels.
	*/

	/*!
	* \qmlproperty list BarDataProxy::columnLabels
	*
	* The optional column labels for the array. Indexes in this array match column indexes in rows.
	* If the list is shorter than the longest row, all columns will not get labels.
	*/

	/*!
	* \qmlproperty Bar3DSeries BarDataProxy::series
	*
	* The series this proxy is attached to.
	*/

	/*!
	* Constructs a bar data proxy with the given \a parent.
	*/
	QStackedBarDataProxy3D::QStackedBarDataProxy3D(QObject *parent) :
		QAbstractDataProxy3D(new QStackedBarDataProxy3DPrivate(this), parent)
	{
	}

	/*!
	* \internal
	*/
	QStackedBarDataProxy3D::QStackedBarDataProxy3D(QStackedBarDataProxy3DPrivate *d, QObject *parent) :
		QAbstractDataProxy3D(d, parent)
	{
	}

	/*!
	* Deletes the bar data proxy.
	*/
	QStackedBarDataProxy3D::~QStackedBarDataProxy3D()
	{
	}

	/*!
	* \property QStackedBarDataProxy3D::series
	*
	* \brief The series this proxy is attached to.
	*/
	QStackedBarSeries3D *QStackedBarDataProxy3D::series() const
	{
		return static_cast<QStackedBarSeries3D *>(d_ptr->series());
	}

	/*!
	* Clears the existing array and row and column labels.
	*/
	void QStackedBarDataProxy3D::resetArray()
	{
		resetArray(0, QStringList(), QStringList());
		emit rowCountChanged(rowCount());
		emit sectionCountChanged(sectionCount());
	}

	/*!
	* Takes ownership of the array \a newArray. Clears the existing array if the
	* new array differs from it. If the arrays are the same, this function
	* just triggers the arrayReset() signal.
	*
	* Passing a null array deletes the old array and creates a new empty array.
	* Row and column labels are not affected.
	*/
	void QStackedBarDataProxy3D::resetArray(QStackedBarDataArray3D *newArray)
	{
		dptr()->resetArray(newArray, 0, 0);
		emit arrayReset();
		emit rowCountChanged(rowCount());
		emit sectionCountChanged(sectionCount());
	}

	/*!
	* Takes ownership of the array \a newArray. Clears the existing array if the
	* new array differs from it. If the arrays are the same, this function
	* just triggers the arrayReset() signal.
	*
	* Passing a null array deletes the old array and creates a new empty array.
	*
	* The \a rowLabels and \a columnLabels lists specify the new labels for rows and columns.
	*/
	void QStackedBarDataProxy3D::resetArray(QStackedBarDataArray3D *newArray, const QStringList &rowLabels,
		const QStringList &columnLabels)
	{
		dptr()->resetArray(newArray, &rowLabels, &columnLabels);
		emit arrayReset();
		emit rowCountChanged(rowCount());
		emit sectionCountChanged(sectionCount());
	}

	/*!
	* Changes an existing row by replacing the row at the position \a rowIndex
	* with the new row specified by \a row. The new row can be
	* the same as the existing row already stored at \a rowIndex.
	* Existing row labels are not affected.
	*/
	void QStackedBarDataProxy3D::setRow(int rowIndex, QStackedBarDataRow3D *row)
	{
		dptr()->setRow(rowIndex, row, 0);
		emit rowsChanged(rowIndex, 1);
	}

	/*!
	* Changes an existing row by replacing the row at the position \a rowIndex
	* with the new row specified by \a row. The new row can be
	* the same as the existing row already stored at \a rowIndex.
	* Changes the row label to \a label.
	*/
	void QStackedBarDataProxy3D::setRow(int rowIndex, QStackedBarDataRow3D *row, const QString &label)
	{
		dptr()->setRow(rowIndex, row, &label);
		emit rowsChanged(rowIndex, 1);
	}

	/*!
	* Changes existing rows by replacing the rows starting at the position
	* \a rowIndex with the new rows specifies by \a rows.
	* Existing row labels are not affected. The rows in the \a rows array can be
	* the same as the existing rows already stored at \a rowIndex.
	*/
	void QStackedBarDataProxy3D::setRows(int rowIndex, const QStackedBarDataArray3D &rows)
	{
		dptr()->setRows(rowIndex, rows, 0);
		emit rowsChanged(rowIndex, rows.size());
	}

	/*!
	* Changes existing rows by replacing the rows starting at the position
	* \a rowIndex with the new rows specifies by \a rows.
	* The row labels are changed to \a labels. The rows in the \a rows array can be
	* the same as the existing rows already stored at \a rowIndex.
	*/
	void QStackedBarDataProxy3D::setRows(int rowIndex, const QStackedBarDataArray3D &rows, const QStringList &labels)
	{
		dptr()->setRows(rowIndex, rows, &labels);
		emit rowsChanged(rowIndex, rows.size());
	}

	/*!
	* Changes a single item at the position specified by \a rowIndex and
	* \a columnIndex to the item \a item.
	*/
	void QStackedBarDataProxy3D::setItem(int rowIndex, int columnIndex, const QStackedBarDataItem3D &item)
	{
		dptr()->setItem(rowIndex, columnIndex, item);
		emit itemChanged(rowIndex, columnIndex);
	}

	/*!
	* Changes a single item at the position \a position to the item \a item.
	* The x-value of \a position indicates the row and the y-value indicates the
	* column.
	*/
	void QStackedBarDataProxy3D::setItem(const QPoint &position, const QStackedBarDataItem3D &item)
	{
		setItem(position.x(), position.y(), item);
	}

	/*!
	* Adds the new row \a row to the end of an array.
	* Existing row labels are not affected.
	*
	* Returns the index of the added row.
	*/
	int QStackedBarDataProxy3D::addRow(QStackedBarDataRow3D *row)
	{
		int addIndex = dptr()->addRow(row, 0);
		emit rowsAdded(addIndex, 1);
		emit rowCountChanged(rowCount());
		return addIndex;
	}

	/*!
	* Adds a the new row \a row with the label \a label to the end of an array.
	*
	* Returns the index of the added row.
	*/
	int QStackedBarDataProxy3D::addRow(QStackedBarDataRow3D *row, const QString &label)
	{
		int addIndex = dptr()->addRow(row, &label);
		emit rowsAdded(addIndex, 1);
		emit rowCountChanged(rowCount());
		return addIndex;
	}

	/*!
	* Adds the new \a rows to the end of an array.
	* Existing row labels are not affected.
	*
	* Returns the index of the first added row.
	*/
	int QStackedBarDataProxy3D::addRows(const QStackedBarDataArray3D &rows)
	{
		int addIndex = dptr()->addRows(rows, 0);
		emit rowsAdded(addIndex, rows.size());
		emit rowCountChanged(rowCount());
		return addIndex;
	}

	/*!
	* Adds the new \a rows with \a labels to the end of the array.
	*
	* Returns the index of the first added row.
	*/
	int QStackedBarDataProxy3D::addRows(const QStackedBarDataArray3D &rows, const QStringList &labels)
	{
		int addIndex = dptr()->addRows(rows, &labels);
		emit rowsAdded(addIndex, rows.size());
		emit rowCountChanged(rowCount());
		return addIndex;
	}

	/*!
	* Inserts the new row \a row into \a rowIndex.
	* If \a rowIndex is equal to the array size, the rows are added to the end of
	* the array.
	* The existing row labels are not affected.
	* \note The row labels array will be out of sync with the row array after this call
	*       if there were labeled rows beyond the inserted row.
	*/
	void QStackedBarDataProxy3D::insertRow(int rowIndex, QStackedBarDataRow3D *row)
	{
		dptr()->insertRow(rowIndex, row, 0);
		emit rowsInserted(rowIndex, 1);
		emit rowCountChanged(rowCount());
	}

	/*!
	* Inserts the new row \a row with the label \a label into \a rowIndex.
	* If \a rowIndex is equal to array size, rows are added to the end of the
	* array.
	*/
	void QStackedBarDataProxy3D::insertRow(int rowIndex, QStackedBarDataRow3D *row, const QString &label)
	{
		dptr()->insertRow(rowIndex, row, &label);
		emit rowsInserted(rowIndex, 1);
		emit rowCountChanged(rowCount());
	}

	/*!
	* Inserts new \a rows into \a rowIndex.
	* If \a rowIndex is equal to the array size, the rows are added to the end of
	* the array. The existing row labels are not affected.
	* \note The row labels array will be out of sync with the row array after this call
	*       if there were labeled rows beyond the inserted rows.
	*/
	void QStackedBarDataProxy3D::insertRows(int rowIndex, const QStackedBarDataArray3D &rows)
	{
		dptr()->insertRows(rowIndex, rows, 0);
		emit rowsInserted(rowIndex, rows.size());
		emit rowCountChanged(rowCount());
	}

	/*!
	* Inserts new \a rows with \a labels into \a rowIndex.
	* If \a rowIndex is equal to the array size, the rows are added to the end of
	* the array.
	*/
	void QStackedBarDataProxy3D::insertRows(int rowIndex, const QStackedBarDataArray3D &rows, const QStringList &labels)
	{
		dptr()->insertRows(rowIndex, rows, &labels);
		emit rowsInserted(rowIndex, rows.size());
		emit rowCountChanged(rowCount());
	}

	/*!
	* Removes the number of rows specified by \a removeCount starting at the
	* position \a rowIndex. Attempting to remove rows past the end of the
	* array does nothing. If \a removeLabels is \c true, the corresponding row
	* labels are also removed. Otherwise, the row labels are not affected.
	* \note If \a removeLabels is \c false, the row labels array will be out of
	* sync with the row array if there are labeled rows beyond the removed rows.
	*/
	void QStackedBarDataProxy3D::removeRows(int rowIndex, int removeCount, bool removeLabels)
	{
		if (rowIndex < rowCount() && removeCount >= 1) {
			dptr()->removeRows(rowIndex, removeCount, removeLabels);
			emit rowsRemoved(rowIndex, removeCount);
			emit rowCountChanged(rowCount());
		}
	}

	/*!
	* \property QStackedBarDataProxy3D::rowCount
	*
	* \brief The number of rows in the array.
	*/
	int QStackedBarDataProxy3D::rowCount() const
	{
		return dptrc()->m_dataArray->size();
	}

	/*!
	* \property QStackedBarDataProxy3D::rowCount
	*
	* \brief The number of rows in the array.
	*/
	int QStackedBarDataProxy3D::sectionCount() const
	{
		return dptrc()->m_sectionCount;
	}

	/*!
	* \property QStackedBarDataProxy3D::rowLabels
	*
	* \brief The optional row labels for the array.
	*
	* Indexes in this array match the row indexes in the data array.
	* If the list is shorter than the number of rows, all rows will not get labels.
	*/
	QStringList QStackedBarDataProxy3D::rowLabels() const
	{
		return dptrc()->m_rowLabels;
	}

	void QStackedBarDataProxy3D::setRowLabels(const QStringList &labels)
	{
		if (dptr()->m_rowLabels != labels) {
			dptr()->m_rowLabels = labels;
			emit rowLabelsChanged();
		}
	}

	/*!
	* \property QStackedBarDataProxy3D::columnLabels
	*
	* \brief The optional column labels for the array.
	*
	* Indexes in this array match column indexes in rows.
	* If the list is shorter than the longest row, all columns will not get labels.
	*/
	QStringList QStackedBarDataProxy3D::columnLabels() const
	{
		return dptrc()->m_columnLabels;
	}

	void QStackedBarDataProxy3D::setColumnLabels(const QStringList &labels)
	{
		if (dptr()->m_columnLabels != labels) {
			dptr()->m_columnLabels = labels;
			emit columnLabelsChanged();
		}
	}

	/*!
	* Returns the pointer to the data array.
	*/
	const QStackedBarDataArray3D *QStackedBarDataProxy3D::array() const
	{
		return dptrc()->m_dataArray;
	}

	/*!
	* Returns the pointer to the row at the position \a rowIndex. It is guaranteed
	* to be valid only until the next call that modifies data.
	*/
	const QStackedBarDataRow3D *QStackedBarDataProxy3D::rowAt(int rowIndex) const
	{
		const QStackedBarDataArray3D &dataArray = *dptrc()->m_dataArray;
		Q_ASSERT(rowIndex >= 0 && rowIndex < dataArray.size());
		return dataArray[rowIndex];
	}

	/*!
	* Returns the pointer to the item at the position specified by \a rowIndex and
	* \a columnIndex. It is guaranteed to be valid only
	* until the next call that modifies data.
	*/
	const QStackedBarDataItem3D *QStackedBarDataProxy3D::itemAt(int rowIndex, int columnIndex) const
	{
		const QStackedBarDataArray3D &dataArray = *dptrc()->m_dataArray;
		Q_ASSERT(rowIndex >= 0 && rowIndex < dataArray.size());
		const QStackedBarDataRow3D &dataRow = *dataArray[rowIndex];
		Q_ASSERT(columnIndex >= 0 && columnIndex < dataRow.size());
		return &dataRow.at(columnIndex);
	}

	/*!
	* Returns the pointer to the item at the position \a position. The x-value of
	* \a position indicates the row and the y-value indicates the column. The item
	* is guaranteed to be valid only until the next call that modifies data.
	*/
	const QStackedBarDataItem3D *QStackedBarDataProxy3D::itemAt(const QPoint &position) const
	{
		return itemAt(position.x(), position.y());
	}

	/*!
	* \internal
	*/
	QStackedBarDataProxy3DPrivate *QStackedBarDataProxy3D::dptr()
	{
		return static_cast<QStackedBarDataProxy3DPrivate *>(d_ptr.data());
	}

	/*!
	* \internal
	*/
	const QStackedBarDataProxy3DPrivate *QStackedBarDataProxy3D::dptrc() const
	{
		return static_cast<const QStackedBarDataProxy3DPrivate *>(d_ptr.data());
	}

	/*!
	* \fn void QStackedBarDataProxy3D::arrayReset()
	*
	* This signal is emitted when the data array is reset.
	* If the contents of the whole array are changed without calling resetArray(),
	* this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QStackedBarDataProxy3D::rowsAdded(int startIndex, int count)
	*
	* This signal is emitted when the number of rows specified by \a count is
	* added starting at the position \a startIndex.
	* If rows are added to the array without calling addRow() or addRows(),
	* this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QStackedBarDataProxy3D::rowsChanged(int startIndex, int count)
	*
	* This signal is emitted when the number of rows specified by \a count is
	* changed starting at the position \a startIndex.
	* If rows are changed in the array without calling setRow() or setRows(),
	* this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QStackedBarDataProxy3D::rowsRemoved(int startIndex, int count)
	*
	* This signal is emitted when the number of rows specified by \a count is
	* removed starting at the position \a startIndex.
	*
	* The index is the current array size if the rows were removed from the end of
	* the array. If rows are removed from the array without calling removeRows(),
	* this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QStackedBarDataProxy3D::rowsInserted(int startIndex, int count)
	*
	* This signal is emitted when the number of rows specified by \a count is
	* inserted at the position \a startIndex.
	*
	* If rows are inserted into the array without calling insertRow() or
	* insertRows(), this signal needs to be emitted to update the graph.
	*/

	/*!
	* \fn void QStackedBarDataProxy3D::itemChanged(int rowIndex, int columnIndex)
	*
	* This signal is emitted when the item at the position specified by \a rowIndex
	* and \a columnIndex changes.
	* If the item is changed in the array without calling setItem(),
	* this signal needs to be emitted to update the graph.
	*/

	// QStackedBarDataProxy3DPrivate

	QStackedBarDataProxy3DPrivate::QStackedBarDataProxy3DPrivate(QStackedBarDataProxy3D *q)
		: QAbstractDataProxy3DPrivate(q, QAbstractDataProxy3D::DataTypeBar),
		m_dataArray(new QStackedBarDataArray3D)
	{
	}

	QStackedBarDataProxy3DPrivate::~QStackedBarDataProxy3DPrivate()
	{
		clearArray();
	}

	int QStackedBarDataProxy3DPrivate::sectionCount() const 
	{
		return m_sectionCount;
	}

	void QStackedBarDataProxy3DPrivate::resetArray(QStackedBarDataArray3D *newArray, const QStringList *rowLabels,
		const QStringList *columnLabels)
	{
		if (rowLabels)
			qptr()->setRowLabels(*rowLabels);
		if (columnLabels)
			qptr()->setColumnLabels(*columnLabels);

		if (!newArray)
		{
			newArray = new QStackedBarDataArray3D;
			// No data array pointer provided means default section count per bar equals 1
			m_sectionCount = 1;
		}

		if (newArray != m_dataArray) {
			clearArray();
			m_dataArray = newArray;
			if (m_dataArray->size() > 0 && m_dataArray->at(0)->size() > 0)
			{
				m_sectionCount = m_dataArray->at(0)->at(0).values().size();
			}
			else
			{
				m_sectionCount = 1;
			}
		}
	}

	void QStackedBarDataProxy3DPrivate::setRow(int rowIndex, QStackedBarDataRow3D *row, const QString *label)
	{
		Q_ASSERT(rowIndex >= 0 && rowIndex < m_dataArray->size());

		if (label)
			fixRowLabels(rowIndex, 1, QStringList(*label), false);
		if (row != m_dataArray->at(rowIndex)) {
			clearRow(rowIndex);
			(*m_dataArray)[rowIndex] = row;
		}
	}

	void QStackedBarDataProxy3DPrivate::setRows(int rowIndex, const QStackedBarDataArray3D &rows,
		const QStringList *labels)
	{
		QStackedBarDataArray3D &dataArray = *m_dataArray;
		Q_ASSERT(rowIndex >= 0 && (rowIndex + rows.size()) <= dataArray.size());
		if (labels)
			fixRowLabels(rowIndex, rows.size(), *labels, false);
		for (int i = 0; i < rows.size(); i++) {
			if (rows.at(i) != dataArray.at(rowIndex)) {
				clearRow(rowIndex);
				dataArray[rowIndex] = rows.at(i);
			}
			rowIndex++;
		}
	}

	void QStackedBarDataProxy3DPrivate::setItem(int rowIndex, int columnIndex, const QStackedBarDataItem3D &item)
	{
		Q_ASSERT(rowIndex >= 0 && rowIndex < m_dataArray->size());
		QStackedBarDataRow3D &row = *(*m_dataArray)[rowIndex];
		Q_ASSERT(columnIndex < row.size());
		row[columnIndex] = item;
	}

	int QStackedBarDataProxy3DPrivate::addRow(QStackedBarDataRow3D *row, const QString *label)
	{
		int currentSize = m_dataArray->size();
		if (label)
			fixRowLabels(currentSize, 1, QStringList(*label), false);
		m_dataArray->append(row);
		return currentSize;
	}

	int QStackedBarDataProxy3DPrivate::addRows(const QStackedBarDataArray3D &rows, const QStringList *labels)
	{
		int currentSize = m_dataArray->size();
		if (labels)
			fixRowLabels(currentSize, rows.size(), *labels, false);
		for (int i = 0; i < rows.size(); i++)
			m_dataArray->append(rows.at(i));
		return currentSize;
	}

	void QStackedBarDataProxy3DPrivate::insertRow(int rowIndex, QStackedBarDataRow3D *row, const QString *label)
	{
		Q_ASSERT(rowIndex >= 0 && rowIndex <= m_dataArray->size());
		if (label)
			fixRowLabels(rowIndex, 1, QStringList(*label), true);
		m_dataArray->insert(rowIndex, row);
	}

	void QStackedBarDataProxy3DPrivate::insertRows(int rowIndex, const QStackedBarDataArray3D &rows,
		const QStringList *labels)
	{
		Q_ASSERT(rowIndex >= 0 && rowIndex <= m_dataArray->size());
		if (labels)
			fixRowLabels(rowIndex, rows.size(), *labels, true);
		for (int i = 0; i < rows.size(); i++)
			m_dataArray->insert(rowIndex++, rows.at(i));
	}

	void QStackedBarDataProxy3DPrivate::removeRows(int rowIndex, int removeCount, bool removeLabels)
	{
		Q_ASSERT(rowIndex >= 0);
		int maxRemoveCount = m_dataArray->size() - rowIndex;
		removeCount = qMin(removeCount, maxRemoveCount);
		bool labelsChanged = false;
		for (int i = 0; i < removeCount; i++) {
			clearRow(rowIndex);
			m_dataArray->removeAt(rowIndex);
			if (removeLabels && m_rowLabels.size() > rowIndex) {
				m_rowLabels.removeAt(rowIndex);
				labelsChanged = true;
			}
		}
		if (labelsChanged)
			emit qptr()->rowLabelsChanged();
	}

	QStackedBarDataProxy3D *QStackedBarDataProxy3DPrivate::qptr()
	{
		return static_cast<QStackedBarDataProxy3D *>(q_ptr);
	}

	void QStackedBarDataProxy3DPrivate::clearRow(int rowIndex)
	{
		if (m_dataArray->at(rowIndex)) {
			delete m_dataArray->at(rowIndex);
			(*m_dataArray)[rowIndex] = 0;
		}
	}

	void QStackedBarDataProxy3DPrivate::clearArray()
	{
		for (int i = 0; i < m_dataArray->size(); i++)
			clearRow(i);
		m_dataArray->clear();
		delete m_dataArray;
	}

	/*!
	* \internal
	* Fixes the row label array to include specified labels.
	*/
	void QStackedBarDataProxy3DPrivate::fixRowLabels(int startIndex, int count, const QStringList &newLabels,
		bool isInsert)
	{
		bool changed = false;
		int currentSize = m_rowLabels.size();

		int newSize = newLabels.size();
		if (startIndex >= currentSize) {
			// Adding labels past old label array, create empty strings to fill intervening space
			if (newSize) {
				for (int i = currentSize; i < startIndex; i++)
					m_rowLabels << QString();
				// Doesn't matter if insert, append, or just change when there were no existing
				// strings, just append new strings.
				m_rowLabels << newLabels;
				changed = true;
			}
		}
		else {
			if (isInsert) {
				int insertIndex = startIndex;
				if (count)
					changed = true;
				for (int i = 0; i < count; i++) {
					if (i < newSize)
						m_rowLabels.insert(insertIndex++, newLabels.at(i));
					else
						m_rowLabels.insert(insertIndex++, QString());
				}
			}
			else {
				// Either append or change, replace labels up to array end and then add new ones
				int lastChangeIndex = count + startIndex;
				int newIndex = 0;
				for (int i = startIndex; i < lastChangeIndex; i++) {
					if (i >= currentSize) {
						// Label past the current size, so just append the new label
						if (newSize < newIndex) {
							changed = true;
							m_rowLabels << newLabels.at(newIndex);
						}
						else {
							break; // No point appending empty strings, so just exit
						}
					}
					else if (newSize > newIndex) {
						// Replace existing label
						if (m_rowLabels.at(i) != newLabels.at(newIndex)) {
							changed = true;
							m_rowLabels[i] = newLabels.at(newIndex);
						}
					}
					else {
						// No more new labels, so clear existing label
						if (!m_rowLabels.at(i).isEmpty()) {
							changed = true;
							m_rowLabels[i] = QString();
						}
					}
					newIndex++;
				}
			}
		}
		if (changed)
			emit qptr()->rowLabelsChanged();
	}

	QPair<GLfloat, GLfloat> QStackedBarDataProxy3DPrivate::limitValues(int startRow, int endRow,
		int startColumn, int endColumn) const
	{
		QPair<GLfloat, GLfloat> limits = qMakePair(0.0f, 0.0f);
		endRow = qMin(endRow, m_dataArray->size() - 1);
		for (int i = startRow; i <= endRow; i++) {
			QStackedBarDataRow3D *row = m_dataArray->at(i);
			if (row) {
				int lastColumn = qMin(endColumn, row->size() - 1);
				for (int j = startColumn; j <= lastColumn; j++) {
					const QStackedBarDataItem3D &item = row->at(j);
					for (int k = 0; k < item.values().size(); k++)
					{
						float itemValue = item.values().at(k);
						if (limits.second < itemValue)
							limits.second = itemValue;
						if (limits.first > itemValue)
							limits.first = itemValue;
					}
				}
			}
		}
		return limits;
	}

	void QStackedBarDataProxy3DPrivate::setSeries(QAbstractSeries3D *series)
	{
		QAbstractDataProxy3DPrivate::setSeries(series);
		QStackedBarSeries3D *barSeries = static_cast<QStackedBarSeries3D *>(series);
		emit qptr()->seriesChanged(barSeries);
	}

}
