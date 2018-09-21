TEMPLATE = app
HEADERS += GraphModifierBase.h GraphModifier.h GraphModifierStacked.h
SOURCES += GraphModifierBase.cpp GraphModifier.cpp GraphModifierStacked.cpp main.cpp
QT += widgets gui
DESTDIR = $${PWD}/../bin

INCLUDEPATH += "$${PWD}/../cpp_lib/include" "$${PWD}/../cpp_lib"
LIBS += -L"$${PWD}/../bin" -lQtStackedBars3D
