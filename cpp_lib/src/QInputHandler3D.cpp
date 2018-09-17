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

#include "QVisualizationGlobals3D.h"
#include "QInputHandler3DPrivate.h"
#include "QAbstractController3D.h"

namespace QtStackedBar3DVis
{

	static const int halfSizeZoomLevel = 50;
	static const int oneToOneZoomLevel = 100;
	static const int driftTowardCenterLevel = 175;
	static const float wheelZoomDrift = 0.1f;

	static const int nearZoomRangeDivider = 12;
	static const int midZoomRangeDivider = 60;
	static const int farZoomRangeDivider = 120;

	static const float rotationSpeed = 100.0f;

	/*!
	* \class QInputHandler3D
	* \inmodule QtDataVisualization
	* \brief Basic wheel mouse based input handler.
	* \since QtDataVisualization 1.0
	*
	* QInputHandler3D is the basic input handler for wheel mouse type of input devices.
	*
	* Default input handler has the following functionalty:
	* \table
	*      \header
	*          \li Mouse action
	*          \li Action
	*      \row
	*          \li Drag with right button pressed
	*          \li Rotate graph within limits set for QCamera3D.
	*      \row
	*          \li Left click
	*          \li Select item under cursor or remove selection if none.
	*              May open the secondary view depending on the
	*              \l {QAbstractGraph3D::selectionMode}{selection mode}.
	*      \row
	*          \li Mouse wheel
	*          \li Zoom in/out within the allowable zoom range set for QCamera3D.
	*      \row
	*          \li Left click on the primary view when the secondary view is visible
	*          \li Closes the secondary view.
	*          \note Secondary view is available only for QBars3D and Q3DSurface graphs.
	* \endtable
	*
	* Rotation, zoom, and selection can each be individually disabled using
	* corresponding properties of this class.
	*/

	/*!
	* \qmltype InputHandler3D
	* \inqmlmodule QtDataVisualization
	* \since QtDataVisualization 1.2
	* \ingroup datavisualization_qml
	* \instantiates QInputHandler3D
	* \brief Basic wheel mouse based input handler.
	*
	* InputHandler3D is the basic input handler for wheel mouse type of input devices.
	*
	* See QInputHandler3D documentation for more details.
	*/

	/*!
	* \qmlproperty bool InputHandler3D::rotationEnabled
	* \since QtDataVisualization 1.2
	*
	* Defines whether this input handler allows graph rotation.
	* Defaults to \c{true}.
	*/

	/*!
	* \qmlproperty bool InputHandler3D::zoomEnabled
	* \since QtDataVisualization 1.2
	*
	* Defines whether this input handler allows graph zooming.
	* Defaults to \c{true}.
	*/

	/*!
	* \qmlproperty bool InputHandler3D::selectionEnabled
	* \since QtDataVisualization 1.2
	*
	* Defines whether this input handler allows selection from the graph.
	* Defaults to \c{true}.
	*/

	/*!
	* \qmlproperty bool InputHandler3D::zoomAtTargetEnabled
	* \since QtDataVisualization 1.2
	*
	* Defines whether zooming changes the camera target to the position of the input
	* at the time of the zoom.
	* Defaults to \c{true}.
	*/

	/*!
	* Constructs the basic mouse input handler. An optional \a parent parameter can be given
	* and is then passed to QObject constructor.
	*/
	QInputHandler3D::QInputHandler3D(QObject *parent) :
		QAbstractInputHandler3D(parent),
		d_ptr(new QInputHandler3DPrivate(this))
	{
	}

	/*!
	*  Destroys the input handler.
	*/
	QInputHandler3D::~QInputHandler3D()
	{
	}

	// Input event listeners
	/*!
	* Override this to change handling of mouse press events.
	* Mouse press event is given in the \a event and the mouse position in \a mousePos.
	*/
	void QInputHandler3D::mousePressEvent(QMouseEvent *event, const QPoint &mousePos)
	{
#if defined(Q_OS_IOS)
		Q_UNUSED(event);
		Q_UNUSED(mousePos);
#else
		if (Qt::LeftButton == event->button()) {
			if (isSelectionEnabled()) {
				if (scene()->isSlicingActive()) {
					if (scene()->isPointInPrimarySubView(mousePos))
						setInputView(InputViewOnPrimary);
					else if (scene()->isPointInSecondarySubView(mousePos))
						setInputView(InputViewOnSecondary);
					else
						setInputView(InputViewNone);
				}
				else {
					// update mouse positions to prevent jumping when releasing or repressing a button
					setInputPosition(mousePos);
					scene()->setSelectionQueryPosition(mousePos);
					setInputView(InputViewOnPrimary);
					d_ptr->m_inputState = QAbstractInputHandler3DPrivate::InputStateSelecting;
				}
			}
		}
		else if (Qt::MiddleButton == event->button()) {
			if (isRotationEnabled()) {
				// reset rotations
				setInputPosition(QPoint(0, 0));
			}
		}
		else if (Qt::RightButton == event->button()) {
			if (isRotationEnabled()) {
				// disable rotating when in slice view
				if (!scene()->isSlicingActive())
					d_ptr->m_inputState = QAbstractInputHandler3DPrivate::InputStateRotating;
				// update mouse positions to prevent jumping when releasing or repressing a button
				setInputPosition(mousePos);
			}
		}
#endif
	}

	/*!
	* Override this to change handling of mouse release events.
	* Mouse release event is given in the \a event and the mouse position in \a mousePos.
	*/
	void QInputHandler3D::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos)
	{
		Q_UNUSED(event);
#if defined(Q_OS_IOS)
		Q_UNUSED(mousePos);
#else
		if (QAbstractInputHandler3DPrivate::InputStateRotating == d_ptr->m_inputState) {
			// update mouse positions to prevent jumping when releasing or repressing a button
			setInputPosition(mousePos);
		}
		d_ptr->m_inputState = QAbstractInputHandler3DPrivate::InputStateNone;
		setInputView(InputViewNone);
#endif
	}

	/*!
	* Override this to change handling of mouse move events.
	* Mouse move event is given in the \a event and the mouse position in \a mousePos.
	*/
	void QInputHandler3D::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos)
	{
		Q_UNUSED(event);
#if defined(Q_OS_IOS)
		Q_UNUSED(mousePos);
#else
		if (QAbstractInputHandler3DPrivate::InputStateRotating == d_ptr->m_inputState
			&& isRotationEnabled()) {
			// Calculate mouse movement since last frame
			float xRotation = scene()->activeCamera()->xRotation();
			float yRotation = scene()->activeCamera()->yRotation();
			float mouseMoveX = float(inputPosition().x() - mousePos.x())
				/ (scene()->viewport().width() / rotationSpeed);
			float mouseMoveY = float(inputPosition().y() - mousePos.y())
				/ (scene()->viewport().height() / rotationSpeed);
			// Apply to rotations
			xRotation -= mouseMoveX;
			yRotation -= mouseMoveY;
			scene()->activeCamera()->setXRotation(xRotation);
			scene()->activeCamera()->setYRotation(yRotation);

			setPreviousInputPos(inputPosition());
			setInputPosition(mousePos);
		}
#endif
	}

/*#ifdef Q_WS_WIN
#if QT_CONFIG(wheelevent)
#endif*/
	/*!
	* Override this to change handling of wheel events.
	* The wheel event is given in the \a event.
	*/
	void QInputHandler3D::wheelEvent(QWheelEvent *event)
	{
		if (isZoomEnabled()) {
			// disable zooming if in slice view
			if (scene()->isSlicingActive())
				return;

			// Adjust zoom level based on what zoom range we're in.
			QCamera3D *camera = scene()->activeCamera();
			int zoomLevel = int(camera->zoomLevel());
			const int minZoomLevel = int(camera->minZoomLevel());
			const int maxZoomLevel = int(camera->maxZoomLevel());
			if (zoomLevel > oneToOneZoomLevel)
				zoomLevel += event->angleDelta().y() / nearZoomRangeDivider;
			else if (zoomLevel > halfSizeZoomLevel)
				zoomLevel += event->angleDelta().y() / midZoomRangeDivider;
			else
				zoomLevel += event->angleDelta().y() / farZoomRangeDivider;
			zoomLevel = qBound(minZoomLevel, zoomLevel, maxZoomLevel);

			if (isZoomAtTargetEnabled()) {
				scene()->setGraphPositionQuery(event->pos());
				d_ptr->m_zoomAtTargetPending = true;
				// If zoom at target is enabled, we don't want to zoom yet, as that causes
				// jitter. Instead, we zoom next frame, when we apply the camera position.
				d_ptr->m_requestedZoomLevel = zoomLevel;
				d_ptr->m_driftMultiplier = wheelZoomDrift;
			}
			else {
				camera->setZoomLevel(zoomLevel);
			}
		}
	}
/*#ifdef Q_WS_WIN
#endif
#endif*/

	/*!
	* \property QInputHandler3D::rotationEnabled
	* \since QtDataVisualization 1.2
	*
	* \brief Whether this input handler allows graph rotation.
	*
	* Defaults to \c{true}.
	*/
	void QInputHandler3D::setRotationEnabled(bool enable)
	{
		if (d_ptr->m_rotationEnabled != enable) {
			d_ptr->m_rotationEnabled = enable;
			emit rotationEnabledChanged(enable);
		}
	}

	bool QInputHandler3D::isRotationEnabled() const
	{
		return d_ptr->m_rotationEnabled;
	}

	/*!
	* \property QInputHandler3D::zoomEnabled
	* \since QtDataVisualization 1.2
	*
	* \brief Whether this input handler allows graph zooming.
	*
	* Defaults to \c{true}.
	*/
	void QInputHandler3D::setZoomEnabled(bool enable)
	{
		if (d_ptr->m_zoomEnabled != enable) {
			d_ptr->m_zoomEnabled = enable;
			emit zoomEnabledChanged(enable);
		}
	}

	bool QInputHandler3D::isZoomEnabled() const
	{
		return d_ptr->m_zoomEnabled;
	}

	/*!
	* \property QInputHandler3D::selectionEnabled
	* \since QtDataVisualization 1.2
	*
	* \brief Whether this input handler allows selection from the graph.
	*
	* Defaults to \c{true}.
	*/
	void QInputHandler3D::setSelectionEnabled(bool enable)
	{
		if (d_ptr->m_selectionEnabled != enable) {
			d_ptr->m_selectionEnabled = enable;
			emit selectionEnabledChanged(enable);
		}
	}

	bool QInputHandler3D::isSelectionEnabled() const
	{
		return d_ptr->m_selectionEnabled;
	}

	/*!
	* \property QInputHandler3D::zoomAtTargetEnabled
	* \since QtDataVisualization 1.2
	*
	* \brief Whether zooming should change the camera target so that the zoomed point
	* of the graph stays at the same location after the zoom.
	*
	* Defaults to \c{true}.
	*/
	void QInputHandler3D::setZoomAtTargetEnabled(bool enable)
	{
		if (d_ptr->m_zoomAtTargetEnabled != enable) {
			d_ptr->m_zoomAtTargetEnabled = enable;
			emit zoomAtTargetEnabledChanged(enable);
		}
	}

	bool QInputHandler3D::isZoomAtTargetEnabled() const
	{
		return d_ptr->m_zoomAtTargetEnabled;
	}

	QInputHandler3DPrivate::QInputHandler3DPrivate(QInputHandler3D *q)
		: q_ptr(q),
		m_inputState(QAbstractInputHandler3DPrivate::InputStateNone),
		m_rotationEnabled(true),
		m_zoomEnabled(true),
		m_selectionEnabled(true),
		m_zoomAtTargetEnabled(true),
		m_zoomAtTargetPending(false),
		m_controller(0),
		m_requestedZoomLevel(0.0f),
		m_driftMultiplier(0.0f)
	{
		QObject::connect(q, &QAbstractInputHandler3D::sceneChanged,
			this, &QInputHandler3DPrivate::handleSceneChange);
	}

	QInputHandler3DPrivate::~QInputHandler3DPrivate()
	{
	}

	void QInputHandler3DPrivate::handleSceneChange(QScene3D *scene)
	{
		if (scene) {
			if (m_controller) {
				QObject::disconnect(m_controller, &QAbstractController3D::queriedGraphPositionChanged,
					this, &QInputHandler3DPrivate::handleQueriedGraphPositionChange);
			}

			m_controller = qobject_cast<QAbstractController3D *>(scene->parent());

			if (m_controller) {
				QObject::connect(m_controller, &QAbstractController3D::queriedGraphPositionChanged,
					this, &QInputHandler3DPrivate::handleQueriedGraphPositionChange);
			}
		}
	}

	void QInputHandler3DPrivate::handleQueriedGraphPositionChange()
	{
		if (m_zoomAtTargetPending) {
			// Check if the zoom point is on graph
			QVector3D newTarget = m_controller->queriedGraphPosition();
			float currentZoom = m_requestedZoomLevel;
			float previousZoom = q_ptr->scene()->activeCamera()->zoomLevel();
			q_ptr->scene()->activeCamera()->setZoomLevel(currentZoom);
			float diffAdj = 0.0f;

			// If zooming in/out outside the graph, or zooming out after certain point,
			// move towards the center.
			if ((qAbs(newTarget.x()) > 1.0f
				|| qAbs(newTarget.y()) > 1.0f
				|| qAbs(newTarget.z()) > 1.0f)
				|| (previousZoom > currentZoom && currentZoom <= driftTowardCenterLevel)) {
				newTarget = zeroVector;
				// Add some extra correction so that we actually reach the center eventually
				diffAdj = m_driftMultiplier;
				if (previousZoom > currentZoom)
					diffAdj *= 2.0f; // Correct towards center little more when zooming out
			}

			float zoomFraction = 1.0f - (previousZoom / currentZoom);

			// Adjust camera towards the zoom point, attempting to keep the cursor at same graph point
			QVector3D oldTarget = q_ptr->scene()->activeCamera()->target();
			QVector3D origDiff = newTarget - oldTarget;
			QVector3D diff = origDiff * zoomFraction + (origDiff.normalized() * diffAdj);
			if (diff.length() > origDiff.length())
				diff = origDiff;
			q_ptr->scene()->activeCamera()->setTarget(oldTarget + diff);

			if (q_ptr->scene()->selectionQueryPosition() == QScene3D::invalidSelectionPoint())
				m_zoomAtTargetPending = false;
		}
	}

}
