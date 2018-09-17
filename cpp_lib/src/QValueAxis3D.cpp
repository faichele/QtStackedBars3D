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

#include "QValueAxis3DPrivate.h"
#include "QAxisValueFormatter3DPrivate.h"
#include "QAbstractController3D.h"
#include "QUtils3D.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QValueAxis3D
	* \inmodule QtDataVisualization
	* \brief The QValueAxis3D class manipulates an axis of a graph.
	* \since QtDataVisualization 1.0
	*
	* A value axis can be given a range of values and segment and subsegment
	* counts to divide the range into.
	*
	* Labels are drawn between each segment. Grid lines are drawn between each segment and each
	* subsegment. \note If visible, there will always be at least two grid lines and labels indicating
	* the minimum and the maximum values of the range, as there is always at least one segment.
	*/

	/*!
	* \qmltype ValueAxis3D
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.0
	* \ingroup datavisualization_qml
	* \instantiates QValueAxis3D
	* \inherits AbstractAxis3D
	* \brief Manipulates an axis of a graph.
	*
	* This type provides an axis that can be given a range of values and segment and subsegment
	* counts to divide the range into.
	*/


	/*!
	* \qmlproperty int ValueAxis3D::segmentCount
	*
	* The number of segments on the axis. This indicates how many labels are drawn. The number
	* of grid lines to be drawn is calculated with the following formula:
	* \c {segments * subsegments + 1}.
	* The preset default is \c 5. The value cannot be below \c 1.
	*/

	/*!
	* \qmlproperty int ValueAxis3D::subSegmentCount
	*
	* The number of subsegments inside each segment on the axis. Grid lines are drawn between
	* each subsegment, in addition to each segment.
	* The preset default is \c 1. The value cannot be below \c 1.
	*/

	/*!
	* \qmlproperty string ValueAxis3D::labelFormat
	*
	* The label format to be used for the labels on this axis.
	*
	* The format string supports the following conversion specifiers, length
	* modifiers, and flags provided by \c printf() in the standard C++ library:
	* d, i, o, x, X, f, F, e, E, g, G, c.
	*
	* If AbstractGraph3D::locale is anything else than \c{"C"}, the supported
	* specifiers are limited to: d, e, E, f, g, G, and i. Also, only the precision
	* modifier is supported. The rest of the formatting comes from the default
	* \l Locale of the application.
	*
	* \sa AbstractGraph3D::locale
	*/

	/*!
	* \qmlproperty ValueAxis3DFormatter ValueAxis3D::formatter
	* \since QtDataVisualization 1.1
	*
	* The axis formatter to be used. Any existing formatter is deleted when a new formatter
	* is set.
	*
	*/

	/*!
	* \qmlproperty bool ValueAxis3D::reversed
	* \since QtDataVisualization 1.1
	*
	* If \c{true}, the axis will be rendered in reverse. That is, the positions of
	* the minimum and maximum values are swapped when the graph is rendered. This
	* property does not affect the actual minimum and maximum values of the axis.
	*/

	/*!
	* Constructs QValueAxis3D with the given \a parent.
	*/
	QValueAxis3D::QValueAxis3D(QObject *parent) :
		QAbstractAxis3D(new QValueAxis3DPrivate(this), parent)
	{
		setFormatter(new QAxisValueFormatter3D);
	}

	/*!
	* Destroys QValueAxis3D.
	*/
	QValueAxis3D::~QValueAxis3D()
	{
	}

	/*!
	* \property QValueAxis3D::segmentCount
	*
	* \brief The number of segments on the axis.
	*
	* This indicates how many labels are drawn. The number
	* of grid lines to be drawn is calculated with formula: \c {segments * subsegments + 1}.
	* The preset default is \c 5. The value cannot be below \c 1.
	*
	* \sa setSubSegmentCount()
	*/
	void QValueAxis3D::setSegmentCount(int count)
	{
		if (count <= 0) {
			qWarning() << "Warning: Illegal segment count automatically adjusted to a legal one:"
				<< count << "-> 1";
			count = 1;
		}
		if (dptr()->m_segmentCount != count) {
			dptr()->m_segmentCount = count;
			dptr()->emitLabelsChanged();
			emit segmentCountChanged(count);
		}
	}

	int QValueAxis3D::segmentCount() const
	{
		return dptrc()->m_segmentCount;
	}

	/*!
	* \property QValueAxis3D::subSegmentCount
	*
	* \brief The number of subsegments inside each segment on the axis.
	*
	* Grid lines are drawn between
	* each subsegment, in addition to each segment.
	* The preset default is \c 1. The value cannot be below \c 1.
	*
	* \sa setSegmentCount()
	*/
	void QValueAxis3D::setSubSegmentCount(int count)
	{
		if (count <= 0) {
			qWarning() << "Warning: Illegal subsegment count automatically adjusted to a legal one:"
				<< count << "-> 1";
			count = 1;
		}
		if (dptr()->m_subSegmentCount != count) {
			dptr()->m_subSegmentCount = count;
			emit subSegmentCountChanged(count);
		}
	}

	int QValueAxis3D::subSegmentCount() const
	{
		return dptrc()->m_subSegmentCount;
	}

	/*!
	* \property QValueAxis3D::labelFormat
	*
	* \brief The label format to be used for the labels on this axis.
	*
	* The format string supports the following conversion specifiers, length
	* modifiers, and flags provided by \c printf() in the standard C++ library:
	* d, i, o, x, X, f, F, e, E, g, G, c.
	*
	* If QAbstractGraph3D::locale is anything else than \c{"C"}, the supported
	* specifiers are limited to: d, e, E, f, g, G, and i. Also, only the precision
	* modifier is supported. The rest of the formatting comes from the default
	* QLocale of the application.
	*
	* Usage example:
	*
	* \c {axis->setLabelFormat("%.2f mm");}
	*
	* \sa formatter, QAbstractGraph3D::locale
	*/
	void QValueAxis3D::setLabelFormat(const QString &format)
	{
		if (dptr()->m_labelFormat != format) {
			dptr()->m_labelFormat = format;
			dptr()->emitLabelsChanged();
			emit labelFormatChanged(format);
		}
	}

	QString QValueAxis3D::labelFormat() const
	{
		return dptrc()->m_labelFormat;
	}

	/*!
	* \property QValueAxis3D::formatter
	* \since QtDataVisualization 1.1
	*
	* \brief The axis formatter to be used.
	*
	* Any existing formatter is deleted when a new formatter
	* is set.
	*/
	void QValueAxis3D::setFormatter(QAxisValueFormatter3D *formatter)
	{
		Q_ASSERT(formatter);

		if (formatter != dptr()->m_formatter) {
			delete dptr()->m_formatter;
			dptr()->m_formatter = formatter;
			formatter->setParent(this);
			formatter->d_ptr->setAxis(this);
			QAbstractController3D *controller = qobject_cast<QAbstractController3D *>(parent());
			if (controller)
				formatter->setLocale(controller->locale());
			emit formatterChanged(formatter);
			emit dptr()->formatterDirty();
		}
	}

	QAxisValueFormatter3D *QValueAxis3D::formatter() const
	{
		return dptrc()->m_formatter;
	}

	/*!
	* \property QValueAxis3D::reversed
	* \since QtDataVisualization 1.1
	*
	* \brief Whether the axis is rendered in reverse.
	*
	* If \c{true}, the axis will be rendered in reverse, i.e. the positions of minimum and maximum
	* values are swapped when the graph is rendered. This property doesn't affect the actual
	* minimum and maximum values of the axis.
	*/
	void QValueAxis3D::setReversed(bool enable)
	{
		if (dptr()->m_reversed != enable) {
			dptr()->m_reversed = enable;
			emit reversedChanged(enable);
		}
	}

	bool QValueAxis3D::reversed() const
	{
		return dptrc()->m_reversed;
	}

	/*!
	* \internal
	*/
	QValueAxis3DPrivate *QValueAxis3D::dptr()
	{
		return static_cast<QValueAxis3DPrivate *>(d_ptr.data());
	}

	/*!
	* \internal
	*/
	const QValueAxis3DPrivate *QValueAxis3D::dptrc() const
	{
		return static_cast<const QValueAxis3DPrivate *>(d_ptr.data());
	}

	QValueAxis3DPrivate::QValueAxis3DPrivate(QValueAxis3D *q)
		: QAbstractAxis3DPrivate(q, QAbstractAxis3D::AxisTypeValue),
		m_segmentCount(5),
		m_subSegmentCount(1),
		m_labelFormat(QUtils3D::defaultLabelFormat()),
		m_labelsDirty(true),
		m_formatter(0),
		m_reversed(false)
	{
	}

	QValueAxis3DPrivate::~QValueAxis3DPrivate()
	{
	}

	void QValueAxis3DPrivate::setRange(float min, float max, bool suppressWarnings)
	{
		bool dirty = (min != m_min || max != m_max);

		QAbstractAxis3DPrivate::setRange(min, max, suppressWarnings);

		if (dirty)
			emitLabelsChanged();
	}

	void QValueAxis3DPrivate::setMin(float min)
	{
		bool dirty = (min != m_min);

		QAbstractAxis3DPrivate::setMin(min);

		if (dirty)
			emitLabelsChanged();
	}

	void QValueAxis3DPrivate::setMax(float max)
	{
		bool dirty = (max != m_max);

		QAbstractAxis3DPrivate::setMax(max);

		if (dirty)
			emitLabelsChanged();
	}

	void QValueAxis3DPrivate::emitLabelsChanged()
	{
		m_labelsDirty = true;
		emit q_ptr->labelsChanged();
	}

	void QValueAxis3DPrivate::updateLabels()
	{
		if (!m_labelsDirty)
			return;

		m_labelsDirty = false;

		m_formatter->d_ptr->recalculate();

		m_labels = m_formatter->labelStrings();
	}

	bool QValueAxis3DPrivate::allowZero()
	{
		return m_formatter->allowZero();
	}

	bool QValueAxis3DPrivate::allowNegatives()
	{
		return m_formatter->allowNegatives();
	}

	bool QValueAxis3DPrivate::allowMinMaxSame()
	{
		return false;
	}

	QValueAxis3D *QValueAxis3DPrivate::qptr()
	{
		return static_cast<QValueAxis3D *>(q_ptr);
	}

}
