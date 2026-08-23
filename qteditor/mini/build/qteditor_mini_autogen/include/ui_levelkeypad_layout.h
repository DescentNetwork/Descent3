/********************************************************************************
** Form generated from reading UI file 'levelkeypad_layout.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LEVELKEYPAD_LAYOUT_H
#define UI_LEVELKEYPAD_LAYOUT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_LevelKeypadLayout
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
    QVBoxLayout *layout_20;
    QGroupBox *IDC_STATIC;
    QVBoxLayout *layout;
    QGridLayout *layout_2;
    QLabel *IDC_STATIC_2;
    QLineEdit *IDC_LEVEL_GRAVITY_EDIT;
    QLabel *IDC_STATIC_3;
    QLineEdit *IDC_LEVEL_CEILING_EDIT;
    QGridLayout *layout_3;
    QCheckBox *IDC_LEVEL_CEILING_EVERYWHERE;
    QCheckBox *IDC_LEVEL_PIW;
    QGroupBox *IDC_STATIC_4;
    QGridLayout *layout_4;
    QComboBox *IDC_FFT1_COMBO;
    QLineEdit *IDC_FIXED_FFT1_EDIT;
    QComboBox *IDC_FFT2_COMBO;
    QLineEdit *IDC_FFT2_EDIT;
    QComboBox *IDC_FFT3_COMBO;
    QLineEdit *IDC_FFT3_EDIT;
    QGroupBox *IDC_STATIC_5;
    QGridLayout *layout_5;
    QLabel *IDC_STATIC_6;
    QLabel *IDC_STATIC_7;
    QComboBox *IDC_BGOS_COMBO;
    QComboBox *IDC_FFOS_COMBO;
    QGroupBox *IDC_STATIC_8;
    QVBoxLayout *layout_8;
    QGridLayout *layout_9;
    QPushButton *IDC_LEVEL_NEW_GOAL_BUTTON;
    QPushButton *IDC_LEVEL_NEXT_GOAL_BUTTON;
    QPushButton *IDC_LEVEL_DELETE_GOAL_BUTTON;
    QPushButton *IDC_LEVEL_PREV_GOAL_BUTTON;
    QGridLayout *layout_10;
    QLabel *IDC_LEVEL_GOAL_NUM_GOALS_STATIC;
    QLabel *IDC_LEVEL_GOAL_CUR_GOAL_STATIC;
    QLabel *IDC_STATIC_9;
    QLineEdit *IDC_LEVEL_GOAL_NAME_EDIT;
    QLabel *IDC_STATIC_10;
    QLineEdit *IDC_LEVEL_GOAL_LOC_NAME_EDIT;
    QLabel *IDC_STATIC_11;
    QTextEdit *IDC_LEVEL_GOAL_DESC_EDIT;
    QLabel *IDC_STATIC_12;
    QLineEdit *IDC_LEVEL_GOAL_COMPLETION_EDIT;
    QGridLayout *layout_11;
    QLabel *IDC_STATIC_13;
    QLineEdit *IDC_LEVEL_GOAL_PRIORITY_EDIT1;
    QGridLayout *layout_12;
    QLineEdit *IDC_EDIT57;
    QLabel *IDC_STATIC_14;
    QGridLayout *layout_13;
    QPushButton *IDC_LEVEL_GOAL_NEW_ITEM_BUTTON;
    QPushButton *IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON;
    QGridLayout *layout_14;
    QPushButton *IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON;
    QPushButton *IDC_LEVEL_GOAL_PREV_ITEM_BUTTON;
    QPushButton *IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON;
    QGridLayout *layout_15;
    QLabel *IDC_LEVEL_GOAL_NUM_ITEMS_STATIC;
    QLabel *IDC_LEVEL_GOAL_CUR_ITEM_STATIC;
    QGroupBox *IDC_STATIC_15;
    QGridLayout *layout_6;
    QComboBox *IDC_LEVELGOAL_ROOM_COMBO;
    QRadioButton *IDC_LEVEL_GOAL_ROOM_ITEM_RADIO;
    QRadioButton *IDC_LEVEL_GOAL_OBJ_ITEM_RADIO;
    QComboBox *IDC_LEVELGOALOBJECT_COMBO;
    QRadioButton *IDC_TRIGGER_RADIO;
    QComboBox *IDC_LEVELGOALTRIGGER_COMBO;
    QRadioButton *IDC_LEVEL_GOAL_CELL_ITEM_RADIO;
    QRadioButton *IDC_LEVEL_GOAL_MINE_RADIO;
    QGroupBox *IDC_STATIC_16;
    QGridLayout *layout_7;
    QRadioButton *IDC_LG_ACTIVATE_RADIO;
    QRadioButton *IDC_LG_LC_RADIO;
    QRadioButton *IDC_LG_ENTER_RADIO;
    QRadioButton *IDC_LG_DALLAS_RADIO;
    QRadioButton *IDC_LG_DESTROYED_RADIO;
    QRadioButton *IDC_LG_PPW_COLLIDE_RADIO;
    QGridLayout *layout_16;
    QCheckBox *IDC_LEVEL_GOAL_SEC_GOAL_CHECK;
    QCheckBox *IDC_LEVEL_GOAL_TLG_CHECK;
    QCheckBox *IDC_LEVEL_GOAL_ENABLED_CHECK;
    QCheckBox *IDC_LEVEL_GOAL_GDKL_CHECK;
    QCheckBox *IDC_LEVEL_GOAL_COMPLETED_CHECK;
    QCheckBox *IDC_LEVEL_GOAL_NLB_CHECK;
    QLabel *IDC_STATIC_17;
    QTextEdit *IDC_GOAL_TABLE_EDIT;
    QGroupBox *IDC_STATIC_18;
    QVBoxLayout *layout_17;
    QGridLayout *layout_18;
    QLabel *IDC_STATIC_19;
    QLabel *IDC_STATIC_20;
    QGridLayout *layout_19;
    QLabel *IDC_STATIC_21;
    QLabel *IDC_STATIC_22;
    QLabel *IDC_STATIC_23;
    QLabel *IDC_STATIC_24;
    QLabel *IDC_STATIC_25;
    QLineEdit *IDC_AMB_N_GROUPS2;
    QLineEdit *IDC_AMB_MAX1;
    QLineEdit *IDC_AMB_MIN1;
    QComboBox *IDC_AMB_TYPE6;
    QLineEdit *IDC_AMB_N_GROUPS1;
    QComboBox *IDC_AMB_TYPE2;
    QCheckBox *IDC_AMB_OUTSIDE1;
    QComboBox *IDC_AMB_TYPE3;
    QLineEdit *IDC_FFT1_EDIT;
    QLabel *IDC_STATIC_26;

    void setupUi(QDialog *IDD_LEVELKEYPAD)
    {
        if (IDD_LEVELKEYPAD->objectName().isEmpty())
            IDD_LEVELKEYPAD->setObjectName(QString::fromUtf8("IDD_LEVELKEYPAD"));
        IDD_LEVELKEYPAD->resize(274, 1248);
        QFont font;
        font.setFamily(QString::fromUtf8("Liberation Sans"));
        font.setPointSize(7);
        font.setBold(false);
        font.setItalic(false);
        IDD_LEVELKEYPAD->setFont(font);
        IDD_LEVELKEYPAD->setEnabled(true);
        IDD_ORPHANHUNTER = new QAction(IDD_LEVELKEYPAD);
        IDD_ORPHANHUNTER->setObjectName(QString::fromUtf8("IDD_ORPHANHUNTER"));
        IDD_REORDER_PAGES = new QAction(IDD_LEVELKEYPAD);
        IDD_REORDER_PAGES->setObjectName(QString::fromUtf8("IDD_REORDER_PAGES"));
        IDD_SHOW_ALL_CHECKED_OUT = new QAction(IDD_LEVELKEYPAD);
        IDD_SHOW_ALL_CHECKED_OUT->setObjectName(QString::fromUtf8("IDD_SHOW_ALL_CHECKED_OUT"));
        IDM_IMPORT_BITMAP = new QAction(IDD_LEVELKEYPAD);
        IDM_IMPORT_BITMAP->setObjectName(QString::fromUtf8("IDM_IMPORT_BITMAP"));
        ID_APP_ABOUT = new QAction(IDD_LEVELKEYPAD);
        ID_APP_ABOUT->setObjectName(QString::fromUtf8("ID_APP_ABOUT"));
        ID_APP_EXIT = new QAction(IDD_LEVELKEYPAD);
        ID_APP_EXIT->setObjectName(QString::fromUtf8("ID_APP_EXIT"));
        ID_BRIEFING_EDITOR = new QAction(IDD_LEVELKEYPAD);
        ID_BRIEFING_EDITOR->setObjectName(QString::fromUtf8("ID_BRIEFING_EDITOR"));
        ID_BUTTON_LIGHTING = new QAction(IDD_LEVELKEYPAD);
        ID_BUTTON_LIGHTING->setObjectName(QString::fromUtf8("ID_BUTTON_LIGHTING"));
        ID_BUTTON_OUTLINE = new QAction(IDD_LEVELKEYPAD);
        ID_BUTTON_OUTLINE->setObjectName(QString::fromUtf8("ID_BUTTON_OUTLINE"));
        ID_BUTTON_WINDOWSELECTION = new QAction(IDD_LEVELKEYPAD);
        ID_BUTTON_WINDOWSELECTION->setObjectName(QString::fromUtf8("ID_BUTTON_WINDOWSELECTION"));
        ID_D3HELP = new QAction(IDD_LEVELKEYPAD);
        ID_D3HELP->setObjectName(QString::fromUtf8("ID_D3HELP"));
        ID_EDITORS_AIPROPERTIES = new QAction(IDD_LEVELKEYPAD);
        ID_EDITORS_AIPROPERTIES->setObjectName(QString::fromUtf8("ID_EDITORS_AIPROPERTIES"));
        ID_EDITORS_AMBIENTSOUNDS = new QAction(IDD_LEVELKEYPAD);
        ID_EDITORS_AMBIENTSOUNDS->setObjectName(QString::fromUtf8("ID_EDITORS_AMBIENTSOUNDS"));
        ID_EDITORS_DALLAS = new QAction(IDD_LEVELKEYPAD);
        ID_EDITORS_DALLAS->setObjectName(QString::fromUtf8("ID_EDITORS_DALLAS"));
        ID_EDITORS_FILES = new QAction(IDD_LEVELKEYPAD);
        ID_EDITORS_FILES->setObjectName(QString::fromUtf8("ID_EDITORS_FILES"));
        ID_EDITORS_MEGACELLS = new QAction(IDD_LEVELKEYPAD);
        ID_EDITORS_MEGACELLS->setObjectName(QString::fromUtf8("ID_EDITORS_MEGACELLS"));
        ID_EDIT_ADDSELECT = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_ADDSELECT->setObjectName(QString::fromUtf8("ID_EDIT_ADDSELECT"));
        ID_EDIT_ATTACH = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_ATTACH->setObjectName(QString::fromUtf8("ID_EDIT_ATTACH"));
        ID_EDIT_CLEARSELECTED = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_CLEARSELECTED->setObjectName(QString::fromUtf8("ID_EDIT_CLEARSELECTED"));
        ID_EDIT_COPY = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_COPY->setObjectName(QString::fromUtf8("ID_EDIT_COPY"));
        ID_EDIT_CUT = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_CUT->setObjectName(QString::fromUtf8("ID_EDIT_CUT"));
        ID_EDIT_DELETE = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_DELETE->setObjectName(QString::fromUtf8("ID_EDIT_DELETE"));
        ID_EDIT_LOADSCRAP = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_LOADSCRAP->setObjectName(QString::fromUtf8("ID_EDIT_LOADSCRAP"));
        ID_EDIT_PASTE = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_PASTE->setObjectName(QString::fromUtf8("ID_EDIT_PASTE"));
        ID_EDIT_PLACE = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_PLACE->setObjectName(QString::fromUtf8("ID_EDIT_PLACE"));
        ID_EDIT_PLACE_TERRAIN = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_PLACE_TERRAIN->setObjectName(QString::fromUtf8("ID_EDIT_PLACE_TERRAIN"));
        ID_EDIT_REMOVESELECT = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_REMOVESELECT->setObjectName(QString::fromUtf8("ID_EDIT_REMOVESELECT"));
        ID_EDIT_SAVESCRAP = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_SAVESCRAP->setObjectName(QString::fromUtf8("ID_EDIT_SAVESCRAP"));
        ID_EDIT_SELECTATTACHED = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_SELECTATTACHED->setObjectName(QString::fromUtf8("ID_EDIT_SELECTATTACHED"));
        ID_EDIT_UNDO = new QAction(IDD_LEVELKEYPAD);
        ID_EDIT_UNDO->setObjectName(QString::fromUtf8("ID_EDIT_UNDO"));
        ID_FILE_AUTOSAVE = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_AUTOSAVE->setObjectName(QString::fromUtf8("ID_FILE_AUTOSAVE"));
        ID_FILE_AUTOSAVE->setCheckable(true);
        ID_FILE_CHECKMINE = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_CHECKMINE->setObjectName(QString::fromUtf8("ID_FILE_CHECKMINE"));
        ID_FILE_FIXCRACKS = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_FIXCRACKS->setObjectName(QString::fromUtf8("ID_FILE_FIXCRACKS"));
        ID_FILE_FIXDEGENERATEFACES = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_FIXDEGENERATEFACES->setObjectName(QString::fromUtf8("ID_FILE_FIXDEGENERATEFACES"));
        ID_FILE_IMPORT_ROOM = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_IMPORT_ROOM->setObjectName(QString::fromUtf8("ID_FILE_IMPORT_ROOM"));
        ID_FILE_INFORMATION = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_INFORMATION->setObjectName(QString::fromUtf8("ID_FILE_INFORMATION"));
        ID_FILE_LEAVE_EDITOR = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_LEAVE_EDITOR->setObjectName(QString::fromUtf8("ID_FILE_LEAVE_EDITOR"));
        ID_FILE_LEVELPROPS = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_LEVELPROPS->setObjectName(QString::fromUtf8("ID_FILE_LEVELPROPS"));
        ID_FILE_LEVEL_INFO = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_LEVEL_INFO->setObjectName(QString::fromUtf8("ID_FILE_LEVEL_INFO"));
        ID_FILE_MRU_FILE1 = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_MRU_FILE1->setObjectName(QString::fromUtf8("ID_FILE_MRU_FILE1"));
        ID_FILE_MRU_FILE1->setEnabled(false);
        ID_FILE_NEW = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_NEW->setObjectName(QString::fromUtf8("ID_FILE_NEW"));
        ID_FILE_OPEN = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_OPEN->setObjectName(QString::fromUtf8("ID_FILE_OPEN"));
        ID_FILE_PLAY640X480 = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_PLAY640X480->setObjectName(QString::fromUtf8("ID_FILE_PLAY640X480"));
        ID_FILE_PREFERENCES = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_PREFERENCES->setObjectName(QString::fromUtf8("ID_FILE_PREFERENCES"));
        ID_FILE_PRINT = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_PRINT->setObjectName(QString::fromUtf8("ID_FILE_PRINT"));
        ID_FILE_PRINT_PREVIEW = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_PRINT_PREVIEW->setObjectName(QString::fromUtf8("ID_FILE_PRINT_PREVIEW"));
        ID_FILE_PRINT_SETUP = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_PRINT_SETUP->setObjectName(QString::fromUtf8("ID_FILE_PRINT_SETUP"));
        ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM->setObjectName(QString::fromUtf8("ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM"));
        ID_FILE_REMOVEEXTRAPOINTS = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_REMOVEEXTRAPOINTS->setObjectName(QString::fromUtf8("ID_FILE_REMOVEEXTRAPOINTS"));
        ID_FILE_RESTOREGAMESTATE = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_RESTOREGAMESTATE->setObjectName(QString::fromUtf8("ID_FILE_RESTOREGAMESTATE"));
        ID_FILE_SAVE = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_SAVE->setObjectName(QString::fromUtf8("ID_FILE_SAVE"));
        ID_FILE_SAVEGOALTEXT = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_SAVEGOALTEXT->setObjectName(QString::fromUtf8("ID_FILE_SAVEGOALTEXT"));
        ID_FILE_SAVE_AS = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_SAVE_AS->setObjectName(QString::fromUtf8("ID_FILE_SAVE_AS"));
        ID_FILE_SAVE_COPY_AS = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_SAVE_COPY_AS->setObjectName(QString::fromUtf8("ID_FILE_SAVE_COPY_AS"));
        ID_FILE_STATS = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_STATS->setObjectName(QString::fromUtf8("ID_FILE_STATS"));
        ID_FILE_UPDATE = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_UPDATE->setObjectName(QString::fromUtf8("ID_FILE_UPDATE"));
        ID_FILE_VERIFY_LEVEL = new QAction(IDD_LEVELKEYPAD);
        ID_FILE_VERIFY_LEVEL->setObjectName(QString::fromUtf8("ID_FILE_VERIFY_LEVEL"));
        ID_HOTSPOT_TGA = new QAction(IDD_LEVELKEYPAD);
        ID_HOTSPOT_TGA->setObjectName(QString::fromUtf8("ID_HOTSPOT_TGA"));
        ID_MINE_VIEW = new QAction(IDD_LEVELKEYPAD);
        ID_MINE_VIEW->setObjectName(QString::fromUtf8("ID_MINE_VIEW"));
        ID_OBJBUTTON = new QAction(IDD_LEVELKEYPAD);
        ID_OBJBUTTON->setObjectName(QString::fromUtf8("ID_OBJBUTTON"));
        ID_OBJECT_DELETEOBJECT = new QAction(IDD_LEVELKEYPAD);
        ID_OBJECT_DELETEOBJECT->setObjectName(QString::fromUtf8("ID_OBJECT_DELETEOBJECT"));
        ID_OBJECT_MOVEPLAYER = new QAction(IDD_LEVELKEYPAD);
        ID_OBJECT_MOVEPLAYER->setObjectName(QString::fromUtf8("ID_OBJECT_MOVEPLAYER"));
        ID_OBJECT_PLACECAMERAATCURRENTFACE = new QAction(IDD_LEVELKEYPAD);
        ID_OBJECT_PLACECAMERAATCURRENTFACE->setObjectName(QString::fromUtf8("ID_OBJECT_PLACECAMERAATCURRENTFACE"));
        ID_OBJECT_PLACECAMERAATVIEWER = new QAction(IDD_LEVELKEYPAD);
        ID_OBJECT_PLACECAMERAATVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_PLACECAMERAATVIEWER"));
        ID_OBJECT_PLACEOBJECT = new QAction(IDD_LEVELKEYPAD);
        ID_OBJECT_PLACEOBJECT->setObjectName(QString::fromUtf8("ID_OBJECT_PLACEOBJECT"));
        ID_OBJECT_PLACESOUNDSOURCEATVIEWER = new QAction(IDD_LEVELKEYPAD);
        ID_OBJECT_PLACESOUNDSOURCEATVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_PLACESOUNDSOURCEATVIEWER"));
        ID_OBJECT_PLACEWAYPOINTATVIEWER = new QAction(IDD_LEVELKEYPAD);
        ID_OBJECT_PLACEWAYPOINTATVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_PLACEWAYPOINTATVIEWER"));
        ID_OBJECT_SELECTBYNUMBER = new QAction(IDD_LEVELKEYPAD);
        ID_OBJECT_SELECTBYNUMBER->setObjectName(QString::fromUtf8("ID_OBJECT_SELECTBYNUMBER"));
        ID_OBJECT_SETCAMERAFROMVIEWER = new QAction(IDD_LEVELKEYPAD);
        ID_OBJECT_SETCAMERAFROMVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_SETCAMERAFROMVIEWER"));
        ID_OBJECT_SETVIEWERFROMCAMERA = new QAction(IDD_LEVELKEYPAD);
        ID_OBJECT_SETVIEWERFROMCAMERA->setObjectName(QString::fromUtf8("ID_OBJECT_SETVIEWERFROMCAMERA"));
        ID_OSIRISCOMPILE = new QAction(IDD_LEVELKEYPAD);
        ID_OSIRISCOMPILE->setObjectName(QString::fromUtf8("ID_OSIRISCOMPILE"));
        ID_REINIT_OPENGL = new QAction(IDD_LEVELKEYPAD);
        ID_REINIT_OPENGL->setObjectName(QString::fromUtf8("ID_REINIT_OPENGL"));
        ID_ROOM_ADD = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_ADD->setObjectName(QString::fromUtf8("ID_ROOM_ADD"));
        ID_ROOM_ADDVERTTONEWFACE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_ADDVERTTONEWFACE->setObjectName(QString::fromUtf8("ID_ROOM_ADDVERTTONEWFACE"));
        ID_ROOM_ATTACHROOM = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_ATTACHROOM->setObjectName(QString::fromUtf8("ID_ROOM_ATTACHROOM"));
        ID_ROOM_BUILDBRIDGE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_BUILDBRIDGE->setObjectName(QString::fromUtf8("ID_ROOM_BUILDBRIDGE"));
        ID_ROOM_BUILDSMOOTHBRIDGE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_BUILDSMOOTHBRIDGE->setObjectName(QString::fromUtf8("ID_ROOM_BUILDSMOOTHBRIDGE"));
        ID_ROOM_CLEARCENTERPOINT = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_CLEARCENTERPOINT->setObjectName(QString::fromUtf8("ID_ROOM_CLEARCENTERPOINT"));
        ID_ROOM_COMBINE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_COMBINE->setObjectName(QString::fromUtf8("ID_ROOM_COMBINE"));
        ID_ROOM_DELETE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_DELETE->setObjectName(QString::fromUtf8("ID_ROOM_DELETE"));
        ID_ROOM_DELETECONNECTEDFACES = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_DELETECONNECTEDFACES->setObjectName(QString::fromUtf8("ID_ROOM_DELETECONNECTEDFACES"));
        ID_ROOM_DELETEFACE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_DELETEFACE->setObjectName(QString::fromUtf8("ID_ROOM_DELETEFACE"));
        ID_ROOM_DELETEPORTAL = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_DELETEPORTAL->setObjectName(QString::fromUtf8("ID_ROOM_DELETEPORTAL"));
        ID_ROOM_DELETEVERT = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_DELETEVERT->setObjectName(QString::fromUtf8("ID_ROOM_DELETEVERT"));
        ID_ROOM_DROPROOM = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_DROPROOM->setObjectName(QString::fromUtf8("ID_ROOM_DROPROOM"));
        ID_ROOM_FACE_ADDVERTTOEDGE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_FACE_ADDVERTTOEDGE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_ADDVERTTOEDGE"));
        ID_ROOM_FACE_DELETEVERTONEDGE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_FACE_DELETEVERTONEDGE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_DELETEVERTONEDGE"));
        ID_ROOM_FACE_MOVEVERTONEDGE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_FACE_MOVEVERTONEDGE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_MOVEVERTONEDGE"));
        ID_ROOM_FACE_SPLITFACE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_FACE_SPLITFACE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_SPLITFACE"));
        ID_ROOM_FINISHNEWFACE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_FINISHNEWFACE->setObjectName(QString::fromUtf8("ID_ROOM_FINISHNEWFACE"));
        ID_ROOM_GRABTEXTURE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_GRABTEXTURE->setObjectName(QString::fromUtf8("ID_ROOM_GRABTEXTURE"));
        ID_ROOM_JOINROOMS = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_JOINROOMS->setObjectName(QString::fromUtf8("ID_ROOM_JOINROOMS"));
        ID_ROOM_JOINROOMSEXACT = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_JOINROOMSEXACT->setObjectName(QString::fromUtf8("ID_ROOM_JOINROOMSEXACT"));
        ID_ROOM_JOIN_ADJACENT_FACES = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_JOIN_ADJACENT_FACES->setObjectName(QString::fromUtf8("ID_ROOM_JOIN_ADJACENT_FACES"));
        ID_ROOM_LINKTONEWEXTERNAL = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_LINKTONEWEXTERNAL->setObjectName(QString::fromUtf8("ID_ROOM_LINKTONEWEXTERNAL"));
        ID_ROOM_MARK = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_MARK->setObjectName(QString::fromUtf8("ID_ROOM_MARK"));
        ID_ROOM_MERGEOBJECTINTOROOM = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_MERGEOBJECTINTOROOM->setObjectName(QString::fromUtf8("ID_ROOM_MERGEOBJECTINTOROOM"));
        ID_ROOM_NEXTFACE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_NEXTFACE->setObjectName(QString::fromUtf8("ID_ROOM_NEXTFACE"));
        ID_ROOM_PLACEROOM = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_PLACEROOM->setObjectName(QString::fromUtf8("ID_ROOM_PLACEROOM"));
        ID_ROOM_PLACETERRAINROOM = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_PLACETERRAINROOM->setObjectName(QString::fromUtf8("ID_ROOM_PLACETERRAINROOM"));
        ID_ROOM_PREVIOUSFACE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_PREVIOUSFACE->setObjectName(QString::fromUtf8("ID_ROOM_PREVIOUSFACE"));
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES->setObjectName(QString::fromUtf8("ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES"));
        ID_ROOM_PROPAGATETOALL = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_PROPAGATETOALL->setObjectName(QString::fromUtf8("ID_ROOM_PROPAGATETOALL"));
        ID_ROOM_RENAMEROOM = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_RENAMEROOM->setObjectName(QString::fromUtf8("ID_ROOM_RENAMEROOM"));
        ID_ROOM_ROTATEPLACEDROOM45DEGREES = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_ROTATEPLACEDROOM45DEGREES->setObjectName(QString::fromUtf8("ID_ROOM_ROTATEPLACEDROOM45DEGREES"));
        ID_ROOM_SAVECURRENTROOM = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_SAVECURRENTROOM->setObjectName(QString::fromUtf8("ID_ROOM_SAVECURRENTROOM"));
        ID_ROOM_SELECTBYNUMBER = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_SELECTBYNUMBER->setObjectName(QString::fromUtf8("ID_ROOM_SELECTBYNUMBER"));
        ID_ROOM_SELECTFACEBYNUMBER = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_SELECTFACEBYNUMBER->setObjectName(QString::fromUtf8("ID_ROOM_SELECTFACEBYNUMBER"));
        ID_ROOM_SETCENTERFROMVIEWER = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_SETCENTERFROMVIEWER->setObjectName(QString::fromUtf8("ID_ROOM_SETCENTERFROMVIEWER"));
        ID_ROOM_SNAPPLACEDROOM = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_SNAPPLACEDROOM->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPLACEDROOM"));
        ID_ROOM_SNAPPOINTTOEDGE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_SNAPPOINTTOEDGE->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPOINTTOEDGE"));
        ID_ROOM_SNAPPOINTTOFACE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_SNAPPOINTTOFACE->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPOINTTOFACE"));
        ID_ROOM_SNAPPOINTTOPOINT = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_SNAPPOINTTOPOINT->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPOINTTOPOINT"));
        ID_ROOM_SPLITFACE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_SPLITFACE->setObjectName(QString::fromUtf8("ID_ROOM_SPLITFACE"));
        ID_ROOM_STARTNEWFACE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_STARTNEWFACE->setObjectName(QString::fromUtf8("ID_ROOM_STARTNEWFACE"));
        ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE->setObjectName(QString::fromUtf8("ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE"));
        ID_ROOM_UNDOSNAP = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_UNDOSNAP->setObjectName(QString::fromUtf8("ID_ROOM_UNDOSNAP"));
        ID_ROOM_UNPLACEROOM = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_UNPLACEROOM->setObjectName(QString::fromUtf8("ID_ROOM_UNPLACEROOM"));
        ID_ROOM_VIEW = new QAction(IDD_LEVELKEYPAD);
        ID_ROOM_VIEW->setObjectName(QString::fromUtf8("ID_ROOM_VIEW"));
        ID_SCRIPT_LEVEL_INTERFACE = new QAction(IDD_LEVELKEYPAD);
        ID_SCRIPT_LEVEL_INTERFACE->setObjectName(QString::fromUtf8("ID_SCRIPT_LEVEL_INTERFACE"));
        ID_SUBEDITORS_FONT = new QAction(IDD_LEVELKEYPAD);
        ID_SUBEDITORS_FONT->setObjectName(QString::fromUtf8("ID_SUBEDITORS_FONT"));
        ID_SUBEDITORS_HOGMAKER = new QAction(IDD_LEVELKEYPAD);
        ID_SUBEDITORS_HOGMAKER->setObjectName(QString::fromUtf8("ID_SUBEDITORS_HOGMAKER"));
        ID_SUBEDITORS_TABLEFILEEDIT = new QAction(IDD_LEVELKEYPAD);
        ID_SUBEDITORS_TABLEFILEEDIT->setObjectName(QString::fromUtf8("ID_SUBEDITORS_TABLEFILEEDIT"));
        ID_SUBEDITORS_TABLEFILEFILTER = new QAction(IDD_LEVELKEYPAD);
        ID_SUBEDITORS_TABLEFILEFILTER->setObjectName(QString::fromUtf8("ID_SUBEDITORS_TABLEFILEFILTER"));
        ID_TABLEEDIT_FILE_EXIT = new QAction(IDD_LEVELKEYPAD);
        ID_TABLEEDIT_FILE_EXIT->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_EXIT"));
        ID_TABLEEDIT_FILE_LOAD = new QAction(IDD_LEVELKEYPAD);
        ID_TABLEEDIT_FILE_LOAD->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_LOAD"));
        ID_TABLEEDIT_FILE_SAVE = new QAction(IDD_LEVELKEYPAD);
        ID_TABLEEDIT_FILE_SAVE->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_SAVE"));
        ID_TABLEEDIT_FILE_SAVE_AS = new QAction(IDD_LEVELKEYPAD);
        ID_TABLEEDIT_FILE_SAVE_AS->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_SAVE_AS"));
        ID_TABLEFILTER_FILE_EXIT = new QAction(IDD_LEVELKEYPAD);
        ID_TABLEFILTER_FILE_EXIT->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_EXIT"));
        ID_TABLEFILTER_FILE_LOAD = new QAction(IDD_LEVELKEYPAD);
        ID_TABLEFILTER_FILE_LOAD->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_LOAD"));
        ID_TABLEFILTER_FILE_NEW = new QAction(IDD_LEVELKEYPAD);
        ID_TABLEFILTER_FILE_NEW->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_NEW"));
        ID_TABLEFILTER_FILE_SAVE = new QAction(IDD_LEVELKEYPAD);
        ID_TABLEFILTER_FILE_SAVE->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_SAVE"));
        ID_TABLEFILTER_FILE_SAVEAS = new QAction(IDD_LEVELKEYPAD);
        ID_TABLEFILTER_FILE_SAVEAS->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_SAVEAS"));
        ID_TERRAIN_VIEW = new QAction(IDD_LEVELKEYPAD);
        ID_TERRAIN_VIEW->setObjectName(QString::fromUtf8("ID_TERRAIN_VIEW"));
        ID_TEST_TEST1 = new QAction(IDD_LEVELKEYPAD);
        ID_TEST_TEST1->setObjectName(QString::fromUtf8("ID_TEST_TEST1"));
        ID_TEST_TEST2 = new QAction(IDD_LEVELKEYPAD);
        ID_TEST_TEST2->setObjectName(QString::fromUtf8("ID_TEST_TEST2"));
        ID_TEST_TEST3 = new QAction(IDD_LEVELKEYPAD);
        ID_TEST_TEST3->setObjectName(QString::fromUtf8("ID_TEST_TEST3"));
        ID_TOOLS_WORLD_OBJECTS_BUILDINGS = new QAction(IDD_LEVELKEYPAD);
        ID_TOOLS_WORLD_OBJECTS_BUILDINGS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_BUILDINGS"));
        ID_TOOLS_WORLD_OBJECTS_CLUTTER = new QAction(IDD_LEVELKEYPAD);
        ID_TOOLS_WORLD_OBJECTS_CLUTTER->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_CLUTTER"));
        ID_TOOLS_WORLD_OBJECTS_DOOR = new QAction(IDD_LEVELKEYPAD);
        ID_TOOLS_WORLD_OBJECTS_DOOR->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_DOOR"));
        ID_TOOLS_WORLD_OBJECTS_LIGHTS = new QAction(IDD_LEVELKEYPAD);
        ID_TOOLS_WORLD_OBJECTS_LIGHTS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_LIGHTS"));
        ID_TOOLS_WORLD_OBJECTS_PLAYER = new QAction(IDD_LEVELKEYPAD);
        ID_TOOLS_WORLD_OBJECTS_PLAYER->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_PLAYER"));
        ID_TOOLS_WORLD_OBJECTS_POWERUPS = new QAction(IDD_LEVELKEYPAD);
        ID_TOOLS_WORLD_OBJECTS_POWERUPS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_POWERUPS"));
        ID_TOOLS_WORLD_OBJECTS_ROBOTS = new QAction(IDD_LEVELKEYPAD);
        ID_TOOLS_WORLD_OBJECTS_ROBOTS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_ROBOTS"));
        ID_TOOLS_WORLD_OBJECTS_SOUND = new QAction(IDD_LEVELKEYPAD);
        ID_TOOLS_WORLD_OBJECTS_SOUND->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_SOUND"));
        ID_TOOLS_WORLD_TEXTURES = new QAction(IDD_LEVELKEYPAD);
        ID_TOOLS_WORLD_TEXTURES->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_TEXTURES"));
        ID_TOOLS_WORLD_WEAPONS = new QAction(IDD_LEVELKEYPAD);
        ID_TOOLS_WORLD_WEAPONS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_WEAPONS"));
        ID_VIEW_CENTERONCUBE = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_CENTERONCUBE->setObjectName(QString::fromUtf8("ID_VIEW_CENTERONCUBE"));
        ID_VIEW_CENTERONMINE = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_CENTERONMINE->setObjectName(QString::fromUtf8("ID_VIEW_CENTERONMINE"));
        ID_VIEW_CENTERONOBJECT = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_CENTERONOBJECT->setObjectName(QString::fromUtf8("ID_VIEW_CENTERONOBJECT"));
        ID_VIEW_DELETEVIEWER = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_DELETEVIEWER->setObjectName(QString::fromUtf8("ID_VIEW_DELETEVIEWER"));
        ID_VIEW_FLIP = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_FLIP->setObjectName(QString::fromUtf8("ID_VIEW_FLIP"));
        ID_VIEW_KEYPAD_TOGGLE = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_KEYPAD_TOGGLE->setObjectName(QString::fromUtf8("ID_VIEW_KEYPAD_TOGGLE"));
        ID_VIEW_MOVECAMERATOCURRENTOBJECT = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_MOVECAMERATOCURRENTOBJECT->setObjectName(QString::fromUtf8("ID_VIEW_MOVECAMERATOCURRENTOBJECT"));
        ID_VIEW_MOVECAMERATOSELECTEDFACE = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_MOVECAMERATOSELECTEDFACE->setObjectName(QString::fromUtf8("ID_VIEW_MOVECAMERATOSELECTEDFACE"));
        ID_VIEW_MOVECAMERATOSELECTEDROOM = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_MOVECAMERATOSELECTEDROOM->setObjectName(QString::fromUtf8("ID_VIEW_MOVECAMERATOSELECTEDROOM"));
        ID_VIEW_NEWVIEWER = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_NEWVIEWER->setObjectName(QString::fromUtf8("ID_VIEW_NEWVIEWER"));
        ID_VIEW_NEXTVIEWER = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_NEXTVIEWER->setObjectName(QString::fromUtf8("ID_VIEW_NEXTVIEWER"));
        ID_VIEW_RESETVIEWRADIUS = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_RESETVIEWRADIUS->setObjectName(QString::fromUtf8("ID_VIEW_RESETVIEWRADIUS"));
        ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW->setObjectName(QString::fromUtf8("ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW"));
        ID_VIEW_SHOWVIEWERFORWARDVECTOR = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_SHOWVIEWERFORWARDVECTOR->setObjectName(QString::fromUtf8("ID_VIEW_SHOWVIEWERFORWARDVECTOR"));
        ID_VIEW_STATUS_BAR = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_STATUS_BAR->setObjectName(QString::fromUtf8("ID_VIEW_STATUS_BAR"));
        ID_VIEW_TEXTUREMINE = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_TEXTUREMINE->setObjectName(QString::fromUtf8("ID_VIEW_TEXTUREMINE"));
        ID_VIEW_TOOLBAR = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_TOOLBAR->setObjectName(QString::fromUtf8("ID_VIEW_TOOLBAR"));
        ID_VIEW_VIEWPROP = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_VIEWPROP->setObjectName(QString::fromUtf8("ID_VIEW_VIEWPROP"));
        ID_VIEW_WIREFRAMEMINE = new QAction(IDD_LEVELKEYPAD);
        ID_VIEW_WIREFRAMEMINE->setObjectName(QString::fromUtf8("ID_VIEW_WIREFRAMEMINE"));
        ID_WINDOW_CASCADE = new QAction(IDD_LEVELKEYPAD);
        ID_WINDOW_CASCADE->setObjectName(QString::fromUtf8("ID_WINDOW_CASCADE"));
        ID_WINDOW_TILE = new QAction(IDD_LEVELKEYPAD);
        ID_WINDOW_TILE->setObjectName(QString::fromUtf8("ID_WINDOW_TILE"));
        ID_ZBUTTON = new QAction(IDD_LEVELKEYPAD);
        ID_ZBUTTON->setObjectName(QString::fromUtf8("ID_ZBUTTON"));
        layout_20 = new QVBoxLayout(IDD_LEVELKEYPAD);
#ifndef Q_OS_MAC
        layout_20->setSpacing(6);
#endif
        layout_20->setContentsMargins(0, 0, 0, 0);
        layout_20->setObjectName(QString::fromUtf8("layout_20"));
        IDC_STATIC = new QGroupBox(IDD_LEVELKEYPAD);
        IDC_STATIC->setObjectName(QString::fromUtf8("IDC_STATIC"));
        IDC_STATIC->setVisible(true);
        IDC_STATIC->setEnabled(true);
        IDC_STATIC->setFlat(false);
        IDC_STATIC->setMinimumSize(QSize(267, 82));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(IDC_STATIC->sizePolicy().hasHeightForWidth());
        IDC_STATIC->setSizePolicy(sizePolicy);
        layout = new QVBoxLayout(IDC_STATIC);
#ifndef Q_OS_MAC
        layout->setSpacing(6);
#endif
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setObjectName(QString::fromUtf8("layout"));
        layout_2 = new QGridLayout();
#ifndef Q_OS_MAC
        layout_2->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        layout_2->setContentsMargins(0, 0, 0, 0);
#endif
        layout_2->setObjectName(QString::fromUtf8("layout_2"));
        IDC_STATIC_2 = new QLabel(IDC_STATIC);
        IDC_STATIC_2->setObjectName(QString::fromUtf8("IDC_STATIC_2"));
        IDC_STATIC_2->setVisible(true);
        IDC_STATIC_2->setEnabled(true);
        IDC_STATIC_2->setAlignment(Qt::AlignLeft);
        IDC_STATIC_2->setMinimumSize(QSize(106, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_2->sizePolicy().hasHeightForWidth());
        IDC_STATIC_2->setSizePolicy(sizePolicy);

        layout_2->addWidget(IDC_STATIC_2, 0, 0, 1, 2);

        IDC_LEVEL_GRAVITY_EDIT = new QLineEdit(IDC_STATIC);
        IDC_LEVEL_GRAVITY_EDIT->setObjectName(QString::fromUtf8("IDC_LEVEL_GRAVITY_EDIT"));
        IDC_LEVEL_GRAVITY_EDIT->setVisible(true);
        IDC_LEVEL_GRAVITY_EDIT->setEnabled(true);
        IDC_LEVEL_GRAVITY_EDIT->setMinimumSize(QSize(52, 16));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GRAVITY_EDIT->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GRAVITY_EDIT->setSizePolicy(sizePolicy);

        layout_2->addWidget(IDC_LEVEL_GRAVITY_EDIT, 0, 2, 2, 1);

        IDC_STATIC_3 = new QLabel(IDC_STATIC);
        IDC_STATIC_3->setObjectName(QString::fromUtf8("IDC_STATIC_3"));
        IDC_STATIC_3->setVisible(true);
        IDC_STATIC_3->setEnabled(true);
        IDC_STATIC_3->setAlignment(Qt::AlignLeft);
        IDC_STATIC_3->setMinimumSize(QSize(89, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_3->sizePolicy().hasHeightForWidth());
        IDC_STATIC_3->setSizePolicy(sizePolicy);

        layout_2->addWidget(IDC_STATIC_3, 2, 0, 1, 1);

        IDC_LEVEL_CEILING_EDIT = new QLineEdit(IDC_STATIC);
        IDC_LEVEL_CEILING_EDIT->setObjectName(QString::fromUtf8("IDC_LEVEL_CEILING_EDIT"));
        IDC_LEVEL_CEILING_EDIT->setVisible(true);
        IDC_LEVEL_CEILING_EDIT->setEnabled(true);
        IDC_LEVEL_CEILING_EDIT->setMinimumSize(QSize(52, 16));
        sizePolicy.setHeightForWidth(IDC_LEVEL_CEILING_EDIT->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_CEILING_EDIT->setSizePolicy(sizePolicy);

        layout_2->addWidget(IDC_LEVEL_CEILING_EDIT, 2, 2, 2, 1);


        layout->addLayout(layout_2);

        layout_3 = new QGridLayout();
#ifndef Q_OS_MAC
        layout_3->setSpacing(6);
#endif
        layout_3->setContentsMargins(0, 0, 0, 0);
        layout_3->setObjectName(QString::fromUtf8("layout_3"));
        IDC_LEVEL_CEILING_EVERYWHERE = new QCheckBox(IDC_STATIC);
        IDC_LEVEL_CEILING_EVERYWHERE->setObjectName(QString::fromUtf8("IDC_LEVEL_CEILING_EVERYWHERE"));
        IDC_LEVEL_CEILING_EVERYWHERE->setVisible(true);
        IDC_LEVEL_CEILING_EVERYWHERE->setEnabled(true);
        IDC_LEVEL_CEILING_EVERYWHERE->setMinimumSize(QSize(246, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_CEILING_EVERYWHERE->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_CEILING_EVERYWHERE->setSizePolicy(sizePolicy);

        layout_3->addWidget(IDC_LEVEL_CEILING_EVERYWHERE, 0, 0, 1, 1);

        IDC_LEVEL_PIW = new QCheckBox(IDC_STATIC);
        IDC_LEVEL_PIW->setObjectName(QString::fromUtf8("IDC_LEVEL_PIW"));
        IDC_LEVEL_PIW->setVisible(true);
        IDC_LEVEL_PIW->setEnabled(true);
        IDC_LEVEL_PIW->setMinimumSize(QSize(246, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_PIW->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_PIW->setSizePolicy(sizePolicy);

        layout_3->addWidget(IDC_LEVEL_PIW, 2, 0, 1, 1);


        layout->addLayout(layout_3);


        layout_20->addWidget(IDC_STATIC);

        IDC_STATIC_4 = new QGroupBox(IDD_LEVELKEYPAD);
        IDC_STATIC_4->setObjectName(QString::fromUtf8("IDC_STATIC_4"));
        IDC_STATIC_4->setVisible(true);
        IDC_STATIC_4->setEnabled(true);
        IDC_STATIC_4->setFlat(false);
        IDC_STATIC_4->setMinimumSize(QSize(253, 89));
        sizePolicy.setHeightForWidth(IDC_STATIC_4->sizePolicy().hasHeightForWidth());
        IDC_STATIC_4->setSizePolicy(sizePolicy);
        layout_4 = new QGridLayout(IDC_STATIC_4);
#ifndef Q_OS_MAC
        layout_4->setSpacing(6);
#endif
        layout_4->setContentsMargins(0, 0, 0, 0);
        layout_4->setObjectName(QString::fromUtf8("layout_4"));
        IDC_FFT1_COMBO = new QComboBox(IDC_STATIC_4);
        IDC_FFT1_COMBO->setObjectName(QString::fromUtf8("IDC_FFT1_COMBO"));
        IDC_FFT1_COMBO->setVisible(true);
        IDC_FFT1_COMBO->setEnabled(true);
        IDC_FFT1_COMBO->setEditable(true);
        IDC_FFT1_COMBO->setMinimumSize(QSize(130, 110));
        sizePolicy.setHeightForWidth(IDC_FFT1_COMBO->sizePolicy().hasHeightForWidth());
        IDC_FFT1_COMBO->setSizePolicy(sizePolicy);

        layout_4->addWidget(IDC_FFT1_COMBO, 0, 0, 5, 1);

        IDC_FIXED_FFT1_EDIT = new QLineEdit(IDC_STATIC_4);
        IDC_FIXED_FFT1_EDIT->setObjectName(QString::fromUtf8("IDC_FIXED_FFT1_EDIT"));
        IDC_FIXED_FFT1_EDIT->setVisible(true);
        IDC_FIXED_FFT1_EDIT->setEnabled(true);
        IDC_FIXED_FFT1_EDIT->setMinimumSize(QSize(68, 20));
        sizePolicy.setHeightForWidth(IDC_FIXED_FFT1_EDIT->sizePolicy().hasHeightForWidth());
        IDC_FIXED_FFT1_EDIT->setSizePolicy(sizePolicy);

        layout_4->addWidget(IDC_FIXED_FFT1_EDIT, 0, 2, 1, 1);

        IDC_FFT2_COMBO = new QComboBox(IDC_STATIC_4);
        IDC_FFT2_COMBO->setObjectName(QString::fromUtf8("IDC_FFT2_COMBO"));
        IDC_FFT2_COMBO->setVisible(true);
        IDC_FFT2_COMBO->setEnabled(true);
        IDC_FFT2_COMBO->setEditable(true);
        IDC_FFT2_COMBO->setMinimumSize(QSize(130, 130));
        sizePolicy.setHeightForWidth(IDC_FFT2_COMBO->sizePolicy().hasHeightForWidth());
        IDC_FFT2_COMBO->setSizePolicy(sizePolicy);

        layout_4->addWidget(IDC_FFT2_COMBO, 1, 0, 5, 1);

        IDC_FFT2_EDIT = new QLineEdit(IDC_STATIC_4);
        IDC_FFT2_EDIT->setObjectName(QString::fromUtf8("IDC_FFT2_EDIT"));
        IDC_FFT2_EDIT->setVisible(true);
        IDC_FFT2_EDIT->setEnabled(true);
        IDC_FFT2_EDIT->setMinimumSize(QSize(68, 20));
        sizePolicy.setHeightForWidth(IDC_FFT2_EDIT->sizePolicy().hasHeightForWidth());
        IDC_FFT2_EDIT->setSizePolicy(sizePolicy);

        layout_4->addWidget(IDC_FFT2_EDIT, 1, 2, 1, 1);

        IDC_FFT3_COMBO = new QComboBox(IDC_STATIC_4);
        IDC_FFT3_COMBO->setObjectName(QString::fromUtf8("IDC_FFT3_COMBO"));
        IDC_FFT3_COMBO->setVisible(true);
        IDC_FFT3_COMBO->setEnabled(true);
        IDC_FFT3_COMBO->setEditable(true);
        IDC_FFT3_COMBO->setMinimumSize(QSize(130, 130));
        sizePolicy.setHeightForWidth(IDC_FFT3_COMBO->sizePolicy().hasHeightForWidth());
        IDC_FFT3_COMBO->setSizePolicy(sizePolicy);

        layout_4->addWidget(IDC_FFT3_COMBO, 3, 0, 4, 1);

        IDC_FFT3_EDIT = new QLineEdit(IDC_STATIC_4);
        IDC_FFT3_EDIT->setObjectName(QString::fromUtf8("IDC_FFT3_EDIT"));
        IDC_FFT3_EDIT->setVisible(true);
        IDC_FFT3_EDIT->setEnabled(true);
        IDC_FFT3_EDIT->setMinimumSize(QSize(68, 20));
        sizePolicy.setHeightForWidth(IDC_FFT3_EDIT->sizePolicy().hasHeightForWidth());
        IDC_FFT3_EDIT->setSizePolicy(sizePolicy);

        layout_4->addWidget(IDC_FFT3_EDIT, 3, 2, 1, 1);


        layout_20->addWidget(IDC_STATIC_4);

        IDC_STATIC_5 = new QGroupBox(IDD_LEVELKEYPAD);
        IDC_STATIC_5->setObjectName(QString::fromUtf8("IDC_STATIC_5"));
        IDC_STATIC_5->setVisible(true);
        IDC_STATIC_5->setEnabled(true);
        IDC_STATIC_5->setFlat(false);
        IDC_STATIC_5->setMinimumSize(QSize(253, 61));
        sizePolicy.setHeightForWidth(IDC_STATIC_5->sizePolicy().hasHeightForWidth());
        IDC_STATIC_5->setSizePolicy(sizePolicy);
        layout_5 = new QGridLayout(IDC_STATIC_5);
#ifndef Q_OS_MAC
        layout_5->setSpacing(6);
#endif
        layout_5->setContentsMargins(0, 0, 0, 0);
        layout_5->setObjectName(QString::fromUtf8("layout_5"));
        IDC_STATIC_6 = new QLabel(IDC_STATIC_5);
        IDC_STATIC_6->setObjectName(QString::fromUtf8("IDC_STATIC_6"));
        IDC_STATIC_6->setVisible(true);
        IDC_STATIC_6->setEnabled(true);
        IDC_STATIC_6->setAlignment(Qt::AlignLeft);
        IDC_STATIC_6->setMinimumSize(QSize(109, 13));
        sizePolicy.setHeightForWidth(IDC_STATIC_6->sizePolicy().hasHeightForWidth());
        IDC_STATIC_6->setSizePolicy(sizePolicy);

        layout_5->addWidget(IDC_STATIC_6, 0, 0, 1, 1);

        IDC_STATIC_7 = new QLabel(IDC_STATIC_5);
        IDC_STATIC_7->setObjectName(QString::fromUtf8("IDC_STATIC_7"));
        IDC_STATIC_7->setVisible(true);
        IDC_STATIC_7->setEnabled(true);
        IDC_STATIC_7->setAlignment(Qt::AlignLeft);
        IDC_STATIC_7->setMinimumSize(QSize(89, 13));
        sizePolicy.setHeightForWidth(IDC_STATIC_7->sizePolicy().hasHeightForWidth());
        IDC_STATIC_7->setSizePolicy(sizePolicy);

        layout_5->addWidget(IDC_STATIC_7, 0, 2, 1, 1);

        IDC_BGOS_COMBO = new QComboBox(IDC_STATIC_5);
        IDC_BGOS_COMBO->setObjectName(QString::fromUtf8("IDC_BGOS_COMBO"));
        IDC_BGOS_COMBO->setVisible(true);
        IDC_BGOS_COMBO->setEnabled(true);
        IDC_BGOS_COMBO->setEditable(true);
        IDC_BGOS_COMBO->setMinimumSize(QSize(116, 122));
        sizePolicy.setHeightForWidth(IDC_BGOS_COMBO->sizePolicy().hasHeightForWidth());
        IDC_BGOS_COMBO->setSizePolicy(sizePolicy);

        layout_5->addWidget(IDC_BGOS_COMBO, 1, 0, 1, 2);

        IDC_FFOS_COMBO = new QComboBox(IDC_STATIC_5);
        IDC_FFOS_COMBO->setObjectName(QString::fromUtf8("IDC_FFOS_COMBO"));
        IDC_FFOS_COMBO->setVisible(true);
        IDC_FFOS_COMBO->setEnabled(true);
        IDC_FFOS_COMBO->setEditable(true);
        IDC_FFOS_COMBO->setMinimumSize(QSize(123, 134));
        sizePolicy.setHeightForWidth(IDC_FFOS_COMBO->sizePolicy().hasHeightForWidth());
        IDC_FFOS_COMBO->setSizePolicy(sizePolicy);

        layout_5->addWidget(IDC_FFOS_COMBO, 1, 2, 2, 2);


        layout_20->addWidget(IDC_STATIC_5);

        IDC_STATIC_8 = new QGroupBox(IDD_LEVELKEYPAD);
        IDC_STATIC_8->setObjectName(QString::fromUtf8("IDC_STATIC_8"));
        IDC_STATIC_8->setVisible(true);
        IDC_STATIC_8->setEnabled(true);
        IDC_STATIC_8->setFlat(false);
        IDC_STATIC_8->setMinimumSize(QSize(267, 919));
        sizePolicy.setHeightForWidth(IDC_STATIC_8->sizePolicy().hasHeightForWidth());
        IDC_STATIC_8->setSizePolicy(sizePolicy);
        layout_8 = new QVBoxLayout(IDC_STATIC_8);
#ifndef Q_OS_MAC
        layout_8->setSpacing(6);
#endif
        layout_8->setContentsMargins(0, 0, 0, 0);
        layout_8->setObjectName(QString::fromUtf8("layout_8"));
        layout_9 = new QGridLayout();
#ifndef Q_OS_MAC
        layout_9->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        layout_9->setContentsMargins(0, 0, 0, 0);
#endif
        layout_9->setObjectName(QString::fromUtf8("layout_9"));
        IDC_LEVEL_NEW_GOAL_BUTTON = new QPushButton(IDC_STATIC_8);
        IDC_LEVEL_NEW_GOAL_BUTTON->setObjectName(QString::fromUtf8("IDC_LEVEL_NEW_GOAL_BUTTON"));
        IDC_LEVEL_NEW_GOAL_BUTTON->setVisible(true);
        IDC_LEVEL_NEW_GOAL_BUTTON->setEnabled(true);
        IDC_LEVEL_NEW_GOAL_BUTTON->setAutoDefault(false);
        IDC_LEVEL_NEW_GOAL_BUTTON->setMinimumSize(QSize(61, 20));
        sizePolicy.setHeightForWidth(IDC_LEVEL_NEW_GOAL_BUTTON->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_NEW_GOAL_BUTTON->setSizePolicy(sizePolicy);

        layout_9->addWidget(IDC_LEVEL_NEW_GOAL_BUTTON, 0, 0, 1, 1);

        IDC_LEVEL_NEXT_GOAL_BUTTON = new QPushButton(IDC_STATIC_8);
        IDC_LEVEL_NEXT_GOAL_BUTTON->setObjectName(QString::fromUtf8("IDC_LEVEL_NEXT_GOAL_BUTTON"));
        IDC_LEVEL_NEXT_GOAL_BUTTON->setVisible(true);
        IDC_LEVEL_NEXT_GOAL_BUTTON->setEnabled(true);
        IDC_LEVEL_NEXT_GOAL_BUTTON->setAutoDefault(false);
        IDC_LEVEL_NEXT_GOAL_BUTTON->setMinimumSize(QSize(61, 20));
        sizePolicy.setHeightForWidth(IDC_LEVEL_NEXT_GOAL_BUTTON->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_NEXT_GOAL_BUTTON->setSizePolicy(sizePolicy);

        layout_9->addWidget(IDC_LEVEL_NEXT_GOAL_BUTTON, 0, 2, 1, 1);

        IDC_LEVEL_DELETE_GOAL_BUTTON = new QPushButton(IDC_STATIC_8);
        IDC_LEVEL_DELETE_GOAL_BUTTON->setObjectName(QString::fromUtf8("IDC_LEVEL_DELETE_GOAL_BUTTON"));
        IDC_LEVEL_DELETE_GOAL_BUTTON->setVisible(true);
        IDC_LEVEL_DELETE_GOAL_BUTTON->setEnabled(true);
        IDC_LEVEL_DELETE_GOAL_BUTTON->setAutoDefault(false);
        IDC_LEVEL_DELETE_GOAL_BUTTON->setMinimumSize(QSize(61, 20));
        sizePolicy.setHeightForWidth(IDC_LEVEL_DELETE_GOAL_BUTTON->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_DELETE_GOAL_BUTTON->setSizePolicy(sizePolicy);

        layout_9->addWidget(IDC_LEVEL_DELETE_GOAL_BUTTON, 0, 4, 1, 1);

        IDC_LEVEL_PREV_GOAL_BUTTON = new QPushButton(IDC_STATIC_8);
        IDC_LEVEL_PREV_GOAL_BUTTON->setObjectName(QString::fromUtf8("IDC_LEVEL_PREV_GOAL_BUTTON"));
        IDC_LEVEL_PREV_GOAL_BUTTON->setVisible(true);
        IDC_LEVEL_PREV_GOAL_BUTTON->setEnabled(true);
        IDC_LEVEL_PREV_GOAL_BUTTON->setAutoDefault(false);
        IDC_LEVEL_PREV_GOAL_BUTTON->setMinimumSize(QSize(61, 20));
        sizePolicy.setHeightForWidth(IDC_LEVEL_PREV_GOAL_BUTTON->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_PREV_GOAL_BUTTON->setSizePolicy(sizePolicy);

        layout_9->addWidget(IDC_LEVEL_PREV_GOAL_BUTTON, 2, 2, 1, 1);


        layout_8->addLayout(layout_9);

        layout_10 = new QGridLayout();
#ifndef Q_OS_MAC
        layout_10->setSpacing(6);
#endif
        layout_10->setContentsMargins(0, 0, 0, 0);
        layout_10->setObjectName(QString::fromUtf8("layout_10"));
        IDC_LEVEL_GOAL_NUM_GOALS_STATIC = new QLabel(IDC_STATIC_8);
        IDC_LEVEL_GOAL_NUM_GOALS_STATIC->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_NUM_GOALS_STATIC"));
        IDC_LEVEL_GOAL_NUM_GOALS_STATIC->setVisible(true);
        IDC_LEVEL_GOAL_NUM_GOALS_STATIC->setEnabled(true);
        IDC_LEVEL_GOAL_NUM_GOALS_STATIC->setAlignment(Qt::AlignLeft);
        IDC_LEVEL_GOAL_NUM_GOALS_STATIC->setMinimumSize(QSize(95, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_NUM_GOALS_STATIC->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_NUM_GOALS_STATIC->setSizePolicy(sizePolicy);

        layout_10->addWidget(IDC_LEVEL_GOAL_NUM_GOALS_STATIC, 0, 0, 1, 1);

        IDC_LEVEL_GOAL_CUR_GOAL_STATIC = new QLabel(IDC_STATIC_8);
        IDC_LEVEL_GOAL_CUR_GOAL_STATIC->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_CUR_GOAL_STATIC"));
        IDC_LEVEL_GOAL_CUR_GOAL_STATIC->setVisible(true);
        IDC_LEVEL_GOAL_CUR_GOAL_STATIC->setEnabled(true);
        IDC_LEVEL_GOAL_CUR_GOAL_STATIC->setAlignment(Qt::AlignLeft);
        IDC_LEVEL_GOAL_CUR_GOAL_STATIC->setMinimumSize(QSize(95, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_CUR_GOAL_STATIC->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_CUR_GOAL_STATIC->setSizePolicy(sizePolicy);

        layout_10->addWidget(IDC_LEVEL_GOAL_CUR_GOAL_STATIC, 0, 1, 1, 1);


        layout_8->addLayout(layout_10);

        IDC_STATIC_9 = new QLabel(IDC_STATIC_8);
        IDC_STATIC_9->setObjectName(QString::fromUtf8("IDC_STATIC_9"));
        IDC_STATIC_9->setVisible(true);
        IDC_STATIC_9->setEnabled(true);
        IDC_STATIC_9->setAlignment(Qt::AlignLeft);
        IDC_STATIC_9->setMinimumSize(QSize(208, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_9->sizePolicy().hasHeightForWidth());
        IDC_STATIC_9->setSizePolicy(sizePolicy);

        layout_8->addWidget(IDC_STATIC_9);

        IDC_LEVEL_GOAL_NAME_EDIT = new QLineEdit(IDC_STATIC_8);
        IDC_LEVEL_GOAL_NAME_EDIT->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_NAME_EDIT"));
        IDC_LEVEL_GOAL_NAME_EDIT->setVisible(true);
        IDC_LEVEL_GOAL_NAME_EDIT->setEnabled(true);
        IDC_LEVEL_GOAL_NAME_EDIT->setMinimumSize(QSize(253, 16));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_NAME_EDIT->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_NAME_EDIT->setSizePolicy(sizePolicy);

        layout_8->addWidget(IDC_LEVEL_GOAL_NAME_EDIT);

        IDC_STATIC_10 = new QLabel(IDC_STATIC_8);
        IDC_STATIC_10->setObjectName(QString::fromUtf8("IDC_STATIC_10"));
        IDC_STATIC_10->setVisible(true);
        IDC_STATIC_10->setEnabled(true);
        IDC_STATIC_10->setAlignment(Qt::AlignLeft);
        IDC_STATIC_10->setMinimumSize(QSize(187, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_10->sizePolicy().hasHeightForWidth());
        IDC_STATIC_10->setSizePolicy(sizePolicy);

        layout_8->addWidget(IDC_STATIC_10);

        IDC_LEVEL_GOAL_LOC_NAME_EDIT = new QLineEdit(IDC_STATIC_8);
        IDC_LEVEL_GOAL_LOC_NAME_EDIT->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_LOC_NAME_EDIT"));
        IDC_LEVEL_GOAL_LOC_NAME_EDIT->setVisible(true);
        IDC_LEVEL_GOAL_LOC_NAME_EDIT->setEnabled(true);
        IDC_LEVEL_GOAL_LOC_NAME_EDIT->setMinimumSize(QSize(253, 16));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_LOC_NAME_EDIT->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_LOC_NAME_EDIT->setSizePolicy(sizePolicy);

        layout_8->addWidget(IDC_LEVEL_GOAL_LOC_NAME_EDIT);

        IDC_STATIC_11 = new QLabel(IDC_STATIC_8);
        IDC_STATIC_11->setObjectName(QString::fromUtf8("IDC_STATIC_11"));
        IDC_STATIC_11->setVisible(true);
        IDC_STATIC_11->setEnabled(true);
        IDC_STATIC_11->setAlignment(Qt::AlignLeft);
        IDC_STATIC_11->setMinimumSize(QSize(253, 20));
        sizePolicy.setHeightForWidth(IDC_STATIC_11->sizePolicy().hasHeightForWidth());
        IDC_STATIC_11->setSizePolicy(sizePolicy);

        layout_8->addWidget(IDC_STATIC_11);

        IDC_LEVEL_GOAL_DESC_EDIT = new QTextEdit(IDC_STATIC_8);
        IDC_LEVEL_GOAL_DESC_EDIT->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_DESC_EDIT"));
        IDC_LEVEL_GOAL_DESC_EDIT->setVisible(true);
        IDC_LEVEL_GOAL_DESC_EDIT->setEnabled(true);
        IDC_LEVEL_GOAL_DESC_EDIT->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        IDC_LEVEL_GOAL_DESC_EDIT->setMinimumSize(QSize(253, 34));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(IDC_LEVEL_GOAL_DESC_EDIT->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_DESC_EDIT->setSizePolicy(sizePolicy1);

        layout_8->addWidget(IDC_LEVEL_GOAL_DESC_EDIT);

        IDC_STATIC_12 = new QLabel(IDC_STATIC_8);
        IDC_STATIC_12->setObjectName(QString::fromUtf8("IDC_STATIC_12"));
        IDC_STATIC_12->setVisible(true);
        IDC_STATIC_12->setEnabled(false);
        IDC_STATIC_12->setAlignment(Qt::AlignLeft);
        IDC_STATIC_12->setMinimumSize(QSize(253, 20));
        sizePolicy.setHeightForWidth(IDC_STATIC_12->sizePolicy().hasHeightForWidth());
        IDC_STATIC_12->setSizePolicy(sizePolicy);

        layout_8->addWidget(IDC_STATIC_12);

        IDC_LEVEL_GOAL_COMPLETION_EDIT = new QLineEdit(IDC_STATIC_8);
        IDC_LEVEL_GOAL_COMPLETION_EDIT->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_COMPLETION_EDIT"));
        IDC_LEVEL_GOAL_COMPLETION_EDIT->setVisible(true);
        IDC_LEVEL_GOAL_COMPLETION_EDIT->setEnabled(false);
        IDC_LEVEL_GOAL_COMPLETION_EDIT->setMinimumSize(QSize(253, 16));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_COMPLETION_EDIT->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_COMPLETION_EDIT->setSizePolicy(sizePolicy);

        layout_8->addWidget(IDC_LEVEL_GOAL_COMPLETION_EDIT);

        layout_11 = new QGridLayout();
#ifndef Q_OS_MAC
        layout_11->setSpacing(6);
#endif
        layout_11->setContentsMargins(0, 0, 0, 0);
        layout_11->setObjectName(QString::fromUtf8("layout_11"));
        IDC_STATIC_13 = new QLabel(IDC_STATIC_8);
        IDC_STATIC_13->setObjectName(QString::fromUtf8("IDC_STATIC_13"));
        IDC_STATIC_13->setVisible(true);
        IDC_STATIC_13->setEnabled(true);
        IDC_STATIC_13->setAlignment(Qt::AlignLeft);
        IDC_STATIC_13->setMinimumSize(QSize(109, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_13->sizePolicy().hasHeightForWidth());
        IDC_STATIC_13->setSizePolicy(sizePolicy);

        layout_11->addWidget(IDC_STATIC_13, 0, 0, 1, 1);

        IDC_LEVEL_GOAL_PRIORITY_EDIT1 = new QLineEdit(IDC_STATIC_8);
        IDC_LEVEL_GOAL_PRIORITY_EDIT1->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_PRIORITY_EDIT1"));
        IDC_LEVEL_GOAL_PRIORITY_EDIT1->setVisible(true);
        IDC_LEVEL_GOAL_PRIORITY_EDIT1->setEnabled(true);
        IDC_LEVEL_GOAL_PRIORITY_EDIT1->setMinimumSize(QSize(68, 16));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_PRIORITY_EDIT1->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_PRIORITY_EDIT1->setSizePolicy(sizePolicy);

        layout_11->addWidget(IDC_LEVEL_GOAL_PRIORITY_EDIT1, 0, 2, 2, 1);

        layout_11->setColumnMinimumWidth(0, 185);

        layout_8->addLayout(layout_11);

        layout_12 = new QGridLayout();
#ifndef Q_OS_MAC
        layout_12->setSpacing(6);
#endif
        layout_12->setContentsMargins(0, 0, 0, 0);
        layout_12->setObjectName(QString::fromUtf8("layout_12"));
        IDC_EDIT57 = new QLineEdit(IDC_STATIC_8);
        IDC_EDIT57->setObjectName(QString::fromUtf8("IDC_EDIT57"));
        IDC_EDIT57->setVisible(true);
        IDC_EDIT57->setEnabled(true);
        IDC_EDIT57->setMinimumSize(QSize(68, 16));
        sizePolicy.setHeightForWidth(IDC_EDIT57->sizePolicy().hasHeightForWidth());
        IDC_EDIT57->setSizePolicy(sizePolicy);

        layout_12->addWidget(IDC_EDIT57, 0, 2, 3, 1);

        IDC_STATIC_14 = new QLabel(IDC_STATIC_8);
        IDC_STATIC_14->setObjectName(QString::fromUtf8("IDC_STATIC_14"));
        IDC_STATIC_14->setVisible(true);
        IDC_STATIC_14->setEnabled(true);
        IDC_STATIC_14->setAlignment(Qt::AlignLeft);
        IDC_STATIC_14->setMinimumSize(QSize(137, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_14->sizePolicy().hasHeightForWidth());
        IDC_STATIC_14->setSizePolicy(sizePolicy);

        layout_12->addWidget(IDC_STATIC_14, 1, 0, 1, 1);

        layout_12->setColumnMinimumWidth(0, 185);

        layout_8->addLayout(layout_12);

        layout_13 = new QGridLayout();
#ifndef Q_OS_MAC
        layout_13->setSpacing(6);
#endif
        layout_13->setContentsMargins(0, 0, 0, 0);
        layout_13->setObjectName(QString::fromUtf8("layout_13"));
        IDC_LEVEL_GOAL_NEW_ITEM_BUTTON = new QPushButton(IDC_STATIC_8);
        IDC_LEVEL_GOAL_NEW_ITEM_BUTTON->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_NEW_ITEM_BUTTON"));
        IDC_LEVEL_GOAL_NEW_ITEM_BUTTON->setVisible(true);
        IDC_LEVEL_GOAL_NEW_ITEM_BUTTON->setEnabled(true);
        IDC_LEVEL_GOAL_NEW_ITEM_BUTTON->setAutoDefault(false);
        IDC_LEVEL_GOAL_NEW_ITEM_BUTTON->setMinimumSize(QSize(68, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_NEW_ITEM_BUTTON->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_NEW_ITEM_BUTTON->setSizePolicy(sizePolicy);

        layout_13->addWidget(IDC_LEVEL_GOAL_NEW_ITEM_BUTTON, 0, 0, 1, 1);

        IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON = new QPushButton(IDC_STATIC_8);
        IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON"));
        IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON->setVisible(true);
        IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON->setEnabled(true);
        IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON->setAutoDefault(false);
        IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON->setMinimumSize(QSize(68, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON->setSizePolicy(sizePolicy);

        layout_13->addWidget(IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON, 0, 2, 1, 1);


        layout_8->addLayout(layout_13);

        layout_14 = new QGridLayout();
#ifndef Q_OS_MAC
        layout_14->setSpacing(6);
#endif
        layout_14->setContentsMargins(0, 0, 0, 0);
        layout_14->setObjectName(QString::fromUtf8("layout_14"));
        IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON = new QPushButton(IDC_STATIC_8);
        IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON"));
        IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON->setVisible(true);
        IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON->setEnabled(true);
        IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON->setAutoDefault(false);
        IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON->setMinimumSize(QSize(68, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON->setSizePolicy(sizePolicy);

        layout_14->addWidget(IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON, 0, 0, 1, 1);

        IDC_LEVEL_GOAL_PREV_ITEM_BUTTON = new QPushButton(IDC_STATIC_8);
        IDC_LEVEL_GOAL_PREV_ITEM_BUTTON->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_PREV_ITEM_BUTTON"));
        IDC_LEVEL_GOAL_PREV_ITEM_BUTTON->setVisible(true);
        IDC_LEVEL_GOAL_PREV_ITEM_BUTTON->setEnabled(true);
        IDC_LEVEL_GOAL_PREV_ITEM_BUTTON->setAutoDefault(false);
        IDC_LEVEL_GOAL_PREV_ITEM_BUTTON->setMinimumSize(QSize(68, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_PREV_ITEM_BUTTON->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_PREV_ITEM_BUTTON->setSizePolicy(sizePolicy);

        layout_14->addWidget(IDC_LEVEL_GOAL_PREV_ITEM_BUTTON, 0, 2, 1, 1);

        IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON = new QPushButton(IDC_STATIC_8);
        IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON"));
        IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON->setVisible(true);
        IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON->setEnabled(true);
        IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON->setAutoDefault(false);
        IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON->setMinimumSize(QSize(68, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON->setSizePolicy(sizePolicy);

        layout_14->addWidget(IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON, 0, 4, 1, 1);


        layout_8->addLayout(layout_14);

        layout_15 = new QGridLayout();
#ifndef Q_OS_MAC
        layout_15->setSpacing(6);
#endif
        layout_15->setContentsMargins(0, 0, 0, 0);
        layout_15->setObjectName(QString::fromUtf8("layout_15"));
        IDC_LEVEL_GOAL_NUM_ITEMS_STATIC = new QLabel(IDC_STATIC_8);
        IDC_LEVEL_GOAL_NUM_ITEMS_STATIC->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_NUM_ITEMS_STATIC"));
        IDC_LEVEL_GOAL_NUM_ITEMS_STATIC->setVisible(true);
        IDC_LEVEL_GOAL_NUM_ITEMS_STATIC->setEnabled(true);
        IDC_LEVEL_GOAL_NUM_ITEMS_STATIC->setAlignment(Qt::AlignLeft);
        IDC_LEVEL_GOAL_NUM_ITEMS_STATIC->setMinimumSize(QSize(95, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_NUM_ITEMS_STATIC->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_NUM_ITEMS_STATIC->setSizePolicy(sizePolicy);

        layout_15->addWidget(IDC_LEVEL_GOAL_NUM_ITEMS_STATIC, 0, 0, 1, 1);

        IDC_LEVEL_GOAL_CUR_ITEM_STATIC = new QLabel(IDC_STATIC_8);
        IDC_LEVEL_GOAL_CUR_ITEM_STATIC->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_CUR_ITEM_STATIC"));
        IDC_LEVEL_GOAL_CUR_ITEM_STATIC->setVisible(true);
        IDC_LEVEL_GOAL_CUR_ITEM_STATIC->setEnabled(true);
        IDC_LEVEL_GOAL_CUR_ITEM_STATIC->setAlignment(Qt::AlignLeft);
        IDC_LEVEL_GOAL_CUR_ITEM_STATIC->setMinimumSize(QSize(95, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_CUR_ITEM_STATIC->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_CUR_ITEM_STATIC->setSizePolicy(sizePolicy);

        layout_15->addWidget(IDC_LEVEL_GOAL_CUR_ITEM_STATIC, 0, 2, 1, 1);

        IDC_STATIC_15 = new QGroupBox(IDC_STATIC_8);
        IDC_STATIC_15->setObjectName(QString::fromUtf8("IDC_STATIC_15"));
        IDC_STATIC_15->setVisible(true);
        IDC_STATIC_15->setEnabled(true);
        IDC_STATIC_15->setFlat(false);
        IDC_STATIC_15->setMinimumSize(QSize(253, 110));
        sizePolicy.setHeightForWidth(IDC_STATIC_15->sizePolicy().hasHeightForWidth());
        IDC_STATIC_15->setSizePolicy(sizePolicy);
        layout_6 = new QGridLayout(IDC_STATIC_15);
#ifndef Q_OS_MAC
        layout_6->setSpacing(6);
#endif
        layout_6->setContentsMargins(0, 0, 0, 0);
        layout_6->setObjectName(QString::fromUtf8("layout_6"));
        IDC_LEVELGOAL_ROOM_COMBO = new QComboBox(IDC_STATIC_15);
        IDC_LEVELGOAL_ROOM_COMBO->setObjectName(QString::fromUtf8("IDC_LEVELGOAL_ROOM_COMBO"));
        IDC_LEVELGOAL_ROOM_COMBO->setVisible(true);
        IDC_LEVELGOAL_ROOM_COMBO->setEnabled(true);
        IDC_LEVELGOAL_ROOM_COMBO->setEditable(true);
        IDC_LEVELGOAL_ROOM_COMBO->setMinimumSize(QSize(137, 72));
        sizePolicy.setHeightForWidth(IDC_LEVELGOAL_ROOM_COMBO->sizePolicy().hasHeightForWidth());
        IDC_LEVELGOAL_ROOM_COMBO->setSizePolicy(sizePolicy);

        layout_6->addWidget(IDC_LEVELGOAL_ROOM_COMBO, 0, 3, 8, 3);

        IDC_LEVEL_GOAL_ROOM_ITEM_RADIO = new QRadioButton(IDC_STATIC_15);
        IDC_LEVEL_GOAL_ROOM_ITEM_RADIO->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_ROOM_ITEM_RADIO"));
        IDC_LEVEL_GOAL_ROOM_ITEM_RADIO->setVisible(true);
        IDC_LEVEL_GOAL_ROOM_ITEM_RADIO->setEnabled(true);
        IDC_LEVEL_GOAL_ROOM_ITEM_RADIO->setMinimumSize(QSize(52, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_ROOM_ITEM_RADIO->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_ROOM_ITEM_RADIO->setSizePolicy(sizePolicy);

        layout_6->addWidget(IDC_LEVEL_GOAL_ROOM_ITEM_RADIO, 1, 0, 1, 1);

        IDC_LEVEL_GOAL_OBJ_ITEM_RADIO = new QRadioButton(IDC_STATIC_15);
        IDC_LEVEL_GOAL_OBJ_ITEM_RADIO->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_OBJ_ITEM_RADIO"));
        IDC_LEVEL_GOAL_OBJ_ITEM_RADIO->setVisible(true);
        IDC_LEVEL_GOAL_OBJ_ITEM_RADIO->setEnabled(true);
        IDC_LEVEL_GOAL_OBJ_ITEM_RADIO->setMinimumSize(QSize(80, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_OBJ_ITEM_RADIO->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_OBJ_ITEM_RADIO->setSizePolicy(sizePolicy);

        layout_6->addWidget(IDC_LEVEL_GOAL_OBJ_ITEM_RADIO, 3, 0, 1, 2);

        IDC_LEVELGOALOBJECT_COMBO = new QComboBox(IDC_STATIC_15);
        IDC_LEVELGOALOBJECT_COMBO->setObjectName(QString::fromUtf8("IDC_LEVELGOALOBJECT_COMBO"));
        IDC_LEVELGOALOBJECT_COMBO->setVisible(true);
        IDC_LEVELGOALOBJECT_COMBO->setEnabled(true);
        IDC_LEVELGOALOBJECT_COMBO->setEditable(true);
        IDC_LEVELGOALOBJECT_COMBO->setMinimumSize(QSize(137, 88));
        sizePolicy.setHeightForWidth(IDC_LEVELGOALOBJECT_COMBO->sizePolicy().hasHeightForWidth());
        IDC_LEVELGOALOBJECT_COMBO->setSizePolicy(sizePolicy);

        layout_6->addWidget(IDC_LEVELGOALOBJECT_COMBO, 3, 3, 9, 3);

        IDC_TRIGGER_RADIO = new QRadioButton(IDC_STATIC_15);
        IDC_TRIGGER_RADIO->setObjectName(QString::fromUtf8("IDC_TRIGGER_RADIO"));
        IDC_TRIGGER_RADIO->setVisible(true);
        IDC_TRIGGER_RADIO->setEnabled(true);
        IDC_TRIGGER_RADIO->setMinimumSize(QSize(95, 13));
        sizePolicy.setHeightForWidth(IDC_TRIGGER_RADIO->sizePolicy().hasHeightForWidth());
        IDC_TRIGGER_RADIO->setSizePolicy(sizePolicy);

        layout_6->addWidget(IDC_TRIGGER_RADIO, 5, 0, 1, 3);

        IDC_LEVELGOALTRIGGER_COMBO = new QComboBox(IDC_STATIC_15);
        IDC_LEVELGOALTRIGGER_COMBO->setObjectName(QString::fromUtf8("IDC_LEVELGOALTRIGGER_COMBO"));
        IDC_LEVELGOALTRIGGER_COMBO->setVisible(true);
        IDC_LEVELGOALTRIGGER_COMBO->setEnabled(true);
        IDC_LEVELGOALTRIGGER_COMBO->setEditable(true);
        IDC_LEVELGOALTRIGGER_COMBO->setMinimumSize(QSize(137, 92));
        sizePolicy.setHeightForWidth(IDC_LEVELGOALTRIGGER_COMBO->sizePolicy().hasHeightForWidth());
        IDC_LEVELGOALTRIGGER_COMBO->setSizePolicy(sizePolicy);

        layout_6->addWidget(IDC_LEVELGOALTRIGGER_COMBO, 5, 3, 8, 3);

        IDC_LEVEL_GOAL_CELL_ITEM_RADIO = new QRadioButton(IDC_STATIC_15);
        IDC_LEVEL_GOAL_CELL_ITEM_RADIO->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_CELL_ITEM_RADIO"));
        IDC_LEVEL_GOAL_CELL_ITEM_RADIO->setVisible(true);
        IDC_LEVEL_GOAL_CELL_ITEM_RADIO->setEnabled(true);
        IDC_LEVEL_GOAL_CELL_ITEM_RADIO->setMinimumSize(QSize(195, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_CELL_ITEM_RADIO->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_CELL_ITEM_RADIO->setSizePolicy(sizePolicy);

        layout_6->addWidget(IDC_LEVEL_GOAL_CELL_ITEM_RADIO, 7, 0, 2, 5);

        IDC_LEVEL_GOAL_MINE_RADIO = new QRadioButton(IDC_STATIC_15);
        IDC_LEVEL_GOAL_MINE_RADIO->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_MINE_RADIO"));
        IDC_LEVEL_GOAL_MINE_RADIO->setVisible(true);
        IDC_LEVEL_GOAL_MINE_RADIO->setEnabled(true);
        IDC_LEVEL_GOAL_MINE_RADIO->setMinimumSize(QSize(176, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_MINE_RADIO->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_MINE_RADIO->setSizePolicy(sizePolicy);

        layout_6->addWidget(IDC_LEVEL_GOAL_MINE_RADIO, 10, 0, 1, 4);


        layout_15->addWidget(IDC_STATIC_15, 2, 0, 1, 4);


        layout_8->addLayout(layout_15);

        IDC_STATIC_16 = new QGroupBox(IDC_STATIC_8);
        IDC_STATIC_16->setObjectName(QString::fromUtf8("IDC_STATIC_16"));
        IDC_STATIC_16->setVisible(true);
        IDC_STATIC_16->setEnabled(true);
        IDC_STATIC_16->setFlat(false);
        IDC_STATIC_16->setMinimumSize(QSize(253, 75));
        sizePolicy.setHeightForWidth(IDC_STATIC_16->sizePolicy().hasHeightForWidth());
        IDC_STATIC_16->setSizePolicy(sizePolicy);
        layout_7 = new QGridLayout(IDC_STATIC_16);
#ifndef Q_OS_MAC
        layout_7->setSpacing(6);
#endif
        layout_7->setContentsMargins(0, 0, 0, 0);
        layout_7->setObjectName(QString::fromUtf8("layout_7"));
        IDC_LG_ACTIVATE_RADIO = new QRadioButton(IDC_STATIC_16);
        IDC_LG_ACTIVATE_RADIO->setObjectName(QString::fromUtf8("IDC_LG_ACTIVATE_RADIO"));
        IDC_LG_ACTIVATE_RADIO->setVisible(true);
        IDC_LG_ACTIVATE_RADIO->setEnabled(true);
        IDC_LG_ACTIVATE_RADIO->setMinimumSize(QSize(116, 13));
        sizePolicy.setHeightForWidth(IDC_LG_ACTIVATE_RADIO->sizePolicy().hasHeightForWidth());
        IDC_LG_ACTIVATE_RADIO->setSizePolicy(sizePolicy);

        layout_7->addWidget(IDC_LG_ACTIVATE_RADIO, 0, 0, 1, 1);

        IDC_LG_LC_RADIO = new QRadioButton(IDC_STATIC_16);
        IDC_LG_LC_RADIO->setObjectName(QString::fromUtf8("IDC_LG_LC_RADIO"));
        IDC_LG_LC_RADIO->setVisible(true);
        IDC_LG_LC_RADIO->setEnabled(true);
        IDC_LG_LC_RADIO->setMinimumSize(QSize(116, 13));
        sizePolicy.setHeightForWidth(IDC_LG_LC_RADIO->sizePolicy().hasHeightForWidth());
        IDC_LG_LC_RADIO->setSizePolicy(sizePolicy);

        layout_7->addWidget(IDC_LG_LC_RADIO, 0, 3, 1, 3);

        IDC_LG_ENTER_RADIO = new QRadioButton(IDC_STATIC_16);
        IDC_LG_ENTER_RADIO->setObjectName(QString::fromUtf8("IDC_LG_ENTER_RADIO"));
        IDC_LG_ENTER_RADIO->setVisible(true);
        IDC_LG_ENTER_RADIO->setEnabled(true);
        IDC_LG_ENTER_RADIO->setMinimumSize(QSize(130, 13));
        sizePolicy.setHeightForWidth(IDC_LG_ENTER_RADIO->sizePolicy().hasHeightForWidth());
        IDC_LG_ENTER_RADIO->setSizePolicy(sizePolicy);

        layout_7->addWidget(IDC_LG_ENTER_RADIO, 2, 0, 2, 3);

        IDC_LG_DALLAS_RADIO = new QRadioButton(IDC_STATIC_16);
        IDC_LG_DALLAS_RADIO->setObjectName(QString::fromUtf8("IDC_LG_DALLAS_RADIO"));
        IDC_LG_DALLAS_RADIO->setVisible(true);
        IDC_LG_DALLAS_RADIO->setEnabled(true);
        IDC_LG_DALLAS_RADIO->setMinimumSize(QSize(109, 13));
        sizePolicy.setHeightForWidth(IDC_LG_DALLAS_RADIO->sizePolicy().hasHeightForWidth());
        IDC_LG_DALLAS_RADIO->setSizePolicy(sizePolicy);

        layout_7->addWidget(IDC_LG_DALLAS_RADIO, 2, 3, 2, 2);

        IDC_LG_DESTROYED_RADIO = new QRadioButton(IDC_STATIC_16);
        IDC_LG_DESTROYED_RADIO->setObjectName(QString::fromUtf8("IDC_LG_DESTROYED_RADIO"));
        IDC_LG_DESTROYED_RADIO->setVisible(true);
        IDC_LG_DESTROYED_RADIO->setEnabled(true);
        IDC_LG_DESTROYED_RADIO->setMinimumSize(QSize(123, 13));
        sizePolicy.setHeightForWidth(IDC_LG_DESTROYED_RADIO->sizePolicy().hasHeightForWidth());
        IDC_LG_DESTROYED_RADIO->setSizePolicy(sizePolicy);

        layout_7->addWidget(IDC_LG_DESTROYED_RADIO, 3, 0, 2, 2);

        IDC_LG_PPW_COLLIDE_RADIO = new QRadioButton(IDC_STATIC_16);
        IDC_LG_PPW_COLLIDE_RADIO->setObjectName(QString::fromUtf8("IDC_LG_PPW_COLLIDE_RADIO"));
        IDC_LG_PPW_COLLIDE_RADIO->setVisible(true);
        IDC_LG_PPW_COLLIDE_RADIO->setEnabled(true);
        IDC_LG_PPW_COLLIDE_RADIO->setMinimumSize(QSize(184, 13));
        sizePolicy.setHeightForWidth(IDC_LG_PPW_COLLIDE_RADIO->sizePolicy().hasHeightForWidth());
        IDC_LG_PPW_COLLIDE_RADIO->setSizePolicy(sizePolicy);

        layout_7->addWidget(IDC_LG_PPW_COLLIDE_RADIO, 6, 0, 1, 4);


        layout_8->addWidget(IDC_STATIC_16);

        layout_16 = new QGridLayout();
#ifndef Q_OS_MAC
        layout_16->setSpacing(6);
#endif
        layout_16->setContentsMargins(0, 0, 0, 0);
        layout_16->setObjectName(QString::fromUtf8("layout_16"));
        IDC_LEVEL_GOAL_SEC_GOAL_CHECK = new QCheckBox(IDC_STATIC_8);
        IDC_LEVEL_GOAL_SEC_GOAL_CHECK->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_SEC_GOAL_CHECK"));
        IDC_LEVEL_GOAL_SEC_GOAL_CHECK->setVisible(true);
        IDC_LEVEL_GOAL_SEC_GOAL_CHECK->setEnabled(true);
        IDC_LEVEL_GOAL_SEC_GOAL_CHECK->setMinimumSize(QSize(95, 11));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_SEC_GOAL_CHECK->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_SEC_GOAL_CHECK->setSizePolicy(sizePolicy);

        layout_16->addWidget(IDC_LEVEL_GOAL_SEC_GOAL_CHECK, 0, 0, 1, 3);

        IDC_LEVEL_GOAL_TLG_CHECK = new QCheckBox(IDC_STATIC_8);
        IDC_LEVEL_GOAL_TLG_CHECK->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_TLG_CHECK"));
        IDC_LEVEL_GOAL_TLG_CHECK->setVisible(true);
        IDC_LEVEL_GOAL_TLG_CHECK->setEnabled(true);
        IDC_LEVEL_GOAL_TLG_CHECK->setMinimumSize(QSize(137, 15));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_TLG_CHECK->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_TLG_CHECK->setSizePolicy(sizePolicy);

        layout_16->addWidget(IDC_LEVEL_GOAL_TLG_CHECK, 0, 4, 3, 2);

        IDC_LEVEL_GOAL_ENABLED_CHECK = new QCheckBox(IDC_STATIC_8);
        IDC_LEVEL_GOAL_ENABLED_CHECK->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_ENABLED_CHECK"));
        IDC_LEVEL_GOAL_ENABLED_CHECK->setVisible(true);
        IDC_LEVEL_GOAL_ENABLED_CHECK->setEnabled(true);
        IDC_LEVEL_GOAL_ENABLED_CHECK->setMinimumSize(QSize(61, 11));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_ENABLED_CHECK->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_ENABLED_CHECK->setSizePolicy(sizePolicy);

        layout_16->addWidget(IDC_LEVEL_GOAL_ENABLED_CHECK, 2, 0, 2, 1);

        IDC_LEVEL_GOAL_GDKL_CHECK = new QCheckBox(IDC_STATIC_8);
        IDC_LEVEL_GOAL_GDKL_CHECK->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_GDKL_CHECK"));
        IDC_LEVEL_GOAL_GDKL_CHECK->setVisible(true);
        IDC_LEVEL_GOAL_GDKL_CHECK->setEnabled(true);
        IDC_LEVEL_GOAL_GDKL_CHECK->setMinimumSize(QSize(143, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_GDKL_CHECK->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_GDKL_CHECK->setSizePolicy(sizePolicy);

        layout_16->addWidget(IDC_LEVEL_GOAL_GDKL_CHECK, 2, 4, 3, 3);

        IDC_LEVEL_GOAL_COMPLETED_CHECK = new QCheckBox(IDC_STATIC_8);
        IDC_LEVEL_GOAL_COMPLETED_CHECK->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_COMPLETED_CHECK"));
        IDC_LEVEL_GOAL_COMPLETED_CHECK->setVisible(true);
        IDC_LEVEL_GOAL_COMPLETED_CHECK->setEnabled(true);
        IDC_LEVEL_GOAL_COMPLETED_CHECK->setMinimumSize(QSize(75, 11));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_COMPLETED_CHECK->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_COMPLETED_CHECK->setSizePolicy(sizePolicy);

        layout_16->addWidget(IDC_LEVEL_GOAL_COMPLETED_CHECK, 6, 0, 1, 2);

        IDC_LEVEL_GOAL_NLB_CHECK = new QCheckBox(IDC_STATIC_8);
        IDC_LEVEL_GOAL_NLB_CHECK->setObjectName(QString::fromUtf8("IDC_LEVEL_GOAL_NLB_CHECK"));
        IDC_LEVEL_GOAL_NLB_CHECK->setVisible(true);
        IDC_LEVEL_GOAL_NLB_CHECK->setEnabled(true);
        IDC_LEVEL_GOAL_NLB_CHECK->setMinimumSize(QSize(130, 13));
        sizePolicy.setHeightForWidth(IDC_LEVEL_GOAL_NLB_CHECK->sizePolicy().hasHeightForWidth());
        IDC_LEVEL_GOAL_NLB_CHECK->setSizePolicy(sizePolicy);

        layout_16->addWidget(IDC_LEVEL_GOAL_NLB_CHECK, 6, 4, 2, 1);


        layout_8->addLayout(layout_16);

        IDC_STATIC_17 = new QLabel(IDC_STATIC_8);
        IDC_STATIC_17->setObjectName(QString::fromUtf8("IDC_STATIC_17"));
        IDC_STATIC_17->setVisible(true);
        IDC_STATIC_17->setEnabled(true);
        IDC_STATIC_17->setAlignment(Qt::AlignLeft);
        IDC_STATIC_17->setMinimumSize(QSize(157, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_17->sizePolicy().hasHeightForWidth());
        IDC_STATIC_17->setSizePolicy(sizePolicy);

        layout_8->addWidget(IDC_STATIC_17);

        IDC_GOAL_TABLE_EDIT = new QTextEdit(IDC_STATIC_8);
        IDC_GOAL_TABLE_EDIT->setObjectName(QString::fromUtf8("IDC_GOAL_TABLE_EDIT"));
        IDC_GOAL_TABLE_EDIT->setVisible(true);
        IDC_GOAL_TABLE_EDIT->setEnabled(true);
        IDC_GOAL_TABLE_EDIT->setReadOnly(true);
        IDC_GOAL_TABLE_EDIT->setTabChangesFocus(false);
        IDC_GOAL_TABLE_EDIT->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        IDC_GOAL_TABLE_EDIT->setMinimumSize(QSize(253, 279));
        sizePolicy1.setHeightForWidth(IDC_GOAL_TABLE_EDIT->sizePolicy().hasHeightForWidth());
        IDC_GOAL_TABLE_EDIT->setSizePolicy(sizePolicy1);

        layout_8->addWidget(IDC_GOAL_TABLE_EDIT);


        layout_20->addWidget(IDC_STATIC_8);

        IDC_STATIC_18 = new QGroupBox(IDD_LEVELKEYPAD);
        IDC_STATIC_18->setObjectName(QString::fromUtf8("IDC_STATIC_18"));
        IDC_STATIC_18->setVisible(true);
        IDC_STATIC_18->setEnabled(true);
        IDC_STATIC_18->setFlat(false);
        IDC_STATIC_18->setMinimumSize(QSize(260, 68));
        sizePolicy.setHeightForWidth(IDC_STATIC_18->sizePolicy().hasHeightForWidth());
        IDC_STATIC_18->setSizePolicy(sizePolicy);
        layout_17 = new QVBoxLayout(IDC_STATIC_18);
#ifndef Q_OS_MAC
        layout_17->setSpacing(6);
#endif
        layout_17->setContentsMargins(0, 0, 0, 0);
        layout_17->setObjectName(QString::fromUtf8("layout_17"));
        layout_18 = new QGridLayout();
#ifndef Q_OS_MAC
        layout_18->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        layout_18->setContentsMargins(0, 0, 0, 0);
#endif
        layout_18->setObjectName(QString::fromUtf8("layout_18"));
        IDC_STATIC_19 = new QLabel(IDC_STATIC_18);
        IDC_STATIC_19->setObjectName(QString::fromUtf8("IDC_STATIC_19"));
        IDC_STATIC_19->setVisible(true);
        IDC_STATIC_19->setEnabled(true);
        IDC_STATIC_19->setAlignment(Qt::AlignLeft);
        IDC_STATIC_19->setMinimumSize(QSize(27, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_19->sizePolicy().hasHeightForWidth());
        IDC_STATIC_19->setSizePolicy(sizePolicy);

        layout_18->addWidget(IDC_STATIC_19, 0, 0, 1, 1);

        IDC_STATIC_20 = new QLabel(IDC_STATIC_18);
        IDC_STATIC_20->setObjectName(QString::fromUtf8("IDC_STATIC_20"));
        IDC_STATIC_20->setVisible(true);
        IDC_STATIC_20->setEnabled(true);
        IDC_STATIC_20->setAlignment(Qt::AlignLeft);
        IDC_STATIC_20->setMinimumSize(QSize(68, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_20->sizePolicy().hasHeightForWidth());
        IDC_STATIC_20->setSizePolicy(sizePolicy);

        layout_18->addWidget(IDC_STATIC_20, 0, 2, 1, 1);


        layout_17->addLayout(layout_18);

        layout_19 = new QGridLayout();
#ifndef Q_OS_MAC
        layout_19->setSpacing(6);
#endif
        layout_19->setContentsMargins(0, 0, 0, 0);
        layout_19->setObjectName(QString::fromUtf8("layout_19"));
        IDC_STATIC_21 = new QLabel(IDC_STATIC_18);
        IDC_STATIC_21->setObjectName(QString::fromUtf8("IDC_STATIC_21"));
        IDC_STATIC_21->setVisible(true);
        IDC_STATIC_21->setEnabled(true);
        IDC_STATIC_21->setAlignment(Qt::AlignLeft);
        IDC_STATIC_21->setMinimumSize(QSize(75, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_21->sizePolicy().hasHeightForWidth());
        IDC_STATIC_21->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_STATIC_21, 0, 1, 2, 1);

        IDC_STATIC_22 = new QLabel(IDC_STATIC_18);
        IDC_STATIC_22->setObjectName(QString::fromUtf8("IDC_STATIC_22"));
        IDC_STATIC_22->setVisible(true);
        IDC_STATIC_22->setEnabled(true);
        IDC_STATIC_22->setAlignment(Qt::AlignLeft);
        IDC_STATIC_22->setMinimumSize(QSize(34, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_22->sizePolicy().hasHeightForWidth());
        IDC_STATIC_22->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_STATIC_22, 0, 4, 2, 2);

        IDC_STATIC_23 = new QLabel(IDC_STATIC_18);
        IDC_STATIC_23->setObjectName(QString::fromUtf8("IDC_STATIC_23"));
        IDC_STATIC_23->setVisible(true);
        IDC_STATIC_23->setEnabled(true);
        IDC_STATIC_23->setAlignment(Qt::AlignLeft);
        IDC_STATIC_23->setMinimumSize(QSize(34, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_23->sizePolicy().hasHeightForWidth());
        IDC_STATIC_23->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_STATIC_23, 0, 7, 2, 1);

        IDC_STATIC_24 = new QLabel(IDC_STATIC_18);
        IDC_STATIC_24->setObjectName(QString::fromUtf8("IDC_STATIC_24"));
        IDC_STATIC_24->setVisible(true);
        IDC_STATIC_24->setEnabled(true);
        IDC_STATIC_24->setAlignment(Qt::AlignLeft);
        IDC_STATIC_24->setMinimumSize(QSize(34, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_24->sizePolicy().hasHeightForWidth());
        IDC_STATIC_24->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_STATIC_24, 0, 8, 2, 2);

        IDC_STATIC_25 = new QLabel(IDC_STATIC_18);
        IDC_STATIC_25->setObjectName(QString::fromUtf8("IDC_STATIC_25"));
        IDC_STATIC_25->setVisible(true);
        IDC_STATIC_25->setEnabled(true);
        IDC_STATIC_25->setAlignment(Qt::AlignLeft);
        IDC_STATIC_25->setMinimumSize(QSize(34, 11));
        sizePolicy.setHeightForWidth(IDC_STATIC_25->sizePolicy().hasHeightForWidth());
        IDC_STATIC_25->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_STATIC_25, 0, 11, 2, 3);

        IDC_AMB_N_GROUPS2 = new QLineEdit(IDC_STATIC_18);
        IDC_AMB_N_GROUPS2->setObjectName(QString::fromUtf8("IDC_AMB_N_GROUPS2"));
        IDC_AMB_N_GROUPS2->setVisible(true);
        IDC_AMB_N_GROUPS2->setEnabled(true);
        IDC_AMB_N_GROUPS2->setMinimumSize(QSize(34, 20));
        sizePolicy.setHeightForWidth(IDC_AMB_N_GROUPS2->sizePolicy().hasHeightForWidth());
        IDC_AMB_N_GROUPS2->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_AMB_N_GROUPS2, 1, 4, 7, 2);

        IDC_AMB_MAX1 = new QLineEdit(IDC_STATIC_18);
        IDC_AMB_MAX1->setObjectName(QString::fromUtf8("IDC_AMB_MAX1"));
        IDC_AMB_MAX1->setVisible(true);
        IDC_AMB_MAX1->setEnabled(true);
        IDC_AMB_MAX1->setMinimumSize(QSize(34, 20));
        sizePolicy.setHeightForWidth(IDC_AMB_MAX1->sizePolicy().hasHeightForWidth());
        IDC_AMB_MAX1->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_AMB_MAX1, 1, 7, 7, 1);

        IDC_AMB_MIN1 = new QLineEdit(IDC_STATIC_18);
        IDC_AMB_MIN1->setObjectName(QString::fromUtf8("IDC_AMB_MIN1"));
        IDC_AMB_MIN1->setVisible(true);
        IDC_AMB_MIN1->setEnabled(true);
        IDC_AMB_MIN1->setMinimumSize(QSize(34, 20));
        sizePolicy.setHeightForWidth(IDC_AMB_MIN1->sizePolicy().hasHeightForWidth());
        IDC_AMB_MIN1->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_AMB_MIN1, 1, 8, 7, 2);

        IDC_AMB_TYPE6 = new QComboBox(IDC_STATIC_18);
        IDC_AMB_TYPE6->setObjectName(QString::fromUtf8("IDC_AMB_TYPE6"));
        IDC_AMB_TYPE6->setVisible(true);
        IDC_AMB_TYPE6->setEnabled(true);
        IDC_AMB_TYPE6->setEditable(true);
        IDC_AMB_TYPE6->setMinimumSize(QSize(89, 89));
        sizePolicy.setHeightForWidth(IDC_AMB_TYPE6->sizePolicy().hasHeightForWidth());
        IDC_AMB_TYPE6->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_AMB_TYPE6, 3, 0, 13, 3);

        IDC_AMB_N_GROUPS1 = new QLineEdit(IDC_STATIC_18);
        IDC_AMB_N_GROUPS1->setObjectName(QString::fromUtf8("IDC_AMB_N_GROUPS1"));
        IDC_AMB_N_GROUPS1->setVisible(true);
        IDC_AMB_N_GROUPS1->setEnabled(true);
        IDC_AMB_N_GROUPS1->setMinimumSize(QSize(34, 20));
        sizePolicy.setHeightForWidth(IDC_AMB_N_GROUPS1->sizePolicy().hasHeightForWidth());
        IDC_AMB_N_GROUPS1->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_AMB_N_GROUPS1, 3, 4, 6, 2);

        IDC_AMB_TYPE2 = new QComboBox(IDC_STATIC_18);
        IDC_AMB_TYPE2->setObjectName(QString::fromUtf8("IDC_AMB_TYPE2"));
        IDC_AMB_TYPE2->setVisible(true);
        IDC_AMB_TYPE2->setEnabled(true);
        IDC_AMB_TYPE2->setEditable(true);
        IDC_AMB_TYPE2->setMinimumSize(QSize(89, 82));
        sizePolicy.setHeightForWidth(IDC_AMB_TYPE2->sizePolicy().hasHeightForWidth());
        IDC_AMB_TYPE2->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_AMB_TYPE2, 4, 0, 10, 3);

        IDC_AMB_OUTSIDE1 = new QCheckBox(IDC_STATIC_18);
        IDC_AMB_OUTSIDE1->setObjectName(QString::fromUtf8("IDC_AMB_OUTSIDE1"));
        IDC_AMB_OUTSIDE1->setVisible(true);
        IDC_AMB_OUTSIDE1->setEnabled(true);
        IDC_AMB_OUTSIDE1->setMinimumSize(QSize(20, 13));
        sizePolicy.setHeightForWidth(IDC_AMB_OUTSIDE1->sizePolicy().hasHeightForWidth());
        IDC_AMB_OUTSIDE1->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_AMB_OUTSIDE1, 4, 11, 3, 2);

        IDC_AMB_TYPE3 = new QComboBox(IDC_STATIC_18);
        IDC_AMB_TYPE3->setObjectName(QString::fromUtf8("IDC_AMB_TYPE3"));
        IDC_AMB_TYPE3->setVisible(true);
        IDC_AMB_TYPE3->setEnabled(true);
        IDC_AMB_TYPE3->setEditable(true);
        IDC_AMB_TYPE3->setMinimumSize(QSize(89, 89));
        sizePolicy.setHeightForWidth(IDC_AMB_TYPE3->sizePolicy().hasHeightForWidth());
        IDC_AMB_TYPE3->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_AMB_TYPE3, 5, 0, 12, 3);

        IDC_FFT1_EDIT = new QLineEdit(IDC_STATIC_18);
        IDC_FFT1_EDIT->setObjectName(QString::fromUtf8("IDC_FFT1_EDIT"));
        IDC_FFT1_EDIT->setVisible(true);
        IDC_FFT1_EDIT->setEnabled(true);
        IDC_FFT1_EDIT->setMinimumSize(QSize(68, 20));
        sizePolicy.setHeightForWidth(IDC_FFT1_EDIT->sizePolicy().hasHeightForWidth());
        IDC_FFT1_EDIT->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_FFT1_EDIT, 6, 5, 5, 4);

        IDC_STATIC_26 = new QLabel(IDC_STATIC_18);
        IDC_STATIC_26->setObjectName(QString::fromUtf8("IDC_STATIC_26"));
        IDC_STATIC_26->setVisible(true);
        IDC_STATIC_26->setEnabled(true);
        IDC_STATIC_26->setAlignment(Qt::AlignLeft);
        IDC_STATIC_26->setMinimumSize(QSize(246, 13));
        sizePolicy.setHeightForWidth(IDC_STATIC_26->sizePolicy().hasHeightForWidth());
        IDC_STATIC_26->setSizePolicy(sizePolicy);

        layout_19->addWidget(IDC_STATIC_26, 10, 0, 2, 12);


        layout_17->addLayout(layout_19);


        layout_20->addWidget(IDC_STATIC_18);


        retranslateUi(IDD_LEVELKEYPAD);

        IDC_LEVEL_NEW_GOAL_BUTTON->setDefault(false);
        IDC_LEVEL_NEXT_GOAL_BUTTON->setDefault(false);
        IDC_LEVEL_DELETE_GOAL_BUTTON->setDefault(false);
        IDC_LEVEL_PREV_GOAL_BUTTON->setDefault(false);
        IDC_LEVEL_GOAL_NEW_ITEM_BUTTON->setDefault(false);
        IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON->setDefault(false);
        IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON->setDefault(false);
        IDC_LEVEL_GOAL_PREV_ITEM_BUTTON->setDefault(false);
        IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON->setDefault(false);


        QMetaObject::connectSlotsByName(IDD_LEVELKEYPAD);
    } // setupUi

    void retranslateUi(QDialog *IDD_LEVELKEYPAD)
    {
        IDD_ORPHANHUNTER->setText(QCoreApplication::translate("LevelKeypadLayout", "Orphan Hunter", nullptr));
        IDD_REORDER_PAGES->setText(QCoreApplication::translate("LevelKeypadLayout", "Reorder Net Pages", nullptr));
        IDD_SHOW_ALL_CHECKED_OUT->setText(QCoreApplication::translate("LevelKeypadLayout", "Show All Pages Checked Out", nullptr));
        IDM_IMPORT_BITMAP->setText(QCoreApplication::translate("LevelKeypadLayout", "Bitmap/Animation Converter...", nullptr));
        ID_APP_ABOUT->setText(QCoreApplication::translate("LevelKeypadLayout", "&About editor...", nullptr));
        ID_APP_EXIT->setText(QCoreApplication::translate("LevelKeypadLayout", "E&xit", nullptr));
#if QT_CONFIG(shortcut)
        ID_APP_EXIT->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Alt+X", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_BRIEFING_EDITOR->setText(QCoreApplication::translate("LevelKeypadLayout", "Briefing Editor...", nullptr));
        ID_BUTTON_LIGHTING->setText(QCoreApplication::translate("LevelKeypadLayout", "ID_BUTTON_LIGHTING", nullptr));
        ID_BUTTON_OUTLINE->setText(QCoreApplication::translate("LevelKeypadLayout", "ID_BUTTON_OUTLINE", nullptr));
        ID_BUTTON_WINDOWSELECTION->setText(QCoreApplication::translate("LevelKeypadLayout", "ID_BUTTON_WINDOWSELECTION", nullptr));
        ID_D3HELP->setText(QCoreApplication::translate("LevelKeypadLayout", "Help on D3 Editor", nullptr));
        ID_EDITORS_AIPROPERTIES->setText(QCoreApplication::translate("LevelKeypadLayout", "AI Properties...", nullptr));
        ID_EDITORS_AMBIENTSOUNDS->setText(QCoreApplication::translate("LevelKeypadLayout", "Ambient Sounds...", nullptr));
        ID_EDITORS_DALLAS->setText(QCoreApplication::translate("LevelKeypadLayout", "Dallas Graphical Script Editor...", nullptr));
        ID_EDITORS_FILES->setText(QCoreApplication::translate("LevelKeypadLayout", "Files...", nullptr));
        ID_EDITORS_MEGACELLS->setText(QCoreApplication::translate("LevelKeypadLayout", "Megacells...", nullptr));
        ID_EDIT_ADDSELECT->setText(QCoreApplication::translate("LevelKeypadLayout", "Add Current Room to Selected List", nullptr));
        ID_EDIT_ATTACH->setText(QCoreApplication::translate("LevelKeypadLayout", "&Attach", nullptr));
        ID_EDIT_CLEARSELECTED->setText(QCoreApplication::translate("LevelKeypadLayout", "Clear Selected List", nullptr));
        ID_EDIT_COPY->setText(QCoreApplication::translate("LevelKeypadLayout", "&Copy", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_COPY->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Ctrl+Ins", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_EDIT_CUT->setText(QCoreApplication::translate("LevelKeypadLayout", "Cu&t", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_CUT->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Shift+Del", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_EDIT_DELETE->setText(QCoreApplication::translate("LevelKeypadLayout", "&Delete", nullptr));
        ID_EDIT_LOADSCRAP->setText(QCoreApplication::translate("LevelKeypadLayout", "Load Scrap from Disk", nullptr));
        ID_EDIT_PASTE->setText(QCoreApplication::translate("LevelKeypadLayout", "&Paste", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_PASTE->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Shift+Ins", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_EDIT_PLACE->setText(QCoreApplication::translate("LevelKeypadLayout", "&Place in Mine", nullptr));
        ID_EDIT_PLACE_TERRAIN->setText(QCoreApplication::translate("LevelKeypadLayout", "&Place on Terrain", nullptr));
        ID_EDIT_REMOVESELECT->setText(QCoreApplication::translate("LevelKeypadLayout", "Remove Current Room from Selected List", nullptr));
        ID_EDIT_SAVESCRAP->setText(QCoreApplication::translate("LevelKeypadLayout", "Save Scrap to Disk", nullptr));
        ID_EDIT_SELECTATTACHED->setText(QCoreApplication::translate("LevelKeypadLayout", "Select All Rooms Attached to Current Room", nullptr));
        ID_EDIT_UNDO->setText(QCoreApplication::translate("LevelKeypadLayout", "&Undo", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_UNDO->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Alt+Backspace", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_AUTOSAVE->setText(QCoreApplication::translate("LevelKeypadLayout", "Autosave", nullptr));
        ID_FILE_CHECKMINE->setText(QCoreApplication::translate("LevelKeypadLayout", "&Check Mine", nullptr));
        ID_FILE_FIXCRACKS->setText(QCoreApplication::translate("LevelKeypadLayout", "Fix Level Cracks", nullptr));
        ID_FILE_FIXDEGENERATEFACES->setText(QCoreApplication::translate("LevelKeypadLayout", "Fix/Remove Degenerate Faces", nullptr));
        ID_FILE_IMPORT_ROOM->setText(QCoreApplication::translate("LevelKeypadLayout", "Import 3DSMax Room", nullptr));
        ID_FILE_INFORMATION->setText(QCoreApplication::translate("LevelKeypadLayout", "&Information", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_INFORMATION->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Alt+I", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_LEAVE_EDITOR->setText(QCoreApplication::translate("LevelKeypadLayout", "Go to main menu...", nullptr));
        ID_FILE_LEVELPROPS->setText(QCoreApplication::translate("LevelKeypadLayout", "Set Selected Room(s) Properties", nullptr));
        ID_FILE_LEVEL_INFO->setText(QCoreApplication::translate("LevelKeypadLayout", "Edit Level Info...", nullptr));
        ID_FILE_MRU_FILE1->setText(QCoreApplication::translate("LevelKeypadLayout", "Recent File", nullptr));
        ID_FILE_NEW->setText(QCoreApplication::translate("LevelKeypadLayout", "&New", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_NEW->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_OPEN->setText(QCoreApplication::translate("LevelKeypadLayout", "&Open...", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_OPEN->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_PLAY640X480->setText(QCoreApplication::translate("LevelKeypadLayout", "&Play in 640 x 480", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_PLAY640X480->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Alt+P", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_PREFERENCES->setText(QCoreApplication::translate("LevelKeypadLayout", "Preferences", nullptr));
        ID_FILE_PRINT->setText(QCoreApplication::translate("LevelKeypadLayout", "&Print...", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_PRINT->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Ctrl+P", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_PRINT_PREVIEW->setText(QCoreApplication::translate("LevelKeypadLayout", "Print Pre&view", nullptr));
        ID_FILE_PRINT_SETUP->setText(QCoreApplication::translate("LevelKeypadLayout", "P&rint Setup...", nullptr));
        ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM->setText(QCoreApplication::translate("LevelKeypadLayout", "Remove Duplicate Faces from Current Room", nullptr));
        ID_FILE_REMOVEEXTRAPOINTS->setText(QCoreApplication::translate("LevelKeypadLayout", "Remove Extra Points", nullptr));
        ID_FILE_RESTOREGAMESTATE->setText(QCoreApplication::translate("LevelKeypadLayout", "Restore Gamestate", nullptr));
        ID_FILE_SAVE->setText(QCoreApplication::translate("LevelKeypadLayout", "&Save", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_SAVE->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_SAVEGOALTEXT->setText(QCoreApplication::translate("LevelKeypadLayout", "Save Goal Text to File", nullptr));
        ID_FILE_SAVE_AS->setText(QCoreApplication::translate("LevelKeypadLayout", "Save &As...", nullptr));
        ID_FILE_SAVE_COPY_AS->setText(QCoreApplication::translate("LevelKeypadLayout", "Save Copy &As...", nullptr));
        ID_FILE_STATS->setText(QCoreApplication::translate("LevelKeypadLayout", "Show Level Stats", nullptr));
        ID_FILE_UPDATE->setText(QCoreApplication::translate("LevelKeypadLayout", "&Update", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_UPDATE->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_VERIFY_LEVEL->setText(QCoreApplication::translate("LevelKeypadLayout", "Verify Level", nullptr));
        ID_HOTSPOT_TGA->setText(QCoreApplication::translate("LevelKeypadLayout", "HotSpot TGA Extractor...", nullptr));
        ID_MINE_VIEW->setText(QCoreApplication::translate("LevelKeypadLayout", "Mine View", nullptr));
#if QT_CONFIG(shortcut)
        ID_MINE_VIEW->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "F2", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_OBJBUTTON->setText(QCoreApplication::translate("LevelKeypadLayout", "ID_OBJBUTTON", nullptr));
        ID_OBJECT_DELETEOBJECT->setText(QCoreApplication::translate("LevelKeypadLayout", "Delete Object", nullptr));
#if QT_CONFIG(shortcut)
        ID_OBJECT_DELETEOBJECT->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Del", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_OBJECT_MOVEPLAYER->setText(QCoreApplication::translate("LevelKeypadLayout", "Move Player 0 to Viewer", nullptr));
        ID_OBJECT_PLACECAMERAATCURRENTFACE->setText(QCoreApplication::translate("LevelKeypadLayout", "Create New Camera at Current Face", nullptr));
        ID_OBJECT_PLACECAMERAATVIEWER->setText(QCoreApplication::translate("LevelKeypadLayout", "Create New Camera at Viewer", nullptr));
        ID_OBJECT_PLACEOBJECT->setText(QCoreApplication::translate("LevelKeypadLayout", "Place Object", nullptr));
#if QT_CONFIG(shortcut)
        ID_OBJECT_PLACEOBJECT->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Ins", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_OBJECT_PLACESOUNDSOURCEATVIEWER->setText(QCoreApplication::translate("LevelKeypadLayout", "Create SoundSource at Viewer", nullptr));
        ID_OBJECT_PLACEWAYPOINTATVIEWER->setText(QCoreApplication::translate("LevelKeypadLayout", "Place Waypoint at Viewer", nullptr));
        ID_OBJECT_SELECTBYNUMBER->setText(QCoreApplication::translate("LevelKeypadLayout", "Select Object By Number", nullptr));
        ID_OBJECT_SETCAMERAFROMVIEWER->setText(QCoreApplication::translate("LevelKeypadLayout", "Move Camera to Viewer", nullptr));
        ID_OBJECT_SETVIEWERFROMCAMERA->setText(QCoreApplication::translate("LevelKeypadLayout", "Move Viewer to Camera", nullptr));
        ID_OSIRISCOMPILE->setText(QCoreApplication::translate("LevelKeypadLayout", "ID_OSIRISCOMPILE", nullptr));
        ID_REINIT_OPENGL->setText(QCoreApplication::translate("LevelKeypadLayout", "ID_REINIT_OPENGL", nullptr));
        ID_ROOM_ADD->setText(QCoreApplication::translate("LevelKeypadLayout", "&Add Room at Current Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_ADD->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Shift+A", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_ADDVERTTONEWFACE->setText(QCoreApplication::translate("LevelKeypadLayout", "Add Vert to New Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_ADDVERTTONEWFACE->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Shift+V", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_ATTACHROOM->setText(QCoreApplication::translate("LevelKeypadLayout", "Attach Room", nullptr));
        ID_ROOM_BUILDBRIDGE->setText(QCoreApplication::translate("LevelKeypadLayout", "&Build Bridge", nullptr));
        ID_ROOM_BUILDSMOOTHBRIDGE->setText(QCoreApplication::translate("LevelKeypadLayout", "Build Smooth Bridge", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_BUILDSMOOTHBRIDGE->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Ctrl+B", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_CLEARCENTERPOINT->setText(QCoreApplication::translate("LevelKeypadLayout", "Clear Center Point", nullptr));
        ID_ROOM_COMBINE->setText(QCoreApplication::translate("LevelKeypadLayout", "Combine Rooms", nullptr));
        ID_ROOM_DELETE->setText(QCoreApplication::translate("LevelKeypadLayout", "Delete Current Room", nullptr));
        ID_ROOM_DELETECONNECTEDFACES->setText(QCoreApplication::translate("LevelKeypadLayout", "Delete Connected Faces", nullptr));
        ID_ROOM_DELETEFACE->setText(QCoreApplication::translate("LevelKeypadLayout", "Delete Current Face", nullptr));
        ID_ROOM_DELETEPORTAL->setText(QCoreApplication::translate("LevelKeypadLayout", "Delete Current Portal", nullptr));
        ID_ROOM_DELETEVERT->setText(QCoreApplication::translate("LevelKeypadLayout", "Delete Current Vert From Face", nullptr));
        ID_ROOM_DROPROOM->setText(QCoreApplication::translate("LevelKeypadLayout", "Drop Room", nullptr));
        ID_ROOM_FACE_ADDVERTTOEDGE->setText(QCoreApplication::translate("LevelKeypadLayout", "Add Vert To Edge", nullptr));
        ID_ROOM_FACE_DELETEVERTONEDGE->setText(QCoreApplication::translate("LevelKeypadLayout", "Delete Vert on Edge", nullptr));
        ID_ROOM_FACE_MOVEVERTONEDGE->setText(QCoreApplication::translate("LevelKeypadLayout", "Move Vert on Edge", nullptr));
        ID_ROOM_FACE_SPLITFACE->setText(QCoreApplication::translate("LevelKeypadLayout", "Split Face", nullptr));
        ID_ROOM_FINISHNEWFACE->setText(QCoreApplication::translate("LevelKeypadLayout", "Add Last Point to New Face (Finish Face)", nullptr));
        ID_ROOM_GRABTEXTURE->setText(QCoreApplication::translate("LevelKeypadLayout", "Grab Texture", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_GRABTEXTURE->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "G", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_JOINROOMS->setText(QCoreApplication::translate("LevelKeypadLayout", "Join Rooms", nullptr));
        ID_ROOM_JOINROOMSEXACT->setText(QCoreApplication::translate("LevelKeypadLayout", "Join Rooms Exact", nullptr));
        ID_ROOM_JOIN_ADJACENT_FACES->setText(QCoreApplication::translate("LevelKeypadLayout", "Join Adjacent Faces", nullptr));
        ID_ROOM_LINKTONEWEXTERNAL->setText(QCoreApplication::translate("LevelKeypadLayout", "Link Room to New External Room", nullptr));
        ID_ROOM_MARK->setText(QCoreApplication::translate("LevelKeypadLayout", "&Mark Room/Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_MARK->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Ctrl+M", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_MERGEOBJECTINTOROOM->setText(QCoreApplication::translate("LevelKeypadLayout", "Merge Object into Room", nullptr));
        ID_ROOM_NEXTFACE->setText(QCoreApplication::translate("LevelKeypadLayout", "Next Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_NEXTFACE->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "F", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_PLACEROOM->setText(QCoreApplication::translate("LevelKeypadLayout", "Place Room at Current Room", nullptr));
        ID_ROOM_PLACETERRAINROOM->setText(QCoreApplication::translate("LevelKeypadLayout", "Place Room at Current Terrain Cell", nullptr));
        ID_ROOM_PREVIOUSFACE->setText(QCoreApplication::translate("LevelKeypadLayout", "Previous Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_PREVIOUSFACE->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Shift+F", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES->setText(QCoreApplication::translate("LevelKeypadLayout", "Propagate to Adjacent Coplanar Faces", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Ctrl+F", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOALL->setText(QCoreApplication::translate("LevelKeypadLayout", "Propagate to All Faces in Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOALL->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Shift+R", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_RENAMEROOM->setText(QCoreApplication::translate("LevelKeypadLayout", "Rename Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_RENAMEROOM->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Ctrl+R", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_ROTATEPLACEDROOM45DEGREES->setText(QCoreApplication::translate("LevelKeypadLayout", "Rotate Placed Room 45 Degrees", nullptr));
        ID_ROOM_SAVECURRENTROOM->setText(QCoreApplication::translate("LevelKeypadLayout", "Save Current Room", nullptr));
        ID_ROOM_SELECTBYNUMBER->setText(QCoreApplication::translate("LevelKeypadLayout", "Select Room by Number", nullptr));
        ID_ROOM_SELECTFACEBYNUMBER->setText(QCoreApplication::translate("LevelKeypadLayout", "Select Face by Number", nullptr));
        ID_ROOM_SETCENTERFROMVIEWER->setText(QCoreApplication::translate("LevelKeypadLayout", "Set Room Center from Viewer", nullptr));
        ID_ROOM_SNAPPLACEDROOM->setText(QCoreApplication::translate("LevelKeypadLayout", "Snap Placed Room", nullptr));
        ID_ROOM_SNAPPOINTTOEDGE->setText(QCoreApplication::translate("LevelKeypadLayout", "Snap Point to Edge", nullptr));
        ID_ROOM_SNAPPOINTTOFACE->setText(QCoreApplication::translate("LevelKeypadLayout", "Snap Point to Face", nullptr));
        ID_ROOM_SNAPPOINTTOPOINT->setText(QCoreApplication::translate("LevelKeypadLayout", "Snap Point to Point", nullptr));
        ID_ROOM_SPLITFACE->setText(QCoreApplication::translate("LevelKeypadLayout", "Triangulate face", nullptr));
        ID_ROOM_STARTNEWFACE->setText(QCoreApplication::translate("LevelKeypadLayout", "Add First Point to New Face (Start Face)", nullptr));
        ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE->setText(QCoreApplication::translate("LevelKeypadLayout", "Swap Marked and Current Room/Face", nullptr));
        ID_ROOM_UNDOSNAP->setText(QCoreApplication::translate("LevelKeypadLayout", "Undo Snap", nullptr));
        ID_ROOM_UNPLACEROOM->setText(QCoreApplication::translate("LevelKeypadLayout", "Un-place Room/Group/Door", nullptr));
        ID_ROOM_VIEW->setText(QCoreApplication::translate("LevelKeypadLayout", "Room View", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_VIEW->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "F4", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_SCRIPT_LEVEL_INTERFACE->setText(QCoreApplication::translate("LevelKeypadLayout", "Levels and Scripts...", nullptr));
        ID_SUBEDITORS_FONT->setText(QCoreApplication::translate("LevelKeypadLayout", "Font Editor...", nullptr));
        ID_SUBEDITORS_HOGMAKER->setText(QCoreApplication::translate("LevelKeypadLayout", "Hog Maker...", nullptr));
        ID_SUBEDITORS_TABLEFILEEDIT->setText(QCoreApplication::translate("LevelKeypadLayout", "Table File Text Editor...", nullptr));
        ID_SUBEDITORS_TABLEFILEFILTER->setText(QCoreApplication::translate("LevelKeypadLayout", "Table File Filter...", nullptr));
        ID_TABLEEDIT_FILE_EXIT->setText(QCoreApplication::translate("LevelKeypadLayout", "&Exit", nullptr));
        ID_TABLEEDIT_FILE_LOAD->setText(QCoreApplication::translate("LevelKeypadLayout", "&Load...", nullptr));
        ID_TABLEEDIT_FILE_SAVE->setText(QCoreApplication::translate("LevelKeypadLayout", "&Save", nullptr));
        ID_TABLEEDIT_FILE_SAVE_AS->setText(QCoreApplication::translate("LevelKeypadLayout", "S&ave as...", nullptr));
        ID_TABLEFILTER_FILE_EXIT->setText(QCoreApplication::translate("LevelKeypadLayout", "Exit", nullptr));
        ID_TABLEFILTER_FILE_LOAD->setText(QCoreApplication::translate("LevelKeypadLayout", "Load...", nullptr));
        ID_TABLEFILTER_FILE_NEW->setText(QCoreApplication::translate("LevelKeypadLayout", "New", nullptr));
        ID_TABLEFILTER_FILE_SAVE->setText(QCoreApplication::translate("LevelKeypadLayout", "Save", nullptr));
        ID_TABLEFILTER_FILE_SAVEAS->setText(QCoreApplication::translate("LevelKeypadLayout", "Save as...", nullptr));
        ID_TERRAIN_VIEW->setText(QCoreApplication::translate("LevelKeypadLayout", "Terrain View", nullptr));
#if QT_CONFIG(shortcut)
        ID_TERRAIN_VIEW->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "F3", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_TEST_TEST1->setText(QCoreApplication::translate("LevelKeypadLayout", "Test function 1", nullptr));
        ID_TEST_TEST2->setText(QCoreApplication::translate("LevelKeypadLayout", "Test function 2", nullptr));
        ID_TEST_TEST3->setText(QCoreApplication::translate("LevelKeypadLayout", "Test function 3", nullptr));
        ID_TOOLS_WORLD_OBJECTS_BUILDINGS->setText(QCoreApplication::translate("LevelKeypadLayout", "Buildings...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_CLUTTER->setText(QCoreApplication::translate("LevelKeypadLayout", "Clutter...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_DOOR->setText(QCoreApplication::translate("LevelKeypadLayout", "Doors...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_LIGHTS->setText(QCoreApplication::translate("LevelKeypadLayout", "Lights...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_PLAYER->setText(QCoreApplication::translate("LevelKeypadLayout", "Player Ships...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_POWERUPS->setText(QCoreApplication::translate("LevelKeypadLayout", "Powerups...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_ROBOTS->setText(QCoreApplication::translate("LevelKeypadLayout", "Robots...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_SOUND->setText(QCoreApplication::translate("LevelKeypadLayout", "Sounds...", nullptr));
        ID_TOOLS_WORLD_TEXTURES->setText(QCoreApplication::translate("LevelKeypadLayout", "Textures...", nullptr));
        ID_TOOLS_WORLD_WEAPONS->setText(QCoreApplication::translate("LevelKeypadLayout", "Weapons...", nullptr));
        ID_VIEW_CENTERONCUBE->setText(QCoreApplication::translate("LevelKeypadLayout", "Center on Current &Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_CENTERONCUBE->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Home", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_CENTERONMINE->setText(QCoreApplication::translate("LevelKeypadLayout", "Center on &Mine Origin", nullptr));
        ID_VIEW_CENTERONOBJECT->setText(QCoreApplication::translate("LevelKeypadLayout", "Center on Current &Object", nullptr));
        ID_VIEW_DELETEVIEWER->setText(QCoreApplication::translate("LevelKeypadLayout", "Delete Viewer", nullptr));
        ID_VIEW_FLIP->setText(QCoreApplication::translate("LevelKeypadLayout", "Flip Viewer", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_FLIP->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "/", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_KEYPAD_TOGGLE->setText(QCoreApplication::translate("LevelKeypadLayout", "Keypad", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_KEYPAD_TOGGLE->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "F10", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOCURRENTOBJECT->setText(QCoreApplication::translate("LevelKeypadLayout", "Move camera to Current Object", nullptr));
        ID_VIEW_MOVECAMERATOSELECTEDFACE->setText(QCoreApplication::translate("LevelKeypadLayout", "Move camera to &Current Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOSELECTEDFACE->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "Shift+C", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOSELECTEDROOM->setText(QCoreApplication::translate("LevelKeypadLayout", "Move camera to &Current Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOSELECTEDROOM->setShortcut(QCoreApplication::translate("LevelKeypadLayout", "C", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_NEWVIEWER->setText(QCoreApplication::translate("LevelKeypadLayout", "New Viewer", nullptr));
        ID_VIEW_NEXTVIEWER->setText(QCoreApplication::translate("LevelKeypadLayout", "Next Viewer", nullptr));
        ID_VIEW_RESETVIEWRADIUS->setText(QCoreApplication::translate("LevelKeypadLayout", "Reset View Radius", nullptr));
        ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW->setText(QCoreApplication::translate("LevelKeypadLayout", "Show objects in Wireframe view", nullptr));
        ID_VIEW_SHOWVIEWERFORWARDVECTOR->setText(QCoreApplication::translate("LevelKeypadLayout", "Show Viewer Forward Vector", nullptr));
        ID_VIEW_STATUS_BAR->setText(QCoreApplication::translate("LevelKeypadLayout", "&Status Bar", nullptr));
        ID_VIEW_TEXTUREMINE->setText(QCoreApplication::translate("LevelKeypadLayout", "Te&xtured view", nullptr));
        ID_VIEW_TOOLBAR->setText(QCoreApplication::translate("LevelKeypadLayout", "&Toolbar", nullptr));
        ID_VIEW_VIEWPROP->setText(QCoreApplication::translate("LevelKeypadLayout", "Display Viewer Properties", nullptr));
        ID_VIEW_WIREFRAMEMINE->setText(QCoreApplication::translate("LevelKeypadLayout", "&Wireframe view", nullptr));
        ID_WINDOW_CASCADE->setText(QCoreApplication::translate("LevelKeypadLayout", "&Cascade", nullptr));
        ID_WINDOW_TILE->setText(QCoreApplication::translate("LevelKeypadLayout", "&Tile", nullptr));
        ID_ZBUTTON->setText(QCoreApplication::translate("LevelKeypadLayout", "ID_ZBUTTON", nullptr));
        IDC_STATIC->setTitle(QCoreApplication::translate("LevelKeypadLayout", "World Properties", nullptr));
        IDC_STATIC_2->setText(QCoreApplication::translate("LevelKeypadLayout", "Gravity (32.2 is Earth)", nullptr));
        IDC_STATIC_3->setText(QCoreApplication::translate("LevelKeypadLayout", "Ceiling height", nullptr));
        IDC_LEVEL_CEILING_EVERYWHERE->setText(QCoreApplication::translate("LevelKeypadLayout", "Check ceiling collisions inside rooms and outside", nullptr));
        IDC_LEVEL_PIW->setText(QCoreApplication::translate("LevelKeypadLayout", "Powerups on this level should ignore wind", nullptr));
        IDC_STATIC_4->setTitle(QCoreApplication::translate("LevelKeypadLayout", "Special Forcefields and bounce factors", nullptr));
        IDC_STATIC_5->setTitle(QCoreApplication::translate("LevelKeypadLayout", "Sound Overrides", nullptr));
        IDC_STATIC_6->setText(QCoreApplication::translate("LevelKeypadLayout", "Glass Breaking Sound", nullptr));
        IDC_STATIC_7->setText(QCoreApplication::translate("LevelKeypadLayout", "Forcefield Sound", nullptr));
        IDC_STATIC_8->setTitle(QCoreApplication::translate("LevelKeypadLayout", "Goal Information", nullptr));
        IDC_LEVEL_NEW_GOAL_BUTTON->setText(QCoreApplication::translate("LevelKeypadLayout", "New Goal", nullptr));
        IDC_LEVEL_NEXT_GOAL_BUTTON->setText(QCoreApplication::translate("LevelKeypadLayout", "Next Goal", nullptr));
        IDC_LEVEL_DELETE_GOAL_BUTTON->setText(QCoreApplication::translate("LevelKeypadLayout", "Delete Goal", nullptr));
        IDC_LEVEL_PREV_GOAL_BUTTON->setText(QCoreApplication::translate("LevelKeypadLayout", "Prev Goal", nullptr));
        IDC_LEVEL_GOAL_NUM_GOALS_STATIC->setText(QCoreApplication::translate("LevelKeypadLayout", "Number of goals: 1", nullptr));
        IDC_LEVEL_GOAL_CUR_GOAL_STATIC->setText(QCoreApplication::translate("LevelKeypadLayout", "Current Goal: 1", nullptr));
        IDC_STATIC_9->setText(QCoreApplication::translate("LevelKeypadLayout", "Goal name (I.e.  Destroy the reactor)", nullptr));
        IDC_STATIC_10->setText(QCoreApplication::translate("LevelKeypadLayout", "Location/Object name (I.e. Reactor Core)", nullptr));
        IDC_STATIC_11->setText(QCoreApplication::translate("LevelKeypadLayout", "Goal Description (I.e.  You must destroy the reactor to disable the matcens and open the excape hatch.)", nullptr));
        IDC_STATIC_12->setText(QCoreApplication::translate("LevelKeypadLayout", "Completion HUD Message (I.e. Reactor destroyed!  Now get out of the mine!)", nullptr));
        IDC_STATIC_13->setText(QCoreApplication::translate("LevelKeypadLayout", "Priority (starts at zero)", nullptr));
        IDC_STATIC_14->setText(QCoreApplication::translate("LevelKeypadLayout", "Primary Objective List (0 - 3)", nullptr));
        IDC_LEVEL_GOAL_NEW_ITEM_BUTTON->setText(QCoreApplication::translate("LevelKeypadLayout", "New Item", nullptr));
        IDC_LEVEL_GOAL_NEXT_ITEM_BUTTON->setText(QCoreApplication::translate("LevelKeypadLayout", "Next Item", nullptr));
        IDC_LEVEL_GOAL_DELETE_ITEM_BUTTON->setText(QCoreApplication::translate("LevelKeypadLayout", "Delete Item", nullptr));
        IDC_LEVEL_GOAL_PREV_ITEM_BUTTON->setText(QCoreApplication::translate("LevelKeypadLayout", "Prev Item", nullptr));
        IDC_LEVEL_GOAL_GOTO_ITEM_BUTTON->setText(QCoreApplication::translate("LevelKeypadLayout", "Move Viewer", nullptr));
        IDC_LEVEL_GOAL_NUM_ITEMS_STATIC->setText(QCoreApplication::translate("LevelKeypadLayout", "Number of items: 1", nullptr));
        IDC_LEVEL_GOAL_CUR_ITEM_STATIC->setText(QCoreApplication::translate("LevelKeypadLayout", "Current Items: 1", nullptr));
        IDC_STATIC_15->setTitle(QCoreApplication::translate("LevelKeypadLayout", "Current Goal Item (Must be same type)", nullptr));
        IDC_LEVEL_GOAL_ROOM_ITEM_RADIO->setText(QCoreApplication::translate("LevelKeypadLayout", "Room", nullptr));
        IDC_LEVEL_GOAL_OBJ_ITEM_RADIO->setText(QCoreApplication::translate("LevelKeypadLayout", "Object Name", nullptr));
        IDC_TRIGGER_RADIO->setText(QCoreApplication::translate("LevelKeypadLayout", "Trigger Name", nullptr));
        IDC_LEVEL_GOAL_CELL_ITEM_RADIO->setText(QCoreApplication::translate("LevelKeypadLayout", "Anywhere on the terrain (Only this item)", nullptr));
        IDC_LEVEL_GOAL_MINE_RADIO->setText(QCoreApplication::translate("LevelKeypadLayout", "Anywhere in a mine (Only this item)", nullptr));
        IDC_STATIC_16->setTitle(QCoreApplication::translate("LevelKeypadLayout", "Completion Rule", nullptr));
        IDC_LG_ACTIVATE_RADIO->setText(QCoreApplication::translate("LevelKeypadLayout", "On Activate (triggers)", nullptr));
        IDC_LG_LC_RADIO->setText(QCoreApplication::translate("LevelKeypadLayout", "Player Collide", nullptr));
        IDC_LG_ENTER_RADIO->setText(QCoreApplication::translate("LevelKeypadLayout", "On Enter(rooms/terrain)", nullptr));
        IDC_LG_DALLAS_RADIO->setText(QCoreApplication::translate("LevelKeypadLayout", "DALLAS Controlled", nullptr));
        IDC_LG_DESTROYED_RADIO->setText(QCoreApplication::translate("LevelKeypadLayout", "On Pickup/Destroyed", nullptr));
        IDC_LG_PPW_COLLIDE_RADIO->setText(QCoreApplication::translate("LevelKeypadLayout", "Player or Player Weapon Collide", nullptr));
        IDC_LEVEL_GOAL_SEC_GOAL_CHECK->setText(QCoreApplication::translate("LevelKeypadLayout", "Secondary Goal", nullptr));
        IDC_LEVEL_GOAL_TLG_CHECK->setText(QCoreApplication::translate("LevelKeypadLayout", "This goal is an Objective", nullptr));
        IDC_LEVEL_GOAL_ENABLED_CHECK->setText(QCoreApplication::translate("LevelKeypadLayout", "Enabled", nullptr));
        IDC_LEVEL_GOAL_GDKL_CHECK->setText(QCoreApplication::translate("LevelKeypadLayout", "GB Doesn't Know Location", nullptr));
        IDC_LEVEL_GOAL_COMPLETED_CHECK->setText(QCoreApplication::translate("LevelKeypadLayout", "Completed", nullptr));
        IDC_LEVEL_GOAL_NLB_CHECK->setText(QCoreApplication::translate("LevelKeypadLayout", "Not Location Based", nullptr));
        IDC_STATIC_17->setText(QCoreApplication::translate("LevelKeypadLayout", "Objective and Goal List", nullptr));
        IDC_STATIC_18->setTitle(QCoreApplication::translate("LevelKeypadLayout", "Ambient creatures", nullptr));
        IDC_STATIC_19->setText(QCoreApplication::translate("LevelKeypadLayout", "Max", nullptr));
        IDC_STATIC_20->setText(QCoreApplication::translate("LevelKeypadLayout", "For each group", nullptr));
        IDC_STATIC_21->setText(QCoreApplication::translate("LevelKeypadLayout", "Creature Type", nullptr));
        IDC_STATIC_22->setText(QCoreApplication::translate("LevelKeypadLayout", "Alive", nullptr));
        IDC_STATIC_23->setText(QCoreApplication::translate("LevelKeypadLayout", "Max", nullptr));
        IDC_STATIC_24->setText(QCoreApplication::translate("LevelKeypadLayout", "Min", nullptr));
        IDC_STATIC_25->setText(QCoreApplication::translate("LevelKeypadLayout", "Inside", nullptr));
        IDC_STATIC_26->setText(QCoreApplication::translate("LevelKeypadLayout", "AMBIENT CREATURES - NO USED -SOMEDAY?", nullptr));
        (void)IDD_LEVELKEYPAD;
    } // retranslateUi

};

namespace Ui {
    class LevelKeypadLayout: public Ui_LevelKeypadLayout {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LEVELKEYPAD_LAYOUT_H
