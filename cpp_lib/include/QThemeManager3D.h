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

#ifndef Q_THEME_MANAGER_3D_H
#define Q_THEME_MANAGER_3D_H

#include "QVisualizationGlobals3D.h"
#include "QTheme3D.h"
#include "QAbstractController3D.h"

namespace QtStackedBar3DVis
{

	class QT_STACKEDBAR3D_EXPORT QThemeManager3D : public QObject
	{
		Q_OBJECT
		public:
			QThemeManager3D(QAbstractController3D *controller);
			~QThemeManager3D();

			void addTheme(QTheme3D *theme);
			void releaseTheme(QTheme3D *theme);
			void setActiveTheme(QTheme3D *theme);
			QTheme3D *activeTheme() const;
			QList<QTheme3D *> themes() const;

			static void setPredefinedPropertiesToTheme(QTheme3D *theme, QTheme3D::Theme type);

		protected:
			void connectThemeSignals();
			static QLinearGradient createGradient(const QColor &color, float colorLevel);
			static void setBaseColors(QTheme3D *theme, const QList<QColor> &colors);
			static void setBackgroundColor(QTheme3D *theme, const QColor &color);
			static void setWindowColor(QTheme3D *theme, const QColor &color);
			static void setTextColor(QTheme3D *theme, const QColor &color);
			static void setTextBackgroundColor(QTheme3D *theme, const QColor &color);
			static void setGridLineColor(QTheme3D *theme, const QColor &color);
			static void setSingleHighlightColor(QTheme3D *theme, const QColor &color);
			static void setMultiHighlightColor(QTheme3D *theme, const QColor &color);
			static void setLightColor(QTheme3D *theme, const QColor &color);
			static void setBaseGradients(QTheme3D *theme, const QList<QLinearGradient> &gradients);
			static void setSingleHighlightGradient(QTheme3D *theme, const QLinearGradient &gradient);
			static void setMultiHighlightGradient(QTheme3D *theme, const QLinearGradient &gradient);
			static void setLightStrength(QTheme3D *theme, float strength);
			static void setAmbientLightStrength(QTheme3D *theme, float strength);
			static void setHighlightLightStrength(QTheme3D *theme, float strength);
			static void setLabelBorderEnabled(QTheme3D *theme, bool enabled);
			static void setFont(QTheme3D *theme, const QFont &font);
			static void setBackgroundEnabled(QTheme3D *theme, bool enabled);
			static void setGridEnabled(QTheme3D *theme, bool enabled);
			static void setLabelBackgroundEnabled(QTheme3D *theme, bool enabled);
			static void setColorStyle(QTheme3D *theme, QTheme3D::ColorStyle style);

		private:
			QTheme3D *m_activeTheme;
			QList<QTheme3D *> m_themes; // List of all added themes
			QAbstractController3D *m_controller;
	};
}

#endif
