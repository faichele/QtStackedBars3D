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

#ifndef Q3DSCENE_P_H
#define Q3DSCENE_P_H

#include "QVisualizationGlobals3D.h"
#include "QScene3D.h"

namespace QtStackedBar3DVis
{
	class QCamera3D;
	class QLight3D;

	struct Q3DSceneChangeBitField {
		bool viewportChanged : 1;
		bool primarySubViewportChanged : 1;
		bool secondarySubViewportChanged : 1;
		bool subViewportOrderChanged : 1;
		bool cameraChanged : 1;
		bool lightChanged : 1;
		bool slicingActivatedChanged : 1;
		bool devicePixelRatioChanged : 1;
		bool selectionQueryPositionChanged : 1;
		bool graphPositionQueryPositionChanged : 1;
		bool windowSizeChanged : 1;

		Q3DSceneChangeBitField()
			: viewportChanged(true),
			primarySubViewportChanged(true),
			secondarySubViewportChanged(true),
			subViewportOrderChanged(true),
			cameraChanged(true),
			lightChanged(true),
			slicingActivatedChanged(true),
			devicePixelRatioChanged(true),
			selectionQueryPositionChanged(false),
			graphPositionQueryPositionChanged(false),
			windowSizeChanged(true)
		{
		}
	};

	class QScene3DPrivate : public QObject
	{
		Q_OBJECT
	public:
		QScene3DPrivate(QScene3D *q);
		~QScene3DPrivate();

		void sync(QScene3DPrivate &other);

		void setViewport(const QRect &viewport);
		void setViewportSize(int width, int height);
		void setWindowSize(const QSize &size);
		QSize windowSize() const;
		void calculateSubViewports();
		void updateGLViewport();
		void updateGLSubViewports();

		QRect glViewport();
		QRect glPrimarySubViewport();
		QRect glSecondarySubViewport();

		void setLightPositionRelativeToCamera(const QVector3D &relativePosition,
			float fixedRotation = 0.0f,
			float distanceModifier = 0.0f);

		void markDirty();

		bool isInArea(const QRect &area, int x, int y) const;

	Q_SIGNALS:
		void needRender();

	public:
		QScene3D *q_ptr;
		Q3DSceneChangeBitField m_changeTracker;

		QRect m_viewport;
		QRect m_primarySubViewport;
		QRect m_secondarySubViewport;
		bool m_isSecondarySubviewOnTop;
		float m_devicePixelRatio;
		QCamera3D *m_camera;
		QLight3D *m_light;
		bool m_isUnderSideCameraEnabled;
		bool m_isSlicingActive;
		QPoint m_selectionQueryPosition;
		QPoint m_graphPositionQueryPosition;
		QSize m_windowSize;
		QRect m_glViewport;
		QRect m_glPrimarySubViewport;
		QRect m_glSecondarySubViewport;
		bool m_sceneDirty;
		QRect m_defaultSmallViewport;
		QRect m_defaultLargeViewport;
	};

}

#endif
