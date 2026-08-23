/****************************************************************************
** Meta object code from reading C++ file 'generic_light_dialog.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../generic_light_dialog.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'generic_light_dialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GenericLightDialog_t {
    QByteArrayData data[16];
    char stringdata0[257];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GenericLightDialog_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GenericLightDialog_t qt_meta_stringdata_GenericLightDialog = {
    {
QT_MOC_LITERAL(0, 0, 18), // "GenericLightDialog"
QT_MOC_LITERAL(1, 19, 14), // "onFlickerRadio"
QT_MOC_LITERAL(2, 34, 0), // ""
QT_MOC_LITERAL(3, 35, 15), // "onAlwaysOnRadio"
QT_MOC_LITERAL(4, 51, 18), // "onUseTimebitsRadio"
QT_MOC_LITERAL(5, 70, 14), // "onPulseToggled"
QT_MOC_LITERAL(6, 85, 7), // "checked"
QT_MOC_LITERAL(7, 93, 22), // "onPulseToSecondToggled"
QT_MOC_LITERAL(8, 116, 24), // "onFlickerSlightlyToggled"
QT_MOC_LITERAL(9, 141, 20), // "onDirectionalToggled"
QT_MOC_LITERAL(10, 162, 19), // "onNoSpecularToggled"
QT_MOC_LITERAL(11, 182, 11), // "onTimeCheck"
QT_MOC_LITERAL(12, 194, 14), // "onRenderStatic"
QT_MOC_LITERAL(13, 209, 15), // "onRenderGouraud"
QT_MOC_LITERAL(14, 225, 17), // "onRenderLightmaps"
QT_MOC_LITERAL(15, 243, 13) // "onFieldEdited"

    },
    "GenericLightDialog\0onFlickerRadio\0\0"
    "onAlwaysOnRadio\0onUseTimebitsRadio\0"
    "onPulseToggled\0checked\0onPulseToSecondToggled\0"
    "onFlickerSlightlyToggled\0onDirectionalToggled\0"
    "onNoSpecularToggled\0onTimeCheck\0"
    "onRenderStatic\0onRenderGouraud\0"
    "onRenderLightmaps\0onFieldEdited"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GenericLightDialog[] = {

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
       5,    1,   82,    2, 0x08 /* Private */,
       7,    1,   85,    2, 0x08 /* Private */,
       8,    1,   88,    2, 0x08 /* Private */,
       9,    1,   91,    2, 0x08 /* Private */,
      10,    1,   94,    2, 0x08 /* Private */,
      11,    0,   97,    2, 0x08 /* Private */,
      12,    0,   98,    2, 0x08 /* Private */,
      13,    0,   99,    2, 0x08 /* Private */,
      14,    0,  100,    2, 0x08 /* Private */,
      15,    0,  101,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void GenericLightDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GenericLightDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onFlickerRadio(); break;
        case 1: _t->onAlwaysOnRadio(); break;
        case 2: _t->onUseTimebitsRadio(); break;
        case 3: _t->onPulseToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->onPulseToSecondToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->onFlickerSlightlyToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->onDirectionalToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->onNoSpecularToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->onTimeCheck(); break;
        case 9: _t->onRenderStatic(); break;
        case 10: _t->onRenderGouraud(); break;
        case 11: _t->onRenderLightmaps(); break;
        case 12: _t->onFieldEdited(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GenericLightDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_GenericLightDialog.data,
    qt_meta_data_GenericLightDialog,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *GenericLightDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GenericLightDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GenericLightDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int GenericLightDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
