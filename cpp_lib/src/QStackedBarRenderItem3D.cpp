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

#include "QStackedBarRenderItem3D.h"

namespace QtStackedBar3DVis
{

	QStackedBarRenderItem3D::QStackedBarRenderItem3D()
		: QAbstractRenderItem3D(),
        m_values(QList<qreal>()),
        m_heights(QList<qreal>())
	{
	}

	QStackedBarRenderItem3D::QStackedBarRenderItem3D(const QStackedBarRenderItem3D &other)
		: QAbstractRenderItem3D(other)
	{
		m_values = other.m_values;
		m_position = other.m_position;
		m_heights = other.m_heights;
	}

	QStackedBarRenderItem3D::~QStackedBarRenderItem3D()
	{
	}

	QStackedBarRenderSliceItem3D::QStackedBarRenderSliceItem3D()
		: QStackedBarRenderItem3D(),
		m_sliceLabelItem(0)
	{
	}

	QStackedBarRenderSliceItem3D::QStackedBarRenderSliceItem3D(const QStackedBarRenderSliceItem3D &other)
		: QStackedBarRenderItem3D(other)
	{
		m_sliceLabel = other.m_sliceLabel;
		m_sliceLabelItem = 0;
	}

	QStackedBarRenderSliceItem3D::~QStackedBarRenderSliceItem3D()
	{
		delete m_sliceLabelItem;
	}

	void QStackedBarRenderSliceItem3D::setItem(const QStackedBarRenderItem3D &renderItem)
	{
		m_translation = renderItem.translation();
		m_rotation = renderItem.rotation();
		m_values = renderItem.values();
		m_position = renderItem.position();
		m_heights = renderItem.heights();
		m_sliceLabel = QString();
		delete m_sliceLabelItem;
		m_sliceLabelItem = 0;
	}

	QLabelItem3D &QStackedBarRenderSliceItem3D::sliceLabelItem()
	{
		if (!m_sliceLabelItem)
			m_sliceLabelItem = new QLabelItem3D;
		return *m_sliceLabelItem;
	}

	void QStackedBarRenderSliceItem3D::setSliceLabel(const QString &label)
	{
		if (m_sliceLabelItem)
			m_sliceLabelItem->clear();
		m_sliceLabel = label;
	}

	const QString &QStackedBarRenderSliceItem3D::sliceLabel() const
	{
		return m_sliceLabel;
	}

}
