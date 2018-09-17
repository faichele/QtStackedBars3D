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

#ifndef Q_ABSTRACT_GRAPH_3D_PRIVATE_H
#define Q_ABSTRACT_GRAPH_3D_PRIVATE_H

#include "QVisualizationGlobals3D.h"
#include "QAbstractController3D.h"

class QOpenGLContext;
class QOffscreenSurface;

namespace QtStackedBar3DVis
{

	class QAbstractGraph3D;
	class QAbstractController3D;
	class QAbstractAxis3D;

	class QAbstractGraph3DPrivate : public QObject
	{
		Q_OBJECT
	public:
		QAbstractGraph3DPrivate(QAbstractGraph3D *q);
		~QAbstractGraph3DPrivate();

		void setVisualController(QAbstractController3D *controller);
		void handleDevicePixelRatioChange();

		void render();

		QImage renderToImage(int msaaSamples, const QSize &imageSize);

	public Q_SLOTS:
		void renderLater();
		void renderNow();

		virtual void handleAxisXChanged(QAbstractAxis3D *axis) = 0;
		virtual void handleAxisYChanged(QAbstractAxis3D *axis) = 0;
		virtual void handleAxisZChanged(QAbstractAxis3D *axis) = 0;

	public:
		QAbstractGraph3D *q_ptr;

		bool m_updatePending;
		QOpenGLContext *m_context;
		QAbstractController3D *m_visualController;
		float m_devicePixelRatio;
		QOffscreenSurface *m_offscreenSurface;
		bool m_initialized;
	};

}

#endif
