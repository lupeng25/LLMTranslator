/****************************************************************************
** Meta object code from reading C++ file 'TranslatorEngine.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.9)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../TranslatorEngine.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TranslatorEngine.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.9. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TranslatorEngine_t {
    QByteArrayData data[10];
    char stringdata0[101];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TranslatorEngine_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TranslatorEngine_t qt_meta_stringdata_TranslatorEngine = {
    {
QT_MOC_LITERAL(0, 0, 16), // "TranslatorEngine"
QT_MOC_LITERAL(1, 17, 15), // "progressUpdated"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 7), // "current"
QT_MOC_LITERAL(4, 42, 5), // "total"
QT_MOC_LITERAL(5, 48, 10), // "logMessage"
QT_MOC_LITERAL(6, 59, 3), // "msg"
QT_MOC_LITERAL(7, 63, 19), // "translationFinished"
QT_MOC_LITERAL(8, 83, 13), // "errorOccurred"
QT_MOC_LITERAL(9, 97, 3) // "err"

    },
    "TranslatorEngine\0progressUpdated\0\0"
    "current\0total\0logMessage\0msg\0"
    "translationFinished\0errorOccurred\0err"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TranslatorEngine[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   34,    2, 0x06 /* Public */,
       5,    1,   39,    2, 0x06 /* Public */,
       7,    0,   42,    2, 0x06 /* Public */,
       8,    1,   43,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::QString,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    9,

       0        // eod
};

void TranslatorEngine::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TranslatorEngine *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->progressUpdated((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->logMessage((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: _t->translationFinished(); break;
        case 3: _t->errorOccurred((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TranslatorEngine::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TranslatorEngine::progressUpdated)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TranslatorEngine::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TranslatorEngine::logMessage)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TranslatorEngine::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TranslatorEngine::translationFinished)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TranslatorEngine::*)(const QString & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TranslatorEngine::errorOccurred)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TranslatorEngine::staticMetaObject = { {
    &QObject::staticMetaObject,
    qt_meta_stringdata_TranslatorEngine.data,
    qt_meta_data_TranslatorEngine,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TranslatorEngine::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TranslatorEngine::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TranslatorEngine.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TranslatorEngine::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void TranslatorEngine::progressUpdated(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TranslatorEngine::logMessage(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TranslatorEngine::translationFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void TranslatorEngine::errorOccurred(const QString & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
