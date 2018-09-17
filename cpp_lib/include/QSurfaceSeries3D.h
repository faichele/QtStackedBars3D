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

#ifndef QSURFACE3DSERIES_H
#define QSURFACE3DSERIES_H

#include "QAbstractSeries3D.h"
#include "QSurfaceDataProxy3D.h"

namespace QtStackedBar3DVis
{

	class QSurfaceSeries3DPrivate;

	class QT_STACKEDBAR3D_EXPORT QSurfaceSeries3D : public QAbstractSeries3D
	{
		Q_OBJECT
			Q_FLAGS(DrawFlag DrawFlags)
			Q_PROPERTY(QSurfaceDataProxy3D *dataProxy READ dataProxy WRITE setDataProxy NOTIFY dataProxyChanged)
			Q_PROPERTY(QPoint selectedPoint READ selectedPoint WRITE setSelectedPoint NOTIFY selectedPointChanged)
			Q_PROPERTY(bool flatShadingEnabled READ isFlatShadingEnabled WRITE setFlatShadingEnabled NOTIFY flatShadingEnabledChanged)
			Q_PROPERTY(bool flatShadingSupported READ isFlatShadingSupported NOTIFY flatShadingSupportedChanged)
			Q_PROPERTY(DrawFlags drawMode READ drawMode WRITE setDrawMode NOTIFY drawModeChanged)
			Q_PROPERTY(QImage texture READ texture WRITE setTexture NOTIFY textureChanged)
			Q_PROPERTY(QString textureFile READ textureFile WRITE setTextureFile NOTIFY textureFileChanged)

	public:
		enum DrawFlag {
			DrawWireframe = 1,
			DrawSurface = 2,
			DrawSurfaceAndWireframe = DrawWireframe | DrawSurface
		};
		Q_DECLARE_FLAGS(DrawFlags, DrawFlag)

		explicit QSurfaceSeries3D(QObject *parent = Q_NULLPTR);
		explicit QSurfaceSeries3D(QSurfaceDataProxy3D *dataProxy, QObject *parent = Q_NULLPTR);
		virtual ~QSurfaceSeries3D();

		void setDataProxy(QSurfaceDataProxy3D *proxy);
		QSurfaceDataProxy3D *dataProxy() const;

		void setSelectedPoint(const QPoint &position);
		QPoint selectedPoint() const;
		static QPoint invalidSelectionPosition();

		void setFlatShadingEnabled(bool enabled);
		bool isFlatShadingEnabled() const;

		void setDrawMode(DrawFlags mode);
		QSurfaceSeries3D::DrawFlags drawMode() const;

		bool isFlatShadingSupported() const;

		void setTexture(const QImage &texture);
		QImage texture() const;
		void setTextureFile(const QString &filename);
		QString textureFile() const;

	Q_SIGNALS:
		void dataProxyChanged(QSurfaceDataProxy3D *proxy);
		void selectedPointChanged(const QPoint &position);
		void flatShadingEnabledChanged(bool enable);
		void flatShadingSupportedChanged(bool enable);
		void drawModeChanged(QSurfaceSeries3D::DrawFlags mode);
		void textureChanged(const QImage &image);
		void textureFileChanged(const QString &filename);

	protected:
		explicit QSurfaceSeries3D(QSurfaceSeries3DPrivate *d, QObject *parent = Q_NULLPTR);
		QSurfaceSeries3DPrivate *dptr();
		const QSurfaceSeries3DPrivate *dptrc() const;

	private:
		Q_DISABLE_COPY(QSurfaceSeries3D)

			friend class QSurfaceController3D;
	};

}

#endif
