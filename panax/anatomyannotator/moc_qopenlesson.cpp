/****************************************************************************
** Meta object code from reading C++ file 'qopenlesson.h'
**
** Created: Wed Feb 19 00:32:20 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qopenlesson.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qopenlesson.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QOpenLesson[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_QOpenLesson[] = {
    "QOpenLesson\0"
};

void QOpenLesson::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QOpenLesson::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QOpenLesson::staticMetaObject = {
    { &QWizard::staticMetaObject, qt_meta_stringdata_QOpenLesson,
      qt_meta_data_QOpenLesson, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QOpenLesson::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QOpenLesson::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QOpenLesson::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QOpenLesson))
        return static_cast<void*>(const_cast< QOpenLesson*>(this));
    return QWizard::qt_metacast(_clname);
}

int QOpenLesson::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizard::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_LessonSlide[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      39,   12,   12,   12, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_LessonSlide[] = {
    "LessonSlide\0\0updateCameraAngleSignal()\0"
    "updateCameraAngle()\0"
};

void LessonSlide::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        LessonSlide *_t = static_cast<LessonSlide *>(_o);
        switch (_id) {
        case 0: _t->updateCameraAngleSignal(); break;
        case 1: _t->updateCameraAngle(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData LessonSlide::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject LessonSlide::staticMetaObject = {
    { &QWizardPage::staticMetaObject, qt_meta_stringdata_LessonSlide,
      qt_meta_data_LessonSlide, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LessonSlide::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LessonSlide::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LessonSlide::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LessonSlide))
        return static_cast<void*>(const_cast< LessonSlide*>(this));
    return QWizardPage::qt_metacast(_clname);
}

int LessonSlide::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizardPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void LessonSlide::updateCameraAngleSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
