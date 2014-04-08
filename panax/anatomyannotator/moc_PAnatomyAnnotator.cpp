/****************************************************************************
** Meta object code from reading C++ file 'PAnatomyAnnotator.h'
**
** Created: Tue Mar 25 14:05:52 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "PAnatomyAnnotator.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PAnatomyAnnotator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PAnatomyAnnotator[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x09,
      35,   18,   18,   18, 0x09,
      48,   18,   18,   18, 0x09,
      67,   62,   18,   18, 0x09,
     116,  101,   18,   18, 0x09,
     155,  101,   18,   18, 0x09,
     194,   18,   18,   18, 0x09,
     207,   18,   18,   18, 0x09,
     221,   18,   18,   18, 0x09,
     235,   18,   18,   18, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_PAnatomyAnnotator[] = {
    "PAnatomyAnnotator\0\0showAnnotator()\0"
    "showLesson()\0hideAnatomy()\0item\0"
    "setLabelAnchor(QTableWidgetItem*)\0"
    "parent,row,col\0insertAnatomyInfo(QModelIndex,int,int)\0"
    "removeAnatomyInfo(QModelIndex,int,int)\0"
    "newProject()\0openProject()\0saveProject()\0"
    "saveProjectAs()\0"
};

void PAnatomyAnnotator::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PAnatomyAnnotator *_t = static_cast<PAnatomyAnnotator *>(_o);
        switch (_id) {
        case 0: _t->showAnnotator(); break;
        case 1: _t->showLesson(); break;
        case 2: _t->hideAnatomy(); break;
        case 3: _t->setLabelAnchor((*reinterpret_cast< QTableWidgetItem*(*)>(_a[1]))); break;
        case 4: _t->insertAnatomyInfo((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 5: _t->removeAnatomyInfo((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 6: _t->newProject(); break;
        case 7: _t->openProject(); break;
        case 8: _t->saveProject(); break;
        case 9: _t->saveProjectAs(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PAnatomyAnnotator::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PAnatomyAnnotator::staticMetaObject = {
    { &PMeshViewer::staticMetaObject, qt_meta_stringdata_PAnatomyAnnotator,
      qt_meta_data_PAnatomyAnnotator, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PAnatomyAnnotator::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PAnatomyAnnotator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PAnatomyAnnotator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PAnatomyAnnotator))
        return static_cast<void*>(const_cast< PAnatomyAnnotator*>(this));
    return PMeshViewer::qt_metacast(_clname);
}

int PAnatomyAnnotator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = PMeshViewer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
