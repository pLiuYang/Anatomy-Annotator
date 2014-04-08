/****************************************************************************
** Meta object code from reading C++ file 'PMeshViewer.h'
**
** Created: Wed Feb 19 00:32:10 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "PMeshViewer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PMeshViewer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PMeshViewer[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x09,
      26,   12,   12,   12, 0x09,
      40,   12,   12,   12, 0x09,
      54,   12,   12,   12, 0x09,
      70,   12,   12,   12, 0x09,
      80,   12,   12,   12, 0x09,
      91,   12,   12,   12, 0x09,
     101,   12,   12,   12, 0x09,
     114,   12,   12,   12, 0x09,
     127,   12,   12,   12, 0x09,
     140,   12,   12,   12, 0x09,
     151,   12,   12,   12, 0x09,
     169,   12,   12,   12, 0x09,
     186,   12,   12,   12, 0x09,
     205,   12,   12,   12, 0x09,
     222,   12,   12,   12, 0x09,
     237,   12,   12,   12, 0x09,
     252,   12,   12,   12, 0x09,
     259,   12,   12,   12, 0x09,
     266,   12,   12,   12, 0x09,
     282,  274,   12,   12, 0x09,
     305,  274,   12,   12, 0x09,
     323,  274,   12,   12, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_PMeshViewer[] = {
    "PMeshViewer\0\0newProject()\0openProject()\0"
    "saveProject()\0saveProjectAs()\0loadDir()\0"
    "loadMesh()\0addMesh()\0deleteMesh()\0"
    "saveDirPly()\0saveDirStl()\0saveView()\0"
    "toggleFrontFace()\0setMeshMode(int)\0"
    "setSmoothSurface()\0setFlatSurface()\0"
    "setFlatLines()\0setWireFrame()\0info()\0"
    "help()\0about()\0row,col\0setVisibility(int,int)\0"
    "setColor(int,int)\0blink(int,int)\0"
};

void PMeshViewer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PMeshViewer *_t = static_cast<PMeshViewer *>(_o);
        switch (_id) {
        case 0: _t->newProject(); break;
        case 1: _t->openProject(); break;
        case 2: _t->saveProject(); break;
        case 3: _t->saveProjectAs(); break;
        case 4: _t->loadDir(); break;
        case 5: _t->loadMesh(); break;
        case 6: _t->addMesh(); break;
        case 7: _t->deleteMesh(); break;
        case 8: _t->saveDirPly(); break;
        case 9: _t->saveDirStl(); break;
        case 10: _t->saveView(); break;
        case 11: _t->toggleFrontFace(); break;
        case 12: _t->setMeshMode((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 13: _t->setSmoothSurface(); break;
        case 14: _t->setFlatSurface(); break;
        case 15: _t->setFlatLines(); break;
        case 16: _t->setWireFrame(); break;
        case 17: _t->info(); break;
        case 18: _t->help(); break;
        case 19: _t->about(); break;
        case 20: _t->setVisibility((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 21: _t->setColor((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 22: _t->blink((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData PMeshViewer::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PMeshViewer::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_PMeshViewer,
      qt_meta_data_PMeshViewer, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PMeshViewer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PMeshViewer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PMeshViewer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PMeshViewer))
        return static_cast<void*>(const_cast< PMeshViewer*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int PMeshViewer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 23)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 23;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
