#ifndef DATAVISUALIZATIONGLOBAL_P_H
#define DATAVISUALIZATIONGLOBAL_P_H

#include <QtGui/QOpenGLFunctions>
#include <QtGui/QVector2D>
#include <QtGui/QVector3D>
#include <QtGui/QQuaternion>
#include <QtCore/QDebug>

#ifndef QT_STATIC
#  if defined(QT_BUILD_STACKEDBAR3D_LIB)
#    define QT_STACKEDBAR3D_EXPORT Q_DECL_EXPORT
#  else
#    define QT_STACKEDBAR3D_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define QT_STACKEDBAR3D_EXPORT
#endif

namespace QtStackedBar3DVis
{
	// Constants used in several files
	// Distance from camera to origin
	static const GLfloat cameraDistance = 6.0f;
	// Size of font to be used in label texture rendering. Doesn't affect the actual font size.
	static const int textureFontSize = 50;
	static const GLfloat defaultRatio = 1.0f / 1.6f; // default aspect ratio 16:10
#if !(defined QT_OPENGL_ES)
	static const float gridLineOffset = 0.0001f; // Offset for lifting grid lines off background
#else
	static const float gridLineOffset = 0.0035f; // Offset for lifting grid lines off background
#endif
											 // Default light position. To have shadows working correctly, light should be as far as camera, or a bit further
											 // y position is added to the minimum height (or can be thought to be that much above or below the camera)
	static const QVector3D defaultLightPos = QVector3D(0.0f, 0.5f, 0.0f);
	static const QVector3D zeroVector = QVector3D(0.0f, 0.0f, 0.0f);
	static const QVector3D oneVector = QVector3D(1.0f, 1.0f, 1.0f);
	static const QVector3D upVector = QVector3D(0.0f, 1.0f, 0.0f);
	static const QVector3D cameraDistanceVector = QVector3D(0.0f, 0.0f, cameraDistance);
	static const QQuaternion identityQuaternion;

	// Skip color == selection texture's background color
	static const QVector4D selectionSkipColor = QVector4D(255.0f, 255.0f, 255.0f, 255.0f);
	static const QVector4D invalidColorVector = QVector4D(-1.0f, -1.0f, -1.0f, -1.0f);
	static const GLfloat itemAlpha = 0.0f;
	static const GLfloat customItemAlpha = 252.0f;
	static const GLfloat labelValueAlpha = 253.0f;
	static const GLfloat labelRowAlpha = 254.0f;
	static const GLfloat labelColumnAlpha = 255.0f;
	static const GLfloat gradientTextureHeight = 1024.0f;
	static const GLfloat gradientTextureWidth = 2.0f;
	static const GLfloat uniformTextureHeight = 64.0f;
	static const GLfloat uniformTextureWidth = 2.0f;
	static const GLfloat labelMargin = 0.05f;
	static const GLfloat gridLineWidth = 0.005f;
}

#endif


