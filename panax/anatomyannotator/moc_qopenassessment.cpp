/****************************************************************************
** Meta object code from reading C++ file 'qopenassessment.h'
**
** Created: Wed Feb 19 00:32:16 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "qopenassessment.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'qopenassessment.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_QOpenAssessment[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_QOpenAssessment[] = {
    "QOpenAssessment\0\0updateAssessmentStatus()\0"
};

void QOpenAssessment::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QOpenAssessment *_t = static_cast<QOpenAssessment *>(_o);
        switch (_id) {
        case 0: _t->updateAssessmentStatus(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QOpenAssessment::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QOpenAssessment::staticMetaObject = {
    { &QWizard::staticMetaObject, qt_meta_stringdata_QOpenAssessment,
      qt_meta_data_QOpenAssessment, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QOpenAssessment::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QOpenAssessment::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QOpenAssessment::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QOpenAssessment))
        return static_cast<void*>(const_cast< QOpenAssessment*>(this));
    return QWizard::qt_metacast(_clname);
}

int QOpenAssessment::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizard::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void QOpenAssessment::updateAssessmentStatus()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_QuestionPage[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_QuestionPage[] = {
    "QuestionPage\0\0showAnswer()\0"
};

void QuestionPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        QuestionPage *_t = static_cast<QuestionPage *>(_o);
        switch (_id) {
        case 0: _t->showAnswer(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData QuestionPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject QuestionPage::staticMetaObject = {
    { &QWizardPage::staticMetaObject, qt_meta_stringdata_QuestionPage,
      qt_meta_data_QuestionPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &QuestionPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *QuestionPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *QuestionPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_QuestionPage))
        return static_cast<void*>(const_cast< QuestionPage*>(this));
    return QWizardPage::qt_metacast(_clname);
}

int QuestionPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizardPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_FeedbackPage[] = {

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

static const char qt_meta_stringdata_FeedbackPage[] = {
    "FeedbackPage\0"
};

void FeedbackPage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FeedbackPage::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FeedbackPage::staticMetaObject = {
    { &QWizardPage::staticMetaObject, qt_meta_stringdata_FeedbackPage,
      qt_meta_data_FeedbackPage, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FeedbackPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FeedbackPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FeedbackPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FeedbackPage))
        return static_cast<void*>(const_cast< FeedbackPage*>(this));
    return QWizardPage::qt_metacast(_clname);
}

int FeedbackPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWizardPage::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
