TEMPLATE = subdirs
CONFIG += ordered nostrip
SUBDIRS = QtStackedBars3D

pep484_stubs.path = /usr/lib/python3.6/site-packages/PyQt5
pep484_stubs.files = QtStackedBars3D.pyi
INSTALLS += pep484_stubs

distinfo.extra = /usr/bin/python3.6 mk_distinfo.py $(INSTALL_ROOT)/usr/lib/python3.6/site-packages/QtStackedBars3D-0.0.1.dist-info installed.txt
distinfo.path = /usr/lib/python3.6/site-packages/PyQt5
INSTALLS += distinfo
