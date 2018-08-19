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

#include "QAxisValueFormatter3DPrivate.h"
#include "QValueAxis3DPrivate.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QAxisValueFormatter3D
	* \inmodule QtDataVisualization
	* \brief The QAxisValueFormatter3D class is a base class for value axis
	* formatters.
	* \since QtDataVisualization 1.1
	*
	* This class provides formatting rules for a linear value 3D axis. Subclass it if you
	* want to implement custom value axes.
	*
	* The base class has no public API beyond constructors and destructors. It is meant to be only
	* used internally. However, subclasses may implement public properties as needed.
	*
	* \sa QValueAxis3D, QLogValue3DAxisFormatter
	*/

	/*!
	* \qmltype ValueAxis3DFormatter
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.1
	* \ingroup datavisualization_qml
	* \instantiates QAxisValueFormatter3D
	* \brief A base type for value axis formatters.
	*
	* This type provides formatting rules for a linear value 3D axis.
	* This type is the default type for ValueAxis3D and thus never needs to be explicitly created.
	* This type has no public functionality.
	*
	* \sa ValueAxis3D
	*/

	/*!
	* \internal
	*/
	QAxisValueFormatter3D::QAxisValueFormatter3D(QAxisValueFormatter3DPrivate *d, QObject *parent) :
		QObject(parent),
		d_ptr(d)
	{
	}

	/*!
	* Constructs a new value 3D axis formatter with the optional parent \a parent.
	*/
	QAxisValueFormatter3D::QAxisValueFormatter3D(QObject *parent) :
		QObject(parent),
		d_ptr(new QAxisValueFormatter3DPrivate(this))
	{
	}

	/*!
	* Deletes the value 3D axis formatter.
	*/
	QAxisValueFormatter3D::~QAxisValueFormatter3D()
	{
	}

	/*!
	* Allows the parent axis to have negative values if \a allow is \c true.
	*/
	void QAxisValueFormatter3D::setAllowNegatives(bool allow)
	{
		d_ptr->m_allowNegatives = allow;
	}

	/*!
	* Returns \c true if negative values are valid values for the parent axis.
	* The default implementation always returns \c true.
	*/
	bool QAxisValueFormatter3D::allowNegatives() const
	{
		return d_ptr->m_allowNegatives;
	}

	/*!
	* Allows the parent axis to have a zero value if \a allow is \c true.
	*/
	void QAxisValueFormatter3D::setAllowZero(bool allow)
	{
		d_ptr->m_allowZero = allow;
	}

	/*!
	* Returns \c true if zero is a valid value for the parent axis.
	* The default implementation always returns \c true.
	*/
	bool QAxisValueFormatter3D::allowZero() const
	{
		return d_ptr->m_allowZero;
	}

	/*!
	* Creates a new empty value 3D axis formatter. Must be reimplemented in a
	* subclass.
	*
	* Returns the new formatter. The renderer uses this method to cache a copy of
	* the formatter. The ownership of the new copy is transferred to the caller.
	*/
	QAxisValueFormatter3D *QAxisValueFormatter3D::createNewInstance() const
	{
		return new QAxisValueFormatter3D();
	}

	/*!
	* Resizes and populates the label and grid line position arrays and the label
	* strings array, as well as calculates any values needed to map a value to its
	* position. The parent axis can be accessed from inside this function.
	*
	* This method must be reimplemented in a subclass if the default array contents are not suitable.
	*
	* See gridPositions(), subGridPositions(), labelPositions(), and labelStrings() methods for
	* documentation about the arrays that need to be resized and populated.
	*
	* \sa gridPositions(), subGridPositions(), labelPositions(), labelStrings(), axis()
	*/
	void QAxisValueFormatter3D::recalculate()
	{
		d_ptr->doRecalculate();
	}

	/*!
	* Returns the formatted label string using the specified \a value and
	* \a format.
	*
	* Reimplement this method in a subclass to resolve the formatted string for a given \a value
	* if the default formatting rules specified for QValueAxis3D::labelFormat property are not
	* sufficient.
	*
	* \sa recalculate(), labelStrings(), QValueAxis3D::labelFormat
	*/
	QString QAxisValueFormatter3D::stringForValue(qreal value, const QString &format) const
	{
		return d_ptr->stringForValue(value, format);
	}

	/*!
	* Returns the normalized position along the axis for the given \a value.
	* The returned value should be between \c 0.0 (the minimum value) and
	* \c 1.0 (the maximum value), inclusive, if the value is within the parent
	* axis range.
	*
	* Reimplement this method if the position cannot be resolved by linear
	* interpolation between the parent axis minimum and maximum values.
	*
	* \sa recalculate(), valueAt()
	*/
	float QAxisValueFormatter3D::positionAt(float value) const
	{
		return d_ptr->positionAt(value);
	}

	/*!
	* Returns the value at the normalized \a position along the axis.
	* The \a position value should be between \c 0.0 (the minimum value) and
	* \c 1.0 (the maximum value), inclusive, to obtain values within the parent
	* axis range.
	*
	* Reimplement this method if the value cannot be resolved by linear
	* interpolation between the parent axis minimum and maximum values.
	*
	* \sa recalculate(), positionAt()
	*/
	float QAxisValueFormatter3D::valueAt(float position) const
	{
		return d_ptr->valueAt(position);
	}

	/*!
	* Copies all the values necessary for resolving positions, values, and strings
	* with this formatter to the \a copy of the formatter. When reimplementing
	* this method in a subclass, call the superclass version at some point.
	* The renderer uses this method to cache a copy of the formatter.
	*
	* Returns the new copy. The ownership of the new copy transfers to the caller.
	*/
	void QAxisValueFormatter3D::populateCopy(QAxisValueFormatter3D &copy) const
	{
		d_ptr->doPopulateCopy(*(copy.d_ptr.data()));
	}

	/*!
	* Marks this formatter dirty, prompting the renderer to make a new copy of its cache on the next
	* renderer synchronization. This method should be called by a subclass whenever the formatter
	* is changed in a way that affects the resolved values. Set \a labelsChange to
	* \c true if the change requires regenerating the parent axis label strings.
	*/
	void QAxisValueFormatter3D::markDirty(bool labelsChange)
	{
		d_ptr->markDirty(labelsChange);
	}

	/*!
	* Returns the parent axis. The parent axis must only be accessed in the recalculate()
	* method to maintain thread safety in environments using a threaded renderer.
	*
	* \sa recalculate()
	*/
	QValueAxis3D *QAxisValueFormatter3D::axis() const
	{
		return d_ptr->m_axis;
	}

	/*!
	* Returns a reference to the array of normalized grid line positions.
	* The default array size is equal to the segment count of the parent axis plus one, but
	* a subclassed implementation of the recalculate() method may resize the array differently.
	* The values should be between \c 0.0 (the minimum value) and \c 1.0 (the
	* maximum value), inclusive.
	*
	* \sa QValueAxis3D::segmentCount, recalculate()
	*/
	QVector<float> &QAxisValueFormatter3D::gridPositions() const
	{
		return d_ptr->m_gridPositions;
	}

	/*!
	* Returns a reference to the array of normalized subgrid line positions.
	* The default array size is equal to the segment count of the parent axis times
	* the sub-segment count of the parent axis minus one, but a subclassed
	* implementation of the recalculate() method may resize the array differently.
	* The values should be between \c 0.0 (the minimum value) and \c 1.0 (the
	* maximum value), inclusive.
	*
	* \sa QValueAxis3D::segmentCount, QValueAxis3D::subSegmentCount, recalculate()
	*/
	QVector<float> &QAxisValueFormatter3D::subGridPositions() const
	{
		return d_ptr->m_subGridPositions;
	}

	/*!
	* Returns a reference to the array of normalized label positions.
	* The default array size is equal to the segment count of the parent axis plus one, but
	* a subclassed implementation of the recalculate() method may resize the array
	* differently. The values should be between \c 0.0 (the minimum value) and
	* \c 1.0 (the maximum value), inclusive.
	* By default, the label at the index zero corresponds to the minimum value
	* of the axis.
	*
	* \sa QValueAxis3D::segmentCount, QAbstractAxis3D::labels, recalculate()
	*/
	QVector<float> &QAxisValueFormatter3D::labelPositions() const
	{
		return d_ptr->m_labelPositions;
	}

	/*!
	* Returns a reference to the string list containing formatter label strings.
	* The array size must be equal to the size of the label positions array, which
	* the indexes also correspond to.
	*
	* \sa labelPositions()
	*/
	QStringList &QAxisValueFormatter3D::labelStrings() const
	{
		return d_ptr->m_labelStrings;
	}

	/*!
	* Sets the \a locale that this formatter uses.
	* The graph automatically sets the formatter's locale to a graph's locale whenever the parent axis
	* is set as an active axis of the graph, the axis formatter is set to an axis attached to
	* the graph, or the graph's locale changes.
	*
	* \sa locale(), QAbstractGraph3D::locale
	*/
	void QAxisValueFormatter3D::setLocale(const QLocale &locale)
	{
		d_ptr->m_cLocaleInUse = (locale == QLocale::c());
		d_ptr->m_locale = locale;
		markDirty(true);
	}
	/*!
	* Returns the current locale this formatter is using.
	*/
	QLocale QAxisValueFormatter3D::locale() const
	{
		return d_ptr->m_locale;
	}

	// QAxisValueFormatter3DPrivate
	QAxisValueFormatter3DPrivate::QAxisValueFormatter3DPrivate(QAxisValueFormatter3D *q)
		: QObject(0),
		q_ptr(q),
		m_needsRecalculate(true),
		m_min(0.0f),
		m_max(0.0f),
		m_rangeNormalizer(0.0f),
		m_axis(0),
		m_preparsedParamType(QUtils3D::ParamTypeUnknown),
		m_allowNegatives(true),
		m_allowZero(true),
		m_formatPrecision(6), // 6 and 'g' are defaults in Qt API for format precision and spec
		m_formatSpec('g'),
		m_cLocaleInUse(true)
	{
	}

	QAxisValueFormatter3DPrivate::~QAxisValueFormatter3DPrivate()
	{
	}

	void QAxisValueFormatter3DPrivate::recalculate()
	{
		// Only recalculate if we need to and have m_axis pointer. If we do not have
		// m_axis, either we are not attached to an axis or this is a renderer cache.
		if (m_axis && m_needsRecalculate) {
			m_min = m_axis->min();
			m_max = m_axis->max();
			m_rangeNormalizer = (m_max - m_min);

			q_ptr->recalculate();
			m_needsRecalculate = false;
		}
	}

	void QAxisValueFormatter3DPrivate::doRecalculate()
	{
		int segmentCount = m_axis->segmentCount();
		int subGridCount = m_axis->subSegmentCount() - 1;
		QString labelFormat = m_axis->labelFormat();

		m_gridPositions.resize(segmentCount + 1);
		m_subGridPositions.resize(segmentCount * subGridCount);

		m_labelPositions.resize(segmentCount + 1);
		m_labelStrings.clear();
		m_labelStrings.reserve(segmentCount + 1);

		// Use qreals for intermediate calculations for better accuracy on label values
		qreal segmentStep = 1.0 / qreal(segmentCount);
		qreal subSegmentStep = 0;
		if (subGridCount > 0)
			subSegmentStep = segmentStep / qreal(subGridCount + 1);

		// Calculate positions
		qreal rangeNormalizer = qreal(m_max - m_min);
		for (int i = 0; i < segmentCount; i++) {
			qreal gridValue = segmentStep * qreal(i);
			m_gridPositions[i] = float(gridValue);
			m_labelPositions[i] = float(gridValue);
			m_labelStrings << q_ptr->stringForValue(gridValue * rangeNormalizer + qreal(m_min),
				labelFormat);
			if (m_subGridPositions.size()) {
				for (int j = 0; j < subGridCount; j++)
					m_subGridPositions[i * subGridCount + j] = gridValue + subSegmentStep * (j + 1);
			}
		}

		// Ensure max value doesn't suffer from any rounding errors
		m_gridPositions[segmentCount] = 1.0f;
		m_labelPositions[segmentCount] = 1.0f;
		m_labelStrings << q_ptr->stringForValue(qreal(m_max), labelFormat);
	}

	void QAxisValueFormatter3DPrivate::populateCopy(QAxisValueFormatter3D &copy)
	{
		recalculate();
		q_ptr->populateCopy(copy);
	}

	void QAxisValueFormatter3DPrivate::doPopulateCopy(QAxisValueFormatter3DPrivate &copy)
	{
		copy.m_min = m_min;
		copy.m_max = m_max;
		copy.m_rangeNormalizer = m_rangeNormalizer;

		copy.m_gridPositions = m_gridPositions;
		copy.m_labelPositions = m_labelPositions;
		copy.m_subGridPositions = m_subGridPositions;
	}

	QString QAxisValueFormatter3DPrivate::stringForValue(qreal value, const QString &format)
	{
		if (m_previousLabelFormat.compare(format)) {
			// Format string different than the previous one used, reparse it
			m_labelFormatArray = format.toUtf8();
			m_previousLabelFormat = format;
			m_preparsedParamType = QUtils3D::preParseFormat(format, m_formatPreStr, m_formatPostStr,
				m_formatPrecision, m_formatSpec);
		}

		if (m_cLocaleInUse) {
			return QUtils3D::formatLabelSprintf(m_labelFormatArray, m_preparsedParamType, value);
		}
		else {
			return QUtils3D::formatLabelLocalized(m_preparsedParamType, value, m_locale, m_formatPreStr,
				m_formatPostStr, m_formatPrecision, m_formatSpec,
				m_labelFormatArray);
		}
	}

	float QAxisValueFormatter3DPrivate::positionAt(float value) const
	{
		return ((value - m_min) / m_rangeNormalizer);
	}

	float QAxisValueFormatter3DPrivate::valueAt(float position) const
	{
		return ((position * m_rangeNormalizer) + m_min);
	}

	void QAxisValueFormatter3DPrivate::setAxis(QValueAxis3D *axis)
	{
		Q_ASSERT(axis);

		// These signals are all connected to markDirtyNoLabelChange slot, even though most of them
		// do require labels to be regenerated. This is because the label regeneration is triggered
		// elsewhere in these cases.
		connect(axis, &QValueAxis3D::segmentCountChanged,
			this, &QAxisValueFormatter3DPrivate::markDirtyNoLabelChange);
		connect(axis, &QValueAxis3D::subSegmentCountChanged,
			this, &QAxisValueFormatter3DPrivate::markDirtyNoLabelChange);
		connect(axis, &QValueAxis3D::labelFormatChanged,
			this, &QAxisValueFormatter3DPrivate::markDirtyNoLabelChange);
		connect(axis, &QAbstractAxis3D::rangeChanged,
			this, &QAxisValueFormatter3DPrivate::markDirtyNoLabelChange);

		m_axis = axis;
	}

	void QAxisValueFormatter3DPrivate::markDirty(bool labelsChange)
	{
		m_needsRecalculate = true;
		if (m_axis) {
			if (labelsChange)
				m_axis->dptr()->emitLabelsChanged();
			if (m_axis && m_axis->orientation() != QAbstractAxis3D::AxisOrientationNone)
				emit m_axis->dptr()->formatterDirty();
		}
	}

	void QAxisValueFormatter3DPrivate::markDirtyNoLabelChange()
	{
		markDirty(false);
	}

}
