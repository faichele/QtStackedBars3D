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

#ifndef Q3DINPUTHANDLER_H
#define Q3DINPUTHANDLER_H

#include "QAbstractInputHandler3D.h"

namespace QtStackedBar3DVis
{

	class QInputHandler3DPrivate;

	class QT_STACKEDBAR3D_EXPORT QInputHandler3D : public QAbstractInputHandler3D
	{
		Q_OBJECT
			Q_PROPERTY(bool rotationEnabled READ isRotationEnabled WRITE setRotationEnabled NOTIFY rotationEnabledChanged)
			Q_PROPERTY(bool zoomEnabled READ isZoomEnabled WRITE setZoomEnabled NOTIFY zoomEnabledChanged)
			Q_PROPERTY(bool selectionEnabled READ isSelectionEnabled WRITE setSelectionEnabled NOTIFY selectionEnabledChanged)
			Q_PROPERTY(bool zoomAtTargetEnabled READ isZoomAtTargetEnabled WRITE setZoomAtTargetEnabled NOTIFY zoomAtTargetEnabledChanged)

	public:
		explicit QInputHandler3D(QObject *parent = Q_NULLPTR);
		virtual ~QInputHandler3D();

		void setRotationEnabled(bool enable);
		bool isRotationEnabled() const;
		void setZoomEnabled(bool enable);
		bool isZoomEnabled() const;
		void setSelectionEnabled(bool enable);
		bool isSelectionEnabled() const;
		void setZoomAtTargetEnabled(bool enable);
		bool isZoomAtTargetEnabled() const;

		// Input event listeners
		virtual void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
		virtual void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
		virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
/*#ifdef Q_WS_WIN
#if QT_CONFIG(wheelevent)
#endif*/
		virtual void wheelEvent(QWheelEvent *event);
/*#ifdef Q_WS_WIN
#endif
#endif*/

	Q_SIGNALS:
		void rotationEnabledChanged(bool enable);
		void zoomEnabledChanged(bool enable);
		void selectionEnabledChanged(bool enable);
		void zoomAtTargetEnabledChanged(bool enable);

	private:
		Q_DISABLE_COPY(QInputHandler3D)

		QScopedPointer<QInputHandler3DPrivate> d_ptr;
	};

}

#endif
