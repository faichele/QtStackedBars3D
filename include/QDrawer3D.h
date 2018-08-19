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

#ifndef Q_DRAWER_3D_H
#define Q_DRAWER_3D_H

#include "QVisualizationGlobals3D.h"
#include <QBars3D.h>
#include "QTheme3D.h"
#include "QLabelItem3D.h"
#include "QAbstractRenderItem3D.h"

namespace QtStackedBar3DVis
{
	class QShaderHelper3D;
	class QObjectHelper3D;
	class QAbstractObjectHelper3D;
	class QSurfaceObject3D;
	class QTextureHelper3D;
	class QCamera3D;
	class QAbstractRenderer3D;
	class QScatterPointBufferHelper3D;

	class QT_STACKEDBAR3D_EXPORT QDrawer3D : public QObject, public QOpenGLFunctions
	{
		Q_OBJECT

	public:
		enum LabelPosition {
			LabelBelow = 0,
			LabelLow,
			LabelMid,
			LabelHigh,
			LabelOver,
			LabelBottom,    // Absolute positions from here onward, used for axes (QDataItem is ignored)
			LabelTop,
			LabelLeft,
			LabelRight
		};

	public:
		explicit QDrawer3D(QTheme3D *theme);
		~QDrawer3D();

		void initializeOpenGL();

		void setTheme(QTheme3D *theme);
		QTheme3D *theme() const;
		QFont font() const;
		inline GLfloat scaledFontSize() const { return m_scaledFontSize; }

		void drawObject(QShaderHelper3D *shader, QAbstractObjectHelper3D *object, GLuint textureId = 0,
			GLuint depthTextureId = 0, GLuint textureId3D = 0);
		void drawSelectionObject(QShaderHelper3D *shader, QAbstractObjectHelper3D *object);
		void drawSurfaceGrid(QShaderHelper3D *shader, QSurfaceObject3D *object);
		void drawPoint(QShaderHelper3D *shader);
		void drawPoints(QShaderHelper3D *shader, QScatterPointBufferHelper3D *object, GLuint textureId);
		void drawLine(QShaderHelper3D *shader);
		void drawLabel(const QAbstractRenderItem3D &item, const QLabelItem3D &labelItem,
			const QMatrix4x4 &viewmatrix, const QMatrix4x4 &projectionmatrix,
			const QVector3D &positionComp, const QQuaternion &rotation, GLfloat itemHeight,
			QAbstractGraph3D::SelectionFlags mode, QShaderHelper3D *shader, QObjectHelper3D *object,
			const QCamera3D *camera, bool useDepth = false, bool rotateAlong = false,
			LabelPosition position = LabelOver,
			Qt::AlignmentFlag alignment = Qt::AlignCenter, bool isSlicing = false,
			bool isSelecting = false, bool smallFontSize = false);

		void generateSelectionLabelTexture(QAbstractRenderer3D *item);
		void generateLabelItem(QLabelItem3D &item, const QString &text, int widestLabel = 0);

	Q_SIGNALS:
		void drawerChanged();

	private:
		QTheme3D *m_theme;
		QTextureHelper3D *m_textureHelper;
		GLuint m_pointbuffer;
		GLuint m_linebuffer;
		GLfloat m_scaledFontSize;
	};

}

#endif
