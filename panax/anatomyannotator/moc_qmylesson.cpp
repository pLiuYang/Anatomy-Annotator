/****************************************************************************
** Meta object code from reading C++ file 'qmylesson.h'
**
** Created: Tue Mar 25 13:15:05 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qmylesson.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qmylesson.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMyLesson[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   11,   10,   10, 0x0a,
      33,   10,   10,   10, 0x0a,
      40,   10,   10,   10, 0x0a,
      47,   10,   10,   10, 0x0a,
      53,   10,   10,   10, 0x0a,
      59,   10,   10,   10, 0x0a,
      81,   76,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QMyLesson[] = {
    "QMyLesson\0\0row,col\0open(int,int)\0"
    "open()\0edit()\0add()\0del()\0showLessonList()\0"
    "lsnS\0updateLessonSlides(QList<Slide>)\0"
};

void QMyLesson::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QMyLesson *_t = static_cast<QMyLesson *>(_o);
        switch (_id) {
        case 0: _t->open((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->open(); break;
        case 2: _t->edit(); break;
        case 3: _t->add(); break;
        case 4: _t->del(); break;
        case 5: _t->showLessonList(); break;
        case 6: _t->updateLessonSlides((*reinterpret_cast< QList<Slide>(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QMyLesson::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QMyLesson::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QMyLesson,
      qt_meta_data_QMyLesson, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QMyLesson::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QMyLesson::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QMyLesson::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMyLesson))
        return static_cast<void*>(const_cast< QMyLesson*>(this));
    return QWidget::qt_metacast(_clname);
}

int QMyLesson::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
