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

#line 149 "/trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/share/sip/PyQt5/QtCore/qpycore_qlist.sip"
#include <qlist.h>
#line 29 "/trunet/data/volume2/src/QtStackedBars3D/python_lib/QtStackedBars3D/sipQtStackedBars3DQList0101QtStackedBar3DVisQCustomItem3D.cpp"

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
#line 26 "sip/QtStackedBars3D/qbardataproxy3d.sip"
#include <QBarDataProxy3D.h>
#line 26 "sip/QtStackedBars3D/qcategoryaxis3d.sip"
#include <QCategoryAxis3D.h>
#line 26 "sip/QtStackedBars3D/qcustomitem3d.sip"
#include <QCustomItem3D.h>
#line 32 "sip/QtStackedBars3D/qcustomitem3d.sip"
#include <QCustomItem3D.h>
#line 67 "/trunet/data/volume2/src/QtStackedBars3D/python_lib/QtStackedBars3D/sipQtStackedBars3DQList0101QtStackedBar3DVisQCustomItem3D.cpp"


extern "C" {static void assign_QList_0101QtStackedBar3DVis_QCustomItem3D(void *, SIP_SSIZE_T, void *);}
static void assign_QList_0101QtStackedBar3DVis_QCustomItem3D(void *sipDst, SIP_SSIZE_T sipDstIdx, void *sipSrc)
{
    reinterpret_cast<QList< ::QtStackedBar3DVis::QCustomItem3D*> *>(sipDst)[sipDstIdx] = *reinterpret_cast<QList< ::QtStackedBar3DVis::QCustomItem3D*> *>(sipSrc);
}


extern "C" {static void *array_QList_0101QtStackedBar3DVis_QCustomItem3D(SIP_SSIZE_T);}
static void *array_QList_0101QtStackedBar3DVis_QCustomItem3D(SIP_SSIZE_T sipNrElem)
{
    return new QList< ::QtStackedBar3DVis::QCustomItem3D*>[sipNrElem];
}


extern "C" {static void *copy_QList_0101QtStackedBar3DVis_QCustomItem3D(const void *, SIP_SSIZE_T);}
static void *copy_QList_0101QtStackedBar3DVis_QCustomItem3D(const void *sipSrc, SIP_SSIZE_T sipSrcIdx)
{
    return new QList< ::QtStackedBar3DVis::QCustomItem3D*>(reinterpret_cast<const QList< ::QtStackedBar3DVis::QCustomItem3D*> *>(sipSrc)[sipSrcIdx]);
}


/* Call the mapped type's destructor. */
extern "C" {static void release_QList_0101QtStackedBar3DVis_QCustomItem3D(void *, int);}
static void release_QList_0101QtStackedBar3DVis_QCustomItem3D(void *ptr, int)
{
    delete reinterpret_cast<QList< ::QtStackedBar3DVis::QCustomItem3D*> *>(ptr);
}



extern "C" {static int convertTo_QList_0101QtStackedBar3DVis_QCustomItem3D(PyObject *, void **, int *, PyObject *);}
static int convertTo_QList_0101QtStackedBar3DVis_QCustomItem3D(PyObject *sipPy,void **sipCppPtrV,int *sipIsErr,PyObject *sipTransferObj)
{
    QList< ::QtStackedBar3DVis::QCustomItem3D*> **sipCppPtr = reinterpret_cast<QList< ::QtStackedBar3DVis::QCustomItem3D*> **>(sipCppPtrV);

#line 184 "/trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/share/sip/PyQt5/QtCore/qpycore_qlist.sip"
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

    QList<QtStackedBar3DVis::QCustomItem3D *> *ql = new QList<QtStackedBar3DVis::QCustomItem3D *>;
 
    for (Py_ssize_t i = 0; ; ++i)
    {
        PyErr_Clear();
        PyObject *itm = PyIter_Next(iter);

        if (!itm)
        {
            if (PyErr_Occurred())
            {
                delete ql;
                Py_DECREF(iter);
                *sipIsErr = 1;

                return 0;
            }

            break;
        }

        QtStackedBar3DVis::QCustomItem3D *t = reinterpret_cast<QtStackedBar3DVis::QCustomItem3D *>(
                sipForceConvertToType(itm, sipType_QtStackedBar3DVis_QCustomItem3D, sipTransferObj, 0,
                        0, sipIsErr));
 
        if (*sipIsErr)
        {
            PyErr_Format(PyExc_TypeError,
                    "index %zd has type '%s' but 'QtStackedBar3DVis::QCustomItem3D' is expected", i,
                    sipPyTypeName(Py_TYPE(itm)));

            Py_DECREF(itm);
            delete ql;
            Py_DECREF(iter);

            return 0;
        }

        ql->append(t);

        Py_DECREF(itm);
    }

    Py_DECREF(iter);
 
    *sipCppPtr = ql;
 
    return sipGetState(sipTransferObj);
#line 175 "/trunet/data/volume2/src/QtStackedBars3D/python_lib/QtStackedBars3D/sipQtStackedBars3DQList0101QtStackedBar3DVisQCustomItem3D.cpp"
}


extern "C" {static PyObject *convertFrom_QList_0101QtStackedBar3DVis_QCustomItem3D(void *, PyObject *);}
static PyObject *convertFrom_QList_0101QtStackedBar3DVis_QCustomItem3D(void *sipCppV, PyObject *sipTransferObj)
{
   QList< ::QtStackedBar3DVis::QCustomItem3D*> *sipCpp = reinterpret_cast<QList< ::QtStackedBar3DVis::QCustomItem3D*> *>(sipCppV);

#line 153 "/trunet/data/volume2/virtualenvs/zyklio_python_packages_dbg/share/sip/PyQt5/QtCore/qpycore_qlist.sip"
    int gc_enabled = sipEnableGC(0);
    PyObject *l = PyList_New(sipCpp->size());

    if (l)
    {
        for (int i = 0; i < sipCpp->size(); ++i)
        {
            QtStackedBar3DVis::QCustomItem3D *t = sipCpp->at(i);

            // The explicit (void *) cast allows QtStackedBar3DVis::QCustomItem3D to be const.
            PyObject *tobj = sipConvertFromType((void *)t, sipType_QtStackedBar3DVis_QCustomItem3D,
                    sipTransferObj);

            if (!tobj)
            {
                Py_DECREF(l);
                l = 0;

                break;
            }

            PyList_SetItem(l, i, tobj);
        }
    }

    sipEnableGC(gc_enabled);

    return l;
#line 213 "/trunet/data/volume2/src/QtStackedBars3D/python_lib/QtStackedBars3D/sipQtStackedBars3DQList0101QtStackedBar3DVisQCustomItem3D.cpp"
}


sipMappedTypeDef sipTypeDef_QtStackedBars3D_QList_0101QtStackedBar3DVis_QCustomItem3D = {
    {
        -1,
        0,
        0,
        SIP_TYPE_MAPPED,
        sipNameNr_1074,     /* QList<QtStackedBar3DVis::QCustomItem3D*> */
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
    assign_QList_0101QtStackedBar3DVis_QCustomItem3D,
    array_QList_0101QtStackedBar3DVis_QCustomItem3D,
    copy_QList_0101QtStackedBar3DVis_QCustomItem3D,
    release_QList_0101QtStackedBar3DVis_QCustomItem3D,
    convertTo_QList_0101QtStackedBar3DVis_QCustomItem3D,
    convertFrom_QList_0101QtStackedBar3DVis_QCustomItem3D
};
