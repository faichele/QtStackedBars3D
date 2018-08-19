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

#ifndef SELECTIONPOINTER_P_H
#define SELECTIONPOINTER_P_H

#include "QVisualizationGlobals3D.h"
#include "QSurfaceController3D.h"

namespace QtStackedBar3DVis
{

	class QShaderHelper3D;
	class QObjectHelper3D;
	class QTextureHelper3D;
	class QDrawer3D;

	class QT_STACKEDBAR3D_EXPORT QSelectionPointer3D : public QObject, protected QOpenGLFunctions
	{
		Q_OBJECT

	public:
		explicit QSelectionPointer3D(QDrawer3D *drawer);
		~QSelectionPointer3D();

		void renderSelectionPointer(GLuint defaultFboHandle = 0, bool useOrtho = false);
		void renderSelectionLabel(GLuint defaultFboHandle = 0, bool useOrtho = false);
		void setPosition(const QVector3D &position);
		void setLabel(const QString &label, bool themeChange = false);
		void setPointerObject(QObjectHelper3D *object);
		void setLabelObject(QObjectHelper3D *object);
		void handleDrawerChange();
		void updateBoundingRect(const QRect &rect);
		void updateScene(QScene3D *scene);
		void updateSliceData(bool sliceActivated, GLfloat autoScaleAdjustment);
		void setHighlightColor(const QVector4D &colorVector);
		void setRotation(const QQuaternion &rotation);

	private:
		void initializeOpenGL();
		void initShaders();

	private:
		QShaderHelper3D *m_labelShader;
		QShaderHelper3D *m_pointShader;
		QObjectHelper3D *m_labelObj; // Not owned
		QObjectHelper3D *m_pointObj; // Not owned
		QTextureHelper3D *m_textureHelper;
		QTheme3D *m_cachedTheme;
		bool m_labelBackground;
		QLabelItem3D m_labelItem;
		QDrawer3D *m_drawer;
		QRect m_mainViewPort;
		QVector3D m_position;
		QScene3D *m_cachedScene;
		QString m_label;
		bool m_cachedIsSlicingActivated;
		GLfloat m_autoScaleAdjustment;
		QVector4D m_highlightColor;
		QQuaternion m_rotation;
	};
}

#endif
