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

#include "QCategoryAxis3DPrivate.h"
#include "QBarsController3D.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QCategoryAxis3D
	* \inmodule QtDataVisualization
	* \brief The QCategoryAxis3D class manipulates an axis of a graph.
	* \since QtDataVisualization 1.0
	*
	* QCategoryAxis3D provides an axis that can be given labels. The axis is divided into equal-sized
	* categories based on the data window size defined by setting the axis range.
	*
	* Grid lines are drawn between categories, if visible. Labels are drawn to positions of categories
	* if provided.
	*/

	/*!
	* \qmltype CategoryAxis3D
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.0
	* \ingroup datavisualization_qml
	* \instantiates QCategoryAxis3D
	* \inherits AbstractAxis3D
	* \brief Manipulates an axis of a graph.
	*
	* This type provides an axis that can be given labels.
	*/

	/*!
	* \qmlproperty list CategoryAxis3D::labels
	*
	* The labels for the axis applied to categories. If there are fewer labels than categories, the
	* remaining ones do not have a label. If category labels are not defined explicitly, labels are
	* generated from the data row (or column) labels of the primary series of the graph.
	*/

	/*!
	* Constructs a category 3D axis with the parent \a parent.
	*/
	QCategoryAxis3D::QCategoryAxis3D(QObject *parent) :
		QAbstractAxis3D(new QCategoryAxis3DPrivate(this), parent)
	{
		connect(this, &QCategoryAxis3D::labelsChanged, this, &QAbstractAxis3D::labelsChanged);
	}

	/*!
	* Destroys the category 3D axis.
	*/
	QCategoryAxis3D::~QCategoryAxis3D()
	{
	}

	/*!
	* \property QCategoryAxis3D::labels
	*
	* \brief The labels for the axis applied to categories.
	*
	* If there are fewer labels than categories, the
	* remaining ones do not have a label. If category labels are not defined explicitly, labels are
	* generated from the data row (or column) labels of the primary series of the graph.
	*/
	QStringList QCategoryAxis3D::labels() const
	{
		return QAbstractAxis3D::labels();
	}

	void QCategoryAxis3D::setLabels(const QStringList &labels)
	{
		dptr()->m_labelsExplicitlySet = !labels.isEmpty();
		bool labelsFromData = false;

		// Get labels from data proxy if axis is attached to a bar controller and an active axis there
		if (labels.isEmpty()) {
			QBarsController3D *controller = qobject_cast<QBarsController3D *>(parent());
			if (controller) {
				if (controller->axisX() == this) {
					controller->handleDataRowLabelsChanged();
					labelsFromData = true;
				}
				else if (controller->axisZ() == this) {
					controller->handleDataColumnLabelsChanged();
					labelsFromData = true;
				}
			}
		}

		if (!labelsFromData && d_ptr->m_labels != labels) {
			d_ptr->m_labels = labels;
			emit labelsChanged();
		}
	}

	/*!
	* \internal
	*/
	QCategoryAxis3DPrivate *QCategoryAxis3D::dptr()
	{
		return static_cast<QCategoryAxis3DPrivate *>(d_ptr.data());
	}

	QCategoryAxis3DPrivate::QCategoryAxis3DPrivate(QCategoryAxis3D *q)
		: QAbstractAxis3DPrivate(q, QAbstractAxis3D::AxisTypeCategory),
		m_labelsExplicitlySet(false)
	{
	}

	QCategoryAxis3DPrivate::~QCategoryAxis3DPrivate()
	{
	}

	/*!
	* \internal
	* Controller uses this function to set labels from data proxy as category labels.
	* If the labels have been set explicitly by the user, data proxy labels are not used.
	*/
	void QCategoryAxis3DPrivate::setDataLabels(const QStringList &labels)
	{
		if (!m_labelsExplicitlySet && m_labels != labels) {
			m_labels = labels;
			emit qptr()->labelsChanged();
		}
	}

	bool QCategoryAxis3DPrivate::allowZero()
	{
		return true;
	}

	bool QCategoryAxis3DPrivate::allowNegatives()
	{
		return false;
	}

	bool QCategoryAxis3DPrivate::allowMinMaxSame()
	{
		return true;
	}

	QCategoryAxis3D *QCategoryAxis3DPrivate::qptr()
	{
		return static_cast<QCategoryAxis3D *>(q_ptr);
	}

}
