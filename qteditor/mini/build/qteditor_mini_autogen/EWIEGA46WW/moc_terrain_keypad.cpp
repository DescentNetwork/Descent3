/****************************************************************************
** Meta object code from reading C++ file 'terrain_keypad.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.8)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../terrain_keypad.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'terrain_keypad.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.8. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TerrainKeypad_t {
    QByteArrayData data[64];
    char stringdata0[950];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TerrainKeypad_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TerrainKeypad_t qt_meta_stringdata_TerrainKeypad = {
    {
QT_MOC_LITERAL(0, 0, 13), // "TerrainKeypad"
QT_MOC_LITERAL(1, 14, 8), // "onMoveUp"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 10), // "onMoveDown"
QT_MOC_LITERAL(4, 35, 9), // "onRaise10"
QT_MOC_LITERAL(5, 45, 9), // "onLower10"
QT_MOC_LITERAL(6, 55, 12), // "onSelectNone"
QT_MOC_LITERAL(7, 68, 11), // "onSelectAll"
QT_MOC_LITERAL(8, 80, 9), // "onMakeMax"
QT_MOC_LITERAL(9, 90, 9), // "onMakeMin"
QT_MOC_LITERAL(10, 100, 10), // "onMakeZero"
QT_MOC_LITERAL(11, 111, 10), // "onFillArea"
QT_MOC_LITERAL(12, 122, 11), // "onMoreMoons"
QT_MOC_LITERAL(13, 134, 11), // "onLessMoons"
QT_MOC_LITERAL(14, 146, 10), // "onNextMoon"
QT_MOC_LITERAL(15, 157, 10), // "onPrevMoon"
QT_MOC_LITERAL(16, 168, 11), // "onMoveSatUp"
QT_MOC_LITERAL(17, 180, 13), // "onMoveSatDown"
QT_MOC_LITERAL(18, 194, 13), // "onMoveSatLeft"
QT_MOC_LITERAL(19, 208, 14), // "onMoveSatRight"
QT_MOC_LITERAL(20, 223, 16), // "onMoveMoonCloser"
QT_MOC_LITERAL(21, 240, 17), // "onMoveMoonFarther"
QT_MOC_LITERAL(22, 258, 13), // "onRenormalize"
QT_MOC_LITERAL(23, 272, 12), // "onRotTexture"
QT_MOC_LITERAL(24, 285, 9), // "onPyramid"
QT_MOC_LITERAL(25, 295, 10), // "onPancakes"
QT_MOC_LITERAL(26, 306, 15), // "onSmoothTerrain"
QT_MOC_LITERAL(27, 322, 13), // "onDropTerrain"
QT_MOC_LITERAL(28, 336, 14), // "onRandomizeSky"
QT_MOC_LITERAL(29, 351, 11), // "onSkyNearer"
QT_MOC_LITERAL(30, 363, 12), // "onSkyFarther"
QT_MOC_LITERAL(31, 376, 10), // "onTileMore"
QT_MOC_LITERAL(32, 387, 10), // "onTileLess"
QT_MOC_LITERAL(33, 398, 12), // "onRedoTopmap"
QT_MOC_LITERAL(34, 411, 18), // "onTerrainOcclusion"
QT_MOC_LITERAL(35, 430, 18), // "onToggleVisibility"
QT_MOC_LITERAL(36, 449, 10), // "onSkyRadio"
QT_MOC_LITERAL(37, 460, 14), // "onHorizonRadio"
QT_MOC_LITERAL(38, 475, 10), // "onFogRadio"
QT_MOC_LITERAL(39, 486, 16), // "onSatelliteRadio"
QT_MOC_LITERAL(40, 503, 14), // "onSkyRedEdited"
QT_MOC_LITERAL(41, 518, 16), // "onSkyGreenEdited"
QT_MOC_LITERAL(42, 535, 15), // "onSkyBlueEdited"
QT_MOC_LITERAL(43, 551, 19), // "onFogDistanceEdited"
QT_MOC_LITERAL(44, 571, 18), // "onPixelErrorEdited"
QT_MOC_LITERAL(45, 590, 17), // "onFogScalarEdited"
QT_MOC_LITERAL(46, 608, 20), // "onDamagePerSecEdited"
QT_MOC_LITERAL(47, 629, 19), // "onRotateSpeedEdited"
QT_MOC_LITERAL(48, 649, 14), // "onStarsToggled"
QT_MOC_LITERAL(49, 664, 7), // "checked"
QT_MOC_LITERAL(50, 672, 18), // "onSatelliteToggled"
QT_MOC_LITERAL(51, 691, 20), // "onTexturedSkyToggled"
QT_MOC_LITERAL(52, 712, 15), // "onUseFogToggled"
QT_MOC_LITERAL(53, 728, 16), // "onUseHaloToggled"
QT_MOC_LITERAL(54, 745, 22), // "onUseAtmosphereToggled"
QT_MOC_LITERAL(55, 768, 20), // "onRotateStarsToggled"
QT_MOC_LITERAL(56, 789, 18), // "onRotateSkyToggled"
QT_MOC_LITERAL(57, 808, 20), // "onFastTerrainToggled"
QT_MOC_LITERAL(58, 829, 20), // "onShowTerrainToggled"
QT_MOC_LITERAL(59, 850, 18), // "onFlatShadeToggled"
QT_MOC_LITERAL(60, 869, 14), // "onNoLodToggled"
QT_MOC_LITERAL(61, 884, 18), // "onTerrain2dToggled"
QT_MOC_LITERAL(62, 903, 22), // "onShowInvisibleToggled"
QT_MOC_LITERAL(63, 926, 23) // "onNoExtRoomsObjsToggled"

    },
    "TerrainKeypad\0onMoveUp\0\0onMoveDown\0"
    "onRaise10\0onLower10\0onSelectNone\0"
    "onSelectAll\0onMakeMax\0onMakeMin\0"
    "onMakeZero\0onFillArea\0onMoreMoons\0"
    "onLessMoons\0onNextMoon\0onPrevMoon\0"
    "onMoveSatUp\0onMoveSatDown\0onMoveSatLeft\0"
    "onMoveSatRight\0onMoveMoonCloser\0"
    "onMoveMoonFarther\0onRenormalize\0"
    "onRotTexture\0onPyramid\0onPancakes\0"
    "onSmoothTerrain\0onDropTerrain\0"
    "onRandomizeSky\0onSkyNearer\0onSkyFarther\0"
    "onTileMore\0onTileLess\0onRedoTopmap\0"
    "onTerrainOcclusion\0onToggleVisibility\0"
    "onSkyRadio\0onHorizonRadio\0onFogRadio\0"
    "onSatelliteRadio\0onSkyRedEdited\0"
    "onSkyGreenEdited\0onSkyBlueEdited\0"
    "onFogDistanceEdited\0onPixelErrorEdited\0"
    "onFogScalarEdited\0onDamagePerSecEdited\0"
    "onRotateSpeedEdited\0onStarsToggled\0"
    "checked\0onSatelliteToggled\0"
    "onTexturedSkyToggled\0onUseFogToggled\0"
    "onUseHaloToggled\0onUseAtmosphereToggled\0"
    "onRotateStarsToggled\0onRotateSkyToggled\0"
    "onFastTerrainToggled\0onShowTerrainToggled\0"
    "onFlatShadeToggled\0onNoLodToggled\0"
    "onTerrain2dToggled\0onShowInvisibleToggled\0"
    "onNoExtRoomsObjsToggled"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TerrainKeypad[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      61,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,  319,    2, 0x08 /* Private */,
       3,    0,  320,    2, 0x08 /* Private */,
       4,    0,  321,    2, 0x08 /* Private */,
       5,    0,  322,    2, 0x08 /* Private */,
       6,    0,  323,    2, 0x08 /* Private */,
       7,    0,  324,    2, 0x08 /* Private */,
       8,    0,  325,    2, 0x08 /* Private */,
       9,    0,  326,    2, 0x08 /* Private */,
      10,    0,  327,    2, 0x08 /* Private */,
      11,    0,  328,    2, 0x08 /* Private */,
      12,    0,  329,    2, 0x08 /* Private */,
      13,    0,  330,    2, 0x08 /* Private */,
      14,    0,  331,    2, 0x08 /* Private */,
      15,    0,  332,    2, 0x08 /* Private */,
      16,    0,  333,    2, 0x08 /* Private */,
      17,    0,  334,    2, 0x08 /* Private */,
      18,    0,  335,    2, 0x08 /* Private */,
      19,    0,  336,    2, 0x08 /* Private */,
      20,    0,  337,    2, 0x08 /* Private */,
      21,    0,  338,    2, 0x08 /* Private */,
      22,    0,  339,    2, 0x08 /* Private */,
      23,    0,  340,    2, 0x08 /* Private */,
      24,    0,  341,    2, 0x08 /* Private */,
      25,    0,  342,    2, 0x08 /* Private */,
      26,    0,  343,    2, 0x08 /* Private */,
      27,    0,  344,    2, 0x08 /* Private */,
      28,    0,  345,    2, 0x08 /* Private */,
      29,    0,  346,    2, 0x08 /* Private */,
      30,    0,  347,    2, 0x08 /* Private */,
      31,    0,  348,    2, 0x08 /* Private */,
      32,    0,  349,    2, 0x08 /* Private */,
      33,    0,  350,    2, 0x08 /* Private */,
      34,    0,  351,    2, 0x08 /* Private */,
      35,    0,  352,    2, 0x08 /* Private */,
      36,    0,  353,    2, 0x08 /* Private */,
      37,    0,  354,    2, 0x08 /* Private */,
      38,    0,  355,    2, 0x08 /* Private */,
      39,    0,  356,    2, 0x08 /* Private */,
      40,    0,  357,    2, 0x08 /* Private */,
      41,    0,  358,    2, 0x08 /* Private */,
      42,    0,  359,    2, 0x08 /* Private */,
      43,    0,  360,    2, 0x08 /* Private */,
      44,    0,  361,    2, 0x08 /* Private */,
      45,    0,  362,    2, 0x08 /* Private */,
      46,    0,  363,    2, 0x08 /* Private */,
      47,    0,  364,    2, 0x08 /* Private */,
      48,    1,  365,    2, 0x08 /* Private */,
      50,    1,  368,    2, 0x08 /* Private */,
      51,    1,  371,    2, 0x08 /* Private */,
      52,    1,  374,    2, 0x08 /* Private */,
      53,    1,  377,    2, 0x08 /* Private */,
      54,    1,  380,    2, 0x08 /* Private */,
      55,    1,  383,    2, 0x08 /* Private */,
      56,    1,  386,    2, 0x08 /* Private */,
      57,    1,  389,    2, 0x08 /* Private */,
      58,    1,  392,    2, 0x08 /* Private */,
      59,    1,  395,    2, 0x08 /* Private */,
      60,    1,  398,    2, 0x08 /* Private */,
      61,    1,  401,    2, 0x08 /* Private */,
      62,    1,  404,    2, 0x08 /* Private */,
      63,    1,  407,    2, 0x08 /* Private */,

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
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,
    QMetaType::Void, QMetaType::Bool,   49,

       0        // eod
};

void TerrainKeypad::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TerrainKeypad *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->onMoveUp(); break;
        case 1: _t->onMoveDown(); break;
        case 2: _t->onRaise10(); break;
        case 3: _t->onLower10(); break;
        case 4: _t->onSelectNone(); break;
        case 5: _t->onSelectAll(); break;
        case 6: _t->onMakeMax(); break;
        case 7: _t->onMakeMin(); break;
        case 8: _t->onMakeZero(); break;
        case 9: _t->onFillArea(); break;
        case 10: _t->onMoreMoons(); break;
        case 11: _t->onLessMoons(); break;
        case 12: _t->onNextMoon(); break;
        case 13: _t->onPrevMoon(); break;
        case 14: _t->onMoveSatUp(); break;
        case 15: _t->onMoveSatDown(); break;
        case 16: _t->onMoveSatLeft(); break;
        case 17: _t->onMoveSatRight(); break;
        case 18: _t->onMoveMoonCloser(); break;
        case 19: _t->onMoveMoonFarther(); break;
        case 20: _t->onRenormalize(); break;
        case 21: _t->onRotTexture(); break;
        case 22: _t->onPyramid(); break;
        case 23: _t->onPancakes(); break;
        case 24: _t->onSmoothTerrain(); break;
        case 25: _t->onDropTerrain(); break;
        case 26: _t->onRandomizeSky(); break;
        case 27: _t->onSkyNearer(); break;
        case 28: _t->onSkyFarther(); break;
        case 29: _t->onTileMore(); break;
        case 30: _t->onTileLess(); break;
        case 31: _t->onRedoTopmap(); break;
        case 32: _t->onTerrainOcclusion(); break;
        case 33: _t->onToggleVisibility(); break;
        case 34: _t->onSkyRadio(); break;
        case 35: _t->onHorizonRadio(); break;
        case 36: _t->onFogRadio(); break;
        case 37: _t->onSatelliteRadio(); break;
        case 38: _t->onSkyRedEdited(); break;
        case 39: _t->onSkyGreenEdited(); break;
        case 40: _t->onSkyBlueEdited(); break;
        case 41: _t->onFogDistanceEdited(); break;
        case 42: _t->onPixelErrorEdited(); break;
        case 43: _t->onFogScalarEdited(); break;
        case 44: _t->onDamagePerSecEdited(); break;
        case 45: _t->onRotateSpeedEdited(); break;
        case 46: _t->onStarsToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 47: _t->onSatelliteToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 48: _t->onTexturedSkyToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 49: _t->onUseFogToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 50: _t->onUseHaloToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 51: _t->onUseAtmosphereToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 52: _t->onRotateStarsToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 53: _t->onRotateSkyToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 54: _t->onFastTerrainToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 55: _t->onShowTerrainToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 56: _t->onFlatShadeToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 57: _t->onNoLodToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 58: _t->onTerrain2dToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 59: _t->onShowInvisibleToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 60: _t->onNoExtRoomsObjsToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TerrainKeypad::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_TerrainKeypad.data,
    qt_meta_data_TerrainKeypad,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TerrainKeypad::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TerrainKeypad::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TerrainKeypad.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int TerrainKeypad::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 61)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 61;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 61)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 61;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
