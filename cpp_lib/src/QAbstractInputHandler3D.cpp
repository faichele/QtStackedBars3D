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

#include "QAbstractInputHandler3DPrivate.h"

namespace QtStackedBar3DVis
{

	/*!
	* \class QAbstractInputHandler3D
	* \inmodule QtDataVisualization
	* \brief The base class for implementations of input handlers.
	* \since QtDataVisualization 1.0
	*
	* QAbstractInputHandler3D is the base class that is subclassed by different input handling implementations
	* that take input events and translate those to camera and light movements. Input handlers also translate
	* raw input events to slicing and selection events in the scene.
	*/

	/*!
	* \enum QAbstractInputHandler3D::InputView
	*
	* Predefined input views for mouse and touch based input handlers.
	*
	* \value InputViewNone
	*        Mouse or touch not on a view.
	* \value InputViewOnPrimary
	*        Mouse or touch input received on the primary view area. If secondary view is displayed when
	*        inputView becomes InputViewOnPrimary, secondary view is closed.
	* \value InputViewOnSecondary
	*        Mouse or touch input received on the secondary view area.
	*/

	/*!
	* \qmltype AbstractInputHandler3D
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.0
	* \ingroup datavisualization_qml
	* \instantiates QAbstractInputHandler3D
	* \brief Base type for all QtDataVisualization input handlers.
	*
	* This type is uncreatable.
	*
	* For AbstractInputHandler3D enums, see \l{QAbstractInputHandler3D::InputView}.
	*/

	/*!
	* Constructs the base class. An optional \a parent parameter can be given
	* and is then passed to QObject constructor.
	*/
	QAbstractInputHandler3D::QAbstractInputHandler3D(QObject *parent) :
		QObject(parent),
		d_ptr(new QAbstractInputHandler3DPrivate(this))
	{
	}

	/*!
	*  Destroys the base class.
	*/
	QAbstractInputHandler3D::~QAbstractInputHandler3D()
	{
	}

	// Input event listeners
	/*!
	* Override this to handle mouse double click events.
	* Mouse double click event is given in the \a event.
	*/
	void QAbstractInputHandler3D::mouseDoubleClickEvent(QMouseEvent *event)
	{
		Q_UNUSED(event);
	}

	/*!
	* Override this to handle touch input events.
	* Touch event is given in the \a event.
	*/
	void QAbstractInputHandler3D::touchEvent(QTouchEvent *event)
	{
		Q_UNUSED(event);
	}

	/*!
	* Override this to handle mouse press events.
	* Mouse press event is given in the \a event and the mouse position in \a mousePos.
	*/
	void QAbstractInputHandler3D::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
	{
		Q_UNUSED(event);
		Q_UNUSED(mousePos);
	}

	/*!
	* Override this to handle mouse release events.
	* Mouse release event is given in the \a event and the mouse position in \a mousePos.
	*/
	void QAbstractInputHandler3D::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
	{
		Q_UNUSED(event);
		Q_UNUSED(mousePos);
	}

	/*!
	* Override this to handle mouse move events.
	* Mouse move event is given in the \a event and the mouse position in \a mousePos.
	*/
	void QAbstractInputHandler3D::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
	{
		Q_UNUSED(event);
		Q_UNUSED(mousePos);
	}

/*#ifdef Q_WS_WIN
#if QT_CONFIG(wheelevent)
#endif*/
	/*!
	* Override this to handle wheel events.
	* Wheel event is given in the \a event.
	*/
	void QAbstractInputHandler3D::wheelEvent(QWheelEvent *event)
	{
		Q_UNUSED(event);
	}
/*#ifdef Q_WS_WIN
#endif
#endif*/

	// Property get/set
	/*!
	* \property QAbstractInputHandler3D::inputView
	*
	* \brief The current enumerated input view based on the view of the processed
	* input events.
	*
	* One of the InputView enum values.
	*
	* When the view changes, the \c inputViewChanged signal is emitted.
	*
	* \sa InputView
	*/
	QAbstractInputHandler3D::InputView QAbstractInputHandler3D::inputView() const
	{
		return d_ptr->m_inputView;
	}

	void QAbstractInputHandler3D::setInputView(InputView inputView)
	{
		if (inputView != d_ptr->m_inputView) {
			d_ptr->m_inputView = inputView;
			emit inputViewChanged(inputView);
		}
	}

	/*!
	* \property QAbstractInputHandler3D::inputPosition
	*
	* \brief The last input position based on the processed input events.
	*/
	QPoint QAbstractInputHandler3D::inputPosition() const
	{
		return d_ptr->m_inputPosition;
	}

	void QAbstractInputHandler3D::setInputPosition(const QPoint &position)
	{
		if (position != d_ptr->m_inputPosition) {
			d_ptr->m_inputPosition = position;
			emit positionChanged(position);
		}
	}

	/*!
	* Returns the manhattan length between last two input positions.
	*/
	int QAbstractInputHandler3D::prevDistance() const
	{
		return d_ptr->m_prevDistance;
	}

	/*!
	* Sets the \a distance (manhattan length) between last two input positions.
	*/
	void QAbstractInputHandler3D::setPrevDistance(int distance)
	{
		d_ptr->m_prevDistance = distance;
	}

	/*!
	* \property QAbstractInputHandler3D::scene
	*
	* \brief The 3D scene this abstract input handler is controlling.
	*
	* One input handler can control one scene. Setting a scene to an input handler
	* does not transfer the ownership of the scene.
	*/
	QScene3D *QAbstractInputHandler3D::scene() const
	{
		return d_ptr->m_scene;
	}

	void QAbstractInputHandler3D::setScene(QScene3D *scene)
	{
		if (scene != d_ptr->m_scene) {
			d_ptr->m_scene = scene;
			emit sceneChanged(scene);
		}
	}

	/*!
	* Sets the previous input position to the point given by \a position.
	*/
	void QAbstractInputHandler3D::setPreviousInputPos(const QPoint &position)
	{
		d_ptr->m_previousInputPos = position;
	}

	/*!
	* Returns the previous input position.
	*/
	QPoint QAbstractInputHandler3D::previousInputPos() const
	{
		return d_ptr->m_previousInputPos;
	}

	QAbstractInputHandler3DPrivate::QAbstractInputHandler3DPrivate(QAbstractInputHandler3D *q) :
		q_ptr(q),
		m_prevDistance(0),
		m_previousInputPos(QPoint(0, 0)),
		m_inputView(QAbstractInputHandler3D::InputViewNone),
		m_inputPosition(QPoint(0, 0)),
		m_scene(0),
		m_isDefaultHandler(false)
	{
	}

	QAbstractInputHandler3DPrivate::~QAbstractInputHandler3DPrivate()
	{

	}
}
