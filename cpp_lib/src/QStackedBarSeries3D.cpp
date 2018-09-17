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

#include "QStackedBarSeries3DPrivate.h"
#include "QStackedBarsController3D.h"
#include "QStackedBarDataProxy3D.h"
#include "QCategoryAxis3D.h"
#include "QValueAxis3D.h"

#include <QtCore/qmath.h>

namespace QtStackedBar3DVis
{

	/*!
	* \class QStackedBarSeries3D
	* \inmodule QtDataVisualization
	* \brief The QStackedBarSeries3D class represents a data series in a 3D bar graph.
	* \since QtDataVisualization 1.0
	*
	* This class manages the series specific visual elements, as well as the series
	* data (via a data proxy).
	*
	* If no data proxy is set explicitly for the series, the series creates a default
	* proxy. Setting another proxy will destroy the existing proxy and all data added to it.
	*
	* QStackedBarSeries3D supports the following format tags for QAbstractSeries3D::setItemLabelFormat():
	* \table
	*   \row
	*     \li @rowTitle      \li Title from row axis
	*   \row
	*     \li @colTitle      \li Title from column axis
	*   \row
	*     \li @valueTitle    \li Title from value axis
	*   \row
	*     \li @rowIdx        \li Visible row index. Localized using the graph locale.
	*   \row
	*     \li @colIdx        \li Visible column index. Localized using the graph locale.
	*   \row
	*     \li @rowLabel      \li Label from row axis
	*   \row
	*     \li @colLabel      \li Label from column axis
	*   \row
	*     \li @valueLabel    \li Item value formatted using the format of the value
	*                            axis attached to the graph. For more information,
	*                            see \l{QValueAxis3D::labelFormat}.
	*   \row
	*     \li @seriesName    \li Name of the series
	*   \row
	*     \li %<format spec> \li Item value in the specified format. Formatted
	*                            using the same rules as \l{QValueAxis3D::labelFormat}.
	* \endtable
	*
	* For example:
	* \snippet doc_src_qtdatavisualization.cpp 1
	*
	* \sa {Qt Data Visualization Data Handling}, QAbstractGraph3D::locale
	*/

	/*!
	* \qmltype Bar3DSeries
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.0
	* \ingroup datavisualization_qml
	* \instantiates QStackedBarSeries3D
	* \inherits Abstract3DSeries
	* \brief Represents a data series in a 3D bar graph.
	*
	* This type manages the series specific visual elements, as well as the series
	* data (via a data proxy).
	*
	* For a more complete description, see QStackedBarSeries3D.
	*
	* \sa {Qt Data Visualization Data Handling}
	*/

	/*!
	* \qmlproperty BarDataProxy Bar3DSeries::dataProxy
	*
	* The active data proxy. The series assumes ownership of any proxy set to
	* it and deletes any previously set proxy when a new one is added. The proxy cannot be null or
	* set to another series.
	*/

	/*!
	* \qmlproperty point Bar3DSeries::selectedBar
	*
	* The bar in the series that is selected.
	*
	* The position of the selected bar is specified as a row and column in the
	* data array of the series.
	*
	* Only one bar can be selected at a time.
	*
	* To clear selection from this series, set invalidSelectionPosition as the position.
	*
	* If this series is added to a graph, the graph can adjust the selection according to user
	* interaction or if it becomes invalid. Selecting a bar on another added series will also
	* clear the selection.
	*
	* Removing rows from or inserting rows to the series before the row of the selected bar
	* will adjust the selection so that the same bar will stay selected.
	*
	* \sa {AbstractGraph3D::clearSelection()}{AbstractGraph3D.clearSelection()}
	*/

	/*!
	* \qmlproperty point Bar3DSeries::invalidSelectionPosition
	* A constant property providing an invalid position for selection. This
	* position is set to the selectedBar property to clear the selection from this
	* series.
	*
	* \sa {AbstractGraph3D::clearSelection()}{AbstractGraph3D.clearSelection()}
	*/

	/*!
	* \qmlproperty real Bar3DSeries::meshAngle
	*
	* A convenience property for defining the series rotation angle in degrees.
	*
	* \note When reading this property, it is calculated from the
	* \l{Abstract3DSeries::meshRotation}{Abstract3DSeries.meshRotation} value
	* using floating point precision and always returns a value from zero to 360 degrees.
	*
	* \sa {Abstract3DSeries::meshRotation}{Abstract3DSeries.meshRotation}
	*/

	/*!
	* Constructsa bar 3D series with the parent \a parent.
	*/
	QStackedBarSeries3D::QStackedBarSeries3D(QObject *parent) :
		QAbstractSeries3D(new QStackedBarSeries3DPrivate(this), parent)
	{
		// Default proxy
		dptr()->setDataProxy(new QStackedBarDataProxy3D);
		dptr()->connectSignals();
	}

	/*!
	* Constructs a bar 3D series with the data proxy \a dataProxy and the parent
	* \a parent.
	*/
	QStackedBarSeries3D::QStackedBarSeries3D(QStackedBarDataProxy3D *dataProxy, QObject *parent) :
		QAbstractSeries3D(new QStackedBarSeries3DPrivate(this), parent)
	{
		dptr()->setDataProxy(dataProxy);
		dptr()->connectSignals();
	}

	/*!
	* Deletes a bar 3D series.
	*/
	QStackedBarSeries3D::~QStackedBarSeries3D()
	{
	}

	/*!
	* \property QStackedBarSeries3D::dataProxy
	*
	* \brief The active data proxy.
	*
	* The series assumes ownership of any proxy set to it and deletes any
	* previously set proxy when a new one is added. The proxy cannot be null or
	* set to another series.
	*/
	void QStackedBarSeries3D::setDataProxy(QStackedBarDataProxy3D *proxy)
	{
		d_ptr->setDataProxy(proxy);
	}

	QStackedBarDataProxy3D *QStackedBarSeries3D::dataProxy() const
	{
		return static_cast<QStackedBarDataProxy3D *>(d_ptr->dataProxy());
	}

	/*!
	* \property QStackedBarSeries3D::selectedBar
	*
	* \brief The bar in the series that is selected.
	*
	*/

	/*!
	* Selects the bar at the \a position position, specified as a row and column in
	* the data array of the series.
	*
	* Only one bar can be selected at a time.
	*
	* To clear selection from this series, invalidSelectionPosition() is set as
	* \a position.
	*
	* If this series is added to a graph, the graph can adjust the selection according to user
	* interaction or if it becomes invalid. Selecting a bar on another added series will also
	* clear the selection.
	*
	* Removing rows from or inserting rows to the series before the row of the selected bar
	* will adjust the selection so that the same bar will stay selected.
	*
	* \sa QAbstractGraph3D::clearSelection()
	*/
	void QStackedBarSeries3D::setSelectedBar(const QPoint &position)
	{
		// Don't do this in private to avoid loops, as that is used for callback from controller.
		if (d_ptr->m_controller)
			static_cast<QStackedBarsController3D *>(d_ptr->m_controller)->setSelectedBar(position, this, true);
		else
			dptr()->setSelectedBar(position);
	}

	QPoint QStackedBarSeries3D::selectedBar() const
	{
		return dptrc()->m_selectedBar;
	}

	/*!
	* Returns an invalid position for selection. This position is set to the
	* selectedBar property to clear the selection from this series.
	*
	* \sa QAbstractGraph3D::clearSelection()
	*/
	QPoint QStackedBarSeries3D::invalidSelectionPosition()
	{
		return QStackedBarsController3D::invalidSelectionPosition();
	}

	static inline float quaternionAngle(const QQuaternion &rotation)
	{
		return qRadiansToDegrees(qAcos(rotation.scalar())) * 2.f;
	}

	/*!
	* \property QStackedBarSeries3D::meshAngle
	*
	* \brief The series rotation angle in degrees.
	*
	* Setting this property is equivalent to the following call:
	* \code setMeshRotation(QQuaternion::fromAxisAndAngle(0.0f, 1.0f, 0.0f, angle)) \endcode
	*
	* \note When reading this property, it is calculated from the
	* QAbstractSeries3D::meshRotation value
	* using floating point precision and always returns a value from zero to 360 degrees.
	*
	* \sa QAbstractSeries3D::meshRotation
	*/
	void QStackedBarSeries3D::setMeshAngle(float angle)
	{
		setMeshRotation(QQuaternion::fromAxisAndAngle(upVector, angle));
	}

	float QStackedBarSeries3D::meshAngle() const
	{
		QQuaternion rotation = meshRotation();

		if (rotation.isIdentity() || rotation.x() != 0.0f || rotation.z() != 0.0f)
			return 0.0f;
		else
			return quaternionAngle(rotation);
	}

	const QList<QColor>& QStackedBarSeries3D::sectionColors()
	{
		return dptr()->sectionColors();
	}

	void QStackedBarSeries3D::setSectionColors(const QList<QColor>& sectionColors)
	{
		dptr()->setSectionColors(sectionColors);
	}

	/*!
	* \internal
	*/
	QStackedBarSeries3DPrivate *QStackedBarSeries3D::dptr()
	{
		return static_cast<QStackedBarSeries3DPrivate *>(d_ptr.data());
	}

	/*!
	* \internal
	*/
	const QStackedBarSeries3DPrivate *QStackedBarSeries3D::dptrc() const
	{
		return static_cast<const QStackedBarSeries3DPrivate *>(d_ptr.data());
	}

	// QStackedBarSeries3DPrivate

	QStackedBarSeries3DPrivate::QStackedBarSeries3DPrivate(QStackedBarSeries3D *q)
		: QAbstractSeries3DPrivate(q, QAbstractSeries3D::SeriesTypeBar),
		m_selectedBar(QStackedBarsController3D::invalidSelectionPosition())
	{
		m_itemLabelFormat = QStringLiteral("@valueLabel");
		m_mesh = QAbstractSeries3D::MeshBevelBar;
	}

	QStackedBarSeries3DPrivate::~QStackedBarSeries3DPrivate()
	{
	}

	QStackedBarSeries3D *QStackedBarSeries3DPrivate::qptr()
	{
		return static_cast<QStackedBarSeries3D *>(q_ptr);
	}

	void QStackedBarSeries3DPrivate::setDataProxy(QAbstractDataProxy3D *proxy)
	{
		Q_ASSERT(proxy->type() == QAbstractDataProxy3D::DataTypeBar);

		QAbstractSeries3DPrivate::setDataProxy(proxy);

		emit qptr()->dataProxyChanged(static_cast<QStackedBarDataProxy3D *>(proxy));
	}

	void QStackedBarSeries3DPrivate::connectControllerAndProxy(QAbstractController3D *newController)
	{
		QStackedBarDataProxy3D *barDataProxy = static_cast<QStackedBarDataProxy3D *>(m_dataProxy);

		if (m_controller && barDataProxy) {
			// Disconnect old controller/old proxy
			QObject::disconnect(barDataProxy, 0, m_controller, 0);
			QObject::disconnect(q_ptr, 0, m_controller, 0);
		}

		if (newController && barDataProxy) {
			QStackedBarsController3D *controller = static_cast<QStackedBarsController3D *>(newController);
			QObject::connect(barDataProxy, &QStackedBarDataProxy3D::arrayReset, controller,
				&QStackedBarsController3D::handleArrayReset);
			QObject::connect(barDataProxy, &QStackedBarDataProxy3D::rowsAdded, controller,
				&QStackedBarsController3D::handleRowsAdded);
			QObject::connect(barDataProxy, &QStackedBarDataProxy3D::rowsChanged, controller,
				&QStackedBarsController3D::handleRowsChanged);
			QObject::connect(barDataProxy, &QStackedBarDataProxy3D::rowsRemoved, controller,
				&QStackedBarsController3D::handleRowsRemoved);
			QObject::connect(barDataProxy, &QStackedBarDataProxy3D::rowsInserted, controller,
				&QStackedBarsController3D::handleRowsInserted);
			QObject::connect(barDataProxy, &QStackedBarDataProxy3D::itemChanged, controller,
				&QStackedBarsController3D::handleItemChanged);
			QObject::connect(barDataProxy, &QStackedBarDataProxy3D::rowLabelsChanged, controller,
				&QStackedBarsController3D::handleDataRowLabelsChanged);
			QObject::connect(barDataProxy, &QStackedBarDataProxy3D::columnLabelsChanged, controller,
				&QStackedBarsController3D::handleDataColumnLabelsChanged);
			QObject::connect(qptr(), &QStackedBarSeries3D::dataProxyChanged, controller,
				&QStackedBarsController3D::handleArrayReset);
		}
	}

	void QStackedBarSeries3DPrivate::createItemLabel()
	{
		static const QString rowIndexTag(QStringLiteral("@rowIdx"));
		static const QString rowLabelTag(QStringLiteral("@rowLabel"));
		static const QString rowTitleTag(QStringLiteral("@rowTitle"));
		static const QString colIndexTag(QStringLiteral("@colIdx"));
		static const QString colLabelTag(QStringLiteral("@colLabel"));
		static const QString colTitleTag(QStringLiteral("@colTitle"));

		static const QString sectionTitleTag(QStringLiteral("@sectionTitle"));
		static const QString sectionLabelTag(QStringLiteral("@sectionLabel"));

		static const QString valueTitleTag(QStringLiteral("@valueTitle"));
		static const QString valueLabelTag(QStringLiteral("@valueLabel"));
		static const QString seriesNameTag(QStringLiteral("@seriesName"));

		if (m_selectedBar == QStackedBarSeries3D::invalidSelectionPosition()) {
			m_itemLabel = QString();
			return;
		}

		QLocale locale(QLocale::c());
		if (m_controller)
			locale = m_controller->locale();

		QCategoryAxis3D *categoryAxisZ = static_cast<QCategoryAxis3D *>(m_controller->axisZ());
		QCategoryAxis3D *categoryAxisX = static_cast<QCategoryAxis3D *>(m_controller->axisX());
		QValueAxis3D *valueAxis = static_cast<QValueAxis3D *>(m_controller->axisY());

		// Custom format expects printf format specifier. There is no tag for it.
		qreal selectedBarValue = 0.0;
		QStringList barValues;
		if (qptr()->dataProxy()->itemAt(m_selectedBar)->values().size() > 0)
		{
			selectedBarValue = qreal(qptr()->dataProxy()->itemAt(m_selectedBar)->values().at(0));
			m_itemLabel = valueAxis->formatter()->stringForValue(selectedBarValue, m_itemLabelFormat);
			for (unsigned int k = 0; k < qptr()->dataProxy()->itemAt(m_selectedBar)->values().size(); k++)
			{
				selectedBarValue = qreal(qptr()->dataProxy()->itemAt(m_selectedBar)->values().at(k));
				barValues << QString::number(selectedBarValue);
			}
		}

		m_itemLabel.append(barValues.join(QString(" -- ")));

		int selBarPosRow = m_selectedBar.x();
		int selBarPosCol = m_selectedBar.y();
		m_itemLabel.replace(rowIndexTag, locale.toString(selBarPosRow));
		if (categoryAxisZ->labels().size() > selBarPosRow)
			m_itemLabel.replace(rowLabelTag, categoryAxisZ->labels().at(selBarPosRow));
		else
			m_itemLabel.replace(rowLabelTag, QString());
		m_itemLabel.replace(rowTitleTag, categoryAxisZ->title());
		m_itemLabel.replace(colIndexTag, locale.toString(selBarPosCol));
		if (categoryAxisX->labels().size() > selBarPosCol)
			m_itemLabel.replace(colLabelTag, categoryAxisX->labels().at(selBarPosCol));
		else
			m_itemLabel.replace(colLabelTag, QString());
		m_itemLabel.replace(colTitleTag, categoryAxisX->title());
		m_itemLabel.replace(valueTitleTag, valueAxis->title());

		if (m_itemLabel.contains(valueLabelTag)) {
			QString valueLabelText = valueAxis->formatter()->stringForValue(selectedBarValue,
				valueAxis->labelFormat());
			m_itemLabel.replace(valueLabelTag, valueLabelText);
		}

		m_itemLabel.replace(seriesNameTag, m_name);
	}

	void QStackedBarSeries3DPrivate::handleMeshRotationChanged(const QQuaternion &rotation)
	{
		emit qptr()->meshAngleChanged(quaternionAngle(rotation));
	}

	void QStackedBarSeries3DPrivate::setSelectedBar(const QPoint &position)
	{
		if (position != m_selectedBar) {
			markItemLabelDirty();
			m_selectedBar = position;
			emit qptr()->selectedBarChanged(m_selectedBar);
		}
	}

	void QStackedBarSeries3DPrivate::connectSignals()
	{
		QObject::connect(q_ptr, &QAbstractSeries3D::meshRotationChanged, this,
			&QStackedBarSeries3DPrivate::handleMeshRotationChanged);
	}

	const QList<QColor>& QStackedBarSeries3DPrivate::sectionColors()
	{
		return m_sectionColors;
	}

	void QStackedBarSeries3DPrivate::setSectionColors(const QList<QColor>& sectionColors)
	{
		m_sectionColors = sectionColors;
		m_changeTracker.colorRangeChanged = true;
		if (m_controller)
			m_controller->markSeriesVisualsDirty();
	}

}
