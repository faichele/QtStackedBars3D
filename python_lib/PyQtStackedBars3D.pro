TEMPLATE = subdirs
CONFIG += ordered nostrip
SUBDIRS = QtStackedBars3D

pep484_stubs.path = /usr/lib/python3.6/site-packages/PyQt5
pep484_stubs.files = QtStackedBars3D.pyi
INSTALLS += pep484_stubs

api.path = /usr/share/qt5/qsci/api/python
api.files = PyQtStackedBars3D.api
INSTALLS += api

distinfo.extra = /usr/bin/python3 mk_distinfo.py $(INSTALL_ROOT)/usr/lib/python3.6/site-packages/PyQtStackedBars3D-5.11.2.dist-info installed.txt
distinfo.path = /usr/lib/python3.6/site-packages/PyQt5
INSTALLS += distinfo
