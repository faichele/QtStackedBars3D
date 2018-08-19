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

#ifndef Q_3D_SCENE_H
#define Q_3D_SCENE_H

#include <QVisualizationGlobals3D.h>
#include "QCamera3D.h"
#include "QLight3D.h"
#include <QtCore/QObject>
#include <QtCore/QRect>

namespace QtStackedBar3DVis
{
	class QScene3DPrivate;

	class QT_STACKEDBAR3D_EXPORT QScene3D : public QObject
	{
		Q_OBJECT
			Q_PROPERTY(QRect viewport READ viewport NOTIFY viewportChanged)
			Q_PROPERTY(QRect primarySubViewport READ primarySubViewport WRITE setPrimarySubViewport NOTIFY primarySubViewportChanged)
			Q_PROPERTY(QRect secondarySubViewport READ secondarySubViewport WRITE setSecondarySubViewport NOTIFY secondarySubViewportChanged)
			Q_PROPERTY(QPoint selectionQueryPosition READ selectionQueryPosition WRITE setSelectionQueryPosition NOTIFY selectionQueryPositionChanged)
			Q_PROPERTY(bool secondarySubviewOnTop READ isSecondarySubviewOnTop  WRITE setSecondarySubviewOnTop  NOTIFY secondarySubviewOnTopChanged)
			Q_PROPERTY(bool slicingActive READ isSlicingActive WRITE setSlicingActive NOTIFY slicingActiveChanged)
			Q_PROPERTY(QCamera3D* activeCamera READ activeCamera WRITE setActiveCamera NOTIFY activeCameraChanged)
			Q_PROPERTY(QLight3D* activeLight READ activeLight WRITE setActiveLight NOTIFY activeLightChanged)
			Q_PROPERTY(float devicePixelRatio READ devicePixelRatio WRITE setDevicePixelRatio NOTIFY devicePixelRatioChanged)
			Q_PROPERTY(QPoint graphPositionQuery READ graphPositionQuery WRITE setGraphPositionQuery NOTIFY graphPositionQueryChanged REVISION 1)

	public:
		explicit QScene3D(QObject *parent = Q_NULLPTR);
		virtual ~QScene3D();

		QRect viewport() const;

		QRect primarySubViewport() const;
		void setPrimarySubViewport(const QRect &primarySubViewport);
		bool isPointInPrimarySubView(const QPoint &point);

		QRect secondarySubViewport() const;
		void setSecondarySubViewport(const QRect &secondarySubViewport);
		bool isPointInSecondarySubView(const QPoint &point);

		void setSelectionQueryPosition(const QPoint &point);
		QPoint selectionQueryPosition() const;
		static QPoint invalidSelectionPoint();

		void setGraphPositionQuery(const QPoint &point);
		QPoint graphPositionQuery() const;

		void setSlicingActive(bool isSlicing);
		bool isSlicingActive() const;

		void setSecondarySubviewOnTop(bool isSecondaryOnTop);
		bool isSecondarySubviewOnTop() const;

		QCamera3D *activeCamera() const;
		void setActiveCamera(QCamera3D *camera);

		QLight3D *activeLight() const;
		void setActiveLight(QLight3D *light);

		float devicePixelRatio() const;
		void setDevicePixelRatio(float pixelRatio);

	Q_SIGNALS:
		void viewportChanged(const QRect &viewport);
		void primarySubViewportChanged(const QRect &subViewport);
		void secondarySubViewportChanged(const QRect &subViewport);
		void secondarySubviewOnTopChanged(bool isSecondaryOnTop);
		void slicingActiveChanged(bool isSlicingActive);
		void activeCameraChanged(QCamera3D *camera);
		void activeLightChanged(QLight3D *light);
		void devicePixelRatioChanged(float pixelRatio);
		void selectionQueryPositionChanged(const QPoint &position);
		Q_REVISION(1) void graphPositionQueryChanged(const QPoint &position);

	private:
		QScopedPointer<QScene3DPrivate> d_ptr;

		Q_DISABLE_COPY(QScene3D)

		friend class AbstractDeclarative;
		friend class QAbstractGraph3D;
		friend class QAbstractGraph3DPrivate;
		friend class QAbstractController3D;
		friend class QBarsController3D;
		friend class QStackedBarsController3D;
		friend class QScene3DPrivate;
		friend class QAbstractRenderer3D;
		friend class QBarsRenderer3D;
		friend class QStackedBarsRenderer3D;
		friend class QSurfaceRenderer3D;
		friend class QScatterRenderer3D;
		friend class QCamera3DPrivate;
		friend class QObject3D;
	};

}
#endif
