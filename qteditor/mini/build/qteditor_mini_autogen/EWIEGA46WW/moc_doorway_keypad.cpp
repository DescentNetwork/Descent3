/****************************************************************************
** Meta object code from reading C++ file 'doorway_keypad.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../doorway_keypad.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'doorway_keypad.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DoorwayKeypad_t {
    QByteArrayData data[14];
    char stringdata0[169];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DoorwayKeypad_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DoorwayKeypad_t qt_meta_stringdata_DoorwayKeypad = {
    {
QT_MOC_LITERAL(0, 0, 13), // "DoorwayKeypad"
QT_MOC_LITERAL(1, 14, 10), // "onNextDoor"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 10), // "onPrevDoor"
QT_MOC_LITERAL(4, 37, 15), // "onLockedToggled"
QT_MOC_LITERAL(5, 53, 7), // "checked"
QT_MOC_LITERAL(6, 61, 13), // "onAutoToggled"
QT_MOC_LITERAL(7, 75, 21), // "onIgnoreLockedToggled"
QT_MOC_LITERAL(8, 97, 12), // "onKeyToggled"
QT_MOC_LITERAL(9, 110, 8), // "onKeyAll"
QT_MOC_LITERAL(10, 119, 12), // "onKeyOnlyOne"
QT_MOC_LITERAL(11, 132, 11), // "onPosEdited"
QT_MOC_LITERAL(12, 144, 11), // "onPlaceDoor"
QT_MOC_LITERAL(13, 156, 12) // "onAttachDoor"

    },
    "DoorwayKeypad\0onNextDoor\0\0onPrevDoor\0"
    "onLockedToggled\0checked\0onAutoToggled\0"
    "onIgnoreLockedToggled\0onKeyToggled\0"
    "onKeyAll\0onKeyOnlyOne\0onPosEdited\0"
    "onPlaceDoor\0onAttachDoor"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DoorwayKeypad[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x08 /* Private */,
       3,    0,   70,    2, 0x08 /* Private */,
       4,    1,   71,    2, 0x08 /* Private */,
       6,    1,   74,    2, 0x08 /* Private */,
       7,    1,   77,    2, 0x08 /* Private */,
       8,    0,   80,    2, 0x08 /* Private */,
       9,    0,   81,    2, 0x08 /* Private */,
      10,    0,   82,    2, 0x08 /* Private */,
      11,    0,   83,    2, 0x08 /* Private */,
      12,    0,   84,    2, 0x08 /* Private */,
      13,    0,   85,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void, QMetaType::Bool,    5,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void DoorwayKeypad::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DoorwayKeypad *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onNextDoor(); break;
        case 1: _t->onPrevDoor(); break;
        case 2: _t->onLockedToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->onAutoToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->onIgnoreLockedToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->onKeyToggled(); break;
        case 6: _t->onKeyAll(); break;
        case 7: _t->onKeyOnlyOne(); break;
        case 8: _t->onPosEdited(); break;
        case 9: _t->onPlaceDoor(); break;
        case 10: _t->onAttachDoor(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject DoorwayKeypad::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_DoorwayKeypad.data,
    qt_meta_data_DoorwayKeypad,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DoorwayKeypad::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DoorwayKeypad::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DoorwayKeypad.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int DoorwayKeypad::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
