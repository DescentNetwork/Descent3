/****************************************************************************
** Meta object code from reading C++ file 'selectrange_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../selectrange_dialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'selectrange_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_SelectRangeDialog_t {
    QByteArrayData data[8];
    char stringdata0[108];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_SelectRangeDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_SelectRangeDialog_t qt_meta_stringdata_SelectRangeDialog = {
    {
QT_MOC_LITERAL(0, 0, 17), // "SelectRangeDialog"
QT_MOC_LITERAL(1, 18, 19), // "onLowerBoundChanged"
QT_MOC_LITERAL(2, 38, 0), // ""
QT_MOC_LITERAL(3, 39, 19), // "onUpperBoundChanged"
QT_MOC_LITERAL(4, 59, 14), // "onSlopeChanged"
QT_MOC_LITERAL(5, 74, 20), // "onSelectSlopeToggled"
QT_MOC_LITERAL(6, 95, 7), // "enabled"
QT_MOC_LITERAL(7, 103, 4) // "onOk"

    },
    "SelectRangeDialog\0onLowerBoundChanged\0"
    "\0onUpperBoundChanged\0onSlopeChanged\0"
    "onSelectSlopeToggled\0enabled\0onOk"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_SelectRangeDialog[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   39,    2, 0x08 /* Private */,
       3,    0,   40,    2, 0x08 /* Private */,
       4,    0,   41,    2, 0x08 /* Private */,
       5,    1,   42,    2, 0x08 /* Private */,
       7,    0,   45,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void,

       0        // eod
};

void SelectRangeDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<SelectRangeDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onLowerBoundChanged(); break;
        case 1: _t->onUpperBoundChanged(); break;
        case 2: _t->onSlopeChanged(); break;
        case 3: _t->onSelectSlopeToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->onOk(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject SelectRangeDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_SelectRangeDialog.data,
    qt_meta_data_SelectRangeDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *SelectRangeDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SelectRangeDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_SelectRangeDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int SelectRangeDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
