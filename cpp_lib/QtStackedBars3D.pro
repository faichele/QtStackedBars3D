QT     += widgets
TARGET = QtStackedBars3D
TEMPLATE = lib

DESTDIR = $${PWD}/../bin

CONFIG += c++11

win32 {
        CONFIG += staticlib
}

DEFINES += QT_BUILD_STACKEDBAR3D_LIB
INCLUDEPATH += $(PWD)/include

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES     = qstackedbars.qrc

HEADERS += \
                           include/QAbstractAxis3D.h \
                           include/QAbstractAxis3DPrivate.h \
                           include/QAbstractController3D.h \
                           include/QAbstractController3DPrivate.h \
                           include/QAbstractDataProxy3D.h \
                           include/QAbstractDataProxy3DPrivate.h \
                           include/QAbstractGraph3D.h \
                           include/QAbstractGraph3DPrivate.h \
                           include/QAbstractInputHandler3D.h \
                           include/QAbstractInputHandler3DPrivate.h \
                           include/QAbstractObjectHelper3D.h \
                           include/QAbstractRenderItem3D.h \
                           include/QAbstractRenderer3D.h \
                           include/QAbstractSeries3D.h \
                           include/QAbstractSeries3DPrivate.h \
                           include/QAxisValueFormatter3D.h \
                           include/QAxisValueFormatter3DPrivate.h \
                           include/QAxisRenderCache3D.h \
                           include/QBars3D.h \
                           include/QBars3DPrivate.h \
                           include/QBarsController3D.h \
                           include/QBarDataItem3D.h \
                           include/QBarDataItem3DPrivate.h \
                           include/QBarDataProxy3D.h \
                           include/QBarDataProxy3DPrivate.h \
                           include/QBarsRenderer3D.h \
                           include/QBarRenderItem3D.h \
                           include/QBarSeries3D.h \
                           include/QBarSeries3DPrivate.h \
                           include/QBarSeriesRenderCache3D.h \
                           include/QCamera3D.h \
                           include/QCamera3DPrivate.h \
                           include/QCategoryAxis3D.h \
                           include/QCategoryAxis3DPrivate.h \
                           include/QCustomItem3D.h \
                           include/QCustomItem3DPrivate.h \
                           include/QCustomLabel3D.h \
                           include/QCustomLabel3DPrivate.h \
                           include/QCustomRenderItem3D.h \
                           include/QCustomVolume3D.h \
                           include/QCustomVolume3DPrivate.h \
                           include/QDrawer3D.h \
                           include/QInputHandler3D.h \
                           include/QInputHandler3DPrivate.h \
                           include/QLabelItem3D.h \
                           include/QLight3D.h \
                           include/QLight3DPrivate.h \
                           include/QMeshLoader3D.h \
                           include/QObject3D.h \
                           include/QObject3DPrivate.h \
                           include/QObjectHelper3D.h \
                           include/QScatterController3D.h \
                           include/QScatterDataItem3D.h \
                           include/QScatterDataItem3DPrivate.h \
                           include/QScatterDataProxy3D.h \
                           include/QScatterDataProxy3DPrivate.h \
                           include/QScatterObjectBufferHelper3D.h \
                           include/QScatterPointBufferHelper3D.h \
                           include/QScatterRenderer3D.h \
                           include/QScatterRenderItem3D.h \
                           include/QScatterSeries3D.h \
                           include/QScatterSeries3DPrivate.h \
                           include/QScatterSeriesRenderCache3D.h \
                           include/QScene3D.h \
                           include/QScene3DPrivate.h \
                           include/QSelectionPointer3D.h \
                           include/QSeriesRenderCache3D.h \
                           include/QShaderHelper3D.h \
                           include/QStackedBars3D.h \
                           include/QStackedBars3DPrivate.h \
                           include/QStackedBarsController3D.h \
                           include/QStackedBarDataItem3D.h \
                           include/QStackedBarDataItem3DPrivate.h \
                           include/QStackedBarDataProxy3D.h \
                           include/QStackedBarDataProxy3DPrivate.h \
                           include/QStackedBarRenderItem3D.h \
                           include/QStackedBarRenderer3D.h \
                           include/QStackedBarSeries3D.h \
                           include/QStackedBarSeries3DPrivate.h \
                           include/QStackedBarSeriesRenderCache3D.h \
                           include/QSurfaceController3D.h \
                           include/QSurfaceDataItem3D.h \
                           include/QSurfaceDataItem3DPrivate.h \
                           include/QSurfaceDataProxy3D.h \
                           include/QSurfaceDataProxy3DPrivate.h \
                           include/QSurfaceObject3D.h \
                           include/QSurfaceRenderer3D.h \
                           include/QSurfaceSeries3D.h \
                           include/QSurfaceSeries3DPrivate.h \
                           include/QSurfaceSeriesRenderCache3D.h \
                           include/QTextureHelper3D.h \
                           include/QTheme3D.h \
                           include/QTheme3DPrivate.h \
                           include/QThemeManager3D.h \
                           include/QTouchInputHandler3D.h \
                           include/QTouchInputHandler3DPrivate.h \
                           include/QUtils3D.h \
                           include/QUtilsGeneric.h \
                           include/QValueAxis3D.h \
                           include/QValueAxis3DPrivate.h \
                           include/QVertexIndexer3D.h \
                           include/QVisualizationGlobals3D.h

SOURCES += \
                        src/QAbstractAxis3D.cpp \
                        src/QAbstractController3D.cpp \
                        src/QAbstractDataProxy3D.cpp \
                        src/QAbstractGraph3D.cpp \
                        src/QAbstractInputHandler3D.cpp \
                        src/QAbstractObjectHelper3D.cpp \
                        src/QAbstractRenderer3D.cpp \
                        src/QAbstractRenderItem3D.cpp \
                        src/QAbstractSeries3D.cpp \
                        src/QAxisRenderCache3D.cpp \
                        src/QAxisValueFormatter3D.cpp \
                        src/QBars3D.cpp \
                        src/QBarDataItem3D.cpp \
                        src/QBarDataProxy3D.cpp \
                        src/QBarsController3D.cpp \
                        src/QBarsRenderer3D.cpp \
                        src/QBarRenderItem3D.cpp \
                        src/QBarSeries3D.cpp \
                        src/QBarSeriesRenderCache3D.cpp \
                        src/QCamera3D.cpp \
                        src/QCategoryAxis3D.cpp \
                        src/QCustomItem3D.cpp \
                        src/QCustomLabel3D.cpp \
                        src/QCustomRenderItem3D.cpp \
                        src/QCustomVolume3D.cpp \
                        src/QDrawer3D.cpp \
                        src/QInputHandler3D.cpp \
                        src/QLabelItem3D.cpp \
                        src/QLight3D.cpp \
                        src/QMeshLoader3D.cpp \
                        src/QObject3D.cpp \
                        src/QObjectHelper3D.cpp \
                        src/QScatterController3D.cpp \
                        src/QScatterDataItem3D.cpp \
                        src/QScatterDataProxy3D.cpp \
                        src/QScatterObjectBufferHelper3D.cpp \
                        src/QScatterPointBufferHelper3D.cpp \
                        src/QScatterRenderItem3D.cpp \
                        src/QScatterRenderer3D.cpp \
                        src/QScatterSeries3D.cpp \
                        src/QScatterSeriesRenderCache3D.cpp \
                        src/QScene3D.cpp \
                        src/QSelectionPointer3D.cpp \
                        src/QSeriesRenderCache3D.cpp \
                        src/QShaderHelper3D.cpp \
                        src/QStackedBars3D.cpp \
                        src/QStackedBarsController3D.cpp \
                        src/QStackedBarDataItem3D.cpp \
                        src/QStackedBarDataProxy3D.cpp \
                        src/QStackedBarRenderer3D.cpp \
                        src/QStackedBarRenderItem3D.cpp \
                        src/QStackedBarSeries3D.cpp \
                        src/QStackedBarSeriesRenderCache3D.cpp \
                        src/QSurfaceController3D.cpp \
                        src/QSurfaceDataItem3D.cpp \
                        src/QSurfaceDataProxy3D.cpp \
                        src/QSurfaceObject3D.cpp \
                        src/QSurfaceRenderer3D.cpp \
                        src/QSurfaceSeries3D.cpp \
                        src/QSurfaceSeriesRenderCache3D.cpp \
                        src/QTextureHelper3D.cpp \
                        src/QTheme3D.cpp \
                        src/QThemeManager3D.cpp \
                        src/QTouchInputHandler3D.cpp \
                        src/QUtils3D.cpp \
                        src/QValueAxis3D.cpp \
                        src/QVertexIndexer3D.cpp

