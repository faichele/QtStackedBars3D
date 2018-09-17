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

#ifndef Q_OBJECT_HELPER_3D_H
#define Q_OBJECT_HELPER_3D_H

#include "QVisualizationGlobals3D.h"
#include "QAbstractObjectHelper3D.h"

namespace QtStackedBar3DVis
{
	class QAbstractRenderer3D;

	class QT_STACKEDBAR3D_EXPORT QObjectHelper3D : public QAbstractObjectHelper3D
	{
	private:
		QObjectHelper3D(const QString &objectFile);
	public:
		virtual ~QObjectHelper3D();

		static void resetObjectHelper(const QAbstractRenderer3D *cacheId, QObjectHelper3D *&obj,
			const QString &meshFile);
		static void releaseObjectHelper(const QAbstractRenderer3D *cacheId, QObjectHelper3D *&obj);
		inline const QString &objectFile() { return m_objectFile; }

		inline const QVector<GLuint> &indices() const { return m_indices; }
		inline const QVector<QVector3D> &indexedvertices() const { return m_indexedVertices; }
		inline const QVector<QVector2D> &indexedUVs() const { return m_indexedUVs; }
		inline const QVector<QVector3D> &indexedNormals() const { return m_indexedNormals; }

	private:
		static QObjectHelper3D *getObjectHelper(const QAbstractRenderer3D *cacheId,
			const QString &objectFile);
		void load();

		QString m_objectFile;
		QVector<GLuint> m_indices;
		QVector<QVector3D> m_indexedVertices;
		QVector<QVector2D> m_indexedUVs;
		QVector<QVector3D> m_indexedNormals;
	};

}

#endif
