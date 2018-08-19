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

#ifndef Q_VALUE_AXIS_3D_H
#define Q_VALUE_AXIS_3D_H

#include "QAbstractAxis3D.h"
#include "QAxisValueFormatter3D.h"

namespace QtStackedBar3DVis
{

	class QValueAxis3DPrivate;

	class QT_STACKEDBAR3D_EXPORT QValueAxis3D : public QAbstractAxis3D
	{
		Q_OBJECT
			Q_PROPERTY(int segmentCount READ segmentCount WRITE setSegmentCount NOTIFY segmentCountChanged)
			Q_PROPERTY(int subSegmentCount READ subSegmentCount WRITE setSubSegmentCount NOTIFY subSegmentCountChanged)
			Q_PROPERTY(QString labelFormat READ labelFormat WRITE setLabelFormat NOTIFY labelFormatChanged)
			Q_PROPERTY(QAxisValueFormatter3D* formatter READ formatter WRITE setFormatter NOTIFY formatterChanged REVISION 1)
			Q_PROPERTY(bool reversed READ reversed WRITE setReversed NOTIFY reversedChanged REVISION 1)

	public:
		explicit QValueAxis3D(QObject *parent = Q_NULLPTR);
		virtual ~QValueAxis3D();

		void setSegmentCount(int count);
		int segmentCount() const;

		void setSubSegmentCount(int count);
		int subSegmentCount() const;

		void setLabelFormat(const QString &format);
		QString labelFormat() const;

		void setFormatter(QAxisValueFormatter3D *formatter);
		QAxisValueFormatter3D *formatter() const;

		void setReversed(bool enable);
		bool reversed() const;

	Q_SIGNALS:
		void segmentCountChanged(int count);
		void subSegmentCountChanged(int count);
		void labelFormatChanged(const QString &format);
		Q_REVISION(1) void formatterChanged(QAxisValueFormatter3D *formatter);
		Q_REVISION(1) void reversedChanged(bool enable);

	protected:
		QValueAxis3DPrivate *dptr();
		const QValueAxis3DPrivate *dptrc() const;

	private:
		Q_DISABLE_COPY(QValueAxis3D)
		friend class QAbstractController3D;
		friend class QBarsController3D;
		friend class QStackedBarsController3D;
		friend class QScatterController3D;
		friend class QSurfaceController3D;
		friend class QAxisValueFormatter3DPrivate;
	};

}

#endif
