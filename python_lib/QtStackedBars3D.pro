TEMPLATE = subdirs
CONFIG += ordered nostrip
SUBDIRS = QtStackedBars3D

pep484_stubs.path = /trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/lib/python3.5/site-packages/PyQt5
pep484_stubs.files = QtStackedBars3D.pyi
INSTALLS += pep484_stubs

distinfo.extra = /trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/bin/python mk_distinfo.py $(INSTALL_ROOT)/trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/lib/python3.5/site-packages/QtStackedBars3D-0.0.1.dist-info installed.txt
distinfo.path = /trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/lib/python3.5/site-packages/PyQt5
INSTALLS += distinfo
