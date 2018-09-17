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

#include "QAbstractAxis3DPrivate.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QAbstractAxis3D
	* \inmodule QtDataVisualization
	* \brief The QAbstractAxis3D class is a base class for the axes of a graph.
	* \since QtDataVisualization 1.0
	*
	* This class specifies the enumerations, properties, and functions shared by
	* graph axes. It should not be used directly, but one of its subclasses should
	* be used instead.
	*
	* \sa QCategoryAxis3D, QValueAxis3D
	*/

	/*!
	* \qmltype AbstractAxis3D
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.0
	* \ingroup datavisualization_qml
	* \instantiates QAbstractAxis3D
	* \brief A base type for the axes of a graph.
	*
	* This type is uncreatable, but contains properties that are exposed via subtypes.
	*
	* For AbstractAxis3D enums, see \l QAbstractAxis3D::AxisOrientation and
	* \l{QAbstractAxis3D::AxisType}.
	*/

	/*!
	* \qmlproperty string AbstractAxis3D::title
	* The title for the axis.
	*
	* \sa titleVisible, titleFixed
	*/

	/*!
	* \qmlproperty list AbstractAxis3D::labels
	* The labels for the axis.
	* \note Setting this property for ValueAxis3D does nothing, as it generates labels automatically.
	*/

	/*!
	* \qmlproperty AbstractAxis3D.AxisOrientation AbstractAxis3D::orientation
	* The orientation of the axis.
	*/

	/*!
	* \qmlproperty AbstractAxis3D.AxisType AbstractAxis3D::type
	* The type of the axis.
	*/

	/*!
	* \qmlproperty real AbstractAxis3D::min
	*
	* The minimum value on the axis.
	* When setting this property, the maximum value is adjusted if necessary, to
	* ensure that the range remains valid.
	*/

	/*!
	* \qmlproperty real AbstractAxis3D::max
	*
	* The maximum value on the axis.
	* When setting this property, the minimum value is adjusted if necessary, to
	* ensure that the range remains valid.
	*/

	/*!
	* \qmlproperty bool AbstractAxis3D::autoAdjustRange
	*
	* Defines whether the axis will automatically adjust the range so that all data fits in it.
	*/

	/*!
	* \qmlproperty real AbstractAxis3D::labelAutoRotation
	*
	* The maximum angle the labels can autorotate when the camera angle changes.
	* The angle can be between 0 and 90, inclusive. The default value is 0.
	* If the value is 0, axis labels do not automatically rotate.
	* If the value is greater than zero, labels attempt to orient themselves toward the camera, up to
	* the specified angle.
	*/

	/*!
	* \qmlproperty bool AbstractAxis3D::titleVisible
	*
	* Defines whether the axis title is visible in the primary graph view.
	*
	* The default value is \c{false}.
	*
	* \sa title, titleFixed
	*/

	/*!
	* \qmlproperty bool AbstractAxis3D::titleFixed
	*
	* The rotation of axis titles.
	*
	* If \c{true}, axis titles in the primary graph view will be rotated towards the camera similarly
	* to the axis labels.
	* If \c{false}, axis titles are only rotated around their axis but are not otherwise oriented
	* towards the camera.
	* This property does not have any effect if the labelAutoRotation property
	* value is zero.
	* Default value is \c{true}.
	*
	* \sa labelAutoRotation, title, titleVisible
	*/

	/*!
	* \enum QAbstractAxis3D::AxisOrientation
	*
	* The orientation of the axis object.
	*
	* \value AxisOrientationNone
	* \value AxisOrientationX
	* \value AxisOrientationY
	* \value AxisOrientationZ
	*/

	/*!
	* \enum QAbstractAxis3D::AxisType
	*
	* The type of the axis object.
	*
	* \value AxisTypeNone
	* \value AxisTypeCategory
	* \value AxisTypeValue
	*/

	/*!
	* \internal
	*/
	QAbstractAxis3D::QAbstractAxis3D(QAbstractAxis3DPrivate *d, QObject *parent) :
		QObject(parent),
		d_ptr(d)
	{
	}

	/*!
	* Destroys QAbstractAxis3D.
	*/
	QAbstractAxis3D::~QAbstractAxis3D()
	{
	}

	/*!
	* \property QAbstractAxis3D::orientation
	*
	* \brief The orientation of the axis.
	*
	* The value is one of AxisOrientation values.
	*/
	QAbstractAxis3D::AxisOrientation QAbstractAxis3D::orientation() const
	{
		return d_ptr->m_orientation;
	}

	/*!
	* \property QAbstractAxis3D::type
	*
	* \brief The type of the axis.
	*
	* The value is one of AxisType values.
	*/
	QAbstractAxis3D::AxisType QAbstractAxis3D::type() const
	{
		return d_ptr->m_type;
	}

	/*!
	* \property QAbstractAxis3D::title
	*
	* \brief The title for the axis.
	*
	* \sa titleVisible, titleFixed
	*/
	void QAbstractAxis3D::setTitle(const QString &title)
	{
		if (d_ptr->m_title != title) {
			d_ptr->m_title = title;
			emit titleChanged(title);
		}
	}

	QString QAbstractAxis3D::title() const
	{
		return d_ptr->m_title;
	}

	/*!
	* \property QAbstractAxis3D::labels
	*
	* \brief The labels for the axis.
	* \note Setting this property for QValueAxis3D does nothing, as it generates labels automatically.
	*/
	void QAbstractAxis3D::setLabels(const QStringList &labels)
	{
		Q_UNUSED(labels)
	}

	QStringList QAbstractAxis3D::labels() const
	{
		d_ptr->updateLabels();
		return d_ptr->m_labels;
	}

	/*!
	* Sets the value range of the axis from \a min to \a max.
	* When setting the range, the maximum value is adjusted if necessary, to ensure
	* that the range remains valid.
	* \note For QCategoryAxis3D, specifies the index range of rows or columns to
	* show.
	*/
	void QAbstractAxis3D::setRange(float min, float max)
	{
		d_ptr->setRange(min, max);
		setAutoAdjustRange(false);
	}

	/*!
	* \property QAbstractAxis3D::labelAutoRotation
	*
	* \brief The maximum angle the labels can autorotate when the camera angle changes.
	*
	* The angle can be between 0 and 90, inclusive. The default value is 0.
	* If the value is 0, axis labels do not automatically rotate.
	* If the value is greater than zero, labels attempt to orient themselves toward the camera, up to
	* the specified angle.
	*/
	void QAbstractAxis3D::setLabelAutoRotation(float angle)
	{
		if (angle < 0.0f)
			angle = 0.0f;
		if (angle > 90.0f)
			angle = 90.0f;
		if (d_ptr->m_labelAutoRotation != angle) {
			d_ptr->m_labelAutoRotation = angle;
			emit labelAutoRotationChanged(angle);
		}
	}

	float QAbstractAxis3D::labelAutoRotation() const
	{
		return d_ptr->m_labelAutoRotation;
	}

	/*!
	* \property QAbstractAxis3D::titleVisible
	*
	* \brief Whether the axis title is visible in the primary graph view.
	*
	* The default value is \c{false}.
	*
	* \sa title, titleFixed
	*/
	void QAbstractAxis3D::setTitleVisible(bool visible)
	{
		if (d_ptr->m_titleVisible != visible) {
			d_ptr->m_titleVisible = visible;
			emit titleVisibilityChanged(visible);
		}
	}

	bool QAbstractAxis3D::isTitleVisible() const
	{
		return d_ptr->m_titleVisible;
	}

	/*!
	* \property QAbstractAxis3D::titleFixed
	*
	* \brief The rotation of the axis titles.
	*
	* If \c{true}, axis titles in the primary graph view will be rotated towards the camera similarly
	* to the axis labels.
	* If \c{false}, axis titles are only rotated around their axis but are not otherwise oriented
	* towards the camera.
	* This property does not have any effect if the labelAutoRotation property
	* value is zero.
	* Default value is \c{true}.
	*
	* \sa labelAutoRotation, title, titleVisible
	*/
	void QAbstractAxis3D::setTitleFixed(bool fixed)
	{
		if (d_ptr->m_titleFixed != fixed) {
			d_ptr->m_titleFixed = fixed;
			emit titleFixedChanged(fixed);
		}
	}

	bool QAbstractAxis3D::isTitleFixed() const
	{
		return d_ptr->m_titleFixed;
	}

	/*!
	* \property QAbstractAxis3D::min
	*
	* \brief The minimum value on the axis.
	*
	* When setting this property, the maximum value is adjusted if necessary, to
	* ensure that the range remains valid.
	* \note For QCategoryAxis3D, specifies the index of the first row or column to
	* show.
	*/
	void QAbstractAxis3D::setMin(float min)
	{
		d_ptr->setMin(min);
		setAutoAdjustRange(false);
	}

	/*!
	* \property QAbstractAxis3D::max
	*
	* \brief The maximum value on the axis.
	*
	* When setting this property, the minimum value is adjusted if necessary, to
	* ensure that the range remains valid.
	* \note For QCategoryAxis3D, specifies the index of the last row or column to
	* show.
	*/
	void QAbstractAxis3D::setMax(float max)
	{
		d_ptr->setMax(max);
		setAutoAdjustRange(false);
	}

	float QAbstractAxis3D::min() const
	{
		return d_ptr->m_min;
	}

	float QAbstractAxis3D::max() const
	{
		return d_ptr->m_max;
	}

	/*!
	* \property QAbstractAxis3D::autoAdjustRange
	*
	* \brief Whether the axis will automatically adjust the range so that all data fits in it.
	*
	* \sa setRange(), setMin(), setMax()
	*/
	void QAbstractAxis3D::setAutoAdjustRange(bool autoAdjust)
	{
		if (d_ptr->m_autoAdjust != autoAdjust) {
			d_ptr->m_autoAdjust = autoAdjust;
			emit autoAdjustRangeChanged(autoAdjust);
		}
	}

	bool QAbstractAxis3D::isAutoAdjustRange() const
	{
		return d_ptr->m_autoAdjust;
	}

	/*!
	* \fn QAbstractAxis3D::rangeChanged(float min, float max)
	*
	* Emits the minimum and maximum values of the range, \a min and \a max, when
	* the range changes.
	*/

	// QAbstractAxis3DPrivate
	QAbstractAxis3DPrivate::QAbstractAxis3DPrivate(QAbstractAxis3D *q, QAbstractAxis3D::AxisType type)
		: QObject(0),
		q_ptr(q),
		m_orientation(QAbstractAxis3D::AxisOrientationNone),
		m_type(type),
		m_isDefaultAxis(false),
		m_min(0.0f),
		m_max(10.0f),
		m_autoAdjust(true),
		m_labelAutoRotation(0.0f),
		m_titleVisible(false),
		m_titleFixed(true)
	{
	}

	QAbstractAxis3DPrivate::~QAbstractAxis3DPrivate()
	{
	}

	void QAbstractAxis3DPrivate::setOrientation(QAbstractAxis3D::AxisOrientation orientation)
	{
		if (m_orientation == QAbstractAxis3D::AxisOrientationNone) {
			m_orientation = orientation;
			emit q_ptr->orientationChanged(orientation);
		}
		else {
			Q_ASSERT("Attempted to reset axis orientation.");
		}
	}

	void QAbstractAxis3DPrivate::updateLabels()
	{
		// Default implementation does nothing
	}

	void QAbstractAxis3DPrivate::setRange(float min, float max, bool suppressWarnings)
	{
		bool adjusted = false;
		if (!allowNegatives()) {
			if (allowZero()) {
				if (min < 0.0f) {
					min = 0.0f;
					adjusted = true;
				}
				if (max < 0.0f) {
					max = 0.0f;
					adjusted = true;
				}
			}
			else {
				if (min <= 0.0f) {
					min = 1.0f;
					adjusted = true;
				}
				if (max <= 0.0f) {
					max = 1.0f;
					adjusted = true;
				}
			}
		}
		// If min >= max, we adjust ranges so that
		// m_max becomes (min + 1.0f)
		// as axes need some kind of valid range.
		bool minDirty = false;
		bool maxDirty = false;
		if (m_min != min) {
			m_min = min;
			minDirty = true;
		}
		if (m_max != max || min > max || (!allowMinMaxSame() && min == max)) {
			if (min > max || (!allowMinMaxSame() && min == max)) {
				m_max = min + 1.0f;
				adjusted = true;
			}
			else {
				m_max = max;
			}
			maxDirty = true;
		}

		if (minDirty || maxDirty) {
			if (adjusted && !suppressWarnings) {
				qWarning() << "Warning: Tried to set invalid range for axis."
					" Range automatically adjusted to a valid one:"
					<< min << "-" << max << "-->" << m_min << "-" << m_max;
			}
			emit q_ptr->rangeChanged(m_min, m_max);
		}

		if (minDirty)
			emit q_ptr->minChanged(m_min);
		if (maxDirty)
			emit q_ptr->maxChanged(m_max);
	}

	void QAbstractAxis3DPrivate::setMin(float min)
	{
		if (!allowNegatives()) {
			if (allowZero()) {
				if (min < 0.0f) {
					min = 0.0f;
					qWarning() << "Warning: Tried to set negative minimum for an axis that only"
						"supports positive values and zero:" << min;
				}
			}
			else {
				if (min <= 0.0f) {
					min = 1.0f;
					qWarning() << "Warning: Tried to set negative or zero minimum for an axis that only"
						"supports positive values:" << min;
				}
			}
		}

		if (m_min != min) {
			bool maxChanged = false;
			if (min > m_max || (!allowMinMaxSame() && min == m_max)) {
				float oldMax = m_max;
				m_max = min + 1.0f;
				qWarning() << "Warning: Tried to set minimum to equal or larger than maximum for"
					" value axis. Maximum automatically adjusted to a valid one:"
					<< oldMax << "-->" << m_max;
				maxChanged = true;
			}
			m_min = min;

			emit q_ptr->rangeChanged(m_min, m_max);
			emit q_ptr->minChanged(m_min);
			if (maxChanged)
				emit q_ptr->maxChanged(m_max);
		}
	}

	void QAbstractAxis3DPrivate::setMax(float max)
	{
		if (!allowNegatives()) {
			if (allowZero()) {
				if (max < 0.0f) {
					max = 0.0f;
					qWarning() << "Warning: Tried to set negative maximum for an axis that only"
						"supports positive values and zero:" << max;
				}
			}
			else {
				if (max <= 0.0f) {
					max = 1.0f;
					qWarning() << "Warning: Tried to set negative or zero maximum for an axis that only"
						"supports positive values:" << max;
				}
			}
		}

		if (m_max != max) {
			bool minChanged = false;
			if (m_min > max || (!allowMinMaxSame() && m_min == max)) {
				float oldMin = m_min;
				m_min = max - 1.0f;
				if (!allowNegatives() && m_min < 0.0f) {
					if (allowZero())
						m_min = 0.0f;
					else
						m_min = max / 2.0f; // Need some positive value smaller than max

					if (!allowMinMaxSame() && max == 0.0f) {
						m_min = oldMin;
						qWarning() << "Unable to set maximum value to zero.";
						return;
					}
				}
				qWarning() << "Warning: Tried to set maximum to equal or smaller than minimum for"
					" value axis. Minimum automatically adjusted to a valid one:"
					<< oldMin << "-->" << m_min;
				minChanged = true;
			}
			m_max = max;
			emit q_ptr->rangeChanged(m_min, m_max);
			emit q_ptr->maxChanged(m_max);
			if (minChanged)
				emit q_ptr->minChanged(m_min);
		}
	}
}