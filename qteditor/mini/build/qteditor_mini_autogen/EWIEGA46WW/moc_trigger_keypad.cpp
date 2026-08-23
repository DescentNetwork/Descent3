/****************************************************************************
** Meta object code from reading C++ file 'trigger_keypad.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../trigger_keypad.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'trigger_keypad.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TriggerKeypad_t {
    QByteArrayData data[11];
    char stringdata0[133];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TriggerKeypad_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TriggerKeypad_t qt_meta_stringdata_TriggerKeypad = {
    {
QT_MOC_LITERAL(0, 0, 13), // "TriggerKeypad"
QT_MOC_LITERAL(1, 14, 8), // "onDelete"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 12), // "onPrevInMine"
QT_MOC_LITERAL(4, 37, 12), // "onNextInMine"
QT_MOC_LITERAL(5, 50, 12), // "onPrevInRoom"
QT_MOC_LITERAL(6, 63, 12), // "onNextInRoom"
QT_MOC_LITERAL(7, 76, 12), // "onNextPortal"
QT_MOC_LITERAL(8, 89, 16), // "onOneshotToggled"
QT_MOC_LITERAL(9, 106, 7), // "checked"
QT_MOC_LITERAL(10, 114, 18) // "onActivatorToggled"

    },
    "TriggerKeypad\0onDelete\0\0onPrevInMine\0"
    "onNextInMine\0onPrevInRoom\0onNextInRoom\0"
    "onNextPortal\0onOneshotToggled\0checked\0"
    "onActivatorToggled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TriggerKeypad[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   54,    2, 0x08 /* Private */,
       3,    0,   55,    2, 0x08 /* Private */,
       4,    0,   56,    2, 0x08 /* Private */,
       5,    0,   57,    2, 0x08 /* Private */,
       6,    0,   58,    2, 0x08 /* Private */,
       7,    0,   59,    2, 0x08 /* Private */,
       8,    1,   60,    2, 0x08 /* Private */,
      10,    0,   63,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void,

       0        // eod
};

void TriggerKeypad::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TriggerKeypad *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onDelete(); break;
        case 1: _t->onPrevInMine(); break;
        case 2: _t->onNextInMine(); break;
        case 3: _t->onPrevInRoom(); break;
        case 4: _t->onNextInRoom(); break;
        case 5: _t->onNextPortal(); break;
        case 6: _t->onOneshotToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->onActivatorToggled(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TriggerKeypad::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_TriggerKeypad.data,
    qt_meta_data_TriggerKeypad,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TriggerKeypad::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TriggerKeypad::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TriggerKeypad.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int TriggerKeypad::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
