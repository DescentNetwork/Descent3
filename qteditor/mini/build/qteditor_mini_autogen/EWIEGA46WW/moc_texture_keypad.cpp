/****************************************************************************
** Meta object code from reading C++ file 'texture_keypad.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../texture_keypad.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'texture_keypad.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TextureKeypad_t {
    QByteArrayData data[22];
    char stringdata0[239];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TextureKeypad_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TextureKeypad_t qt_meta_stringdata_TextureKeypad = {
    {
QT_MOC_LITERAL(0, 0, 13), // "TextureKeypad"
QT_MOC_LITERAL(1, 14, 11), // "onSlideLeft"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 12), // "onSlideRight"
QT_MOC_LITERAL(4, 40, 9), // "onSlideUp"
QT_MOC_LITERAL(5, 50, 11), // "onSlideDown"
QT_MOC_LITERAL(6, 62, 9), // "onRotLeft"
QT_MOC_LITERAL(7, 72, 10), // "onRotRight"
QT_MOC_LITERAL(8, 83, 10), // "onRotate90"
QT_MOC_LITERAL(9, 94, 7), // "onFlipX"
QT_MOC_LITERAL(10, 102, 7), // "onFlipY"
QT_MOC_LITERAL(11, 110, 9), // "onExpandU"
QT_MOC_LITERAL(12, 120, 11), // "onContractU"
QT_MOC_LITERAL(13, 132, 9), // "onExpandV"
QT_MOC_LITERAL(14, 142, 11), // "onContractV"
QT_MOC_LITERAL(15, 154, 13), // "onStretchLess"
QT_MOC_LITERAL(16, 168, 13), // "onStretchMore"
QT_MOC_LITERAL(17, 182, 12), // "onSetDefault"
QT_MOC_LITERAL(18, 195, 6), // "onGrab"
QT_MOC_LITERAL(19, 202, 9), // "onReplace"
QT_MOC_LITERAL(20, 212, 9), // "onFaceMap"
QT_MOC_LITERAL(21, 222, 16) // "onTexScaleEdited"

    },
    "TextureKeypad\0onSlideLeft\0\0onSlideRight\0"
    "onSlideUp\0onSlideDown\0onRotLeft\0"
    "onRotRight\0onRotate90\0onFlipX\0onFlipY\0"
    "onExpandU\0onContractU\0onExpandV\0"
    "onContractV\0onStretchLess\0onStretchMore\0"
    "onSetDefault\0onGrab\0onReplace\0onFaceMap\0"
    "onTexScaleEdited"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TextureKeypad[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      20,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  114,    2, 0x08 /* Private */,
       3,    0,  115,    2, 0x08 /* Private */,
       4,    0,  116,    2, 0x08 /* Private */,
       5,    0,  117,    2, 0x08 /* Private */,
       6,    0,  118,    2, 0x08 /* Private */,
       7,    0,  119,    2, 0x08 /* Private */,
       8,    0,  120,    2, 0x08 /* Private */,
       9,    0,  121,    2, 0x08 /* Private */,
      10,    0,  122,    2, 0x08 /* Private */,
      11,    0,  123,    2, 0x08 /* Private */,
      12,    0,  124,    2, 0x08 /* Private */,
      13,    0,  125,    2, 0x08 /* Private */,
      14,    0,  126,    2, 0x08 /* Private */,
      15,    0,  127,    2, 0x08 /* Private */,
      16,    0,  128,    2, 0x08 /* Private */,
      17,    0,  129,    2, 0x08 /* Private */,
      18,    0,  130,    2, 0x08 /* Private */,
      19,    0,  131,    2, 0x08 /* Private */,
      20,    0,  132,    2, 0x08 /* Private */,
      21,    0,  133,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void TextureKeypad::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TextureKeypad *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onSlideLeft(); break;
        case 1: _t->onSlideRight(); break;
        case 2: _t->onSlideUp(); break;
        case 3: _t->onSlideDown(); break;
        case 4: _t->onRotLeft(); break;
        case 5: _t->onRotRight(); break;
        case 6: _t->onRotate90(); break;
        case 7: _t->onFlipX(); break;
        case 8: _t->onFlipY(); break;
        case 9: _t->onExpandU(); break;
        case 10: _t->onContractU(); break;
        case 11: _t->onExpandV(); break;
        case 12: _t->onContractV(); break;
        case 13: _t->onStretchLess(); break;
        case 14: _t->onStretchMore(); break;
        case 15: _t->onSetDefault(); break;
        case 16: _t->onGrab(); break;
        case 17: _t->onReplace(); break;
        case 18: _t->onFaceMap(); break;
        case 19: _t->onTexScaleEdited(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject TextureKeypad::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_TextureKeypad.data,
    qt_meta_data_TextureKeypad,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TextureKeypad::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TextureKeypad::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TextureKeypad.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int TextureKeypad::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 20)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 20;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 20)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 20;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
