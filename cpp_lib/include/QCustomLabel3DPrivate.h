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

#ifndef QCUSTOMLABELITEM_P_H
#define QCUSTOMLABELITEM_P_H

#include "QCustomLabel3D.h"
#include "QCustomItem3DPrivate.h"

namespace QtStackedBar3DVis
{

	class QCustomLabel3DPrivate : public QCustomItem3DPrivate
	{
		Q_OBJECT

	public:
		QCustomLabel3DPrivate(QCustomLabel3D *q);
		QCustomLabel3DPrivate(QCustomLabel3D *q, const QString &text, const QFont &font,
			const QVector3D &position, const QVector3D &scaling,
			const QQuaternion &rotation);
		virtual ~QCustomLabel3DPrivate();

		void resetDirtyBits();
		void createTextureImage();
		void createTextureImage(const QColor &bgrColor, const QColor &txtColor, bool background,
			bool borders);
		void handleTextureChange();

	public:
		QString m_text;
		QFont m_font;
		QColor m_bgrColor;
		QColor m_txtColor;
		bool m_background;
		bool m_borders;
		bool m_facingCamera;

		bool m_customVisuals;

		bool m_facingCameraDirty;

	private:
		friend class QCustomLabel3D;
	};

}

#endif
