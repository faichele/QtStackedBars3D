TEMPLATE = lib
QT += core gui widgets printsupport
CONFIG += debug
CONFIG += plugin plugin_bundle
CONFIG += QtStackedBars3D
CONFIG -= android_install
TARGET = QtStackedBars3D

win32 {
    PY_MODULE = QtStackedBars3D.pyd
    PY_MODULE_SRC = $(DESTDIR_TARGET)

    LIBS += -L/trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/lib
} else {
    PY_MODULE = QtStackedBars3D.so

    macx {
        PY_MODULE_SRC = $(TARGET).plugin/Contents/MacOS/$(TARGET)

        QMAKE_LFLAGS += "-undefined dynamic_lookup"

        equals(QT_MAJOR_VERSION, 5) {
            equals(QT_MINOR_VERSION, 5) {
                QMAKE_RPATHDIR += $$[QT_INSTALL_LIBS]
            }
        }
    } else {
        PY_MODULE_SRC = $(TARGET)
    }
}

QMAKE_POST_LINK = $(COPY_FILE) $$PY_MODULE_SRC $$PY_MODULE

target.CONFIG = no_check_exist
target.files = $$PY_MODULE

target.path = /trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/lib/python3.5/site-packages/PyQt5
INSTALLS += target

sip.path = /trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/share/sip/PyQt5/QtStackedBars3D
sip.files = \
    ../sip/QtStackedBars3D/qabstractinputhandler3d.sip \
    ../sip/QtStackedBars3D/qsurfacedataproxy3d.sip \
    ../sip/QtStackedBars3D/qstackedbars3d.sip \
    ../sip/QtStackedBars3D/qbardataitem3d.sip \
    ../sip/QtStackedBars3D/qutils3d.sip \
    ../sip/QtStackedBars3D/qbars3d.sip \
    ../sip/QtStackedBars3D/qsurfacedataitem3d.sip \
    ../sip/QtStackedBars3D/qvalueaxis3d.sip \
    ../sip/QtStackedBars3D/qabstractaxis3d.sip \
    ../sip/QtStackedBars3D/qlist.sip \
    ../sip/QtStackedBars3D/qscene3d.sip \
    ../sip/QtStackedBars3D/qbarseries3d.sip \
    ../sip/QtStackedBars3D/qaxisvalueformatter3d.sip \
    ../sip/QtStackedBars3D/qcustomitem3d.sip \
    ../sip/QtStackedBars3D/qscatterdataproxy3d.sip \
    ../sip/QtStackedBars3D/qstackedbar3dglobal.sip \
    ../sip/QtStackedBars3D/qlight3d.sip \
    ../sip/QtStackedBars3D/qobject3d.sip \
    ../sip/QtStackedBars3D/qstackedbardataproxy3d.sip \
    ../sip/QtStackedBars3D/qsurfaceseries3d.sip \
    ../sip/QtStackedBars3D/qscatterseries3d.sip \
    ../sip/QtStackedBars3D/qinputhandler3d.sip \
    ../sip/QtStackedBars3D/qabstractdataproxy3d.sip \
    ../sip/QtStackedBars3D/qcustomlabel3d.sip \
    ../sip/QtStackedBars3D/qcategoryaxis3d.sip \
    ../sip/QtStackedBars3D/qstackedbardataitem3d.sip \
    ../sip/QtStackedBars3D/qtouchinputhandler3d.sip \
    ../sip/QtStackedBars3D/qsurface3d.sip \
    ../sip/QtStackedBars3D/QtStackedBars3Dmod.sip \
    ../sip/QtStackedBars3D/qcamera3d.sip \
    ../sip/QtStackedBars3D/qabstractgraph3d.sip \
    ../sip/QtStackedBars3D/qtheme3d.sip \
    ../sip/QtStackedBars3D/qabstractseries3d.sip \
    ../sip/QtStackedBars3D/qbardataproxy3d.sip \
    ../sip/QtStackedBars3D/qscatterdataitem3d.sip \
    ../sip/QtStackedBars3D/qstackedbarseries3d.sip
INSTALLS += sip

QMAKE_CXXFLAGS += -fno-exceptions
QMAKE_LFLAGS += -Wl,--version-script=QtStackedBars3D.exp
DEFINES += SIP_PROTECTED_IS_PUBLIC protected=public
DEFINES += QWT_PYTHON_WRAPPER
DEFINES += Py_TRACE_REFS
INCLUDEPATH += /trunet/data/volume2/src/QtStackedBars3D/python_lib/../cpp_lib/include
INCLUDEPATH += /trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/include/python3.5dm
INCLUDEPATH += /usr/include/python3.5dm
LIBS += -L/trunet/data/volume2/src/QtStackedBars3D/cpp_lib -lQtStackedBars3D
LIBS += -lpython3.5dm
macx {
    QMAKE_POST_LINK = $$QMAKE_POST_LINK$$escape_expand(\\n\\t)$$quote(install_name_tool -change libqwt_debug.6.dylib /trunet/data/volume2/src/QtStackedBars3D/cpp_lib/libqwt_debug.6.dylib $$PY_MODULE)
}

HEADERS = sipAPIQtStackedBars3D.h
SOURCES = \
    sipQtStackedBars3DQtStackedBar3DVisQAbstractAxis3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQValueAxis3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQBarDataItem3D.cpp \
    sipQtStackedBars3DQVector0100QtStackedBar3DVisQBarDataItem3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQCamera3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQAbstractDataProxy3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQBarSeries3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQStackedBarSeries3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQCustomLabel3D.cpp \
    sipQtStackedBars3DQVector0100QtStackedBar3DVisQStackedBarDataItem3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQAbstractGraph3DSelectionFlags.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQAxisValueFormatter3D.cpp \
    sipQtStackedBars3DQList0101QtStackedBar3DVisQBarSeries3D.cpp \
    sipQtStackedBars3DQList0601QVector0100QtStackedBar3DVisQStackedBarDataItem3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQInputHandler3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQTouchInputHandler3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQAbstractInputHandler3D.cpp \
    sipQtStackedBars3DQList0101QtStackedBar3DVisQAbstractInputHandler3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQLight3D.cpp \
    sipQtStackedBars3DQList0101QtStackedBar3DVisQTheme3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQAbstractGraph3DOptimizationHints.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQStackedBars3D.cpp \
    sipQtStackedBars3DQList0101QtStackedBar3DVisQCustomItem3D.cpp \
    sipQtStackedBars3DQList0100QColor.cpp \
    sipQtStackedBars3DQVector0100QtStackedBar3DVisQSurfaceDataItem3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQCategoryAxis3D.cpp \
    sipQtStackedBars3Dcmodule.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQObject3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQScene3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQCustomItem3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQAbstractGraph3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQBarDataProxy3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQSurfaceDataItem3D.cpp \
    sipQtStackedBars3DQList0601QVector0100QtStackedBar3DVisQBarDataItem3D.cpp \
    sipQtStackedBars3DQList0101QtStackedBar3DVisQStackedBarSeries3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVis.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQAbstractSeries3D.cpp \
    sipQtStackedBars3DQList0601QVector0100QtStackedBar3DVisQSurfaceDataItem3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQStackedBarDataItem3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQSurfaceDataProxy3D.cpp \
    sipQtStackedBars3DQList0101QtStackedBar3DVisQAbstractAxis3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQSurfaceSeries3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQTheme3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQSurfaceSeries3DDrawFlags.cpp \
    sipQtStackedBars3DQList0100QLinearGradient.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQBars3D.cpp \
    sipQtStackedBars3DQtStackedBar3DVisQStackedBarDataProxy3D.cpp
