/****************************************************************************
** Meta object code from reading C++ file 'viewer_prop_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../viewer_prop_dialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'viewer_prop_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_ViewerPropDialog_t {
    QByteArrayData data[15];
    char stringdata0[184];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_ViewerPropDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_ViewerPropDialog_t qt_meta_stringdata_ViewerPropDialog = {
    {
QT_MOC_LITERAL(0, 0, 16), // "ViewerPropDialog"
QT_MOC_LITERAL(1, 17, 13), // "onAlignUpYpos"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 11), // "onAlignXneg"
QT_MOC_LITERAL(4, 44, 11), // "onAlignXpos"
QT_MOC_LITERAL(5, 56, 11), // "onAlignYneg"
QT_MOC_LITERAL(6, 68, 11), // "onAlignYpos"
QT_MOC_LITERAL(7, 80, 11), // "onAlignZneg"
QT_MOC_LITERAL(8, 92, 11), // "onAlignZpos"
QT_MOC_LITERAL(9, 104, 14), // "onOrientCommit"
QT_MOC_LITERAL(10, 119, 11), // "onPosCommit"
QT_MOC_LITERAL(11, 131, 14), // "onXMoveToggled"
QT_MOC_LITERAL(12, 146, 7), // "checked"
QT_MOC_LITERAL(13, 154, 14), // "onYMoveToggled"
QT_MOC_LITERAL(14, 169, 14) // "onZMoveToggled"

    },
    "ViewerPropDialog\0onAlignUpYpos\0\0"
    "onAlignXneg\0onAlignXpos\0onAlignYneg\0"
    "onAlignYpos\0onAlignZneg\0onAlignZpos\0"
    "onOrientCommit\0onPosCommit\0onXMoveToggled\0"
    "checked\0onYMoveToggled\0onZMoveToggled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_ViewerPropDialog[] = {

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
       5,    0,   77,    2, 0x08 /* Private */,
       6,    0,   78,    2, 0x08 /* Private */,
       7,    0,   79,    2, 0x08 /* Private */,
       8,    0,   80,    2, 0x08 /* Private */,
       9,    0,   81,    2, 0x08 /* Private */,
      10,    0,   82,    2, 0x08 /* Private */,
      11,    1,   83,    2, 0x08 /* Private */,
      13,    1,   86,    2, 0x08 /* Private */,
      14,    1,   89,    2, 0x08 /* Private */,

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
    QMetaType::Void, QMetaType::Bool,   12,
    QMetaType::Void, QMetaType::Bool,   12,
    QMetaType::Void, QMetaType::Bool,   12,

       0        // eod
};

void ViewerPropDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<ViewerPropDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onAlignUpYpos(); break;
        case 1: _t->onAlignXneg(); break;
        case 2: _t->onAlignXpos(); break;
        case 3: _t->onAlignYneg(); break;
        case 4: _t->onAlignYpos(); break;
        case 5: _t->onAlignZneg(); break;
        case 6: _t->onAlignZpos(); break;
        case 7: _t->onOrientCommit(); break;
        case 8: _t->onPosCommit(); break;
        case 9: _t->onXMoveToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->onYMoveToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: _t->onZMoveToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject ViewerPropDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_ViewerPropDialog.data,
    qt_meta_data_ViewerPropDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *ViewerPropDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ViewerPropDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ViewerPropDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int ViewerPropDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
