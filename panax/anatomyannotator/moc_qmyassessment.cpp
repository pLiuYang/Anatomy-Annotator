/****************************************************************************
** Meta object code from reading C++ file 'qmyassessment.h'
**
** Created: Tue Mar 18 15:45:12 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qmyassessment.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qmyassessment.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QMyAssessment[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   15,   14,   14, 0x0a,
      37,   14,   14,   14, 0x0a,
      44,   14,   14,   14, 0x0a,
      51,   14,   14,   14, 0x0a,
      57,   14,   14,   14, 0x0a,
      63,   14,   14,   14, 0x0a,
      87,   84,   14,   14, 0x0a,
     123,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QMyAssessment[] = {
    "QMyAssessment\0\0row,col\0open(int,int)\0"
    "open()\0edit()\0add()\0del()\0"
    "showAssessmentList()\0qL\0"
    "updateQuestionList(QList<Question>)\0"
    "updateAsmStatus()\0"
};

void QMyAssessment::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QMyAssessment *_t = static_cast<QMyAssessment *>(_o);
        switch (_id) {
        case 0: _t->open((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->open(); break;
        case 2: _t->edit(); break;
        case 3: _t->add(); break;
        case 4: _t->del(); break;
        case 5: _t->showAssessmentList(); break;
        case 6: _t->updateQuestionList((*reinterpret_cast< QList<Question>(*)>(_a[1]))); break;
        case 7: _t->updateAsmStatus(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData QMyAssessment::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QMyAssessment::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_QMyAssessment,
      qt_meta_data_QMyAssessment, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QMyAssessment::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QMyAssessment::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QMyAssessment::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QMyAssessment))
        return static_cast<void*>(const_cast< QMyAssessment*>(this));
    return QWidget::qt_metacast(_clname);
}

int QMyAssessment::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
