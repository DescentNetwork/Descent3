/****************************************************************************
** Meta object code from reading C++ file 'lighting_keypad.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../lighting_keypad.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'lighting_keypad.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_LightingKeypad_t {
    QByteArrayData data[10];
    char stringdata0[115];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_LightingKeypad_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_LightingKeypad_t qt_meta_stringdata_LightingKeypad = {
    {
QT_MOC_LITERAL(0, 0, 14), // "LightingKeypad"
QT_MOC_LITERAL(1, 15, 15), // "onShowLightmaps"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 7), // "checked"
QT_MOC_LITERAL(4, 40, 9), // "onBestFit"
QT_MOC_LITERAL(5, 50, 10), // "onHemicube"
QT_MOC_LITERAL(6, 61, 9), // "onElement"
QT_MOC_LITERAL(7, 71, 15), // "onIgnoreTerrain"
QT_MOC_LITERAL(8, 87, 18), // "onIgnoreSatellites"
QT_MOC_LITERAL(9, 106, 8) // "onUseBsp"

    },
    "LightingKeypad\0onShowLightmaps\0\0checked\0"
    "onBestFit\0onHemicube\0onElement\0"
    "onIgnoreTerrain\0onIgnoreSatellites\0"
    "onUseBsp"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_LightingKeypad[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   49,    2, 0x08 /* Private */,
       4,    1,   52,    2, 0x08 /* Private */,
       5,    1,   55,    2, 0x08 /* Private */,
       6,    1,   58,    2, 0x08 /* Private */,
       7,    1,   61,    2, 0x08 /* Private */,
       8,    1,   64,    2, 0x08 /* Private */,
       9,    1,   67,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,
    QMetaType::Void, QMetaType::Bool,    3,

       0        // eod
};

void LightingKeypad::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<LightingKeypad *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onShowLightmaps((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: _t->onBestFit((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 2: _t->onHemicube((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->onElement((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->onIgnoreTerrain((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->onIgnoreSatellites((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->onUseBsp((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject LightingKeypad::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_LightingKeypad.data,
    qt_meta_data_LightingKeypad,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *LightingKeypad::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LightingKeypad::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_LightingKeypad.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int LightingKeypad::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 7;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
