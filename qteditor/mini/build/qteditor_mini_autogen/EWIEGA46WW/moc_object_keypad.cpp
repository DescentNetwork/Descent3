/****************************************************************************
** Meta object code from reading C++ file 'object_keypad.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../object_keypad.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'object_keypad.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ObjectKeypad_t {
    QByteArrayData data[19];
    char stringdata0[203];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ObjectKeypad_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ObjectKeypad_t qt_meta_stringdata_ObjectKeypad = {
    {
QT_MOC_LITERAL(0, 0, 12), // "ObjectKeypad"
QT_MOC_LITERAL(1, 13, 13), // "onPlaceObject"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 14), // "onDeleteObject"
QT_MOC_LITERAL(4, 43, 12), // "onNextObject"
QT_MOC_LITERAL(5, 56, 12), // "onFlipObject"
QT_MOC_LITERAL(6, 69, 14), // "onResetObjects"
QT_MOC_LITERAL(7, 84, 12), // "onSetDefault"
QT_MOC_LITERAL(8, 97, 7), // "onRot90"
QT_MOC_LITERAL(9, 105, 11), // "onDeleteAll"
QT_MOC_LITERAL(10, 117, 18), // "onPushThroughWalls"
QT_MOC_LITERAL(11, 136, 7), // "checked"
QT_MOC_LITERAL(12, 144, 10), // "onMoveAxis"
QT_MOC_LITERAL(13, 155, 7), // "onAxisX"
QT_MOC_LITERAL(14, 163, 7), // "onAxisY"
QT_MOC_LITERAL(15, 171, 7), // "onAxisZ"
QT_MOC_LITERAL(16, 179, 7), // "onAxisP"
QT_MOC_LITERAL(17, 187, 7), // "onAxisH"
QT_MOC_LITERAL(18, 195, 7) // "onAxisB"

    },
    "ObjectKeypad\0onPlaceObject\0\0onDeleteObject\0"
    "onNextObject\0onFlipObject\0onResetObjects\0"
    "onSetDefault\0onRot90\0onDeleteAll\0"
    "onPushThroughWalls\0checked\0onMoveAxis\0"
    "onAxisX\0onAxisY\0onAxisZ\0onAxisP\0onAxisH\0"
    "onAxisB"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ObjectKeypad[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      16,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   94,    2, 0x08 /* Private */,
       3,    0,   95,    2, 0x08 /* Private */,
       4,    0,   96,    2, 0x08 /* Private */,
       5,    0,   97,    2, 0x08 /* Private */,
       6,    0,   98,    2, 0x08 /* Private */,
       7,    0,   99,    2, 0x08 /* Private */,
       8,    0,  100,    2, 0x08 /* Private */,
       9,    0,  101,    2, 0x08 /* Private */,
      10,    1,  102,    2, 0x08 /* Private */,
      12,    0,  105,    2, 0x08 /* Private */,
      13,    0,  106,    2, 0x08 /* Private */,
      14,    0,  107,    2, 0x08 /* Private */,
      15,    0,  108,    2, 0x08 /* Private */,
      16,    0,  109,    2, 0x08 /* Private */,
      17,    0,  110,    2, 0x08 /* Private */,
      18,    0,  111,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void ObjectKeypad::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ObjectKeypad *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onPlaceObject(); break;
        case 1: _t->onDeleteObject(); break;
        case 2: _t->onNextObject(); break;
        case 3: _t->onFlipObject(); break;
        case 4: _t->onResetObjects(); break;
        case 5: _t->onSetDefault(); break;
        case 6: _t->onRot90(); break;
        case 7: _t->onDeleteAll(); break;
        case 8: _t->onPushThroughWalls((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->onMoveAxis(); break;
        case 10: _t->onAxisX(); break;
        case 11: _t->onAxisY(); break;
        case 12: _t->onAxisZ(); break;
        case 13: _t->onAxisP(); break;
        case 14: _t->onAxisH(); break;
        case 15: _t->onAxisB(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ObjectKeypad::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_ObjectKeypad.data,
    qt_meta_data_ObjectKeypad,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ObjectKeypad::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ObjectKeypad::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ObjectKeypad.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int ObjectKeypad::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 16;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
