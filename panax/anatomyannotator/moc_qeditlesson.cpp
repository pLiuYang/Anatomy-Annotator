/****************************************************************************
** Meta object code from reading C++ file 'qeditlesson.h'
**
** Created: Wed Feb 19 01:22:43 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qeditlesson.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qeditlesson.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QEditLesson[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   13,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      51,   12,   12,   12, 0x0a,
      58,   12,   12,   12, 0x0a,
      64,   12,   12,   12, 0x0a,
      72,   70,   12,   12, 0x0a,
      94,   70,   12,   12, 0x0a,
     121,   70,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QEditLesson[] = {
    "QEditLesson\0\0lsnS\0updateLessonSlides(QList<Slide>)\0"
    "save()\0add()\0del()\0,\0updateSlides(int,int)\0"
    "enterSlideContent(int,int)\0"
    "storeCameraPosition(int,int)\0"
};

void QEditLesson::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QEditLesson *_t = static_cast<QEditLesson *>(_o);
        switch (_id) {
        case 0: _t->updateLessonSlides((*reinterpret_cast< QList<Slide>(*)>(_a[1]))); break;
        case 1: _t->save(); break;
        case 2: _t->add(); break;
        case 3: _t->del(); break;
        case 4: _t->updateSlides((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: _t->enterSlideContent((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 6: _t->storeCameraPosition((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QEditLesson::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QEditLesson::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_QEditLesson,
      qt_meta_data_QEditLesson, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QEditLesson::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QEditLesson::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QEditLesson::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QEditLesson))
        return static_cast<void*>(const_cast< QEditLesson*>(this));
    return QDialog::qt_metacast(_clname);
}

int QEditLesson::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void QEditLesson::updateLessonSlides(QList<Slide> _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
