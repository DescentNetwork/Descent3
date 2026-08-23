/****************************************************************************
** Meta object code from reading C++ file 'editor_view.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../editor_view.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editor_view.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_EditorView_t {
    QByteArrayData data[12];
    char stringdata0[150];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_EditorView_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_EditorView_t qt_meta_stringdata_EditorView = {
    {
QT_MOC_LITERAL(0, 0, 10), // "EditorView"
QT_MOC_LITERAL(1, 11, 12), // "faceSelected"
QT_MOC_LITERAL(2, 24, 0), // ""
QT_MOC_LITERAL(3, 25, 9), // "roomIndex"
QT_MOC_LITERAL(4, 35, 9), // "faceIndex"
QT_MOC_LITERAL(5, 45, 14), // "objectSelected"
QT_MOC_LITERAL(6, 60, 8), // "objIndex"
QT_MOC_LITERAL(7, 69, 16), // "selectionCleared"
QT_MOC_LITERAL(8, 86, 26), // "objectContextMenuRequested"
QT_MOC_LITERAL(9, 113, 9), // "globalPos"
QT_MOC_LITERAL(10, 123, 21), // "rectSelectionFinished"
QT_MOC_LITERAL(11, 145, 4) // "rect"

    },
    "EditorView\0faceSelected\0\0roomIndex\0"
    "faceIndex\0objectSelected\0objIndex\0"
    "selectionCleared\0objectContextMenuRequested\0"
    "globalPos\0rectSelectionFinished\0rect"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_EditorView[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    2,   39,    2, 0x06 /* Public */,
       5,    1,   44,    2, 0x06 /* Public */,
       7,    0,   47,    2, 0x06 /* Public */,
       8,    2,   48,    2, 0x06 /* Public */,
      10,    1,   53,    2, 0x06 /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void, QMetaType::Int,    6,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QPoint, QMetaType::Int,    9,    6,
    QMetaType::Void, QMetaType::QRect,   11,

       0        // eod
};

void EditorView::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<EditorView *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->faceSelected((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->objectSelected((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->selectionCleared(); break;
        case 3: _t->objectContextMenuRequested((*reinterpret_cast< const QPoint(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 4: _t->rectSelectionFinished((*reinterpret_cast< const QRect(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (EditorView::*)(int , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EditorView::faceSelected)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (EditorView::*)(int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EditorView::objectSelected)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (EditorView::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EditorView::selectionCleared)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (EditorView::*)(const QPoint & , int );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EditorView::objectContextMenuRequested)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (EditorView::*)(const QRect & );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&EditorView::rectSelectionFinished)) {
                *result = 4;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject EditorView::staticMetaObject = { {
    QMetaObject::SuperData::link<QOpenGLWidget::staticMetaObject>(),
    qt_meta_stringdata_EditorView.data,
    qt_meta_data_EditorView,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *EditorView::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EditorView::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_EditorView.stringdata0))
        return static_cast<void*>(this);
    return QOpenGLWidget::qt_metacast(_clname);
}

int EditorView::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QOpenGLWidget::qt_metacall(_c, _id, _a);
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

// SIGNAL 0
void EditorView::faceSelected(int _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void EditorView::objectSelected(int _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void EditorView::selectionCleared()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void EditorView::objectContextMenuRequested(const QPoint & _t1, int _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void EditorView::rectSelectionFinished(const QRect & _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
