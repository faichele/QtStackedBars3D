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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the QtDataVisualization API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef Q_STACKED_BAR_RENDER_ITEM_H
#define Q_STACKED_BAR_RENDER_ITEM_H

#include "QAbstractRenderItem3D.h"

namespace QtStackedBar3DVis
{

	class QT_STACKEDBAR3D_EXPORT QStackedBarRenderItem3D : public QAbstractRenderItem3D
	{
	public:
		QStackedBarRenderItem3D();
		QStackedBarRenderItem3D(const QStackedBarRenderItem3D &other);
		virtual ~QStackedBarRenderItem3D();

		// Position relative to data window (for bar label generation)
		inline void setPosition(const QPoint &pos) { m_position = pos; }
		inline const QPoint &position() const { return m_position; }

		// Actual cached data value of the bar (needed to trigger label reformats)
        inline void setValues(const QList<qreal>& values) { m_values = values; }
        inline QList<qreal> values() const { return m_values; }

		// Normalized bar height
        inline void setHeights(const QList<qreal>& heights) { m_heights = heights; }
        inline QList<qreal> heights() const { return m_heights; }

	protected:
        QList<qreal> m_values;
		QPoint m_position; // x = row, y = column
        QList<qreal> m_heights;

		friend class QBarDataItem3D;
	};

	class QStackedBarRenderSliceItem3D : public QStackedBarRenderItem3D
	{
	public:
		QStackedBarRenderSliceItem3D();
		QStackedBarRenderSliceItem3D(const QStackedBarRenderSliceItem3D &other);
		virtual ~QStackedBarRenderSliceItem3D();

		void setItem(const QStackedBarRenderItem3D &renderItem);

		// Label item for formatted label
		QLabelItem3D &sliceLabelItem();

		// Formatted label for item.
		void setSliceLabel(const QString &label);
		const QString &sliceLabel() const; // Formats label if not previously formatted

	protected:
		QString m_sliceLabel;
		QLabelItem3D *m_sliceLabelItem;
		bool m_isNull;
	};

	typedef QVector<QStackedBarRenderItem3D> QStackedBarRenderItemRow3D;
	typedef QVector<QStackedBarRenderItemRow3D> QStackedBarRenderItemArray3D;

}

#endif
