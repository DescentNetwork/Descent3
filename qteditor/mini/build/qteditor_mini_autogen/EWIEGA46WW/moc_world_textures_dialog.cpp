/****************************************************************************
** Meta object code from reading C++ file 'world_textures_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../world_textures_dialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'world_textures_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_WorldTexturesDialog_t {
    QByteArrayData data[15];
    char stringdata0[169];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_WorldTexturesDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_WorldTexturesDialog_t qt_meta_stringdata_WorldTexturesDialog = {
    {
QT_MOC_LITERAL(0, 0, 19), // "WorldTexturesDialog"
QT_MOC_LITERAL(1, 20, 8), // "onAddNew"
QT_MOC_LITERAL(2, 29, 0), // ""
QT_MOC_LITERAL(3, 30, 8), // "onDelete"
QT_MOC_LITERAL(4, 39, 6), // "onLock"
QT_MOC_LITERAL(5, 46, 9), // "onCheckin"
QT_MOC_LITERAL(6, 56, 12), // "onCheckedOut"
QT_MOC_LITERAL(7, 69, 10), // "onOverride"
QT_MOC_LITERAL(8, 80, 12), // "onChangeName"
QT_MOC_LITERAL(9, 93, 12), // "onLoadBitmap"
QT_MOC_LITERAL(10, 106, 9), // "onCurrent"
QT_MOC_LITERAL(11, 116, 6), // "onNext"
QT_MOC_LITERAL(12, 123, 6), // "onPrev"
QT_MOC_LITERAL(13, 130, 16), // "onTexListChanged"
QT_MOC_LITERAL(14, 147, 21) // "onAmbientSoundChanged"

    },
    "WorldTexturesDialog\0onAddNew\0\0onDelete\0"
    "onLock\0onCheckin\0onCheckedOut\0onOverride\0"
    "onChangeName\0onLoadBitmap\0onCurrent\0"
    "onNext\0onPrev\0onTexListChanged\0"
    "onAmbientSoundChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_WorldTexturesDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   79,    2, 0x08 /* Private */,
       3,    0,   80,    2, 0x08 /* Private */,
       4,    0,   81,    2, 0x08 /* Private */,
       5,    0,   82,    2, 0x08 /* Private */,
       6,    0,   83,    2, 0x08 /* Private */,
       7,    0,   84,    2, 0x08 /* Private */,
       8,    0,   85,    2, 0x08 /* Private */,
       9,    0,   86,    2, 0x08 /* Private */,
      10,    0,   87,    2, 0x08 /* Private */,
      11,    0,   88,    2, 0x08 /* Private */,
      12,    0,   89,    2, 0x08 /* Private */,
      13,    0,   90,    2, 0x08 /* Private */,
      14,    0,   91,    2, 0x08 /* Private */,

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

       0        // eod
};

void WorldTexturesDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<WorldTexturesDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onAddNew(); break;
        case 1: _t->onDelete(); break;
        case 2: _t->onLock(); break;
        case 3: _t->onCheckin(); break;
        case 4: _t->onCheckedOut(); break;
        case 5: _t->onOverride(); break;
        case 6: _t->onChangeName(); break;
        case 7: _t->onLoadBitmap(); break;
        case 8: _t->onCurrent(); break;
        case 9: _t->onNext(); break;
        case 10: _t->onPrev(); break;
        case 11: _t->onTexListChanged(); break;
        case 12: _t->onAmbientSoundChanged(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject WorldTexturesDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_WorldTexturesDialog.data,
    qt_meta_data_WorldTexturesDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *WorldTexturesDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *WorldTexturesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_WorldTexturesDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int WorldTexturesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
