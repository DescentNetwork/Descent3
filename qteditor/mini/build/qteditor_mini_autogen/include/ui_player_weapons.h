/********************************************************************************
** Form generated from reading UI file 'player_weapons.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PLAYER_WEAPONS_H
#define UI_PLAYER_WEAPONS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_PlayerWeaponsDialog
{
public:
    QAction *IDD_ORPHANHUNTER;
    QAction *IDD_REORDER_PAGES;
    QAction *IDD_SHOW_ALL_CHECKED_OUT;
    QAction *IDM_IMPORT_BITMAP;
    QAction *ID_APP_ABOUT;
    QAction *ID_APP_EXIT;
    QAction *ID_BRIEFING_EDITOR;
    QAction *ID_BUTTON_LIGHTING;
    QAction *ID_BUTTON_OUTLINE;
    QAction *ID_BUTTON_WINDOWSELECTION;
    QAction *ID_D3HELP;
    QAction *ID_EDITORS_AIPROPERTIES;
    QAction *ID_EDITORS_AMBIENTSOUNDS;
    QAction *ID_EDITORS_DALLAS;
    QAction *ID_EDITORS_FILES;
    QAction *ID_EDITORS_MEGACELLS;
    QAction *ID_EDIT_ADDSELECT;
    QAction *ID_EDIT_ATTACH;
    QAction *ID_EDIT_CLEARSELECTED;
    QAction *ID_EDIT_COPY;
    QAction *ID_EDIT_CUT;
    QAction *ID_EDIT_DELETE;
    QAction *ID_EDIT_LOADSCRAP;
    QAction *ID_EDIT_PASTE;
    QAction *ID_EDIT_PLACE;
    QAction *ID_EDIT_PLACE_TERRAIN;
    QAction *ID_EDIT_REMOVESELECT;
    QAction *ID_EDIT_SAVESCRAP;
    QAction *ID_EDIT_SELECTATTACHED;
    QAction *ID_EDIT_UNDO;
    QAction *ID_FILE_AUTOSAVE;
    QAction *ID_FILE_CHECKMINE;
    QAction *ID_FILE_FIXCRACKS;
    QAction *ID_FILE_FIXDEGENERATEFACES;
    QAction *ID_FILE_IMPORT_ROOM;
    QAction *ID_FILE_INFORMATION;
    QAction *ID_FILE_LEAVE_EDITOR;
    QAction *ID_FILE_LEVELPROPS;
    QAction *ID_FILE_LEVEL_INFO;
    QAction *ID_FILE_MRU_FILE1;
    QAction *ID_FILE_NEW;
    QAction *ID_FILE_OPEN;
    QAction *ID_FILE_PLAY640X480;
    QAction *ID_FILE_PREFERENCES;
    QAction *ID_FILE_PRINT;
    QAction *ID_FILE_PRINT_PREVIEW;
    QAction *ID_FILE_PRINT_SETUP;
    QAction *ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM;
    QAction *ID_FILE_REMOVEEXTRAPOINTS;
    QAction *ID_FILE_RESTOREGAMESTATE;
    QAction *ID_FILE_SAVE;
    QAction *ID_FILE_SAVEGOALTEXT;
    QAction *ID_FILE_SAVE_AS;
    QAction *ID_FILE_SAVE_COPY_AS;
    QAction *ID_FILE_STATS;
    QAction *ID_FILE_UPDATE;
    QAction *ID_FILE_VERIFY_LEVEL;
    QAction *ID_HOTSPOT_TGA;
    QAction *ID_MINE_VIEW;
    QAction *ID_OBJBUTTON;
    QAction *ID_OBJECT_DELETEOBJECT;
    QAction *ID_OBJECT_MOVEPLAYER;
    QAction *ID_OBJECT_PLACECAMERAATCURRENTFACE;
    QAction *ID_OBJECT_PLACECAMERAATVIEWER;
    QAction *ID_OBJECT_PLACEOBJECT;
    QAction *ID_OBJECT_PLACESOUNDSOURCEATVIEWER;
    QAction *ID_OBJECT_PLACEWAYPOINTATVIEWER;
    QAction *ID_OBJECT_SELECTBYNUMBER;
    QAction *ID_OBJECT_SETCAMERAFROMVIEWER;
    QAction *ID_OBJECT_SETVIEWERFROMCAMERA;
    QAction *ID_OSIRISCOMPILE;
    QAction *ID_REINIT_OPENGL;
    QAction *ID_ROOM_ADD;
    QAction *ID_ROOM_ADDVERTTONEWFACE;
    QAction *ID_ROOM_ATTACHROOM;
    QAction *ID_ROOM_BUILDBRIDGE;
    QAction *ID_ROOM_BUILDSMOOTHBRIDGE;
    QAction *ID_ROOM_CLEARCENTERPOINT;
    QAction *ID_ROOM_COMBINE;
    QAction *ID_ROOM_DELETE;
    QAction *ID_ROOM_DELETECONNECTEDFACES;
    QAction *ID_ROOM_DELETEFACE;
    QAction *ID_ROOM_DELETEPORTAL;
    QAction *ID_ROOM_DELETEVERT;
    QAction *ID_ROOM_DROPROOM;
    QAction *ID_ROOM_FACE_ADDVERTTOEDGE;
    QAction *ID_ROOM_FACE_DELETEVERTONEDGE;
    QAction *ID_ROOM_FACE_MOVEVERTONEDGE;
    QAction *ID_ROOM_FACE_SPLITFACE;
    QAction *ID_ROOM_FINISHNEWFACE;
    QAction *ID_ROOM_GRABTEXTURE;
    QAction *ID_ROOM_JOINROOMS;
    QAction *ID_ROOM_JOINROOMSEXACT;
    QAction *ID_ROOM_JOIN_ADJACENT_FACES;
    QAction *ID_ROOM_LINKTONEWEXTERNAL;
    QAction *ID_ROOM_MARK;
    QAction *ID_ROOM_MERGEOBJECTINTOROOM;
    QAction *ID_ROOM_NEXTFACE;
    QAction *ID_ROOM_PLACEROOM;
    QAction *ID_ROOM_PLACETERRAINROOM;
    QAction *ID_ROOM_PREVIOUSFACE;
    QAction *ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES;
    QAction *ID_ROOM_PROPAGATETOALL;
    QAction *ID_ROOM_RENAMEROOM;
    QAction *ID_ROOM_ROTATEPLACEDROOM45DEGREES;
    QAction *ID_ROOM_SAVECURRENTROOM;
    QAction *ID_ROOM_SELECTBYNUMBER;
    QAction *ID_ROOM_SELECTFACEBYNUMBER;
    QAction *ID_ROOM_SETCENTERFROMVIEWER;
    QAction *ID_ROOM_SNAPPLACEDROOM;
    QAction *ID_ROOM_SNAPPOINTTOEDGE;
    QAction *ID_ROOM_SNAPPOINTTOFACE;
    QAction *ID_ROOM_SNAPPOINTTOPOINT;
    QAction *ID_ROOM_SPLITFACE;
    QAction *ID_ROOM_STARTNEWFACE;
    QAction *ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE;
    QAction *ID_ROOM_UNDOSNAP;
    QAction *ID_ROOM_UNPLACEROOM;
    QAction *ID_ROOM_VIEW;
    QAction *ID_SCRIPT_LEVEL_INTERFACE;
    QAction *ID_SUBEDITORS_FONT;
    QAction *ID_SUBEDITORS_HOGMAKER;
    QAction *ID_SUBEDITORS_TABLEFILEEDIT;
    QAction *ID_SUBEDITORS_TABLEFILEFILTER;
    QAction *ID_TABLEEDIT_FILE_EXIT;
    QAction *ID_TABLEEDIT_FILE_LOAD;
    QAction *ID_TABLEEDIT_FILE_SAVE;
    QAction *ID_TABLEEDIT_FILE_SAVE_AS;
    QAction *ID_TABLEFILTER_FILE_EXIT;
    QAction *ID_TABLEFILTER_FILE_LOAD;
    QAction *ID_TABLEFILTER_FILE_NEW;
    QAction *ID_TABLEFILTER_FILE_SAVE;
    QAction *ID_TABLEFILTER_FILE_SAVEAS;
    QAction *ID_TERRAIN_VIEW;
    QAction *ID_TEST_TEST1;
    QAction *ID_TEST_TEST2;
    QAction *ID_TEST_TEST3;
    QAction *ID_TOOLS_WORLD_OBJECTS_BUILDINGS;
    QAction *ID_TOOLS_WORLD_OBJECTS_CLUTTER;
    QAction *ID_TOOLS_WORLD_OBJECTS_DOOR;
    QAction *ID_TOOLS_WORLD_OBJECTS_LIGHTS;
    QAction *ID_TOOLS_WORLD_OBJECTS_PLAYER;
    QAction *ID_TOOLS_WORLD_OBJECTS_POWERUPS;
    QAction *ID_TOOLS_WORLD_OBJECTS_ROBOTS;
    QAction *ID_TOOLS_WORLD_OBJECTS_SOUND;
    QAction *ID_TOOLS_WORLD_TEXTURES;
    QAction *ID_TOOLS_WORLD_WEAPONS;
    QAction *ID_VIEW_CENTERONCUBE;
    QAction *ID_VIEW_CENTERONMINE;
    QAction *ID_VIEW_CENTERONOBJECT;
    QAction *ID_VIEW_DELETEVIEWER;
    QAction *ID_VIEW_FLIP;
    QAction *ID_VIEW_KEYPAD_TOGGLE;
    QAction *ID_VIEW_MOVECAMERATOCURRENTOBJECT;
    QAction *ID_VIEW_MOVECAMERATOSELECTEDFACE;
    QAction *ID_VIEW_MOVECAMERATOSELECTEDROOM;
    QAction *ID_VIEW_NEWVIEWER;
    QAction *ID_VIEW_NEXTVIEWER;
    QAction *ID_VIEW_RESETVIEWRADIUS;
    QAction *ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW;
    QAction *ID_VIEW_SHOWVIEWERFORWARDVECTOR;
    QAction *ID_VIEW_STATUS_BAR;
    QAction *ID_VIEW_TEXTUREMINE;
    QAction *ID_VIEW_TOOLBAR;
    QAction *ID_VIEW_VIEWPROP;
    QAction *ID_VIEW_WIREFRAMEMINE;
    QAction *ID_WINDOW_CASCADE;
    QAction *ID_WINDOW_TILE;
    QAction *ID_ZBUTTON;
    QGroupBox *IDC_STATIC;
    QCheckBox *IDC_FIRES_FUSION;
    QCheckBox *IDC_ONOFF;
    QCheckBox *IDC_ZOOM;
    QCheckBox *IDC_CONTINUOUS_FIRING_SOUND;
    QComboBox *IDC_FIRING_SOUND_PULLDOWN;
    QCheckBox *IDC_FIRING_RELEASE_SOUND;
    QComboBox *IDC_RELEASE_SOUND_PULLDOWN;
    QLabel *IDC_STATIC_2;
    QComboBox *IDC_SPEW_POWERUP_PULLDOWN;
    QLabel *IDC_STATIC_3;
    QLineEdit *IDC_MAX_AMMO;
    QCheckBox *IDC_SHOW_TENTHS;
    QLabel *IDC_STATIC_4;
    QComboBox *IDC_CURRENT_WEAPON_BATTERY_COMBO;
    QPushButton *IDC_EDIT_WB_BUTTON;
    QPushButton *IDOK;

    void setupUi(QDialog *IDD_PLAYER_WEAPONS)
    {
        if (IDD_PLAYER_WEAPONS->objectName().isEmpty())
            IDD_PLAYER_WEAPONS->setObjectName(QString::fromUtf8("IDD_PLAYER_WEAPONS"));
        IDD_PLAYER_WEAPONS->resize(247, 462);
        QFont font;
        font.setFamily(QString::fromUtf8("Liberation Sans"));
        font.setPointSize(8);
        font.setBold(false);
        font.setItalic(false);
        IDD_PLAYER_WEAPONS->setFont(font);
        IDD_PLAYER_WEAPONS->setEnabled(true);
        IDD_PLAYER_WEAPONS->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
        IDD_PLAYER_WEAPONS->setMinimumSize(QSize(247, 462));
        IDD_PLAYER_WEAPONS->setMaximumSize(QSize(247, 462));
        IDD_ORPHANHUNTER = new QAction(IDD_PLAYER_WEAPONS);
        IDD_ORPHANHUNTER->setObjectName(QString::fromUtf8("IDD_ORPHANHUNTER"));
        IDD_REORDER_PAGES = new QAction(IDD_PLAYER_WEAPONS);
        IDD_REORDER_PAGES->setObjectName(QString::fromUtf8("IDD_REORDER_PAGES"));
        IDD_SHOW_ALL_CHECKED_OUT = new QAction(IDD_PLAYER_WEAPONS);
        IDD_SHOW_ALL_CHECKED_OUT->setObjectName(QString::fromUtf8("IDD_SHOW_ALL_CHECKED_OUT"));
        IDM_IMPORT_BITMAP = new QAction(IDD_PLAYER_WEAPONS);
        IDM_IMPORT_BITMAP->setObjectName(QString::fromUtf8("IDM_IMPORT_BITMAP"));
        ID_APP_ABOUT = new QAction(IDD_PLAYER_WEAPONS);
        ID_APP_ABOUT->setObjectName(QString::fromUtf8("ID_APP_ABOUT"));
        ID_APP_EXIT = new QAction(IDD_PLAYER_WEAPONS);
        ID_APP_EXIT->setObjectName(QString::fromUtf8("ID_APP_EXIT"));
        ID_BRIEFING_EDITOR = new QAction(IDD_PLAYER_WEAPONS);
        ID_BRIEFING_EDITOR->setObjectName(QString::fromUtf8("ID_BRIEFING_EDITOR"));
        ID_BUTTON_LIGHTING = new QAction(IDD_PLAYER_WEAPONS);
        ID_BUTTON_LIGHTING->setObjectName(QString::fromUtf8("ID_BUTTON_LIGHTING"));
        ID_BUTTON_OUTLINE = new QAction(IDD_PLAYER_WEAPONS);
        ID_BUTTON_OUTLINE->setObjectName(QString::fromUtf8("ID_BUTTON_OUTLINE"));
        ID_BUTTON_WINDOWSELECTION = new QAction(IDD_PLAYER_WEAPONS);
        ID_BUTTON_WINDOWSELECTION->setObjectName(QString::fromUtf8("ID_BUTTON_WINDOWSELECTION"));
        ID_D3HELP = new QAction(IDD_PLAYER_WEAPONS);
        ID_D3HELP->setObjectName(QString::fromUtf8("ID_D3HELP"));
        ID_EDITORS_AIPROPERTIES = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDITORS_AIPROPERTIES->setObjectName(QString::fromUtf8("ID_EDITORS_AIPROPERTIES"));
        ID_EDITORS_AMBIENTSOUNDS = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDITORS_AMBIENTSOUNDS->setObjectName(QString::fromUtf8("ID_EDITORS_AMBIENTSOUNDS"));
        ID_EDITORS_DALLAS = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDITORS_DALLAS->setObjectName(QString::fromUtf8("ID_EDITORS_DALLAS"));
        ID_EDITORS_FILES = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDITORS_FILES->setObjectName(QString::fromUtf8("ID_EDITORS_FILES"));
        ID_EDITORS_MEGACELLS = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDITORS_MEGACELLS->setObjectName(QString::fromUtf8("ID_EDITORS_MEGACELLS"));
        ID_EDIT_ADDSELECT = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_ADDSELECT->setObjectName(QString::fromUtf8("ID_EDIT_ADDSELECT"));
        ID_EDIT_ATTACH = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_ATTACH->setObjectName(QString::fromUtf8("ID_EDIT_ATTACH"));
        ID_EDIT_CLEARSELECTED = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_CLEARSELECTED->setObjectName(QString::fromUtf8("ID_EDIT_CLEARSELECTED"));
        ID_EDIT_COPY = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_COPY->setObjectName(QString::fromUtf8("ID_EDIT_COPY"));
        ID_EDIT_CUT = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_CUT->setObjectName(QString::fromUtf8("ID_EDIT_CUT"));
        ID_EDIT_DELETE = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_DELETE->setObjectName(QString::fromUtf8("ID_EDIT_DELETE"));
        ID_EDIT_LOADSCRAP = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_LOADSCRAP->setObjectName(QString::fromUtf8("ID_EDIT_LOADSCRAP"));
        ID_EDIT_PASTE = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_PASTE->setObjectName(QString::fromUtf8("ID_EDIT_PASTE"));
        ID_EDIT_PLACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_PLACE->setObjectName(QString::fromUtf8("ID_EDIT_PLACE"));
        ID_EDIT_PLACE_TERRAIN = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_PLACE_TERRAIN->setObjectName(QString::fromUtf8("ID_EDIT_PLACE_TERRAIN"));
        ID_EDIT_REMOVESELECT = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_REMOVESELECT->setObjectName(QString::fromUtf8("ID_EDIT_REMOVESELECT"));
        ID_EDIT_SAVESCRAP = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_SAVESCRAP->setObjectName(QString::fromUtf8("ID_EDIT_SAVESCRAP"));
        ID_EDIT_SELECTATTACHED = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_SELECTATTACHED->setObjectName(QString::fromUtf8("ID_EDIT_SELECTATTACHED"));
        ID_EDIT_UNDO = new QAction(IDD_PLAYER_WEAPONS);
        ID_EDIT_UNDO->setObjectName(QString::fromUtf8("ID_EDIT_UNDO"));
        ID_FILE_AUTOSAVE = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_AUTOSAVE->setObjectName(QString::fromUtf8("ID_FILE_AUTOSAVE"));
        ID_FILE_AUTOSAVE->setCheckable(true);
        ID_FILE_CHECKMINE = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_CHECKMINE->setObjectName(QString::fromUtf8("ID_FILE_CHECKMINE"));
        ID_FILE_FIXCRACKS = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_FIXCRACKS->setObjectName(QString::fromUtf8("ID_FILE_FIXCRACKS"));
        ID_FILE_FIXDEGENERATEFACES = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_FIXDEGENERATEFACES->setObjectName(QString::fromUtf8("ID_FILE_FIXDEGENERATEFACES"));
        ID_FILE_IMPORT_ROOM = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_IMPORT_ROOM->setObjectName(QString::fromUtf8("ID_FILE_IMPORT_ROOM"));
        ID_FILE_INFORMATION = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_INFORMATION->setObjectName(QString::fromUtf8("ID_FILE_INFORMATION"));
        ID_FILE_LEAVE_EDITOR = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_LEAVE_EDITOR->setObjectName(QString::fromUtf8("ID_FILE_LEAVE_EDITOR"));
        ID_FILE_LEVELPROPS = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_LEVELPROPS->setObjectName(QString::fromUtf8("ID_FILE_LEVELPROPS"));
        ID_FILE_LEVEL_INFO = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_LEVEL_INFO->setObjectName(QString::fromUtf8("ID_FILE_LEVEL_INFO"));
        ID_FILE_MRU_FILE1 = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_MRU_FILE1->setObjectName(QString::fromUtf8("ID_FILE_MRU_FILE1"));
        ID_FILE_MRU_FILE1->setEnabled(false);
        ID_FILE_NEW = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_NEW->setObjectName(QString::fromUtf8("ID_FILE_NEW"));
        ID_FILE_OPEN = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_OPEN->setObjectName(QString::fromUtf8("ID_FILE_OPEN"));
        ID_FILE_PLAY640X480 = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_PLAY640X480->setObjectName(QString::fromUtf8("ID_FILE_PLAY640X480"));
        ID_FILE_PREFERENCES = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_PREFERENCES->setObjectName(QString::fromUtf8("ID_FILE_PREFERENCES"));
        ID_FILE_PRINT = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_PRINT->setObjectName(QString::fromUtf8("ID_FILE_PRINT"));
        ID_FILE_PRINT_PREVIEW = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_PRINT_PREVIEW->setObjectName(QString::fromUtf8("ID_FILE_PRINT_PREVIEW"));
        ID_FILE_PRINT_SETUP = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_PRINT_SETUP->setObjectName(QString::fromUtf8("ID_FILE_PRINT_SETUP"));
        ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM->setObjectName(QString::fromUtf8("ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM"));
        ID_FILE_REMOVEEXTRAPOINTS = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_REMOVEEXTRAPOINTS->setObjectName(QString::fromUtf8("ID_FILE_REMOVEEXTRAPOINTS"));
        ID_FILE_RESTOREGAMESTATE = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_RESTOREGAMESTATE->setObjectName(QString::fromUtf8("ID_FILE_RESTOREGAMESTATE"));
        ID_FILE_SAVE = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_SAVE->setObjectName(QString::fromUtf8("ID_FILE_SAVE"));
        ID_FILE_SAVEGOALTEXT = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_SAVEGOALTEXT->setObjectName(QString::fromUtf8("ID_FILE_SAVEGOALTEXT"));
        ID_FILE_SAVE_AS = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_SAVE_AS->setObjectName(QString::fromUtf8("ID_FILE_SAVE_AS"));
        ID_FILE_SAVE_COPY_AS = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_SAVE_COPY_AS->setObjectName(QString::fromUtf8("ID_FILE_SAVE_COPY_AS"));
        ID_FILE_STATS = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_STATS->setObjectName(QString::fromUtf8("ID_FILE_STATS"));
        ID_FILE_UPDATE = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_UPDATE->setObjectName(QString::fromUtf8("ID_FILE_UPDATE"));
        ID_FILE_VERIFY_LEVEL = new QAction(IDD_PLAYER_WEAPONS);
        ID_FILE_VERIFY_LEVEL->setObjectName(QString::fromUtf8("ID_FILE_VERIFY_LEVEL"));
        ID_HOTSPOT_TGA = new QAction(IDD_PLAYER_WEAPONS);
        ID_HOTSPOT_TGA->setObjectName(QString::fromUtf8("ID_HOTSPOT_TGA"));
        ID_MINE_VIEW = new QAction(IDD_PLAYER_WEAPONS);
        ID_MINE_VIEW->setObjectName(QString::fromUtf8("ID_MINE_VIEW"));
        ID_OBJBUTTON = new QAction(IDD_PLAYER_WEAPONS);
        ID_OBJBUTTON->setObjectName(QString::fromUtf8("ID_OBJBUTTON"));
        ID_OBJECT_DELETEOBJECT = new QAction(IDD_PLAYER_WEAPONS);
        ID_OBJECT_DELETEOBJECT->setObjectName(QString::fromUtf8("ID_OBJECT_DELETEOBJECT"));
        ID_OBJECT_MOVEPLAYER = new QAction(IDD_PLAYER_WEAPONS);
        ID_OBJECT_MOVEPLAYER->setObjectName(QString::fromUtf8("ID_OBJECT_MOVEPLAYER"));
        ID_OBJECT_PLACECAMERAATCURRENTFACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_OBJECT_PLACECAMERAATCURRENTFACE->setObjectName(QString::fromUtf8("ID_OBJECT_PLACECAMERAATCURRENTFACE"));
        ID_OBJECT_PLACECAMERAATVIEWER = new QAction(IDD_PLAYER_WEAPONS);
        ID_OBJECT_PLACECAMERAATVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_PLACECAMERAATVIEWER"));
        ID_OBJECT_PLACEOBJECT = new QAction(IDD_PLAYER_WEAPONS);
        ID_OBJECT_PLACEOBJECT->setObjectName(QString::fromUtf8("ID_OBJECT_PLACEOBJECT"));
        ID_OBJECT_PLACESOUNDSOURCEATVIEWER = new QAction(IDD_PLAYER_WEAPONS);
        ID_OBJECT_PLACESOUNDSOURCEATVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_PLACESOUNDSOURCEATVIEWER"));
        ID_OBJECT_PLACEWAYPOINTATVIEWER = new QAction(IDD_PLAYER_WEAPONS);
        ID_OBJECT_PLACEWAYPOINTATVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_PLACEWAYPOINTATVIEWER"));
        ID_OBJECT_SELECTBYNUMBER = new QAction(IDD_PLAYER_WEAPONS);
        ID_OBJECT_SELECTBYNUMBER->setObjectName(QString::fromUtf8("ID_OBJECT_SELECTBYNUMBER"));
        ID_OBJECT_SETCAMERAFROMVIEWER = new QAction(IDD_PLAYER_WEAPONS);
        ID_OBJECT_SETCAMERAFROMVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_SETCAMERAFROMVIEWER"));
        ID_OBJECT_SETVIEWERFROMCAMERA = new QAction(IDD_PLAYER_WEAPONS);
        ID_OBJECT_SETVIEWERFROMCAMERA->setObjectName(QString::fromUtf8("ID_OBJECT_SETVIEWERFROMCAMERA"));
        ID_OSIRISCOMPILE = new QAction(IDD_PLAYER_WEAPONS);
        ID_OSIRISCOMPILE->setObjectName(QString::fromUtf8("ID_OSIRISCOMPILE"));
        ID_REINIT_OPENGL = new QAction(IDD_PLAYER_WEAPONS);
        ID_REINIT_OPENGL->setObjectName(QString::fromUtf8("ID_REINIT_OPENGL"));
        ID_ROOM_ADD = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_ADD->setObjectName(QString::fromUtf8("ID_ROOM_ADD"));
        ID_ROOM_ADDVERTTONEWFACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_ADDVERTTONEWFACE->setObjectName(QString::fromUtf8("ID_ROOM_ADDVERTTONEWFACE"));
        ID_ROOM_ATTACHROOM = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_ATTACHROOM->setObjectName(QString::fromUtf8("ID_ROOM_ATTACHROOM"));
        ID_ROOM_BUILDBRIDGE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_BUILDBRIDGE->setObjectName(QString::fromUtf8("ID_ROOM_BUILDBRIDGE"));
        ID_ROOM_BUILDSMOOTHBRIDGE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_BUILDSMOOTHBRIDGE->setObjectName(QString::fromUtf8("ID_ROOM_BUILDSMOOTHBRIDGE"));
        ID_ROOM_CLEARCENTERPOINT = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_CLEARCENTERPOINT->setObjectName(QString::fromUtf8("ID_ROOM_CLEARCENTERPOINT"));
        ID_ROOM_COMBINE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_COMBINE->setObjectName(QString::fromUtf8("ID_ROOM_COMBINE"));
        ID_ROOM_DELETE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_DELETE->setObjectName(QString::fromUtf8("ID_ROOM_DELETE"));
        ID_ROOM_DELETECONNECTEDFACES = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_DELETECONNECTEDFACES->setObjectName(QString::fromUtf8("ID_ROOM_DELETECONNECTEDFACES"));
        ID_ROOM_DELETEFACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_DELETEFACE->setObjectName(QString::fromUtf8("ID_ROOM_DELETEFACE"));
        ID_ROOM_DELETEPORTAL = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_DELETEPORTAL->setObjectName(QString::fromUtf8("ID_ROOM_DELETEPORTAL"));
        ID_ROOM_DELETEVERT = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_DELETEVERT->setObjectName(QString::fromUtf8("ID_ROOM_DELETEVERT"));
        ID_ROOM_DROPROOM = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_DROPROOM->setObjectName(QString::fromUtf8("ID_ROOM_DROPROOM"));
        ID_ROOM_FACE_ADDVERTTOEDGE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_FACE_ADDVERTTOEDGE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_ADDVERTTOEDGE"));
        ID_ROOM_FACE_DELETEVERTONEDGE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_FACE_DELETEVERTONEDGE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_DELETEVERTONEDGE"));
        ID_ROOM_FACE_MOVEVERTONEDGE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_FACE_MOVEVERTONEDGE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_MOVEVERTONEDGE"));
        ID_ROOM_FACE_SPLITFACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_FACE_SPLITFACE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_SPLITFACE"));
        ID_ROOM_FINISHNEWFACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_FINISHNEWFACE->setObjectName(QString::fromUtf8("ID_ROOM_FINISHNEWFACE"));
        ID_ROOM_GRABTEXTURE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_GRABTEXTURE->setObjectName(QString::fromUtf8("ID_ROOM_GRABTEXTURE"));
        ID_ROOM_JOINROOMS = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_JOINROOMS->setObjectName(QString::fromUtf8("ID_ROOM_JOINROOMS"));
        ID_ROOM_JOINROOMSEXACT = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_JOINROOMSEXACT->setObjectName(QString::fromUtf8("ID_ROOM_JOINROOMSEXACT"));
        ID_ROOM_JOIN_ADJACENT_FACES = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_JOIN_ADJACENT_FACES->setObjectName(QString::fromUtf8("ID_ROOM_JOIN_ADJACENT_FACES"));
        ID_ROOM_LINKTONEWEXTERNAL = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_LINKTONEWEXTERNAL->setObjectName(QString::fromUtf8("ID_ROOM_LINKTONEWEXTERNAL"));
        ID_ROOM_MARK = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_MARK->setObjectName(QString::fromUtf8("ID_ROOM_MARK"));
        ID_ROOM_MERGEOBJECTINTOROOM = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_MERGEOBJECTINTOROOM->setObjectName(QString::fromUtf8("ID_ROOM_MERGEOBJECTINTOROOM"));
        ID_ROOM_NEXTFACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_NEXTFACE->setObjectName(QString::fromUtf8("ID_ROOM_NEXTFACE"));
        ID_ROOM_PLACEROOM = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_PLACEROOM->setObjectName(QString::fromUtf8("ID_ROOM_PLACEROOM"));
        ID_ROOM_PLACETERRAINROOM = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_PLACETERRAINROOM->setObjectName(QString::fromUtf8("ID_ROOM_PLACETERRAINROOM"));
        ID_ROOM_PREVIOUSFACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_PREVIOUSFACE->setObjectName(QString::fromUtf8("ID_ROOM_PREVIOUSFACE"));
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES->setObjectName(QString::fromUtf8("ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES"));
        ID_ROOM_PROPAGATETOALL = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_PROPAGATETOALL->setObjectName(QString::fromUtf8("ID_ROOM_PROPAGATETOALL"));
        ID_ROOM_RENAMEROOM = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_RENAMEROOM->setObjectName(QString::fromUtf8("ID_ROOM_RENAMEROOM"));
        ID_ROOM_ROTATEPLACEDROOM45DEGREES = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_ROTATEPLACEDROOM45DEGREES->setObjectName(QString::fromUtf8("ID_ROOM_ROTATEPLACEDROOM45DEGREES"));
        ID_ROOM_SAVECURRENTROOM = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_SAVECURRENTROOM->setObjectName(QString::fromUtf8("ID_ROOM_SAVECURRENTROOM"));
        ID_ROOM_SELECTBYNUMBER = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_SELECTBYNUMBER->setObjectName(QString::fromUtf8("ID_ROOM_SELECTBYNUMBER"));
        ID_ROOM_SELECTFACEBYNUMBER = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_SELECTFACEBYNUMBER->setObjectName(QString::fromUtf8("ID_ROOM_SELECTFACEBYNUMBER"));
        ID_ROOM_SETCENTERFROMVIEWER = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_SETCENTERFROMVIEWER->setObjectName(QString::fromUtf8("ID_ROOM_SETCENTERFROMVIEWER"));
        ID_ROOM_SNAPPLACEDROOM = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_SNAPPLACEDROOM->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPLACEDROOM"));
        ID_ROOM_SNAPPOINTTOEDGE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_SNAPPOINTTOEDGE->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPOINTTOEDGE"));
        ID_ROOM_SNAPPOINTTOFACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_SNAPPOINTTOFACE->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPOINTTOFACE"));
        ID_ROOM_SNAPPOINTTOPOINT = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_SNAPPOINTTOPOINT->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPOINTTOPOINT"));
        ID_ROOM_SPLITFACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_SPLITFACE->setObjectName(QString::fromUtf8("ID_ROOM_SPLITFACE"));
        ID_ROOM_STARTNEWFACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_STARTNEWFACE->setObjectName(QString::fromUtf8("ID_ROOM_STARTNEWFACE"));
        ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE->setObjectName(QString::fromUtf8("ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE"));
        ID_ROOM_UNDOSNAP = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_UNDOSNAP->setObjectName(QString::fromUtf8("ID_ROOM_UNDOSNAP"));
        ID_ROOM_UNPLACEROOM = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_UNPLACEROOM->setObjectName(QString::fromUtf8("ID_ROOM_UNPLACEROOM"));
        ID_ROOM_VIEW = new QAction(IDD_PLAYER_WEAPONS);
        ID_ROOM_VIEW->setObjectName(QString::fromUtf8("ID_ROOM_VIEW"));
        ID_SCRIPT_LEVEL_INTERFACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_SCRIPT_LEVEL_INTERFACE->setObjectName(QString::fromUtf8("ID_SCRIPT_LEVEL_INTERFACE"));
        ID_SUBEDITORS_FONT = new QAction(IDD_PLAYER_WEAPONS);
        ID_SUBEDITORS_FONT->setObjectName(QString::fromUtf8("ID_SUBEDITORS_FONT"));
        ID_SUBEDITORS_HOGMAKER = new QAction(IDD_PLAYER_WEAPONS);
        ID_SUBEDITORS_HOGMAKER->setObjectName(QString::fromUtf8("ID_SUBEDITORS_HOGMAKER"));
        ID_SUBEDITORS_TABLEFILEEDIT = new QAction(IDD_PLAYER_WEAPONS);
        ID_SUBEDITORS_TABLEFILEEDIT->setObjectName(QString::fromUtf8("ID_SUBEDITORS_TABLEFILEEDIT"));
        ID_SUBEDITORS_TABLEFILEFILTER = new QAction(IDD_PLAYER_WEAPONS);
        ID_SUBEDITORS_TABLEFILEFILTER->setObjectName(QString::fromUtf8("ID_SUBEDITORS_TABLEFILEFILTER"));
        ID_TABLEEDIT_FILE_EXIT = new QAction(IDD_PLAYER_WEAPONS);
        ID_TABLEEDIT_FILE_EXIT->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_EXIT"));
        ID_TABLEEDIT_FILE_LOAD = new QAction(IDD_PLAYER_WEAPONS);
        ID_TABLEEDIT_FILE_LOAD->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_LOAD"));
        ID_TABLEEDIT_FILE_SAVE = new QAction(IDD_PLAYER_WEAPONS);
        ID_TABLEEDIT_FILE_SAVE->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_SAVE"));
        ID_TABLEEDIT_FILE_SAVE_AS = new QAction(IDD_PLAYER_WEAPONS);
        ID_TABLEEDIT_FILE_SAVE_AS->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_SAVE_AS"));
        ID_TABLEFILTER_FILE_EXIT = new QAction(IDD_PLAYER_WEAPONS);
        ID_TABLEFILTER_FILE_EXIT->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_EXIT"));
        ID_TABLEFILTER_FILE_LOAD = new QAction(IDD_PLAYER_WEAPONS);
        ID_TABLEFILTER_FILE_LOAD->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_LOAD"));
        ID_TABLEFILTER_FILE_NEW = new QAction(IDD_PLAYER_WEAPONS);
        ID_TABLEFILTER_FILE_NEW->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_NEW"));
        ID_TABLEFILTER_FILE_SAVE = new QAction(IDD_PLAYER_WEAPONS);
        ID_TABLEFILTER_FILE_SAVE->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_SAVE"));
        ID_TABLEFILTER_FILE_SAVEAS = new QAction(IDD_PLAYER_WEAPONS);
        ID_TABLEFILTER_FILE_SAVEAS->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_SAVEAS"));
        ID_TERRAIN_VIEW = new QAction(IDD_PLAYER_WEAPONS);
        ID_TERRAIN_VIEW->setObjectName(QString::fromUtf8("ID_TERRAIN_VIEW"));
        ID_TEST_TEST1 = new QAction(IDD_PLAYER_WEAPONS);
        ID_TEST_TEST1->setObjectName(QString::fromUtf8("ID_TEST_TEST1"));
        ID_TEST_TEST2 = new QAction(IDD_PLAYER_WEAPONS);
        ID_TEST_TEST2->setObjectName(QString::fromUtf8("ID_TEST_TEST2"));
        ID_TEST_TEST3 = new QAction(IDD_PLAYER_WEAPONS);
        ID_TEST_TEST3->setObjectName(QString::fromUtf8("ID_TEST_TEST3"));
        ID_TOOLS_WORLD_OBJECTS_BUILDINGS = new QAction(IDD_PLAYER_WEAPONS);
        ID_TOOLS_WORLD_OBJECTS_BUILDINGS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_BUILDINGS"));
        ID_TOOLS_WORLD_OBJECTS_CLUTTER = new QAction(IDD_PLAYER_WEAPONS);
        ID_TOOLS_WORLD_OBJECTS_CLUTTER->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_CLUTTER"));
        ID_TOOLS_WORLD_OBJECTS_DOOR = new QAction(IDD_PLAYER_WEAPONS);
        ID_TOOLS_WORLD_OBJECTS_DOOR->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_DOOR"));
        ID_TOOLS_WORLD_OBJECTS_LIGHTS = new QAction(IDD_PLAYER_WEAPONS);
        ID_TOOLS_WORLD_OBJECTS_LIGHTS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_LIGHTS"));
        ID_TOOLS_WORLD_OBJECTS_PLAYER = new QAction(IDD_PLAYER_WEAPONS);
        ID_TOOLS_WORLD_OBJECTS_PLAYER->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_PLAYER"));
        ID_TOOLS_WORLD_OBJECTS_POWERUPS = new QAction(IDD_PLAYER_WEAPONS);
        ID_TOOLS_WORLD_OBJECTS_POWERUPS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_POWERUPS"));
        ID_TOOLS_WORLD_OBJECTS_ROBOTS = new QAction(IDD_PLAYER_WEAPONS);
        ID_TOOLS_WORLD_OBJECTS_ROBOTS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_ROBOTS"));
        ID_TOOLS_WORLD_OBJECTS_SOUND = new QAction(IDD_PLAYER_WEAPONS);
        ID_TOOLS_WORLD_OBJECTS_SOUND->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_SOUND"));
        ID_TOOLS_WORLD_TEXTURES = new QAction(IDD_PLAYER_WEAPONS);
        ID_TOOLS_WORLD_TEXTURES->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_TEXTURES"));
        ID_TOOLS_WORLD_WEAPONS = new QAction(IDD_PLAYER_WEAPONS);
        ID_TOOLS_WORLD_WEAPONS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_WEAPONS"));
        ID_VIEW_CENTERONCUBE = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_CENTERONCUBE->setObjectName(QString::fromUtf8("ID_VIEW_CENTERONCUBE"));
        ID_VIEW_CENTERONMINE = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_CENTERONMINE->setObjectName(QString::fromUtf8("ID_VIEW_CENTERONMINE"));
        ID_VIEW_CENTERONOBJECT = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_CENTERONOBJECT->setObjectName(QString::fromUtf8("ID_VIEW_CENTERONOBJECT"));
        ID_VIEW_DELETEVIEWER = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_DELETEVIEWER->setObjectName(QString::fromUtf8("ID_VIEW_DELETEVIEWER"));
        ID_VIEW_FLIP = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_FLIP->setObjectName(QString::fromUtf8("ID_VIEW_FLIP"));
        ID_VIEW_KEYPAD_TOGGLE = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_KEYPAD_TOGGLE->setObjectName(QString::fromUtf8("ID_VIEW_KEYPAD_TOGGLE"));
        ID_VIEW_MOVECAMERATOCURRENTOBJECT = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_MOVECAMERATOCURRENTOBJECT->setObjectName(QString::fromUtf8("ID_VIEW_MOVECAMERATOCURRENTOBJECT"));
        ID_VIEW_MOVECAMERATOSELECTEDFACE = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_MOVECAMERATOSELECTEDFACE->setObjectName(QString::fromUtf8("ID_VIEW_MOVECAMERATOSELECTEDFACE"));
        ID_VIEW_MOVECAMERATOSELECTEDROOM = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_MOVECAMERATOSELECTEDROOM->setObjectName(QString::fromUtf8("ID_VIEW_MOVECAMERATOSELECTEDROOM"));
        ID_VIEW_NEWVIEWER = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_NEWVIEWER->setObjectName(QString::fromUtf8("ID_VIEW_NEWVIEWER"));
        ID_VIEW_NEXTVIEWER = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_NEXTVIEWER->setObjectName(QString::fromUtf8("ID_VIEW_NEXTVIEWER"));
        ID_VIEW_RESETVIEWRADIUS = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_RESETVIEWRADIUS->setObjectName(QString::fromUtf8("ID_VIEW_RESETVIEWRADIUS"));
        ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW->setObjectName(QString::fromUtf8("ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW"));
        ID_VIEW_SHOWVIEWERFORWARDVECTOR = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_SHOWVIEWERFORWARDVECTOR->setObjectName(QString::fromUtf8("ID_VIEW_SHOWVIEWERFORWARDVECTOR"));
        ID_VIEW_STATUS_BAR = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_STATUS_BAR->setObjectName(QString::fromUtf8("ID_VIEW_STATUS_BAR"));
        ID_VIEW_TEXTUREMINE = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_TEXTUREMINE->setObjectName(QString::fromUtf8("ID_VIEW_TEXTUREMINE"));
        ID_VIEW_TOOLBAR = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_TOOLBAR->setObjectName(QString::fromUtf8("ID_VIEW_TOOLBAR"));
        ID_VIEW_VIEWPROP = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_VIEWPROP->setObjectName(QString::fromUtf8("ID_VIEW_VIEWPROP"));
        ID_VIEW_WIREFRAMEMINE = new QAction(IDD_PLAYER_WEAPONS);
        ID_VIEW_WIREFRAMEMINE->setObjectName(QString::fromUtf8("ID_VIEW_WIREFRAMEMINE"));
        ID_WINDOW_CASCADE = new QAction(IDD_PLAYER_WEAPONS);
        ID_WINDOW_CASCADE->setObjectName(QString::fromUtf8("ID_WINDOW_CASCADE"));
        ID_WINDOW_TILE = new QAction(IDD_PLAYER_WEAPONS);
        ID_WINDOW_TILE->setObjectName(QString::fromUtf8("ID_WINDOW_TILE"));
        ID_ZBUTTON = new QAction(IDD_PLAYER_WEAPONS);
        ID_ZBUTTON->setObjectName(QString::fromUtf8("ID_ZBUTTON"));
        IDC_STATIC = new QGroupBox(IDD_PLAYER_WEAPONS);
        IDC_STATIC->setObjectName(QString::fromUtf8("IDC_STATIC"));
        IDC_STATIC->setGeometry(QRect(16, 89, 216, 326));
        IDC_STATIC->setVisible(true);
        IDC_STATIC->setEnabled(true);
        IDC_STATIC->setFlat(false);
        IDC_FIRES_FUSION = new QCheckBox(IDC_STATIC);
        IDC_FIRES_FUSION->setObjectName(QString::fromUtf8("IDC_FIRES_FUSION"));
        IDC_FIRES_FUSION->setGeometry(QRect(24, 30, 120, 19));
        IDC_FIRES_FUSION->setVisible(true);
        IDC_FIRES_FUSION->setEnabled(true);
        IDC_ONOFF = new QCheckBox(IDC_STATIC);
        IDC_ONOFF->setObjectName(QString::fromUtf8("IDC_ONOFF"));
        IDC_ONOFF->setGeometry(QRect(24, 53, 117, 19));
        IDC_ONOFF->setVisible(true);
        IDC_ONOFF->setEnabled(true);
        IDC_ZOOM = new QCheckBox(IDC_STATIC);
        IDC_ZOOM->setObjectName(QString::fromUtf8("IDC_ZOOM"));
        IDC_ZOOM->setGeometry(QRect(24, 76, 114, 19));
        IDC_ZOOM->setVisible(true);
        IDC_ZOOM->setEnabled(true);
        IDC_CONTINUOUS_FIRING_SOUND = new QCheckBox(IDC_STATIC);
        IDC_CONTINUOUS_FIRING_SOUND->setObjectName(QString::fromUtf8("IDC_CONTINUOUS_FIRING_SOUND"));
        IDC_CONTINUOUS_FIRING_SOUND->setGeometry(QRect(24, 106, 152, 19));
        IDC_CONTINUOUS_FIRING_SOUND->setVisible(true);
        IDC_CONTINUOUS_FIRING_SOUND->setEnabled(true);
        IDC_FIRING_SOUND_PULLDOWN = new QComboBox(IDC_STATIC);
        IDC_FIRING_SOUND_PULLDOWN->setObjectName(QString::fromUtf8("IDC_FIRING_SOUND_PULLDOWN"));
        IDC_FIRING_SOUND_PULLDOWN->setMaxVisibleItems(14);
        IDC_FIRING_SOUND_PULLDOWN->setGeometry(QRect(24, 126, 136, 22));
        IDC_FIRING_SOUND_PULLDOWN->setVisible(true);
        IDC_FIRING_SOUND_PULLDOWN->setEnabled(true);
        IDC_FIRING_SOUND_PULLDOWN->setEditable(true);
        IDC_FIRING_RELEASE_SOUND = new QCheckBox(IDC_STATIC);
        IDC_FIRING_RELEASE_SOUND->setObjectName(QString::fromUtf8("IDC_FIRING_RELEASE_SOUND"));
        IDC_FIRING_RELEASE_SOUND->setGeometry(QRect(24, 158, 144, 19));
        IDC_FIRING_RELEASE_SOUND->setVisible(true);
        IDC_FIRING_RELEASE_SOUND->setEnabled(true);
        IDC_RELEASE_SOUND_PULLDOWN = new QComboBox(IDC_STATIC);
        IDC_RELEASE_SOUND_PULLDOWN->setObjectName(QString::fromUtf8("IDC_RELEASE_SOUND_PULLDOWN"));
        IDC_RELEASE_SOUND_PULLDOWN->setMaxVisibleItems(10);
        IDC_RELEASE_SOUND_PULLDOWN->setGeometry(QRect(24, 178, 136, 22));
        IDC_RELEASE_SOUND_PULLDOWN->setVisible(true);
        IDC_RELEASE_SOUND_PULLDOWN->setEnabled(true);
        IDC_RELEASE_SOUND_PULLDOWN->setEditable(true);
        IDC_STATIC_2 = new QLabel(IDC_STATIC);
        IDC_STATIC_2->setObjectName(QString::fromUtf8("IDC_STATIC_2"));
        IDC_STATIC_2->setGeometry(QRect(24, 211, 73, 13));
        IDC_STATIC_2->setVisible(true);
        IDC_STATIC_2->setEnabled(true);
        IDC_STATIC_2->setAlignment(Qt::AlignLeft);
        IDC_SPEW_POWERUP_PULLDOWN = new QComboBox(IDC_STATIC);
        IDC_SPEW_POWERUP_PULLDOWN->setObjectName(QString::fromUtf8("IDC_SPEW_POWERUP_PULLDOWN"));
        IDC_SPEW_POWERUP_PULLDOWN->setMaxVisibleItems(5);
        IDC_SPEW_POWERUP_PULLDOWN->setGeometry(QRect(24, 227, 136, 22));
        IDC_SPEW_POWERUP_PULLDOWN->setVisible(true);
        IDC_SPEW_POWERUP_PULLDOWN->setEnabled(true);
        IDC_SPEW_POWERUP_PULLDOWN->setEditable(true);
        IDC_STATIC_3 = new QLabel(IDC_STATIC);
        IDC_STATIC_3->setObjectName(QString::fromUtf8("IDC_STATIC_3"));
        IDC_STATIC_3->setGeometry(QRect(24, 260, 136, 16));
        IDC_STATIC_3->setVisible(true);
        IDC_STATIC_3->setEnabled(true);
        IDC_STATIC_3->setAlignment(Qt::AlignLeft);
        IDC_MAX_AMMO = new QLineEdit(IDC_STATIC);
        IDC_MAX_AMMO->setObjectName(QString::fromUtf8("IDC_MAX_AMMO"));
        IDC_MAX_AMMO->setGeometry(QRect(24, 276, 136, 21));
        IDC_MAX_AMMO->setVisible(true);
        IDC_MAX_AMMO->setEnabled(true);
        IDC_SHOW_TENTHS = new QCheckBox(IDC_STATIC);
        IDC_SHOW_TENTHS->setObjectName(QString::fromUtf8("IDC_SHOW_TENTHS"));
        IDC_SHOW_TENTHS->setGeometry(QRect(25, 307, 138, 19));
        IDC_SHOW_TENTHS->setVisible(true);
        IDC_SHOW_TENTHS->setEnabled(true);
        IDC_STATIC_4 = new QLabel(IDD_PLAYER_WEAPONS);
        IDC_STATIC_4->setObjectName(QString::fromUtf8("IDC_STATIC_4"));
        IDC_STATIC_4->setGeometry(QRect(16, 8, 88, 16));
        IDC_STATIC_4->setVisible(true);
        IDC_STATIC_4->setEnabled(true);
        IDC_STATIC_4->setAlignment(Qt::AlignLeft);
        IDC_CURRENT_WEAPON_BATTERY_COMBO = new QComboBox(IDD_PLAYER_WEAPONS);
        IDC_CURRENT_WEAPON_BATTERY_COMBO->setObjectName(QString::fromUtf8("IDC_CURRENT_WEAPON_BATTERY_COMBO"));
        IDC_CURRENT_WEAPON_BATTERY_COMBO->setMaxVisibleItems(22);
        IDC_CURRENT_WEAPON_BATTERY_COMBO->setGeometry(QRect(112, 8, 104, 22));
        IDC_CURRENT_WEAPON_BATTERY_COMBO->setVisible(true);
        IDC_CURRENT_WEAPON_BATTERY_COMBO->setEnabled(true);
        IDC_CURRENT_WEAPON_BATTERY_COMBO->setEditable(true);
        IDC_EDIT_WB_BUTTON = new QPushButton(IDD_PLAYER_WEAPONS);
        IDC_EDIT_WB_BUTTON->setObjectName(QString::fromUtf8("IDC_EDIT_WB_BUTTON"));
        IDC_EDIT_WB_BUTTON->setGeometry(QRect(32, 48, 184, 24));
        IDC_EDIT_WB_BUTTON->setVisible(true);
        IDC_EDIT_WB_BUTTON->setEnabled(true);
        IDC_EDIT_WB_BUTTON->setAutoDefault(false);
        IDOK = new QPushButton(IDD_PLAYER_WEAPONS);
        IDOK->setObjectName(QString::fromUtf8("IDOK"));
        IDOK->setGeometry(QRect(80, 440, 80, 22));
        IDOK->setVisible(true);
        IDOK->setEnabled(true);
        IDOK->setAutoDefault(true);

        retranslateUi(IDD_PLAYER_WEAPONS);

        IDC_EDIT_WB_BUTTON->setDefault(false);
        IDOK->setDefault(true);


        QMetaObject::connectSlotsByName(IDD_PLAYER_WEAPONS);
    } // setupUi

    void retranslateUi(QDialog *IDD_PLAYER_WEAPONS)
    {
        IDD_PLAYER_WEAPONS->setWindowTitle(QCoreApplication::translate("PlayerWeaponsDialog", "Player Weapons Dialog", nullptr));
        IDD_ORPHANHUNTER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Orphan Hunter", nullptr));
        IDD_REORDER_PAGES->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Reorder Net Pages", nullptr));
        IDD_SHOW_ALL_CHECKED_OUT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Show All Pages Checked Out", nullptr));
        IDM_IMPORT_BITMAP->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Bitmap/Animation Converter...", nullptr));
        ID_APP_ABOUT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&About editor...", nullptr));
        ID_APP_EXIT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "E&xit", nullptr));
#if QT_CONFIG(shortcut)
        ID_APP_EXIT->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Alt+X", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_BRIEFING_EDITOR->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Briefing Editor...", nullptr));
        ID_BUTTON_LIGHTING->setText(QCoreApplication::translate("PlayerWeaponsDialog", "ID_BUTTON_LIGHTING", nullptr));
        ID_BUTTON_OUTLINE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "ID_BUTTON_OUTLINE", nullptr));
        ID_BUTTON_WINDOWSELECTION->setText(QCoreApplication::translate("PlayerWeaponsDialog", "ID_BUTTON_WINDOWSELECTION", nullptr));
        ID_D3HELP->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Help on D3 Editor", nullptr));
        ID_EDITORS_AIPROPERTIES->setText(QCoreApplication::translate("PlayerWeaponsDialog", "AI Properties...", nullptr));
        ID_EDITORS_AMBIENTSOUNDS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Ambient Sounds...", nullptr));
        ID_EDITORS_DALLAS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Dallas Graphical Script Editor...", nullptr));
        ID_EDITORS_FILES->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Files...", nullptr));
        ID_EDITORS_MEGACELLS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Megacells...", nullptr));
        ID_EDIT_ADDSELECT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Add Current Room to Selected List", nullptr));
        ID_EDIT_ATTACH->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Attach", nullptr));
        ID_EDIT_CLEARSELECTED->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Clear Selected List", nullptr));
        ID_EDIT_COPY->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Copy", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_COPY->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Ctrl+Ins", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_EDIT_CUT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Cu&t", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_CUT->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Shift+Del", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_EDIT_DELETE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Delete", nullptr));
        ID_EDIT_LOADSCRAP->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Load Scrap from Disk", nullptr));
        ID_EDIT_PASTE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Paste", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_PASTE->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Shift+Ins", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_EDIT_PLACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Place in Mine", nullptr));
        ID_EDIT_PLACE_TERRAIN->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Place on Terrain", nullptr));
        ID_EDIT_REMOVESELECT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Remove Current Room from Selected List", nullptr));
        ID_EDIT_SAVESCRAP->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Save Scrap to Disk", nullptr));
        ID_EDIT_SELECTATTACHED->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Select All Rooms Attached to Current Room", nullptr));
        ID_EDIT_UNDO->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Undo", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_UNDO->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Alt+Backspace", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_AUTOSAVE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Autosave", nullptr));
        ID_FILE_CHECKMINE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Check Mine", nullptr));
        ID_FILE_FIXCRACKS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Fix Level Cracks", nullptr));
        ID_FILE_FIXDEGENERATEFACES->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Fix/Remove Degenerate Faces", nullptr));
        ID_FILE_IMPORT_ROOM->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Import 3DSMax Room", nullptr));
        ID_FILE_INFORMATION->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Information", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_INFORMATION->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Alt+I", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_LEAVE_EDITOR->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Go to main menu...", nullptr));
        ID_FILE_LEVELPROPS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Set Selected Room(s) Properties", nullptr));
        ID_FILE_LEVEL_INFO->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Edit Level Info...", nullptr));
        ID_FILE_MRU_FILE1->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Recent File", nullptr));
        ID_FILE_NEW->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&New", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_NEW->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_OPEN->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Open...", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_OPEN->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_PLAY640X480->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Play in 640 x 480", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_PLAY640X480->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Alt+P", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_PREFERENCES->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Preferences", nullptr));
        ID_FILE_PRINT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Print...", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_PRINT->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Ctrl+P", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_PRINT_PREVIEW->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Print Pre&view", nullptr));
        ID_FILE_PRINT_SETUP->setText(QCoreApplication::translate("PlayerWeaponsDialog", "P&rint Setup...", nullptr));
        ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Remove Duplicate Faces from Current Room", nullptr));
        ID_FILE_REMOVEEXTRAPOINTS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Remove Extra Points", nullptr));
        ID_FILE_RESTOREGAMESTATE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Restore Gamestate", nullptr));
        ID_FILE_SAVE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Save", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_SAVE->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_SAVEGOALTEXT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Save Goal Text to File", nullptr));
        ID_FILE_SAVE_AS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Save &As...", nullptr));
        ID_FILE_SAVE_COPY_AS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Save Copy &As...", nullptr));
        ID_FILE_STATS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Show Level Stats", nullptr));
        ID_FILE_UPDATE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Update", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_UPDATE->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_VERIFY_LEVEL->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Verify Level", nullptr));
        ID_HOTSPOT_TGA->setText(QCoreApplication::translate("PlayerWeaponsDialog", "HotSpot TGA Extractor...", nullptr));
        ID_MINE_VIEW->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Mine View", nullptr));
#if QT_CONFIG(shortcut)
        ID_MINE_VIEW->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "F2", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_OBJBUTTON->setText(QCoreApplication::translate("PlayerWeaponsDialog", "ID_OBJBUTTON", nullptr));
        ID_OBJECT_DELETEOBJECT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Delete Object", nullptr));
#if QT_CONFIG(shortcut)
        ID_OBJECT_DELETEOBJECT->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Del", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_OBJECT_MOVEPLAYER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Move Player 0 to Viewer", nullptr));
        ID_OBJECT_PLACECAMERAATCURRENTFACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Create New Camera at Current Face", nullptr));
        ID_OBJECT_PLACECAMERAATVIEWER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Create New Camera at Viewer", nullptr));
        ID_OBJECT_PLACEOBJECT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Place Object", nullptr));
#if QT_CONFIG(shortcut)
        ID_OBJECT_PLACEOBJECT->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Ins", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_OBJECT_PLACESOUNDSOURCEATVIEWER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Create SoundSource at Viewer", nullptr));
        ID_OBJECT_PLACEWAYPOINTATVIEWER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Place Waypoint at Viewer", nullptr));
        ID_OBJECT_SELECTBYNUMBER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Select Object By Number", nullptr));
        ID_OBJECT_SETCAMERAFROMVIEWER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Move Camera to Viewer", nullptr));
        ID_OBJECT_SETVIEWERFROMCAMERA->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Move Viewer to Camera", nullptr));
        ID_OSIRISCOMPILE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "ID_OSIRISCOMPILE", nullptr));
        ID_REINIT_OPENGL->setText(QCoreApplication::translate("PlayerWeaponsDialog", "ID_REINIT_OPENGL", nullptr));
        ID_ROOM_ADD->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Add Room at Current Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_ADD->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Shift+A", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_ADDVERTTONEWFACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Add Vert to New Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_ADDVERTTONEWFACE->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Shift+V", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_ATTACHROOM->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Attach Room", nullptr));
        ID_ROOM_BUILDBRIDGE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Build Bridge", nullptr));
        ID_ROOM_BUILDSMOOTHBRIDGE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Build Smooth Bridge", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_BUILDSMOOTHBRIDGE->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Ctrl+B", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_CLEARCENTERPOINT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Clear Center Point", nullptr));
        ID_ROOM_COMBINE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Combine Rooms", nullptr));
        ID_ROOM_DELETE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Delete Current Room", nullptr));
        ID_ROOM_DELETECONNECTEDFACES->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Delete Connected Faces", nullptr));
        ID_ROOM_DELETEFACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Delete Current Face", nullptr));
        ID_ROOM_DELETEPORTAL->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Delete Current Portal", nullptr));
        ID_ROOM_DELETEVERT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Delete Current Vert From Face", nullptr));
        ID_ROOM_DROPROOM->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Drop Room", nullptr));
        ID_ROOM_FACE_ADDVERTTOEDGE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Add Vert To Edge", nullptr));
        ID_ROOM_FACE_DELETEVERTONEDGE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Delete Vert on Edge", nullptr));
        ID_ROOM_FACE_MOVEVERTONEDGE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Move Vert on Edge", nullptr));
        ID_ROOM_FACE_SPLITFACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Split Face", nullptr));
        ID_ROOM_FINISHNEWFACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Add Last Point to New Face (Finish Face)", nullptr));
        ID_ROOM_GRABTEXTURE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Grab Texture", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_GRABTEXTURE->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "G", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_JOINROOMS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Join Rooms", nullptr));
        ID_ROOM_JOINROOMSEXACT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Join Rooms Exact", nullptr));
        ID_ROOM_JOIN_ADJACENT_FACES->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Join Adjacent Faces", nullptr));
        ID_ROOM_LINKTONEWEXTERNAL->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Link Room to New External Room", nullptr));
        ID_ROOM_MARK->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Mark Room/Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_MARK->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Ctrl+M", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_MERGEOBJECTINTOROOM->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Merge Object into Room", nullptr));
        ID_ROOM_NEXTFACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Next Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_NEXTFACE->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "F", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_PLACEROOM->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Place Room at Current Room", nullptr));
        ID_ROOM_PLACETERRAINROOM->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Place Room at Current Terrain Cell", nullptr));
        ID_ROOM_PREVIOUSFACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Previous Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_PREVIOUSFACE->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Shift+F", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Propagate to Adjacent Coplanar Faces", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Ctrl+F", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOALL->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Propagate to All Faces in Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOALL->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Shift+R", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_RENAMEROOM->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Rename Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_RENAMEROOM->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Ctrl+R", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_ROTATEPLACEDROOM45DEGREES->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Rotate Placed Room 45 Degrees", nullptr));
        ID_ROOM_SAVECURRENTROOM->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Save Current Room", nullptr));
        ID_ROOM_SELECTBYNUMBER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Select Room by Number", nullptr));
        ID_ROOM_SELECTFACEBYNUMBER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Select Face by Number", nullptr));
        ID_ROOM_SETCENTERFROMVIEWER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Set Room Center from Viewer", nullptr));
        ID_ROOM_SNAPPLACEDROOM->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Snap Placed Room", nullptr));
        ID_ROOM_SNAPPOINTTOEDGE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Snap Point to Edge", nullptr));
        ID_ROOM_SNAPPOINTTOFACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Snap Point to Face", nullptr));
        ID_ROOM_SNAPPOINTTOPOINT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Snap Point to Point", nullptr));
        ID_ROOM_SPLITFACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Triangulate face", nullptr));
        ID_ROOM_STARTNEWFACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Add First Point to New Face (Start Face)", nullptr));
        ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Swap Marked and Current Room/Face", nullptr));
        ID_ROOM_UNDOSNAP->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Undo Snap", nullptr));
        ID_ROOM_UNPLACEROOM->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Un-place Room/Group/Door", nullptr));
        ID_ROOM_VIEW->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Room View", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_VIEW->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "F4", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_SCRIPT_LEVEL_INTERFACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Levels and Scripts...", nullptr));
        ID_SUBEDITORS_FONT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Font Editor...", nullptr));
        ID_SUBEDITORS_HOGMAKER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Hog Maker...", nullptr));
        ID_SUBEDITORS_TABLEFILEEDIT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Table File Text Editor...", nullptr));
        ID_SUBEDITORS_TABLEFILEFILTER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Table File Filter...", nullptr));
        ID_TABLEEDIT_FILE_EXIT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Exit", nullptr));
        ID_TABLEEDIT_FILE_LOAD->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Load...", nullptr));
        ID_TABLEEDIT_FILE_SAVE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Save", nullptr));
        ID_TABLEEDIT_FILE_SAVE_AS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "S&ave as...", nullptr));
        ID_TABLEFILTER_FILE_EXIT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Exit", nullptr));
        ID_TABLEFILTER_FILE_LOAD->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Load...", nullptr));
        ID_TABLEFILTER_FILE_NEW->setText(QCoreApplication::translate("PlayerWeaponsDialog", "New", nullptr));
        ID_TABLEFILTER_FILE_SAVE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Save", nullptr));
        ID_TABLEFILTER_FILE_SAVEAS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Save as...", nullptr));
        ID_TERRAIN_VIEW->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Terrain View", nullptr));
#if QT_CONFIG(shortcut)
        ID_TERRAIN_VIEW->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "F3", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_TEST_TEST1->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Test function 1", nullptr));
        ID_TEST_TEST2->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Test function 2", nullptr));
        ID_TEST_TEST3->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Test function 3", nullptr));
        ID_TOOLS_WORLD_OBJECTS_BUILDINGS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Buildings...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_CLUTTER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Clutter...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_DOOR->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Doors...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_LIGHTS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Lights...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_PLAYER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Player Ships...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_POWERUPS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Powerups...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_ROBOTS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Robots...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_SOUND->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Sounds...", nullptr));
        ID_TOOLS_WORLD_TEXTURES->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Textures...", nullptr));
        ID_TOOLS_WORLD_WEAPONS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Weapons...", nullptr));
        ID_VIEW_CENTERONCUBE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Center on Current &Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_CENTERONCUBE->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Home", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_CENTERONMINE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Center on &Mine Origin", nullptr));
        ID_VIEW_CENTERONOBJECT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Center on Current &Object", nullptr));
        ID_VIEW_DELETEVIEWER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Delete Viewer", nullptr));
        ID_VIEW_FLIP->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Flip Viewer", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_FLIP->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "/", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_KEYPAD_TOGGLE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Keypad", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_KEYPAD_TOGGLE->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "F10", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOCURRENTOBJECT->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Move camera to Current Object", nullptr));
        ID_VIEW_MOVECAMERATOSELECTEDFACE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Move camera to &Current Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOSELECTEDFACE->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "Shift+C", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOSELECTEDROOM->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Move camera to &Current Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOSELECTEDROOM->setShortcut(QCoreApplication::translate("PlayerWeaponsDialog", "C", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_NEWVIEWER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "New Viewer", nullptr));
        ID_VIEW_NEXTVIEWER->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Next Viewer", nullptr));
        ID_VIEW_RESETVIEWRADIUS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Reset View Radius", nullptr));
        ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Show objects in Wireframe view", nullptr));
        ID_VIEW_SHOWVIEWERFORWARDVECTOR->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Show Viewer Forward Vector", nullptr));
        ID_VIEW_STATUS_BAR->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Status Bar", nullptr));
        ID_VIEW_TEXTUREMINE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Te&xtured view", nullptr));
        ID_VIEW_TOOLBAR->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Toolbar", nullptr));
        ID_VIEW_VIEWPROP->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Display Viewer Properties", nullptr));
        ID_VIEW_WIREFRAMEMINE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Wireframe view", nullptr));
        ID_WINDOW_CASCADE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Cascade", nullptr));
        ID_WINDOW_TILE->setText(QCoreApplication::translate("PlayerWeaponsDialog", "&Tile", nullptr));
        ID_ZBUTTON->setText(QCoreApplication::translate("PlayerWeaponsDialog", "ID_ZBUTTON", nullptr));
        IDC_STATIC->setTitle(QCoreApplication::translate("PlayerWeaponsDialog", "Additional Weapon Battery Properties", nullptr));
        IDC_FIRES_FUSION->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Fires like Fusion", nullptr));
        IDC_ONOFF->setText(QCoreApplication::translate("PlayerWeaponsDialog", "On/Off weapon", nullptr));
        IDC_ZOOM->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Zoom weapon", nullptr));
        IDC_CONTINUOUS_FIRING_SOUND->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Has continuous firing sound", nullptr));
        IDC_FIRING_RELEASE_SOUND->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Has firing release sound", nullptr));
        IDC_STATIC_2->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Spew powerup", nullptr));
        IDC_STATIC_3->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Max Ammo", nullptr));
        IDC_SHOW_TENTHS->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Show as tenths", nullptr));
        IDC_STATIC_4->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Current Weapon", nullptr));
        IDC_EDIT_WB_BUTTON->setText(QCoreApplication::translate("PlayerWeaponsDialog", "Edit Weapon Battery Info", nullptr));
        IDOK->setText(QCoreApplication::translate("PlayerWeaponsDialog", "OK", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PlayerWeaponsDialog: public Ui_PlayerWeaponsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PLAYER_WEAPONS_H
