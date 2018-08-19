#ifndef Q_ABSTRACT_3D_CONTROLLER_PRIVATE_H
#define Q_ABSTRACT_3D_CONTROLLER_PRIVATE_H

#include "QVisualizationGlobals3D.h"
#include "QAbstractController3D.h"

#include "QThemeManager3D.h"

namespace QtStackedBar3DVis
{
	class QAbstractController3DPrivate: public QObject
	{
		Q_OBJECT
	public:
		QAbstractController3DPrivate(QAbstractController3D *q);
		~QAbstractController3DPrivate();

		QThemeManager3D* themeManager() { return m_themeManager; }

	protected:
		QAbstractController3D *q_ptr;
		QThemeManager3D *m_themeManager;

	private:
		friend class QAbstractDataProxy3D;
	};


}

#endif