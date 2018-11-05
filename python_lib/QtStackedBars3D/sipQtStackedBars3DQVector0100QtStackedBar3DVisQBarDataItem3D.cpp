/*
 * Interface wrapper code.
 *
 * Generated by SIP 4.19.12
 *
 * Copyright (c) 2018 Riverbank Computing Limited <info@riverbankcomputing.com>
 * 
 * This file is part of PyQtStackedBars3D.
 * 
 * This file may be used under the terms of the GNU General Public License
 * version 3.0 as published by the Free Software Foundation and appearing in
 * the file LICENSE included in the packaging of this file.  Please review the
 * following information to ensure the GNU General Public License version 3.0
 * requirements will be met: http://www.gnu.org/copyleft/gpl.html.
 * 
 * If you do not wish to use this file under the terms of the GPL version 3.0
 * then you may purchase a commercial license.  For more information contact
 * info@riverbankcomputing.com.
 * 
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "sipAPIQtStackedBars3D.h"

#line 28 "/trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/share/sip/PyQt5/QtCore/qpycore_qvector.sip"
#include <qvector.h>
#line 29 "/trunet/data/volume2/src/QtStackedBars3D/python_lib/QtStackedBars3D/sipQtStackedBars3DQVector0100QtStackedBar3DVisQBarDataItem3D.cpp"

#line 26 "sip/QtStackedBars3D/qbars3d.sip"
#include <QBars3D.h>
#line 26 "sip/QtStackedBars3D/qcamera3d.sip"
#include <QCamera3D.h>
#line 26 "sip/QtStackedBars3D/qinputhandler3d.sip"
#include <QInputHandler3D.h>
#line 26 "sip/QtStackedBars3D/qlight3d.sip"
#include <QLight3D.h>
#line 26 "sip/QtStackedBars3D/qobject3d.sip"
#include <QObject3D.h>
#line 26 "sip/QtStackedBars3D/qscene3d.sip"
#include <QScene3D.h>
#line 26 "sip/QtStackedBars3D/qtheme3d.sip"
#include <QTheme3D.h>
#line 26 "sip/QtStackedBars3D/qabstractaxis3d.sip"
#include <QAbstractAxis3D.h>
#line 26 "sip/QtStackedBars3D/qabstractgraph3d.sip"
#include <QAbstractGraph3D.h>
#line 26 "sip/QtStackedBars3D/qabstractinputhandler3d.sip"
#include <QAbstractInputHandler3D.h>
#line 26 "sip/QtStackedBars3D/qabstractseries3d.sip"
#include <QAbstractSeries3D.h>
#line 26 "sip/QtStackedBars3D/qabstractdataproxy3d.sip"
#include <QAbstractDataProxy3D.h>
#line 26 "sip/QtStackedBars3D/qbarseries3d.sip"
#include <QBarSeries3D.h>
#line 26 "sip/QtStackedBars3D/qbardataitem3d.sip"
#include <QBarDataItem3D.h>
#line 32 "sip/QtStackedBars3D/qbardataitem3d.sip"
#include <QBarDataItem3D.h>
#line 61 "/trunet/data/volume2/src/QtStackedBars3D/python_lib/QtStackedBars3D/sipQtStackedBars3DQVector0100QtStackedBar3DVisQBarDataItem3D.cpp"


extern "C" {static void assign_QVector_0100QtStackedBar3DVis_QBarDataItem3D(void *, SIP_SSIZE_T, void *);}
static void assign_QVector_0100QtStackedBar3DVis_QBarDataItem3D(void *sipDst, SIP_SSIZE_T sipDstIdx, void *sipSrc)
{
    reinterpret_cast<QVector< ::QtStackedBar3DVis::QBarDataItem3D> *>(sipDst)[sipDstIdx] = *reinterpret_cast<QVector< ::QtStackedBar3DVis::QBarDataItem3D> *>(sipSrc);
}


extern "C" {static void *array_QVector_0100QtStackedBar3DVis_QBarDataItem3D(SIP_SSIZE_T);}
static void *array_QVector_0100QtStackedBar3DVis_QBarDataItem3D(SIP_SSIZE_T sipNrElem)
{
    return new QVector< ::QtStackedBar3DVis::QBarDataItem3D>[sipNrElem];
}


extern "C" {static void *copy_QVector_0100QtStackedBar3DVis_QBarDataItem3D(const void *, SIP_SSIZE_T);}
static void *copy_QVector_0100QtStackedBar3DVis_QBarDataItem3D(const void *sipSrc, SIP_SSIZE_T sipSrcIdx)
{
    return new QVector< ::QtStackedBar3DVis::QBarDataItem3D>(reinterpret_cast<const QVector< ::QtStackedBar3DVis::QBarDataItem3D> *>(sipSrc)[sipSrcIdx]);
}


/* Call the mapped type's destructor. */
extern "C" {static void release_QVector_0100QtStackedBar3DVis_QBarDataItem3D(void *, int);}
static void release_QVector_0100QtStackedBar3DVis_QBarDataItem3D(void *ptr, int)
{
    delete reinterpret_cast<QVector< ::QtStackedBar3DVis::QBarDataItem3D> *>(ptr);
}



extern "C" {static int convertTo_QVector_0100QtStackedBar3DVis_QBarDataItem3D(PyObject *, void **, int *, PyObject *);}
static int convertTo_QVector_0100QtStackedBar3DVis_QBarDataItem3D(PyObject *sipPy,void **sipCppPtrV,int *sipIsErr,PyObject *sipTransferObj)
{
    QVector< ::QtStackedBar3DVis::QBarDataItem3D> **sipCppPtr = reinterpret_cast<QVector< ::QtStackedBar3DVis::QBarDataItem3D> **>(sipCppPtrV);

#line 58 "/trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/share/sip/PyQt5/QtCore/qpycore_qvector.sip"
    PyObject *iter = PyObject_GetIter(sipPy);

    if (!sipIsErr)
    {
        PyErr_Clear();
        Py_XDECREF(iter);

        return (iter
#if PY_MAJOR_VERSION < 3
                && !PyString_Check(sipPy)
#endif
                && !PyUnicode_Check(sipPy));
    }

    if (!iter)
    {
        *sipIsErr = 1;

        return 0;
    }

    QVector<QtStackedBar3DVis::QBarDataItem3D> *qv = new QVector<QtStackedBar3DVis::QBarDataItem3D>;
 
    for (Py_ssize_t i = 0; ; ++i)
    {
        PyErr_Clear();
        PyObject *itm = PyIter_Next(iter);

        if (!itm)
        {
            if (PyErr_Occurred())
            {
                delete qv;
                Py_DECREF(iter);
                *sipIsErr = 1;

                return 0;
            }

            break;
        }

        int state;
        QtStackedBar3DVis::QBarDataItem3D *t = reinterpret_cast<QtStackedBar3DVis::QBarDataItem3D *>(
                sipForceConvertToType(itm, sipType_QtStackedBar3DVis_QBarDataItem3D, sipTransferObj,
                        SIP_NOT_NONE, &state, sipIsErr));

        if (*sipIsErr)
        {
            PyErr_Format(PyExc_TypeError,
                    "index %zd has type '%s' but 'QtStackedBar3DVis::QBarDataItem3D' is expected", i,
                    sipPyTypeName(Py_TYPE(itm)));

            Py_DECREF(itm);
            delete qv;
            Py_DECREF(iter);

            return 0;
        }

        qv->append(*t);

        sipReleaseType(t, sipType_QtStackedBar3DVis_QBarDataItem3D, state);
        Py_DECREF(itm);
    }

    Py_DECREF(iter);
 
    *sipCppPtr = qv;
 
    return sipGetState(sipTransferObj);
#line 171 "/trunet/data/volume2/src/QtStackedBars3D/python_lib/QtStackedBars3D/sipQtStackedBars3DQVector0100QtStackedBar3DVisQBarDataItem3D.cpp"
}


extern "C" {static PyObject *convertFrom_QVector_0100QtStackedBar3DVis_QBarDataItem3D(void *, PyObject *);}
static PyObject *convertFrom_QVector_0100QtStackedBar3DVis_QBarDataItem3D(void *sipCppV, PyObject *sipTransferObj)
{
   QVector< ::QtStackedBar3DVis::QBarDataItem3D> *sipCpp = reinterpret_cast<QVector< ::QtStackedBar3DVis::QBarDataItem3D> *>(sipCppV);

#line 32 "/trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/share/sip/PyQt5/QtCore/qpycore_qvector.sip"
    PyObject *l = PyList_New(sipCpp->size());

    if (!l)
        return 0;

    for (int i = 0; i < sipCpp->size(); ++i)
    {
        QtStackedBar3DVis::QBarDataItem3D *t = new QtStackedBar3DVis::QBarDataItem3D(sipCpp->at(i));
        PyObject *tobj = sipConvertFromNewType(t, sipType_QtStackedBar3DVis_QBarDataItem3D,
                sipTransferObj);

        if (!tobj)
        {
            delete t;
            Py_DECREF(l);

            return 0;
        }

        PyList_SetItem(l, i, tobj);
    }

    return l;
#line 204 "/trunet/data/volume2/src/QtStackedBars3D/python_lib/QtStackedBars3D/sipQtStackedBars3DQVector0100QtStackedBar3DVisQBarDataItem3D.cpp"
}


sipMappedTypeDef sipTypeDef_QtStackedBars3D_QVector_0100QtStackedBar3DVis_QBarDataItem3D = {
    {
        -1,
        0,
        0,
        SIP_TYPE_MAPPED,
        sipNameNr_902,     /* QVector<QtStackedBar3DVis::QBarDataItem3D> */
        {0},
        0
    },
    {
        -1,
        {0, 0, 1},
        0, 0,
        0, 0,
        0, 0,
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    },
    assign_QVector_0100QtStackedBar3DVis_QBarDataItem3D,
    array_QVector_0100QtStackedBar3DVis_QBarDataItem3D,
    copy_QVector_0100QtStackedBar3DVis_QBarDataItem3D,
    release_QVector_0100QtStackedBar3DVis_QBarDataItem3D,
    convertTo_QVector_0100QtStackedBar3DVis_QBarDataItem3D,
    convertFrom_QVector_0100QtStackedBar3DVis_QBarDataItem3D
};
