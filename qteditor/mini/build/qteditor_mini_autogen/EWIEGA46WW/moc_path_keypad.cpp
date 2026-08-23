/****************************************************************************
** Meta object code from reading C++ file 'path_keypad.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../path_keypad.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'path_keypad.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PathKeypad_t {
    QByteArrayData data[14];
    char stringdata0[175];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PathKeypad_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PathKeypad_t qt_meta_stringdata_PathKeypad = {
    {
QT_MOC_LITERAL(0, 0, 10), // "PathKeypad"
QT_MOC_LITERAL(1, 11, 9), // "onAddPath"
QT_MOC_LITERAL(2, 21, 0), // ""
QT_MOC_LITERAL(3, 22, 12), // "onDeletePath"
QT_MOC_LITERAL(4, 35, 21), // "onPathPulldownChanged"
QT_MOC_LITERAL(5, 57, 10), // "onNextNode"
QT_MOC_LITERAL(6, 68, 10), // "onPrevNode"
QT_MOC_LITERAL(7, 79, 19), // "onCurrentNodeEdited"
QT_MOC_LITERAL(8, 99, 12), // "onInsertNode"
QT_MOC_LITERAL(9, 112, 12), // "onDeleteNode"
QT_MOC_LITERAL(10, 125, 10), // "onMoveNode"
QT_MOC_LITERAL(11, 136, 18), // "onShowNodesToggled"
QT_MOC_LITERAL(12, 155, 7), // "checked"
QT_MOC_LITERAL(13, 163, 11) // "onIncEdited"

    },
    "PathKeypad\0onAddPath\0\0onDeletePath\0"
    "onPathPulldownChanged\0onNextNode\0"
    "onPrevNode\0onCurrentNodeEdited\0"
    "onInsertNode\0onDeleteNode\0onMoveNode\0"
    "onShowNodesToggled\0checked\0onIncEdited"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PathKeypad[] = {

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
      11,    1,   78,    2, 0x08 /* Private */,
      13,    0,   81,    2, 0x08 /* Private */,

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
    QMetaType::Void,

       0        // eod
};

void PathKeypad::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PathKeypad *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onAddPath(); break;
        case 1: _t->onDeletePath(); break;
        case 2: _t->onPathPulldownChanged(); break;
        case 3: _t->onNextNode(); break;
        case 4: _t->onPrevNode(); break;
        case 5: _t->onCurrentNodeEdited(); break;
        case 6: _t->onInsertNode(); break;
        case 7: _t->onDeleteNode(); break;
        case 8: _t->onMoveNode(); break;
        case 9: _t->onShowNodesToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: _t->onIncEdited(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PathKeypad::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_PathKeypad.data,
    qt_meta_data_PathKeypad,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PathKeypad::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PathKeypad::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PathKeypad.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int PathKeypad::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
