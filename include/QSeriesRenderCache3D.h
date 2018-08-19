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

#ifndef Q_SERIES_RENDER_CACHE_H
#define Q_SERIES_RENDER_CACHE_H

#include "QVisualizationGlobals3D.h"
#include "QAbstractSeries3D.h"

namespace QtStackedBar3DVis
{

	class QAbstractRenderer3D;
	class QObjectHelper3D;
	class QTextureHelper3D;

	class QT_STACKEDBAR3D_EXPORT QSeriesRenderCache3D
	{
	public:
		QSeriesRenderCache3D(QAbstractSeries3D *series, QAbstractRenderer3D *renderer);
		virtual ~QSeriesRenderCache3D();

		virtual void populate(bool newSeries);
		virtual void cleanup(QTextureHelper3D *texHelper);

		// NOTE: Series pointer can only be used to access the series when syncing with controller.
		// It is not guaranteed to be valid while rendering and should only be used as an identifier.
		inline QAbstractSeries3D *series() const { return m_series; }

		inline const QAbstractSeries3D::Mesh &mesh() const { return m_mesh; }
		inline const QQuaternion &meshRotation() const { return m_meshRotation; }
		inline void setMeshRotation(const QQuaternion &rotation) { m_meshRotation = rotation; }
		inline QObjectHelper3D *object() const { return m_object; }
		inline const QTheme3D::ColorStyle &colorStyle() const { return m_colorStyle; }
		inline const QVector4D &baseColor() const { return m_baseColor; }
		inline const GLuint &baseUniformTexture() const { return m_baseUniformTexture; }
		inline const GLuint &baseGradientTexture() const { return m_baseGradientTexture; }
		inline const QImage &gradientImage() const { return m_gradientImage; }
		inline const QVector4D &singleHighlightColor() const { return m_singleHighlightColor; }
		inline const GLuint &singleHighlightGradientTexture() const { return m_singleHighlightGradientTexture; }
		inline const QVector4D &multiHighlightColor() const { return m_multiHighlightColor; }
		inline const GLuint &multiHighlightGradientTexture() const { return m_multiHighlightGradientTexture; }
		inline const QString &name() const { return m_name; }
		inline const QString &itemLabel() const { return m_itemLabel; }
		inline void setValid(bool valid) { m_valid = valid; }
		inline bool isValid() const { return m_valid; }
		inline bool isVisible() const { return m_visible; }
		inline void setDataDirty(bool state) { m_objectDirty = state; }
		inline bool dataDirty() const { return m_objectDirty; }
		inline void setStaticObjectUVDirty(bool state) { m_staticObjectUVDirty = state; }
		inline bool staticObjectUVDirty() { return m_staticObjectUVDirty; }

	protected:
		QAbstractSeries3D *m_series;
		QObjectHelper3D *m_object; // Shared reference
		QAbstractSeries3D::Mesh m_mesh;
		QQuaternion m_meshRotation;

		QTheme3D::ColorStyle m_colorStyle;
		QVector4D m_baseColor;
		GLuint m_baseUniformTexture;
		GLuint m_baseGradientTexture;
		QImage m_gradientImage;
		QVector4D m_singleHighlightColor;
		GLuint m_singleHighlightGradientTexture;
		QVector4D m_multiHighlightColor;
		GLuint m_multiHighlightGradientTexture;

		QString m_name;
		QString m_itemLabel;
		bool m_valid;
		bool m_visible;
		QAbstractRenderer3D *m_renderer;
		bool m_objectDirty;
		bool m_staticObjectUVDirty;
	};

}

#endif

