/********************************************************************************
** Form generated from reading UI file 'keypads.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KEYPADS_H
#define UI_KEYPADS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QTabWidget>
#include "doorway_keypad.h"
#include "level_keypad.h"
#include "lighting_keypad.h"
#include "matcen_keypad.h"
#include "megacell_keypad.h"
#include "object_keypad.h"
#include "path_keypad.h"
#include "room_keypad.h"
#include "terrain_keypad.h"
#include "texture_keypad.h"
#include "trigger_keypad.h"

QT_BEGIN_NAMESPACE

class Ui_KeypadsTabWidget
{
public:
    MegacellKeypad *tab_megacell;
    DoorwayKeypad *tab_doorway;
    TriggerKeypad *tab_trigger;
    PathKeypad *tab_path;
    RoomKeypad *tab_room;
    ObjectKeypad *tab_object;
    LevelKeypad *tab_level;
    LightingKeypad *tab_lighting;
    MatcenKeypad *tab_matcen;
    TerrainKeypad *tab_terrain;
    TextureKeypad *tab_texture;

    void setupUi(QTabWidget *keypadTabs)
    {
        if (keypadTabs->objectName().isEmpty())
            keypadTabs->setObjectName(QString::fromUtf8("keypadTabs"));
        keypadTabs->resize(302, 712);
        tab_megacell = new MegacellKeypad();
        tab_megacell->setObjectName(QString::fromUtf8("tab_megacell"));
        keypadTabs->addTab(tab_megacell, QString());
        tab_doorway = new DoorwayKeypad();
        tab_doorway->setObjectName(QString::fromUtf8("tab_doorway"));
        keypadTabs->addTab(tab_doorway, QString());
        tab_trigger = new TriggerKeypad();
        tab_trigger->setObjectName(QString::fromUtf8("tab_trigger"));
        keypadTabs->addTab(tab_trigger, QString());
        tab_path = new PathKeypad();
        tab_path->setObjectName(QString::fromUtf8("tab_path"));
        keypadTabs->addTab(tab_path, QString());
        tab_room = new RoomKeypad();
        tab_room->setObjectName(QString::fromUtf8("tab_room"));
        keypadTabs->addTab(tab_room, QString());
        tab_object = new ObjectKeypad();
        tab_object->setObjectName(QString::fromUtf8("tab_object"));
        keypadTabs->addTab(tab_object, QString());
        tab_level = new LevelKeypad();
        tab_level->setObjectName(QString::fromUtf8("tab_level"));
        keypadTabs->addTab(tab_level, QString());
        tab_lighting = new LightingKeypad();
        tab_lighting->setObjectName(QString::fromUtf8("tab_lighting"));
        keypadTabs->addTab(tab_lighting, QString());
        tab_matcen = new MatcenKeypad();
        tab_matcen->setObjectName(QString::fromUtf8("tab_matcen"));
        keypadTabs->addTab(tab_matcen, QString());
        tab_terrain = new TerrainKeypad();
        tab_terrain->setObjectName(QString::fromUtf8("tab_terrain"));
        keypadTabs->addTab(tab_terrain, QString());
        tab_texture = new TextureKeypad();
        tab_texture->setObjectName(QString::fromUtf8("tab_texture"));
        keypadTabs->addTab(tab_texture, QString());

        retranslateUi(keypadTabs);

        keypadTabs->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(keypadTabs);
    } // setupUi

    void retranslateUi(QTabWidget *keypadTabs)
    {
        keypadTabs->setTabText(keypadTabs->indexOf(tab_megacell), QCoreApplication::translate("KeypadsTabWidget", "Megacells", nullptr));
        keypadTabs->setTabText(keypadTabs->indexOf(tab_doorway), QCoreApplication::translate("KeypadsTabWidget", "Doorways", nullptr));
        keypadTabs->setTabText(keypadTabs->indexOf(tab_trigger), QCoreApplication::translate("KeypadsTabWidget", "Triggers", nullptr));
        keypadTabs->setTabText(keypadTabs->indexOf(tab_path), QCoreApplication::translate("KeypadsTabWidget", "Paths", nullptr));
        keypadTabs->setTabText(keypadTabs->indexOf(tab_room), QCoreApplication::translate("KeypadsTabWidget", "Rooms", nullptr));
        keypadTabs->setTabText(keypadTabs->indexOf(tab_object), QCoreApplication::translate("KeypadsTabWidget", "Objects", nullptr));
        keypadTabs->setTabText(keypadTabs->indexOf(tab_level), QCoreApplication::translate("KeypadsTabWidget", "Level", nullptr));
        keypadTabs->setTabText(keypadTabs->indexOf(tab_lighting), QCoreApplication::translate("KeypadsTabWidget", "Lighting", nullptr));
        keypadTabs->setTabText(keypadTabs->indexOf(tab_matcen), QCoreApplication::translate("KeypadsTabWidget", "Matcens", nullptr));
        keypadTabs->setTabText(keypadTabs->indexOf(tab_terrain), QCoreApplication::translate("KeypadsTabWidget", "Terrain", nullptr));
        keypadTabs->setTabText(keypadTabs->indexOf(tab_texture), QCoreApplication::translate("KeypadsTabWidget", "Textures", nullptr));
        (void)keypadTabs;
    } // retranslateUi

};

namespace Ui {
    class KeypadsTabWidget: public Ui_KeypadsTabWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KEYPADS_H
