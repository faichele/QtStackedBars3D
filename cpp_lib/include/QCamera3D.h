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

#ifndef Q_CAMERA_3D_H
#define Q_CAMERA_3D_H

#include "QObject3D.h"

namespace QtStackedBar3DVis
{
	class QCamera3DPrivate;

	class QSelectionPointer3D;

	class QT_STACKEDBAR3D_EXPORT QCamera3D : public QObject3D
	{
		Q_OBJECT
			Q_ENUMS(CameraPreset)
			Q_PROPERTY(float xRotation READ xRotation WRITE setXRotation NOTIFY xRotationChanged)
			Q_PROPERTY(float yRotation READ yRotation WRITE setYRotation NOTIFY yRotationChanged)
			Q_PROPERTY(float zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
			Q_PROPERTY(CameraPreset cameraPreset READ cameraPreset WRITE setCameraPreset NOTIFY cameraPresetChanged)
			Q_PROPERTY(bool wrapXRotation READ wrapXRotation WRITE setWrapXRotation NOTIFY wrapXRotationChanged)
			Q_PROPERTY(bool wrapYRotation READ wrapYRotation WRITE setWrapYRotation NOTIFY wrapYRotationChanged)
			Q_PROPERTY(QVector3D target READ target WRITE setTarget NOTIFY targetChanged REVISION 1)
			Q_PROPERTY(float minZoomLevel READ minZoomLevel WRITE setMinZoomLevel NOTIFY minZoomLevelChanged REVISION 1)
			Q_PROPERTY(float maxZoomLevel READ maxZoomLevel WRITE setMaxZoomLevel NOTIFY maxZoomLevelChanged REVISION 1)

	public:
		enum CameraPreset {
			CameraPresetNone = -1,
			CameraPresetFrontLow = 0,
			CameraPresetFront,
			CameraPresetFrontHigh,
			CameraPresetLeftLow,
			CameraPresetLeft,
			CameraPresetLeftHigh,
			CameraPresetRightLow,
			CameraPresetRight,
			CameraPresetRightHigh,
			CameraPresetBehindLow,
			CameraPresetBehind,
			CameraPresetBehindHigh,
			CameraPresetIsometricLeft,
			CameraPresetIsometricLeftHigh,
			CameraPresetIsometricRight,
			CameraPresetIsometricRightHigh,
			CameraPresetDirectlyAbove,
			CameraPresetDirectlyAboveCW45,
			CameraPresetDirectlyAboveCCW45,
			CameraPresetFrontBelow,
			CameraPresetLeftBelow,
			CameraPresetRightBelow,
			CameraPresetBehindBelow,
			CameraPresetDirectlyBelow
		};

		explicit QCamera3D(QObject *parent = Q_NULLPTR);
		virtual ~QCamera3D();

		float xRotation() const;
		void setXRotation(float rotation);
		float yRotation() const;
		void setYRotation(float rotation);

		bool wrapXRotation() const;
		void setWrapXRotation(bool isEnabled);

		bool wrapYRotation() const;
		void setWrapYRotation(bool isEnabled);

		virtual void copyValuesFrom(const QObject3D &source);

		CameraPreset cameraPreset() const;
		void setCameraPreset(CameraPreset preset);

		float zoomLevel() const;
		void setZoomLevel(float zoomLevel);
		float minZoomLevel() const;
		void setMinZoomLevel(float zoomLevel);
		float maxZoomLevel() const;
		void setMaxZoomLevel(float zoomLevel);

		void setCameraPosition(float horizontal, float vertical, float zoom = 100.0f);

		QVector3D target() const;
		void setTarget(const QVector3D &target);

		friend class QtStackedBar3DVis::QSelectionPointer3D;

	Q_SIGNALS:
		void xRotationChanged(float rotation);
		void yRotationChanged(float rotation);
		void zoomLevelChanged(float zoomLevel);
		void cameraPresetChanged(QCamera3D::CameraPreset preset);
		void wrapXRotationChanged(bool isEnabled);
		void wrapYRotationChanged(bool isEnabled);
		Q_REVISION(1) void targetChanged(const QVector3D &target);
		Q_REVISION(1) void minZoomLevelChanged(float zoomLevel);
		Q_REVISION(1) void maxZoomLevelChanged(float zoomLevel);

	private:
		QScopedPointer<QCamera3DPrivate> d_ptr;

		Q_DISABLE_COPY(QCamera3D)

		friend class QCamera3DPrivate;
		friend class QScene3DPrivate;
		friend class QAbstractRenderer3D;
		friend class QBarsRenderer3D;
		friend class QStackedBarsRenderer3D;
		friend class QSurfaceRenderer3D;
		friend class QScatterRenderer3D;
		
		friend class QInputHandler3D;
		friend class QTouchInputHandler3DPrivate;
	};
}

#endif
