/****************************************************************************
** Meta object code from reading C++ file 'assessmentwidget.h'
**
** Created: Tue Mar 18 15:45:17 2014
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "assessmentwidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'assessmentwidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AssessmentWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      43,   17,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
      66,   17,   17,   17, 0x0a,
      73,   17,   17,   17, 0x0a,
      79,   17,   17,   17, 0x0a,
      85,   17,   17,   17, 0x0a,
     100,   17,   17,   17, 0x0a,
     114,   17,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_AssessmentWidget[] = {
    "AssessmentWidget\0\0updateAssessmentStatus()\0"
    "backtoAssessmentList()\0back()\0add()\0"
    "del()\0editQuestion()\0slideInNext()\0"
    "slideInPrev()\0"
};

void AssessmentWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        AssessmentWidget *_t = static_cast<AssessmentWidget *>(_o);
        switch (_id) {
        case 0: _t->updateAssessmentStatus(); break;
        case 1: _t->backtoAssessmentList(); break;
        case 2: _t->back(); break;
        case 3: _t->add(); break;
        case 4: _t->del(); break;
        case 5: _t->editQuestion(); break;
        case 6: _t->slideInNext(); break;
        case 7: _t->slideInPrev(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData AssessmentWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject AssessmentWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_AssessmentWidget,
      qt_meta_data_AssessmentWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &AssessmentWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *AssessmentWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *AssessmentWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AssessmentWidget))
        return static_cast<void*>(const_cast< AssessmentWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int AssessmentWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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

// SIGNAL 0
void AssessmentWidget::updateAssessmentStatus()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void AssessmentWidget::backtoAssessmentList()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
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
    { &QWidget::staticMetaObject, qt_meta_stringdata_QuestionPage,
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
    return QWidget::qt_metacast(_clname);
}

int QuestionPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
    { &QWidget::staticMetaObject, qt_meta_stringdata_FeedbackPage,
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
    return QWidget::qt_metacast(_clname);
}

int FeedbackPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
