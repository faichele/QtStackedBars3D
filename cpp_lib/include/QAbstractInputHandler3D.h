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

#ifndef Q_ABSTRACT_INPUT_HANDLER_3D_H
#define Q_ABSTRACT_INPUT_HANDLER_3D_H

#include "QVisualizationGlobals3D.h"
#include "QScene3D.h"

#include <QtCore/QObject>
#include <QtCore/QPoint>
#include <QtGui/QWheelEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QTouchEvent>

namespace QtStackedBar3DVis
{

	class QAbstractInputHandler3DPrivate;

	class QT_STACKEDBAR3D_EXPORT QAbstractInputHandler3D : public QObject
	{
		Q_OBJECT
			Q_ENUMS(InputView)
			Q_PROPERTY(InputView inputView READ inputView WRITE setInputView NOTIFY inputViewChanged)
			Q_PROPERTY(QPoint inputPosition READ inputPosition WRITE setInputPosition NOTIFY positionChanged)
			Q_PROPERTY(QScene3D *scene READ scene WRITE setScene NOTIFY sceneChanged)

	public:
		enum InputView {
			InputViewNone = 0,
			InputViewOnPrimary,
			InputViewOnSecondary
		};

	protected:
		explicit QAbstractInputHandler3D(QObject *parent = Q_NULLPTR);
	public:
		virtual ~QAbstractInputHandler3D();

		// Input event listeners
		virtual void mouseDoubleClickEvent(QMouseEvent *event);
		virtual void touchEvent(QTouchEvent *event);
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
		InputView inputView() const;
		void setInputView(InputView inputView);

		QPoint inputPosition() const;
		void setInputPosition(const QPoint &position);

		QScene3D *scene() const;
		void setScene(QScene3D *scene);

	Q_SIGNALS:
		void positionChanged(const QPoint &position);
		void inputViewChanged(QAbstractInputHandler3D::InputView view);
		void sceneChanged(QScene3D *scene);

	protected:
		void setPrevDistance(int distance);
		int prevDistance() const;
		void setPreviousInputPos(const QPoint &position);
		QPoint previousInputPos() const;

	private:
		Q_DISABLE_COPY(QAbstractInputHandler3D)

		QScopedPointer<QAbstractInputHandler3DPrivate> d_ptr;

		friend class QAbstractController3D;
		friend class QTouchInputHandler3DPrivate;
	};

}

#endif
