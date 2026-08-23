/****************************************************************************
** Meta object code from reading C++ file 'death_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../death_dialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'death_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_DeathDialog_t {
    QByteArrayData data[13];
    char stringdata0[168];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_DeathDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_DeathDialog_t qt_meta_stringdata_DeathDialog = {
    {
QT_MOC_LITERAL(0, 0, 11), // "DeathDialog"
QT_MOC_LITERAL(1, 12, 4), // "onOk"
QT_MOC_LITERAL(2, 17, 0), // ""
QT_MOC_LITERAL(3, 18, 16), // "onExplosionSmall"
QT_MOC_LITERAL(4, 35, 17), // "onExplosionMedium"
QT_MOC_LITERAL(5, 53, 16), // "onExplosionLarge"
QT_MOC_LITERAL(6, 70, 15), // "onCommonDescent"
QT_MOC_LITERAL(7, 86, 22), // "onCommonQuickExplosion"
QT_MOC_LITERAL(8, 109, 15), // "onCommonShootUp"
QT_MOC_LITERAL(9, 125, 12), // "onCommonR2D2"
QT_MOC_LITERAL(10, 138, 14), // "onCommonAnimal"
QT_MOC_LITERAL(11, 153, 6), // "onCopy"
QT_MOC_LITERAL(12, 160, 7) // "onPaste"

    },
    "DeathDialog\0onOk\0\0onExplosionSmall\0"
    "onExplosionMedium\0onExplosionLarge\0"
    "onCommonDescent\0onCommonQuickExplosion\0"
    "onCommonShootUp\0onCommonR2D2\0"
    "onCommonAnimal\0onCopy\0onPaste"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_DeathDialog[] = {

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
       4,    0,   71,    2, 0x08 /* Private */,
       5,    0,   72,    2, 0x08 /* Private */,
       6,    0,   73,    2, 0x08 /* Private */,
       7,    0,   74,    2, 0x08 /* Private */,
       8,    0,   75,    2, 0x08 /* Private */,
       9,    0,   76,    2, 0x08 /* Private */,
      10,    0,   77,    2, 0x08 /* Private */,
      11,    0,   78,    2, 0x08 /* Private */,
      12,    0,   79,    2, 0x08 /* Private */,

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

       0        // eod
};

void DeathDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<DeathDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onOk(); break;
        case 1: _t->onExplosionSmall(); break;
        case 2: _t->onExplosionMedium(); break;
        case 3: _t->onExplosionLarge(); break;
        case 4: _t->onCommonDescent(); break;
        case 5: _t->onCommonQuickExplosion(); break;
        case 6: _t->onCommonShootUp(); break;
        case 7: _t->onCommonR2D2(); break;
        case 8: _t->onCommonAnimal(); break;
        case 9: _t->onCopy(); break;
        case 10: _t->onPaste(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject DeathDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_DeathDialog.data,
    qt_meta_data_DeathDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *DeathDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DeathDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_DeathDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int DeathDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
