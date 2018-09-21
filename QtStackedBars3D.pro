TEMPLATE = subdirs
SUBDIRS = cpp_lib cpp_example
cpp_example.depends = cpp_lib

include(./cpp_lib/QtStackedBars3D.pri)
