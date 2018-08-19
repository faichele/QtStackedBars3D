#ifndef Q_UTILS_3D_H
#define Q_UTILS_3D_H

#include "QVisualizationGlobals3D.h"

QT_FORWARD_DECLARE_CLASS(QLinearGradient)

namespace QtStackedBar3DVis
{
	class QT_STACKEDBAR3D_EXPORT QUtils3D
	{
	public:
		enum ParamType {
			ParamTypeUnknown = 0,
			ParamTypeInt,
			ParamTypeUInt,
			ParamTypeReal
		};

		static GLuint getNearestPowerOfTwo(GLuint value);
		static QVector4D vectorFromColor(const QColor &color);
		static QColor colorFromVector(const QVector3D &colorVector);
		static QColor colorFromVector(const QVector4D &colorVector);
		static QImage printTextToImage(const QFont &font,
			const QString &text,
			const QColor &bgrColor,
			const QColor &txtColor,
			bool labelBackground,
			bool borders = false,
			int maxLabelWidth = 0);
		static QVector4D getSelection(QPoint mousepos, int height);
		static QImage getGradientImage(QLinearGradient &gradient);

		static ParamType preParseFormat(const QString &format, QString &preStr, QString &postStr,
			int &precision, char &formatSpec);
		static QString formatLabelSprintf(const QByteArray &format, ParamType paramType, qreal value);
		static QString formatLabelLocalized(ParamType paramType, qreal value,
			const QLocale &locale, const QString &preStr, const QString &postStr,
			int precision, char formatSpec, const QByteArray &format);
		static QString defaultLabelFormat();

		static float wrapValue(float value, float min, float max);
		static QQuaternion calculateRotation(const QVector3D &xyzRotations);
		static bool isOpenGLES();
		static void resolveStatics();

	private:
		static ParamType mapFormatCharToParamType(char formatSpec);
	};
}

#endif

