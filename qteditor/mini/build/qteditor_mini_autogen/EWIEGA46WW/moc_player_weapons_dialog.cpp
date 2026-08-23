/****************************************************************************
** Meta object code from reading C++ file 'player_weapons_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../player_weapons_dialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'player_weapons_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PlayerWeaponsDialog_t {
    QByteArrayData data[15];
    char stringdata0[234];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PlayerWeaponsDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PlayerWeaponsDialog_t qt_meta_stringdata_PlayerWeaponsDialog = {
    {
QT_MOC_LITERAL(0, 0, 19), // "PlayerWeaponsDialog"
QT_MOC_LITERAL(1, 20, 14), // "onEditWbButton"
QT_MOC_LITERAL(2, 35, 0), // ""
QT_MOC_LITERAL(3, 36, 22), // "onCurrentWeaponChanged"
QT_MOC_LITERAL(4, 59, 13), // "onFiresFusion"
QT_MOC_LITERAL(5, 73, 23), // "onContinuousFiringSound"
QT_MOC_LITERAL(6, 97, 7), // "checked"
QT_MOC_LITERAL(7, 105, 20), // "onFiringReleaseSound"
QT_MOC_LITERAL(8, 126, 20), // "onFiringSoundChanged"
QT_MOC_LITERAL(9, 147, 21), // "onReleaseSoundChanged"
QT_MOC_LITERAL(10, 169, 20), // "onSpewPowerupChanged"
QT_MOC_LITERAL(11, 190, 7), // "onOnOff"
QT_MOC_LITERAL(12, 198, 6), // "onZoom"
QT_MOC_LITERAL(13, 205, 15), // "onMaxAmmoEdited"
QT_MOC_LITERAL(14, 221, 12) // "onShowTenths"

    },
    "PlayerWeaponsDialog\0onEditWbButton\0\0"
    "onCurrentWeaponChanged\0onFiresFusion\0"
    "onContinuousFiringSound\0checked\0"
    "onFiringReleaseSound\0onFiringSoundChanged\0"
    "onReleaseSoundChanged\0onSpewPowerupChanged\0"
    "onOnOff\0onZoom\0onMaxAmmoEdited\0"
    "onShowTenths"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PlayerWeaponsDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x08 /* Private */,
       3,    0,   75,    2, 0x08 /* Private */,
       4,    0,   76,    2, 0x08 /* Private */,
       5,    1,   77,    2, 0x08 /* Private */,
       7,    1,   80,    2, 0x08 /* Private */,
       8,    0,   83,    2, 0x08 /* Private */,
       9,    0,   84,    2, 0x08 /* Private */,
      10,    0,   85,    2, 0x08 /* Private */,
      11,    0,   86,    2, 0x08 /* Private */,
      12,    0,   87,    2, 0x08 /* Private */,
      13,    0,   88,    2, 0x08 /* Private */,
      14,    1,   89,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    6,

       0        // eod
};

void PlayerWeaponsDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PlayerWeaponsDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onEditWbButton(); break;
        case 1: _t->onCurrentWeaponChanged(); break;
        case 2: _t->onFiresFusion(); break;
        case 3: _t->onContinuousFiringSound((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->onFiringReleaseSound((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->onFiringSoundChanged(); break;
        case 6: _t->onReleaseSoundChanged(); break;
        case 7: _t->onSpewPowerupChanged(); break;
        case 8: _t->onOnOff(); break;
        case 9: _t->onZoom(); break;
        case 10: _t->onMaxAmmoEdited(); break;
        case 11: _t->onShowTenths((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PlayerWeaponsDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_PlayerWeaponsDialog.data,
    qt_meta_data_PlayerWeaponsDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PlayerWeaponsDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PlayerWeaponsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PlayerWeaponsDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int PlayerWeaponsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
