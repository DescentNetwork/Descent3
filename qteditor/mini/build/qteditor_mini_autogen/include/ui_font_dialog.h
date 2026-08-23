/********************************************************************************
** Form generated from reading UI file 'font_dialog.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FONT_DIALOG_H
#define UI_FONT_DIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_FontDialog
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
    QLabel *IDC_STATIC_2;
    QLineEdit *IDC_EDIT_MINASCII;
    QRadioButton *IDC_PROP_MONO;
    QLabel *IDC_STATIC_PROP;
    QRadioButton *IDC_PROP_BITMAPPED;
    QLabel *IDC_STATIC_3;
    QLabel *IDC_STATIC_MAXASCII;
    QLineEdit *IDC_BRIGHTNESS;
    QCheckBox *IDC_PROP_GRADIANT;
    QLabel *IDC_STATIC_4;
    QGroupBox *IDC_STATIC_FONTNAME;
    QLabel *IDC_STATIC_5;
    QLineEdit *IDC_EDIT_FONTNAME;
    QPushButton *IDC_OPEN;
    QPushButton *IDC_SAVE;
    QPushButton *IDC_SAVEAS;
    QLabel *IDC_STATIC_FONTVIEW;
    QPushButton *IDOK;
    QPushButton *IDCANCEL;
    QPushButton *IDC_NEW;
    QPushButton *IDC_LOCK;
    QPushButton *IDC_CHECKIN;
    QPushButton *IDC_DELETE;

    void setupUi(QDialog *IDD_FONT_DIALOG)
    {
        if (IDD_FONT_DIALOG->objectName().isEmpty())
            IDD_FONT_DIALOG->setObjectName(QString::fromUtf8("IDD_FONT_DIALOG"));
        IDD_FONT_DIALOG->resize(725, 517);
        QFont font;
        font.setFamily(QString::fromUtf8("Liberation Sans"));
        font.setPointSize(8);
        font.setBold(false);
        font.setItalic(false);
        IDD_FONT_DIALOG->setFont(font);
        IDD_FONT_DIALOG->setEnabled(true);
        IDD_FONT_DIALOG->setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
        IDD_FONT_DIALOG->setMinimumSize(QSize(725, 517));
        IDD_FONT_DIALOG->setMaximumSize(QSize(725, 517));
        IDD_ORPHANHUNTER = new QAction(IDD_FONT_DIALOG);
        IDD_ORPHANHUNTER->setObjectName(QString::fromUtf8("IDD_ORPHANHUNTER"));
        IDD_REORDER_PAGES = new QAction(IDD_FONT_DIALOG);
        IDD_REORDER_PAGES->setObjectName(QString::fromUtf8("IDD_REORDER_PAGES"));
        IDD_SHOW_ALL_CHECKED_OUT = new QAction(IDD_FONT_DIALOG);
        IDD_SHOW_ALL_CHECKED_OUT->setObjectName(QString::fromUtf8("IDD_SHOW_ALL_CHECKED_OUT"));
        IDM_IMPORT_BITMAP = new QAction(IDD_FONT_DIALOG);
        IDM_IMPORT_BITMAP->setObjectName(QString::fromUtf8("IDM_IMPORT_BITMAP"));
        ID_APP_ABOUT = new QAction(IDD_FONT_DIALOG);
        ID_APP_ABOUT->setObjectName(QString::fromUtf8("ID_APP_ABOUT"));
        ID_APP_EXIT = new QAction(IDD_FONT_DIALOG);
        ID_APP_EXIT->setObjectName(QString::fromUtf8("ID_APP_EXIT"));
        ID_BRIEFING_EDITOR = new QAction(IDD_FONT_DIALOG);
        ID_BRIEFING_EDITOR->setObjectName(QString::fromUtf8("ID_BRIEFING_EDITOR"));
        ID_BUTTON_LIGHTING = new QAction(IDD_FONT_DIALOG);
        ID_BUTTON_LIGHTING->setObjectName(QString::fromUtf8("ID_BUTTON_LIGHTING"));
        ID_BUTTON_OUTLINE = new QAction(IDD_FONT_DIALOG);
        ID_BUTTON_OUTLINE->setObjectName(QString::fromUtf8("ID_BUTTON_OUTLINE"));
        ID_BUTTON_WINDOWSELECTION = new QAction(IDD_FONT_DIALOG);
        ID_BUTTON_WINDOWSELECTION->setObjectName(QString::fromUtf8("ID_BUTTON_WINDOWSELECTION"));
        ID_D3HELP = new QAction(IDD_FONT_DIALOG);
        ID_D3HELP->setObjectName(QString::fromUtf8("ID_D3HELP"));
        ID_EDITORS_AIPROPERTIES = new QAction(IDD_FONT_DIALOG);
        ID_EDITORS_AIPROPERTIES->setObjectName(QString::fromUtf8("ID_EDITORS_AIPROPERTIES"));
        ID_EDITORS_AMBIENTSOUNDS = new QAction(IDD_FONT_DIALOG);
        ID_EDITORS_AMBIENTSOUNDS->setObjectName(QString::fromUtf8("ID_EDITORS_AMBIENTSOUNDS"));
        ID_EDITORS_DALLAS = new QAction(IDD_FONT_DIALOG);
        ID_EDITORS_DALLAS->setObjectName(QString::fromUtf8("ID_EDITORS_DALLAS"));
        ID_EDITORS_FILES = new QAction(IDD_FONT_DIALOG);
        ID_EDITORS_FILES->setObjectName(QString::fromUtf8("ID_EDITORS_FILES"));
        ID_EDITORS_MEGACELLS = new QAction(IDD_FONT_DIALOG);
        ID_EDITORS_MEGACELLS->setObjectName(QString::fromUtf8("ID_EDITORS_MEGACELLS"));
        ID_EDIT_ADDSELECT = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_ADDSELECT->setObjectName(QString::fromUtf8("ID_EDIT_ADDSELECT"));
        ID_EDIT_ATTACH = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_ATTACH->setObjectName(QString::fromUtf8("ID_EDIT_ATTACH"));
        ID_EDIT_CLEARSELECTED = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_CLEARSELECTED->setObjectName(QString::fromUtf8("ID_EDIT_CLEARSELECTED"));
        ID_EDIT_COPY = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_COPY->setObjectName(QString::fromUtf8("ID_EDIT_COPY"));
        ID_EDIT_CUT = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_CUT->setObjectName(QString::fromUtf8("ID_EDIT_CUT"));
        ID_EDIT_DELETE = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_DELETE->setObjectName(QString::fromUtf8("ID_EDIT_DELETE"));
        ID_EDIT_LOADSCRAP = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_LOADSCRAP->setObjectName(QString::fromUtf8("ID_EDIT_LOADSCRAP"));
        ID_EDIT_PASTE = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_PASTE->setObjectName(QString::fromUtf8("ID_EDIT_PASTE"));
        ID_EDIT_PLACE = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_PLACE->setObjectName(QString::fromUtf8("ID_EDIT_PLACE"));
        ID_EDIT_PLACE_TERRAIN = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_PLACE_TERRAIN->setObjectName(QString::fromUtf8("ID_EDIT_PLACE_TERRAIN"));
        ID_EDIT_REMOVESELECT = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_REMOVESELECT->setObjectName(QString::fromUtf8("ID_EDIT_REMOVESELECT"));
        ID_EDIT_SAVESCRAP = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_SAVESCRAP->setObjectName(QString::fromUtf8("ID_EDIT_SAVESCRAP"));
        ID_EDIT_SELECTATTACHED = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_SELECTATTACHED->setObjectName(QString::fromUtf8("ID_EDIT_SELECTATTACHED"));
        ID_EDIT_UNDO = new QAction(IDD_FONT_DIALOG);
        ID_EDIT_UNDO->setObjectName(QString::fromUtf8("ID_EDIT_UNDO"));
        ID_FILE_AUTOSAVE = new QAction(IDD_FONT_DIALOG);
        ID_FILE_AUTOSAVE->setObjectName(QString::fromUtf8("ID_FILE_AUTOSAVE"));
        ID_FILE_AUTOSAVE->setCheckable(true);
        ID_FILE_CHECKMINE = new QAction(IDD_FONT_DIALOG);
        ID_FILE_CHECKMINE->setObjectName(QString::fromUtf8("ID_FILE_CHECKMINE"));
        ID_FILE_FIXCRACKS = new QAction(IDD_FONT_DIALOG);
        ID_FILE_FIXCRACKS->setObjectName(QString::fromUtf8("ID_FILE_FIXCRACKS"));
        ID_FILE_FIXDEGENERATEFACES = new QAction(IDD_FONT_DIALOG);
        ID_FILE_FIXDEGENERATEFACES->setObjectName(QString::fromUtf8("ID_FILE_FIXDEGENERATEFACES"));
        ID_FILE_IMPORT_ROOM = new QAction(IDD_FONT_DIALOG);
        ID_FILE_IMPORT_ROOM->setObjectName(QString::fromUtf8("ID_FILE_IMPORT_ROOM"));
        ID_FILE_INFORMATION = new QAction(IDD_FONT_DIALOG);
        ID_FILE_INFORMATION->setObjectName(QString::fromUtf8("ID_FILE_INFORMATION"));
        ID_FILE_LEAVE_EDITOR = new QAction(IDD_FONT_DIALOG);
        ID_FILE_LEAVE_EDITOR->setObjectName(QString::fromUtf8("ID_FILE_LEAVE_EDITOR"));
        ID_FILE_LEVELPROPS = new QAction(IDD_FONT_DIALOG);
        ID_FILE_LEVELPROPS->setObjectName(QString::fromUtf8("ID_FILE_LEVELPROPS"));
        ID_FILE_LEVEL_INFO = new QAction(IDD_FONT_DIALOG);
        ID_FILE_LEVEL_INFO->setObjectName(QString::fromUtf8("ID_FILE_LEVEL_INFO"));
        ID_FILE_MRU_FILE1 = new QAction(IDD_FONT_DIALOG);
        ID_FILE_MRU_FILE1->setObjectName(QString::fromUtf8("ID_FILE_MRU_FILE1"));
        ID_FILE_MRU_FILE1->setEnabled(false);
        ID_FILE_NEW = new QAction(IDD_FONT_DIALOG);
        ID_FILE_NEW->setObjectName(QString::fromUtf8("ID_FILE_NEW"));
        ID_FILE_OPEN = new QAction(IDD_FONT_DIALOG);
        ID_FILE_OPEN->setObjectName(QString::fromUtf8("ID_FILE_OPEN"));
        ID_FILE_PLAY640X480 = new QAction(IDD_FONT_DIALOG);
        ID_FILE_PLAY640X480->setObjectName(QString::fromUtf8("ID_FILE_PLAY640X480"));
        ID_FILE_PREFERENCES = new QAction(IDD_FONT_DIALOG);
        ID_FILE_PREFERENCES->setObjectName(QString::fromUtf8("ID_FILE_PREFERENCES"));
        ID_FILE_PRINT = new QAction(IDD_FONT_DIALOG);
        ID_FILE_PRINT->setObjectName(QString::fromUtf8("ID_FILE_PRINT"));
        ID_FILE_PRINT_PREVIEW = new QAction(IDD_FONT_DIALOG);
        ID_FILE_PRINT_PREVIEW->setObjectName(QString::fromUtf8("ID_FILE_PRINT_PREVIEW"));
        ID_FILE_PRINT_SETUP = new QAction(IDD_FONT_DIALOG);
        ID_FILE_PRINT_SETUP->setObjectName(QString::fromUtf8("ID_FILE_PRINT_SETUP"));
        ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM = new QAction(IDD_FONT_DIALOG);
        ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM->setObjectName(QString::fromUtf8("ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM"));
        ID_FILE_REMOVEEXTRAPOINTS = new QAction(IDD_FONT_DIALOG);
        ID_FILE_REMOVEEXTRAPOINTS->setObjectName(QString::fromUtf8("ID_FILE_REMOVEEXTRAPOINTS"));
        ID_FILE_RESTOREGAMESTATE = new QAction(IDD_FONT_DIALOG);
        ID_FILE_RESTOREGAMESTATE->setObjectName(QString::fromUtf8("ID_FILE_RESTOREGAMESTATE"));
        ID_FILE_SAVE = new QAction(IDD_FONT_DIALOG);
        ID_FILE_SAVE->setObjectName(QString::fromUtf8("ID_FILE_SAVE"));
        ID_FILE_SAVEGOALTEXT = new QAction(IDD_FONT_DIALOG);
        ID_FILE_SAVEGOALTEXT->setObjectName(QString::fromUtf8("ID_FILE_SAVEGOALTEXT"));
        ID_FILE_SAVE_AS = new QAction(IDD_FONT_DIALOG);
        ID_FILE_SAVE_AS->setObjectName(QString::fromUtf8("ID_FILE_SAVE_AS"));
        ID_FILE_SAVE_COPY_AS = new QAction(IDD_FONT_DIALOG);
        ID_FILE_SAVE_COPY_AS->setObjectName(QString::fromUtf8("ID_FILE_SAVE_COPY_AS"));
        ID_FILE_STATS = new QAction(IDD_FONT_DIALOG);
        ID_FILE_STATS->setObjectName(QString::fromUtf8("ID_FILE_STATS"));
        ID_FILE_UPDATE = new QAction(IDD_FONT_DIALOG);
        ID_FILE_UPDATE->setObjectName(QString::fromUtf8("ID_FILE_UPDATE"));
        ID_FILE_VERIFY_LEVEL = new QAction(IDD_FONT_DIALOG);
        ID_FILE_VERIFY_LEVEL->setObjectName(QString::fromUtf8("ID_FILE_VERIFY_LEVEL"));
        ID_HOTSPOT_TGA = new QAction(IDD_FONT_DIALOG);
        ID_HOTSPOT_TGA->setObjectName(QString::fromUtf8("ID_HOTSPOT_TGA"));
        ID_MINE_VIEW = new QAction(IDD_FONT_DIALOG);
        ID_MINE_VIEW->setObjectName(QString::fromUtf8("ID_MINE_VIEW"));
        ID_OBJBUTTON = new QAction(IDD_FONT_DIALOG);
        ID_OBJBUTTON->setObjectName(QString::fromUtf8("ID_OBJBUTTON"));
        ID_OBJECT_DELETEOBJECT = new QAction(IDD_FONT_DIALOG);
        ID_OBJECT_DELETEOBJECT->setObjectName(QString::fromUtf8("ID_OBJECT_DELETEOBJECT"));
        ID_OBJECT_MOVEPLAYER = new QAction(IDD_FONT_DIALOG);
        ID_OBJECT_MOVEPLAYER->setObjectName(QString::fromUtf8("ID_OBJECT_MOVEPLAYER"));
        ID_OBJECT_PLACECAMERAATCURRENTFACE = new QAction(IDD_FONT_DIALOG);
        ID_OBJECT_PLACECAMERAATCURRENTFACE->setObjectName(QString::fromUtf8("ID_OBJECT_PLACECAMERAATCURRENTFACE"));
        ID_OBJECT_PLACECAMERAATVIEWER = new QAction(IDD_FONT_DIALOG);
        ID_OBJECT_PLACECAMERAATVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_PLACECAMERAATVIEWER"));
        ID_OBJECT_PLACEOBJECT = new QAction(IDD_FONT_DIALOG);
        ID_OBJECT_PLACEOBJECT->setObjectName(QString::fromUtf8("ID_OBJECT_PLACEOBJECT"));
        ID_OBJECT_PLACESOUNDSOURCEATVIEWER = new QAction(IDD_FONT_DIALOG);
        ID_OBJECT_PLACESOUNDSOURCEATVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_PLACESOUNDSOURCEATVIEWER"));
        ID_OBJECT_PLACEWAYPOINTATVIEWER = new QAction(IDD_FONT_DIALOG);
        ID_OBJECT_PLACEWAYPOINTATVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_PLACEWAYPOINTATVIEWER"));
        ID_OBJECT_SELECTBYNUMBER = new QAction(IDD_FONT_DIALOG);
        ID_OBJECT_SELECTBYNUMBER->setObjectName(QString::fromUtf8("ID_OBJECT_SELECTBYNUMBER"));
        ID_OBJECT_SETCAMERAFROMVIEWER = new QAction(IDD_FONT_DIALOG);
        ID_OBJECT_SETCAMERAFROMVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_SETCAMERAFROMVIEWER"));
        ID_OBJECT_SETVIEWERFROMCAMERA = new QAction(IDD_FONT_DIALOG);
        ID_OBJECT_SETVIEWERFROMCAMERA->setObjectName(QString::fromUtf8("ID_OBJECT_SETVIEWERFROMCAMERA"));
        ID_OSIRISCOMPILE = new QAction(IDD_FONT_DIALOG);
        ID_OSIRISCOMPILE->setObjectName(QString::fromUtf8("ID_OSIRISCOMPILE"));
        ID_REINIT_OPENGL = new QAction(IDD_FONT_DIALOG);
        ID_REINIT_OPENGL->setObjectName(QString::fromUtf8("ID_REINIT_OPENGL"));
        ID_ROOM_ADD = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_ADD->setObjectName(QString::fromUtf8("ID_ROOM_ADD"));
        ID_ROOM_ADDVERTTONEWFACE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_ADDVERTTONEWFACE->setObjectName(QString::fromUtf8("ID_ROOM_ADDVERTTONEWFACE"));
        ID_ROOM_ATTACHROOM = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_ATTACHROOM->setObjectName(QString::fromUtf8("ID_ROOM_ATTACHROOM"));
        ID_ROOM_BUILDBRIDGE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_BUILDBRIDGE->setObjectName(QString::fromUtf8("ID_ROOM_BUILDBRIDGE"));
        ID_ROOM_BUILDSMOOTHBRIDGE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_BUILDSMOOTHBRIDGE->setObjectName(QString::fromUtf8("ID_ROOM_BUILDSMOOTHBRIDGE"));
        ID_ROOM_CLEARCENTERPOINT = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_CLEARCENTERPOINT->setObjectName(QString::fromUtf8("ID_ROOM_CLEARCENTERPOINT"));
        ID_ROOM_COMBINE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_COMBINE->setObjectName(QString::fromUtf8("ID_ROOM_COMBINE"));
        ID_ROOM_DELETE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_DELETE->setObjectName(QString::fromUtf8("ID_ROOM_DELETE"));
        ID_ROOM_DELETECONNECTEDFACES = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_DELETECONNECTEDFACES->setObjectName(QString::fromUtf8("ID_ROOM_DELETECONNECTEDFACES"));
        ID_ROOM_DELETEFACE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_DELETEFACE->setObjectName(QString::fromUtf8("ID_ROOM_DELETEFACE"));
        ID_ROOM_DELETEPORTAL = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_DELETEPORTAL->setObjectName(QString::fromUtf8("ID_ROOM_DELETEPORTAL"));
        ID_ROOM_DELETEVERT = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_DELETEVERT->setObjectName(QString::fromUtf8("ID_ROOM_DELETEVERT"));
        ID_ROOM_DROPROOM = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_DROPROOM->setObjectName(QString::fromUtf8("ID_ROOM_DROPROOM"));
        ID_ROOM_FACE_ADDVERTTOEDGE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_FACE_ADDVERTTOEDGE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_ADDVERTTOEDGE"));
        ID_ROOM_FACE_DELETEVERTONEDGE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_FACE_DELETEVERTONEDGE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_DELETEVERTONEDGE"));
        ID_ROOM_FACE_MOVEVERTONEDGE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_FACE_MOVEVERTONEDGE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_MOVEVERTONEDGE"));
        ID_ROOM_FACE_SPLITFACE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_FACE_SPLITFACE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_SPLITFACE"));
        ID_ROOM_FINISHNEWFACE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_FINISHNEWFACE->setObjectName(QString::fromUtf8("ID_ROOM_FINISHNEWFACE"));
        ID_ROOM_GRABTEXTURE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_GRABTEXTURE->setObjectName(QString::fromUtf8("ID_ROOM_GRABTEXTURE"));
        ID_ROOM_JOINROOMS = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_JOINROOMS->setObjectName(QString::fromUtf8("ID_ROOM_JOINROOMS"));
        ID_ROOM_JOINROOMSEXACT = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_JOINROOMSEXACT->setObjectName(QString::fromUtf8("ID_ROOM_JOINROOMSEXACT"));
        ID_ROOM_JOIN_ADJACENT_FACES = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_JOIN_ADJACENT_FACES->setObjectName(QString::fromUtf8("ID_ROOM_JOIN_ADJACENT_FACES"));
        ID_ROOM_LINKTONEWEXTERNAL = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_LINKTONEWEXTERNAL->setObjectName(QString::fromUtf8("ID_ROOM_LINKTONEWEXTERNAL"));
        ID_ROOM_MARK = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_MARK->setObjectName(QString::fromUtf8("ID_ROOM_MARK"));
        ID_ROOM_MERGEOBJECTINTOROOM = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_MERGEOBJECTINTOROOM->setObjectName(QString::fromUtf8("ID_ROOM_MERGEOBJECTINTOROOM"));
        ID_ROOM_NEXTFACE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_NEXTFACE->setObjectName(QString::fromUtf8("ID_ROOM_NEXTFACE"));
        ID_ROOM_PLACEROOM = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_PLACEROOM->setObjectName(QString::fromUtf8("ID_ROOM_PLACEROOM"));
        ID_ROOM_PLACETERRAINROOM = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_PLACETERRAINROOM->setObjectName(QString::fromUtf8("ID_ROOM_PLACETERRAINROOM"));
        ID_ROOM_PREVIOUSFACE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_PREVIOUSFACE->setObjectName(QString::fromUtf8("ID_ROOM_PREVIOUSFACE"));
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES->setObjectName(QString::fromUtf8("ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES"));
        ID_ROOM_PROPAGATETOALL = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_PROPAGATETOALL->setObjectName(QString::fromUtf8("ID_ROOM_PROPAGATETOALL"));
        ID_ROOM_RENAMEROOM = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_RENAMEROOM->setObjectName(QString::fromUtf8("ID_ROOM_RENAMEROOM"));
        ID_ROOM_ROTATEPLACEDROOM45DEGREES = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_ROTATEPLACEDROOM45DEGREES->setObjectName(QString::fromUtf8("ID_ROOM_ROTATEPLACEDROOM45DEGREES"));
        ID_ROOM_SAVECURRENTROOM = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_SAVECURRENTROOM->setObjectName(QString::fromUtf8("ID_ROOM_SAVECURRENTROOM"));
        ID_ROOM_SELECTBYNUMBER = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_SELECTBYNUMBER->setObjectName(QString::fromUtf8("ID_ROOM_SELECTBYNUMBER"));
        ID_ROOM_SELECTFACEBYNUMBER = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_SELECTFACEBYNUMBER->setObjectName(QString::fromUtf8("ID_ROOM_SELECTFACEBYNUMBER"));
        ID_ROOM_SETCENTERFROMVIEWER = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_SETCENTERFROMVIEWER->setObjectName(QString::fromUtf8("ID_ROOM_SETCENTERFROMVIEWER"));
        ID_ROOM_SNAPPLACEDROOM = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_SNAPPLACEDROOM->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPLACEDROOM"));
        ID_ROOM_SNAPPOINTTOEDGE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_SNAPPOINTTOEDGE->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPOINTTOEDGE"));
        ID_ROOM_SNAPPOINTTOFACE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_SNAPPOINTTOFACE->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPOINTTOFACE"));
        ID_ROOM_SNAPPOINTTOPOINT = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_SNAPPOINTTOPOINT->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPOINTTOPOINT"));
        ID_ROOM_SPLITFACE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_SPLITFACE->setObjectName(QString::fromUtf8("ID_ROOM_SPLITFACE"));
        ID_ROOM_STARTNEWFACE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_STARTNEWFACE->setObjectName(QString::fromUtf8("ID_ROOM_STARTNEWFACE"));
        ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE->setObjectName(QString::fromUtf8("ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE"));
        ID_ROOM_UNDOSNAP = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_UNDOSNAP->setObjectName(QString::fromUtf8("ID_ROOM_UNDOSNAP"));
        ID_ROOM_UNPLACEROOM = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_UNPLACEROOM->setObjectName(QString::fromUtf8("ID_ROOM_UNPLACEROOM"));
        ID_ROOM_VIEW = new QAction(IDD_FONT_DIALOG);
        ID_ROOM_VIEW->setObjectName(QString::fromUtf8("ID_ROOM_VIEW"));
        ID_SCRIPT_LEVEL_INTERFACE = new QAction(IDD_FONT_DIALOG);
        ID_SCRIPT_LEVEL_INTERFACE->setObjectName(QString::fromUtf8("ID_SCRIPT_LEVEL_INTERFACE"));
        ID_SUBEDITORS_FONT = new QAction(IDD_FONT_DIALOG);
        ID_SUBEDITORS_FONT->setObjectName(QString::fromUtf8("ID_SUBEDITORS_FONT"));
        ID_SUBEDITORS_HOGMAKER = new QAction(IDD_FONT_DIALOG);
        ID_SUBEDITORS_HOGMAKER->setObjectName(QString::fromUtf8("ID_SUBEDITORS_HOGMAKER"));
        ID_SUBEDITORS_TABLEFILEEDIT = new QAction(IDD_FONT_DIALOG);
        ID_SUBEDITORS_TABLEFILEEDIT->setObjectName(QString::fromUtf8("ID_SUBEDITORS_TABLEFILEEDIT"));
        ID_SUBEDITORS_TABLEFILEFILTER = new QAction(IDD_FONT_DIALOG);
        ID_SUBEDITORS_TABLEFILEFILTER->setObjectName(QString::fromUtf8("ID_SUBEDITORS_TABLEFILEFILTER"));
        ID_TABLEEDIT_FILE_EXIT = new QAction(IDD_FONT_DIALOG);
        ID_TABLEEDIT_FILE_EXIT->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_EXIT"));
        ID_TABLEEDIT_FILE_LOAD = new QAction(IDD_FONT_DIALOG);
        ID_TABLEEDIT_FILE_LOAD->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_LOAD"));
        ID_TABLEEDIT_FILE_SAVE = new QAction(IDD_FONT_DIALOG);
        ID_TABLEEDIT_FILE_SAVE->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_SAVE"));
        ID_TABLEEDIT_FILE_SAVE_AS = new QAction(IDD_FONT_DIALOG);
        ID_TABLEEDIT_FILE_SAVE_AS->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_SAVE_AS"));
        ID_TABLEFILTER_FILE_EXIT = new QAction(IDD_FONT_DIALOG);
        ID_TABLEFILTER_FILE_EXIT->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_EXIT"));
        ID_TABLEFILTER_FILE_LOAD = new QAction(IDD_FONT_DIALOG);
        ID_TABLEFILTER_FILE_LOAD->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_LOAD"));
        ID_TABLEFILTER_FILE_NEW = new QAction(IDD_FONT_DIALOG);
        ID_TABLEFILTER_FILE_NEW->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_NEW"));
        ID_TABLEFILTER_FILE_SAVE = new QAction(IDD_FONT_DIALOG);
        ID_TABLEFILTER_FILE_SAVE->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_SAVE"));
        ID_TABLEFILTER_FILE_SAVEAS = new QAction(IDD_FONT_DIALOG);
        ID_TABLEFILTER_FILE_SAVEAS->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_SAVEAS"));
        ID_TERRAIN_VIEW = new QAction(IDD_FONT_DIALOG);
        ID_TERRAIN_VIEW->setObjectName(QString::fromUtf8("ID_TERRAIN_VIEW"));
        ID_TEST_TEST1 = new QAction(IDD_FONT_DIALOG);
        ID_TEST_TEST1->setObjectName(QString::fromUtf8("ID_TEST_TEST1"));
        ID_TEST_TEST2 = new QAction(IDD_FONT_DIALOG);
        ID_TEST_TEST2->setObjectName(QString::fromUtf8("ID_TEST_TEST2"));
        ID_TEST_TEST3 = new QAction(IDD_FONT_DIALOG);
        ID_TEST_TEST3->setObjectName(QString::fromUtf8("ID_TEST_TEST3"));
        ID_TOOLS_WORLD_OBJECTS_BUILDINGS = new QAction(IDD_FONT_DIALOG);
        ID_TOOLS_WORLD_OBJECTS_BUILDINGS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_BUILDINGS"));
        ID_TOOLS_WORLD_OBJECTS_CLUTTER = new QAction(IDD_FONT_DIALOG);
        ID_TOOLS_WORLD_OBJECTS_CLUTTER->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_CLUTTER"));
        ID_TOOLS_WORLD_OBJECTS_DOOR = new QAction(IDD_FONT_DIALOG);
        ID_TOOLS_WORLD_OBJECTS_DOOR->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_DOOR"));
        ID_TOOLS_WORLD_OBJECTS_LIGHTS = new QAction(IDD_FONT_DIALOG);
        ID_TOOLS_WORLD_OBJECTS_LIGHTS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_LIGHTS"));
        ID_TOOLS_WORLD_OBJECTS_PLAYER = new QAction(IDD_FONT_DIALOG);
        ID_TOOLS_WORLD_OBJECTS_PLAYER->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_PLAYER"));
        ID_TOOLS_WORLD_OBJECTS_POWERUPS = new QAction(IDD_FONT_DIALOG);
        ID_TOOLS_WORLD_OBJECTS_POWERUPS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_POWERUPS"));
        ID_TOOLS_WORLD_OBJECTS_ROBOTS = new QAction(IDD_FONT_DIALOG);
        ID_TOOLS_WORLD_OBJECTS_ROBOTS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_ROBOTS"));
        ID_TOOLS_WORLD_OBJECTS_SOUND = new QAction(IDD_FONT_DIALOG);
        ID_TOOLS_WORLD_OBJECTS_SOUND->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_SOUND"));
        ID_TOOLS_WORLD_TEXTURES = new QAction(IDD_FONT_DIALOG);
        ID_TOOLS_WORLD_TEXTURES->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_TEXTURES"));
        ID_TOOLS_WORLD_WEAPONS = new QAction(IDD_FONT_DIALOG);
        ID_TOOLS_WORLD_WEAPONS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_WEAPONS"));
        ID_VIEW_CENTERONCUBE = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_CENTERONCUBE->setObjectName(QString::fromUtf8("ID_VIEW_CENTERONCUBE"));
        ID_VIEW_CENTERONMINE = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_CENTERONMINE->setObjectName(QString::fromUtf8("ID_VIEW_CENTERONMINE"));
        ID_VIEW_CENTERONOBJECT = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_CENTERONOBJECT->setObjectName(QString::fromUtf8("ID_VIEW_CENTERONOBJECT"));
        ID_VIEW_DELETEVIEWER = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_DELETEVIEWER->setObjectName(QString::fromUtf8("ID_VIEW_DELETEVIEWER"));
        ID_VIEW_FLIP = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_FLIP->setObjectName(QString::fromUtf8("ID_VIEW_FLIP"));
        ID_VIEW_KEYPAD_TOGGLE = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_KEYPAD_TOGGLE->setObjectName(QString::fromUtf8("ID_VIEW_KEYPAD_TOGGLE"));
        ID_VIEW_MOVECAMERATOCURRENTOBJECT = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_MOVECAMERATOCURRENTOBJECT->setObjectName(QString::fromUtf8("ID_VIEW_MOVECAMERATOCURRENTOBJECT"));
        ID_VIEW_MOVECAMERATOSELECTEDFACE = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_MOVECAMERATOSELECTEDFACE->setObjectName(QString::fromUtf8("ID_VIEW_MOVECAMERATOSELECTEDFACE"));
        ID_VIEW_MOVECAMERATOSELECTEDROOM = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_MOVECAMERATOSELECTEDROOM->setObjectName(QString::fromUtf8("ID_VIEW_MOVECAMERATOSELECTEDROOM"));
        ID_VIEW_NEWVIEWER = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_NEWVIEWER->setObjectName(QString::fromUtf8("ID_VIEW_NEWVIEWER"));
        ID_VIEW_NEXTVIEWER = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_NEXTVIEWER->setObjectName(QString::fromUtf8("ID_VIEW_NEXTVIEWER"));
        ID_VIEW_RESETVIEWRADIUS = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_RESETVIEWRADIUS->setObjectName(QString::fromUtf8("ID_VIEW_RESETVIEWRADIUS"));
        ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW->setObjectName(QString::fromUtf8("ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW"));
        ID_VIEW_SHOWVIEWERFORWARDVECTOR = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_SHOWVIEWERFORWARDVECTOR->setObjectName(QString::fromUtf8("ID_VIEW_SHOWVIEWERFORWARDVECTOR"));
        ID_VIEW_STATUS_BAR = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_STATUS_BAR->setObjectName(QString::fromUtf8("ID_VIEW_STATUS_BAR"));
        ID_VIEW_TEXTUREMINE = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_TEXTUREMINE->setObjectName(QString::fromUtf8("ID_VIEW_TEXTUREMINE"));
        ID_VIEW_TOOLBAR = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_TOOLBAR->setObjectName(QString::fromUtf8("ID_VIEW_TOOLBAR"));
        ID_VIEW_VIEWPROP = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_VIEWPROP->setObjectName(QString::fromUtf8("ID_VIEW_VIEWPROP"));
        ID_VIEW_WIREFRAMEMINE = new QAction(IDD_FONT_DIALOG);
        ID_VIEW_WIREFRAMEMINE->setObjectName(QString::fromUtf8("ID_VIEW_WIREFRAMEMINE"));
        ID_WINDOW_CASCADE = new QAction(IDD_FONT_DIALOG);
        ID_WINDOW_CASCADE->setObjectName(QString::fromUtf8("ID_WINDOW_CASCADE"));
        ID_WINDOW_TILE = new QAction(IDD_FONT_DIALOG);
        ID_WINDOW_TILE->setObjectName(QString::fromUtf8("ID_WINDOW_TILE"));
        ID_ZBUTTON = new QAction(IDD_FONT_DIALOG);
        ID_ZBUTTON->setObjectName(QString::fromUtf8("ID_ZBUTTON"));
        IDC_STATIC = new QGroupBox(IDD_FONT_DIALOG);
        IDC_STATIC->setObjectName(QString::fromUtf8("IDC_STATIC"));
        IDC_STATIC->setGeometry(QRect(16, 398, 313, 119));
        IDC_STATIC->setVisible(true);
        IDC_STATIC->setEnabled(true);
        IDC_STATIC->setFlat(false);
        IDC_STATIC_2 = new QLabel(IDC_STATIC);
        IDC_STATIC_2->setObjectName(QString::fromUtf8("IDC_STATIC_2"));
        IDC_STATIC_2->setGeometry(QRect(8, 30, 51, 16));
        IDC_STATIC_2->setVisible(true);
        IDC_STATIC_2->setEnabled(true);
        IDC_STATIC_2->setAlignment(Qt::AlignLeft);
        IDC_EDIT_MINASCII = new QLineEdit(IDC_STATIC);
        IDC_EDIT_MINASCII->setObjectName(QString::fromUtf8("IDC_EDIT_MINASCII"));
        IDC_EDIT_MINASCII->setGeometry(QRect(40, 30, 32, 24));
        IDC_EDIT_MINASCII->setVisible(true);
        IDC_EDIT_MINASCII->setEnabled(true);
        IDC_PROP_MONO = new QRadioButton(IDC_STATIC);
        IDC_PROP_MONO->setObjectName(QString::fromUtf8("IDC_PROP_MONO"));
        IDC_PROP_MONO->setGeometry(QRect(88, 30, 104, 19));
        IDC_PROP_MONO->setVisible(true);
        IDC_PROP_MONO->setEnabled(true);
        IDC_STATIC_PROP = new QLabel(IDC_STATIC);
        IDC_STATIC_PROP->setObjectName(QString::fromUtf8("IDC_STATIC_PROP"));
        IDC_STATIC_PROP->setGeometry(QRect(200, 30, 104, 16));
        IDC_STATIC_PROP->setVisible(true);
        IDC_STATIC_PROP->setEnabled(true);
        IDC_STATIC_PROP->setAlignment(Qt::AlignLeft);
        IDC_PROP_BITMAPPED = new QRadioButton(IDC_STATIC);
        IDC_PROP_BITMAPPED->setObjectName(QString::fromUtf8("IDC_PROP_BITMAPPED"));
        IDC_PROP_BITMAPPED->setGeometry(QRect(88, 61, 112, 19));
        IDC_PROP_BITMAPPED->setVisible(true);
        IDC_PROP_BITMAPPED->setEnabled(true);
        IDC_STATIC_3 = new QLabel(IDC_STATIC);
        IDC_STATIC_3->setObjectName(QString::fromUtf8("IDC_STATIC_3"));
        IDC_STATIC_3->setGeometry(QRect(8, 78, 51, 16));
        IDC_STATIC_3->setVisible(true);
        IDC_STATIC_3->setEnabled(true);
        IDC_STATIC_3->setAlignment(Qt::AlignLeft);
        IDC_STATIC_MAXASCII = new QLabel(IDC_STATIC);
        IDC_STATIC_MAXASCII->setObjectName(QString::fromUtf8("IDC_STATIC_MAXASCII"));
        IDC_STATIC_MAXASCII->setGeometry(QRect(40, 78, 51, 13));
        IDC_STATIC_MAXASCII->setVisible(true);
        IDC_STATIC_MAXASCII->setEnabled(true);
        IDC_STATIC_MAXASCII->setAlignment(Qt::AlignLeft);
        IDC_BRIGHTNESS = new QLineEdit(IDC_STATIC);
        IDC_BRIGHTNESS->setObjectName(QString::fromUtf8("IDC_BRIGHTNESS"));
        IDC_BRIGHTNESS->setGeometry(QRect(272, 92, 32, 24));
        IDC_BRIGHTNESS->setVisible(true);
        IDC_BRIGHTNESS->setEnabled(true);
        IDC_PROP_GRADIANT = new QCheckBox(IDC_STATIC);
        IDC_PROP_GRADIANT->setObjectName(QString::fromUtf8("IDC_PROP_GRADIANT"));
        IDC_PROP_GRADIANT->setGeometry(QRect(88, 100, 80, 19));
        IDC_PROP_GRADIANT->setVisible(true);
        IDC_PROP_GRADIANT->setEnabled(true);
        IDC_STATIC_4 = new QLabel(IDC_STATIC);
        IDC_STATIC_4->setObjectName(QString::fromUtf8("IDC_STATIC_4"));
        IDC_STATIC_4->setGeometry(QRect(200, 100, 64, 16));
        IDC_STATIC_4->setVisible(true);
        IDC_STATIC_4->setEnabled(true);
        IDC_STATIC_4->setAlignment(Qt::AlignLeft);
        IDC_STATIC_FONTNAME = new QGroupBox(IDD_FONT_DIALOG);
        IDC_STATIC_FONTNAME->setObjectName(QString::fromUtf8("IDC_STATIC_FONTNAME"));
        IDC_STATIC_FONTNAME->setGeometry(QRect(337, 398, 264, 97));
        IDC_STATIC_FONTNAME->setVisible(true);
        IDC_STATIC_FONTNAME->setEnabled(true);
        IDC_STATIC_FONTNAME->setFlat(false);
        IDC_STATIC_5 = new QLabel(IDC_STATIC_FONTNAME);
        IDC_STATIC_5->setObjectName(QString::fromUtf8("IDC_STATIC_5"));
        IDC_STATIC_5->setGeometry(QRect(8, 30, 51, 16));
        IDC_STATIC_5->setVisible(true);
        IDC_STATIC_5->setEnabled(true);
        IDC_STATIC_5->setAlignment(Qt::AlignLeft);
        IDC_EDIT_FONTNAME = new QLineEdit(IDC_STATIC_FONTNAME);
        IDC_EDIT_FONTNAME->setObjectName(QString::fromUtf8("IDC_EDIT_FONTNAME"));
        IDC_EDIT_FONTNAME->setGeometry(QRect(48, 30, 200, 21));
        IDC_EDIT_FONTNAME->setVisible(true);
        IDC_EDIT_FONTNAME->setEnabled(true);
        IDC_OPEN = new QPushButton(IDC_STATIC_FONTNAME);
        IDC_OPEN->setObjectName(QString::fromUtf8("IDC_OPEN"));
        IDC_OPEN->setGeometry(QRect(72, 73, 80, 24));
        IDC_OPEN->setVisible(true);
        IDC_OPEN->setEnabled(true);
        IDC_OPEN->setAutoDefault(false);
        IDC_SAVE = new QPushButton(IDC_STATIC_FONTNAME);
        IDC_SAVE->setObjectName(QString::fromUtf8("IDC_SAVE"));
        IDC_SAVE->setGeometry(QRect(136, 73, 80, 24));
        IDC_SAVE->setVisible(true);
        IDC_SAVE->setEnabled(true);
        IDC_SAVE->setAutoDefault(false);
        IDC_SAVEAS = new QPushButton(IDC_STATIC_FONTNAME);
        IDC_SAVEAS->setObjectName(QString::fromUtf8("IDC_SAVEAS"));
        IDC_SAVEAS->setGeometry(QRect(200, 73, 80, 24));
        IDC_SAVEAS->setVisible(true);
        IDC_SAVEAS->setEnabled(true);
        IDC_SAVEAS->setAutoDefault(false);
        IDC_STATIC_FONTVIEW = new QLabel(IDD_FONT_DIALOG);
        IDC_STATIC_FONTVIEW->setObjectName(QString::fromUtf8("IDC_STATIC_FONTVIEW"));
        IDC_STATIC_FONTVIEW->setGeometry(QRect(16, 8, 586, 373));
        IDC_STATIC_FONTVIEW->setVisible(true);
        IDC_STATIC_FONTVIEW->setEnabled(true);
        IDC_STATIC_FONTVIEW->setAlignment(Qt::AlignHCenter);
        IDOK = new QPushButton(IDD_FONT_DIALOG);
        IDOK->setObjectName(QString::fromUtf8("IDOK"));
        IDOK->setGeometry(QRect(634, 8, 80, 24));
        IDOK->setVisible(true);
        IDOK->setEnabled(true);
        IDOK->setAutoDefault(true);
        IDCANCEL = new QPushButton(IDD_FONT_DIALOG);
        IDCANCEL->setObjectName(QString::fromUtf8("IDCANCEL"));
        IDCANCEL->setGeometry(QRect(634, 40, 80, 24));
        IDCANCEL->setVisible(true);
        IDCANCEL->setEnabled(true);
        IDCANCEL->setAutoDefault(false);
        IDC_NEW = new QPushButton(IDD_FONT_DIALOG);
        IDC_NEW->setObjectName(QString::fromUtf8("IDC_NEW"));
        IDC_NEW->setGeometry(QRect(634, 113, 80, 24));
        IDC_NEW->setVisible(true);
        IDC_NEW->setEnabled(true);
        IDC_NEW->setAutoDefault(false);
        IDC_LOCK = new QPushButton(IDD_FONT_DIALOG);
        IDC_LOCK->setObjectName(QString::fromUtf8("IDC_LOCK"));
        IDC_LOCK->setGeometry(QRect(634, 146, 80, 24));
        IDC_LOCK->setVisible(true);
        IDC_LOCK->setEnabled(true);
        IDC_LOCK->setAutoDefault(false);
        IDC_CHECKIN = new QPushButton(IDD_FONT_DIALOG);
        IDC_CHECKIN->setObjectName(QString::fromUtf8("IDC_CHECKIN"));
        IDC_CHECKIN->setGeometry(QRect(634, 178, 80, 24));
        IDC_CHECKIN->setVisible(true);
        IDC_CHECKIN->setEnabled(true);
        IDC_CHECKIN->setAutoDefault(false);
        IDC_DELETE = new QPushButton(IDD_FONT_DIALOG);
        IDC_DELETE->setObjectName(QString::fromUtf8("IDC_DELETE"));
        IDC_DELETE->setGeometry(QRect(634, 211, 80, 24));
        IDC_DELETE->setVisible(true);
        IDC_DELETE->setEnabled(true);
        IDC_DELETE->setAutoDefault(false);

        retranslateUi(IDD_FONT_DIALOG);

        IDC_OPEN->setDefault(false);
        IDC_SAVE->setDefault(false);
        IDC_SAVEAS->setDefault(false);
        IDOK->setDefault(true);
        IDCANCEL->setDefault(false);
        IDC_NEW->setDefault(false);
        IDC_LOCK->setDefault(false);
        IDC_CHECKIN->setDefault(false);
        IDC_DELETE->setDefault(false);


        QMetaObject::connectSlotsByName(IDD_FONT_DIALOG);
    } // setupUi

    void retranslateUi(QDialog *IDD_FONT_DIALOG)
    {
        IDD_FONT_DIALOG->setWindowTitle(QCoreApplication::translate("FontDialog", "Subeditors >> Font", nullptr));
        IDD_ORPHANHUNTER->setText(QCoreApplication::translate("FontDialog", "Orphan Hunter", nullptr));
        IDD_REORDER_PAGES->setText(QCoreApplication::translate("FontDialog", "Reorder Net Pages", nullptr));
        IDD_SHOW_ALL_CHECKED_OUT->setText(QCoreApplication::translate("FontDialog", "Show All Pages Checked Out", nullptr));
        IDM_IMPORT_BITMAP->setText(QCoreApplication::translate("FontDialog", "Bitmap/Animation Converter...", nullptr));
        ID_APP_ABOUT->setText(QCoreApplication::translate("FontDialog", "&About editor...", nullptr));
        ID_APP_EXIT->setText(QCoreApplication::translate("FontDialog", "E&xit", nullptr));
#if QT_CONFIG(shortcut)
        ID_APP_EXIT->setShortcut(QCoreApplication::translate("FontDialog", "Alt+X", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_BRIEFING_EDITOR->setText(QCoreApplication::translate("FontDialog", "Briefing Editor...", nullptr));
        ID_BUTTON_LIGHTING->setText(QCoreApplication::translate("FontDialog", "ID_BUTTON_LIGHTING", nullptr));
        ID_BUTTON_OUTLINE->setText(QCoreApplication::translate("FontDialog", "ID_BUTTON_OUTLINE", nullptr));
        ID_BUTTON_WINDOWSELECTION->setText(QCoreApplication::translate("FontDialog", "ID_BUTTON_WINDOWSELECTION", nullptr));
        ID_D3HELP->setText(QCoreApplication::translate("FontDialog", "Help on D3 Editor", nullptr));
        ID_EDITORS_AIPROPERTIES->setText(QCoreApplication::translate("FontDialog", "AI Properties...", nullptr));
        ID_EDITORS_AMBIENTSOUNDS->setText(QCoreApplication::translate("FontDialog", "Ambient Sounds...", nullptr));
        ID_EDITORS_DALLAS->setText(QCoreApplication::translate("FontDialog", "Dallas Graphical Script Editor...", nullptr));
        ID_EDITORS_FILES->setText(QCoreApplication::translate("FontDialog", "Files...", nullptr));
        ID_EDITORS_MEGACELLS->setText(QCoreApplication::translate("FontDialog", "Megacells...", nullptr));
        ID_EDIT_ADDSELECT->setText(QCoreApplication::translate("FontDialog", "Add Current Room to Selected List", nullptr));
        ID_EDIT_ATTACH->setText(QCoreApplication::translate("FontDialog", "&Attach", nullptr));
        ID_EDIT_CLEARSELECTED->setText(QCoreApplication::translate("FontDialog", "Clear Selected List", nullptr));
        ID_EDIT_COPY->setText(QCoreApplication::translate("FontDialog", "&Copy", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_COPY->setShortcut(QCoreApplication::translate("FontDialog", "Ctrl+Ins", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_EDIT_CUT->setText(QCoreApplication::translate("FontDialog", "Cu&t", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_CUT->setShortcut(QCoreApplication::translate("FontDialog", "Shift+Del", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_EDIT_DELETE->setText(QCoreApplication::translate("FontDialog", "&Delete", nullptr));
        ID_EDIT_LOADSCRAP->setText(QCoreApplication::translate("FontDialog", "Load Scrap from Disk", nullptr));
        ID_EDIT_PASTE->setText(QCoreApplication::translate("FontDialog", "&Paste", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_PASTE->setShortcut(QCoreApplication::translate("FontDialog", "Shift+Ins", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_EDIT_PLACE->setText(QCoreApplication::translate("FontDialog", "&Place in Mine", nullptr));
        ID_EDIT_PLACE_TERRAIN->setText(QCoreApplication::translate("FontDialog", "&Place on Terrain", nullptr));
        ID_EDIT_REMOVESELECT->setText(QCoreApplication::translate("FontDialog", "Remove Current Room from Selected List", nullptr));
        ID_EDIT_SAVESCRAP->setText(QCoreApplication::translate("FontDialog", "Save Scrap to Disk", nullptr));
        ID_EDIT_SELECTATTACHED->setText(QCoreApplication::translate("FontDialog", "Select All Rooms Attached to Current Room", nullptr));
        ID_EDIT_UNDO->setText(QCoreApplication::translate("FontDialog", "&Undo", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_UNDO->setShortcut(QCoreApplication::translate("FontDialog", "Alt+Backspace", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_AUTOSAVE->setText(QCoreApplication::translate("FontDialog", "Autosave", nullptr));
        ID_FILE_CHECKMINE->setText(QCoreApplication::translate("FontDialog", "&Check Mine", nullptr));
        ID_FILE_FIXCRACKS->setText(QCoreApplication::translate("FontDialog", "Fix Level Cracks", nullptr));
        ID_FILE_FIXDEGENERATEFACES->setText(QCoreApplication::translate("FontDialog", "Fix/Remove Degenerate Faces", nullptr));
        ID_FILE_IMPORT_ROOM->setText(QCoreApplication::translate("FontDialog", "Import 3DSMax Room", nullptr));
        ID_FILE_INFORMATION->setText(QCoreApplication::translate("FontDialog", "&Information", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_INFORMATION->setShortcut(QCoreApplication::translate("FontDialog", "Alt+I", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_LEAVE_EDITOR->setText(QCoreApplication::translate("FontDialog", "Go to main menu...", nullptr));
        ID_FILE_LEVELPROPS->setText(QCoreApplication::translate("FontDialog", "Set Selected Room(s) Properties", nullptr));
        ID_FILE_LEVEL_INFO->setText(QCoreApplication::translate("FontDialog", "Edit Level Info...", nullptr));
        ID_FILE_MRU_FILE1->setText(QCoreApplication::translate("FontDialog", "Recent File", nullptr));
        ID_FILE_NEW->setText(QCoreApplication::translate("FontDialog", "&New", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_NEW->setShortcut(QCoreApplication::translate("FontDialog", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_OPEN->setText(QCoreApplication::translate("FontDialog", "&Open...", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_OPEN->setShortcut(QCoreApplication::translate("FontDialog", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_PLAY640X480->setText(QCoreApplication::translate("FontDialog", "&Play in 640 x 480", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_PLAY640X480->setShortcut(QCoreApplication::translate("FontDialog", "Alt+P", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_PREFERENCES->setText(QCoreApplication::translate("FontDialog", "Preferences", nullptr));
        ID_FILE_PRINT->setText(QCoreApplication::translate("FontDialog", "&Print...", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_PRINT->setShortcut(QCoreApplication::translate("FontDialog", "Ctrl+P", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_PRINT_PREVIEW->setText(QCoreApplication::translate("FontDialog", "Print Pre&view", nullptr));
        ID_FILE_PRINT_SETUP->setText(QCoreApplication::translate("FontDialog", "P&rint Setup...", nullptr));
        ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM->setText(QCoreApplication::translate("FontDialog", "Remove Duplicate Faces from Current Room", nullptr));
        ID_FILE_REMOVEEXTRAPOINTS->setText(QCoreApplication::translate("FontDialog", "Remove Extra Points", nullptr));
        ID_FILE_RESTOREGAMESTATE->setText(QCoreApplication::translate("FontDialog", "Restore Gamestate", nullptr));
        ID_FILE_SAVE->setText(QCoreApplication::translate("FontDialog", "&Save", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_SAVE->setShortcut(QCoreApplication::translate("FontDialog", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_SAVEGOALTEXT->setText(QCoreApplication::translate("FontDialog", "Save Goal Text to File", nullptr));
        ID_FILE_SAVE_AS->setText(QCoreApplication::translate("FontDialog", "Save &As...", nullptr));
        ID_FILE_SAVE_COPY_AS->setText(QCoreApplication::translate("FontDialog", "Save Copy &As...", nullptr));
        ID_FILE_STATS->setText(QCoreApplication::translate("FontDialog", "Show Level Stats", nullptr));
        ID_FILE_UPDATE->setText(QCoreApplication::translate("FontDialog", "&Update", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_UPDATE->setShortcut(QCoreApplication::translate("FontDialog", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_VERIFY_LEVEL->setText(QCoreApplication::translate("FontDialog", "Verify Level", nullptr));
        ID_HOTSPOT_TGA->setText(QCoreApplication::translate("FontDialog", "HotSpot TGA Extractor...", nullptr));
        ID_MINE_VIEW->setText(QCoreApplication::translate("FontDialog", "Mine View", nullptr));
#if QT_CONFIG(shortcut)
        ID_MINE_VIEW->setShortcut(QCoreApplication::translate("FontDialog", "F2", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_OBJBUTTON->setText(QCoreApplication::translate("FontDialog", "ID_OBJBUTTON", nullptr));
        ID_OBJECT_DELETEOBJECT->setText(QCoreApplication::translate("FontDialog", "Delete Object", nullptr));
#if QT_CONFIG(shortcut)
        ID_OBJECT_DELETEOBJECT->setShortcut(QCoreApplication::translate("FontDialog", "Del", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_OBJECT_MOVEPLAYER->setText(QCoreApplication::translate("FontDialog", "Move Player 0 to Viewer", nullptr));
        ID_OBJECT_PLACECAMERAATCURRENTFACE->setText(QCoreApplication::translate("FontDialog", "Create New Camera at Current Face", nullptr));
        ID_OBJECT_PLACECAMERAATVIEWER->setText(QCoreApplication::translate("FontDialog", "Create New Camera at Viewer", nullptr));
        ID_OBJECT_PLACEOBJECT->setText(QCoreApplication::translate("FontDialog", "Place Object", nullptr));
#if QT_CONFIG(shortcut)
        ID_OBJECT_PLACEOBJECT->setShortcut(QCoreApplication::translate("FontDialog", "Ins", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_OBJECT_PLACESOUNDSOURCEATVIEWER->setText(QCoreApplication::translate("FontDialog", "Create SoundSource at Viewer", nullptr));
        ID_OBJECT_PLACEWAYPOINTATVIEWER->setText(QCoreApplication::translate("FontDialog", "Place Waypoint at Viewer", nullptr));
        ID_OBJECT_SELECTBYNUMBER->setText(QCoreApplication::translate("FontDialog", "Select Object By Number", nullptr));
        ID_OBJECT_SETCAMERAFROMVIEWER->setText(QCoreApplication::translate("FontDialog", "Move Camera to Viewer", nullptr));
        ID_OBJECT_SETVIEWERFROMCAMERA->setText(QCoreApplication::translate("FontDialog", "Move Viewer to Camera", nullptr));
        ID_OSIRISCOMPILE->setText(QCoreApplication::translate("FontDialog", "ID_OSIRISCOMPILE", nullptr));
        ID_REINIT_OPENGL->setText(QCoreApplication::translate("FontDialog", "ID_REINIT_OPENGL", nullptr));
        ID_ROOM_ADD->setText(QCoreApplication::translate("FontDialog", "&Add Room at Current Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_ADD->setShortcut(QCoreApplication::translate("FontDialog", "Shift+A", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_ADDVERTTONEWFACE->setText(QCoreApplication::translate("FontDialog", "Add Vert to New Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_ADDVERTTONEWFACE->setShortcut(QCoreApplication::translate("FontDialog", "Shift+V", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_ATTACHROOM->setText(QCoreApplication::translate("FontDialog", "Attach Room", nullptr));
        ID_ROOM_BUILDBRIDGE->setText(QCoreApplication::translate("FontDialog", "&Build Bridge", nullptr));
        ID_ROOM_BUILDSMOOTHBRIDGE->setText(QCoreApplication::translate("FontDialog", "Build Smooth Bridge", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_BUILDSMOOTHBRIDGE->setShortcut(QCoreApplication::translate("FontDialog", "Ctrl+B", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_CLEARCENTERPOINT->setText(QCoreApplication::translate("FontDialog", "Clear Center Point", nullptr));
        ID_ROOM_COMBINE->setText(QCoreApplication::translate("FontDialog", "Combine Rooms", nullptr));
        ID_ROOM_DELETE->setText(QCoreApplication::translate("FontDialog", "Delete Current Room", nullptr));
        ID_ROOM_DELETECONNECTEDFACES->setText(QCoreApplication::translate("FontDialog", "Delete Connected Faces", nullptr));
        ID_ROOM_DELETEFACE->setText(QCoreApplication::translate("FontDialog", "Delete Current Face", nullptr));
        ID_ROOM_DELETEPORTAL->setText(QCoreApplication::translate("FontDialog", "Delete Current Portal", nullptr));
        ID_ROOM_DELETEVERT->setText(QCoreApplication::translate("FontDialog", "Delete Current Vert From Face", nullptr));
        ID_ROOM_DROPROOM->setText(QCoreApplication::translate("FontDialog", "Drop Room", nullptr));
        ID_ROOM_FACE_ADDVERTTOEDGE->setText(QCoreApplication::translate("FontDialog", "Add Vert To Edge", nullptr));
        ID_ROOM_FACE_DELETEVERTONEDGE->setText(QCoreApplication::translate("FontDialog", "Delete Vert on Edge", nullptr));
        ID_ROOM_FACE_MOVEVERTONEDGE->setText(QCoreApplication::translate("FontDialog", "Move Vert on Edge", nullptr));
        ID_ROOM_FACE_SPLITFACE->setText(QCoreApplication::translate("FontDialog", "Split Face", nullptr));
        ID_ROOM_FINISHNEWFACE->setText(QCoreApplication::translate("FontDialog", "Add Last Point to New Face (Finish Face)", nullptr));
        ID_ROOM_GRABTEXTURE->setText(QCoreApplication::translate("FontDialog", "Grab Texture", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_GRABTEXTURE->setShortcut(QCoreApplication::translate("FontDialog", "G", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_JOINROOMS->setText(QCoreApplication::translate("FontDialog", "Join Rooms", nullptr));
        ID_ROOM_JOINROOMSEXACT->setText(QCoreApplication::translate("FontDialog", "Join Rooms Exact", nullptr));
        ID_ROOM_JOIN_ADJACENT_FACES->setText(QCoreApplication::translate("FontDialog", "Join Adjacent Faces", nullptr));
        ID_ROOM_LINKTONEWEXTERNAL->setText(QCoreApplication::translate("FontDialog", "Link Room to New External Room", nullptr));
        ID_ROOM_MARK->setText(QCoreApplication::translate("FontDialog", "&Mark Room/Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_MARK->setShortcut(QCoreApplication::translate("FontDialog", "Ctrl+M", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_MERGEOBJECTINTOROOM->setText(QCoreApplication::translate("FontDialog", "Merge Object into Room", nullptr));
        ID_ROOM_NEXTFACE->setText(QCoreApplication::translate("FontDialog", "Next Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_NEXTFACE->setShortcut(QCoreApplication::translate("FontDialog", "F", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_PLACEROOM->setText(QCoreApplication::translate("FontDialog", "Place Room at Current Room", nullptr));
        ID_ROOM_PLACETERRAINROOM->setText(QCoreApplication::translate("FontDialog", "Place Room at Current Terrain Cell", nullptr));
        ID_ROOM_PREVIOUSFACE->setText(QCoreApplication::translate("FontDialog", "Previous Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_PREVIOUSFACE->setShortcut(QCoreApplication::translate("FontDialog", "Shift+F", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES->setText(QCoreApplication::translate("FontDialog", "Propagate to Adjacent Coplanar Faces", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES->setShortcut(QCoreApplication::translate("FontDialog", "Ctrl+F", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOALL->setText(QCoreApplication::translate("FontDialog", "Propagate to All Faces in Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOALL->setShortcut(QCoreApplication::translate("FontDialog", "Shift+R", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_RENAMEROOM->setText(QCoreApplication::translate("FontDialog", "Rename Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_RENAMEROOM->setShortcut(QCoreApplication::translate("FontDialog", "Ctrl+R", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_ROTATEPLACEDROOM45DEGREES->setText(QCoreApplication::translate("FontDialog", "Rotate Placed Room 45 Degrees", nullptr));
        ID_ROOM_SAVECURRENTROOM->setText(QCoreApplication::translate("FontDialog", "Save Current Room", nullptr));
        ID_ROOM_SELECTBYNUMBER->setText(QCoreApplication::translate("FontDialog", "Select Room by Number", nullptr));
        ID_ROOM_SELECTFACEBYNUMBER->setText(QCoreApplication::translate("FontDialog", "Select Face by Number", nullptr));
        ID_ROOM_SETCENTERFROMVIEWER->setText(QCoreApplication::translate("FontDialog", "Set Room Center from Viewer", nullptr));
        ID_ROOM_SNAPPLACEDROOM->setText(QCoreApplication::translate("FontDialog", "Snap Placed Room", nullptr));
        ID_ROOM_SNAPPOINTTOEDGE->setText(QCoreApplication::translate("FontDialog", "Snap Point to Edge", nullptr));
        ID_ROOM_SNAPPOINTTOFACE->setText(QCoreApplication::translate("FontDialog", "Snap Point to Face", nullptr));
        ID_ROOM_SNAPPOINTTOPOINT->setText(QCoreApplication::translate("FontDialog", "Snap Point to Point", nullptr));
        ID_ROOM_SPLITFACE->setText(QCoreApplication::translate("FontDialog", "Triangulate face", nullptr));
        ID_ROOM_STARTNEWFACE->setText(QCoreApplication::translate("FontDialog", "Add First Point to New Face (Start Face)", nullptr));
        ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE->setText(QCoreApplication::translate("FontDialog", "Swap Marked and Current Room/Face", nullptr));
        ID_ROOM_UNDOSNAP->setText(QCoreApplication::translate("FontDialog", "Undo Snap", nullptr));
        ID_ROOM_UNPLACEROOM->setText(QCoreApplication::translate("FontDialog", "Un-place Room/Group/Door", nullptr));
        ID_ROOM_VIEW->setText(QCoreApplication::translate("FontDialog", "Room View", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_VIEW->setShortcut(QCoreApplication::translate("FontDialog", "F4", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_SCRIPT_LEVEL_INTERFACE->setText(QCoreApplication::translate("FontDialog", "Levels and Scripts...", nullptr));
        ID_SUBEDITORS_FONT->setText(QCoreApplication::translate("FontDialog", "Font Editor...", nullptr));
        ID_SUBEDITORS_HOGMAKER->setText(QCoreApplication::translate("FontDialog", "Hog Maker...", nullptr));
        ID_SUBEDITORS_TABLEFILEEDIT->setText(QCoreApplication::translate("FontDialog", "Table File Text Editor...", nullptr));
        ID_SUBEDITORS_TABLEFILEFILTER->setText(QCoreApplication::translate("FontDialog", "Table File Filter...", nullptr));
        ID_TABLEEDIT_FILE_EXIT->setText(QCoreApplication::translate("FontDialog", "&Exit", nullptr));
        ID_TABLEEDIT_FILE_LOAD->setText(QCoreApplication::translate("FontDialog", "&Load...", nullptr));
        ID_TABLEEDIT_FILE_SAVE->setText(QCoreApplication::translate("FontDialog", "&Save", nullptr));
        ID_TABLEEDIT_FILE_SAVE_AS->setText(QCoreApplication::translate("FontDialog", "S&ave as...", nullptr));
        ID_TABLEFILTER_FILE_EXIT->setText(QCoreApplication::translate("FontDialog", "Exit", nullptr));
        ID_TABLEFILTER_FILE_LOAD->setText(QCoreApplication::translate("FontDialog", "Load...", nullptr));
        ID_TABLEFILTER_FILE_NEW->setText(QCoreApplication::translate("FontDialog", "New", nullptr));
        ID_TABLEFILTER_FILE_SAVE->setText(QCoreApplication::translate("FontDialog", "Save", nullptr));
        ID_TABLEFILTER_FILE_SAVEAS->setText(QCoreApplication::translate("FontDialog", "Save as...", nullptr));
        ID_TERRAIN_VIEW->setText(QCoreApplication::translate("FontDialog", "Terrain View", nullptr));
#if QT_CONFIG(shortcut)
        ID_TERRAIN_VIEW->setShortcut(QCoreApplication::translate("FontDialog", "F3", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_TEST_TEST1->setText(QCoreApplication::translate("FontDialog", "Test function 1", nullptr));
        ID_TEST_TEST2->setText(QCoreApplication::translate("FontDialog", "Test function 2", nullptr));
        ID_TEST_TEST3->setText(QCoreApplication::translate("FontDialog", "Test function 3", nullptr));
        ID_TOOLS_WORLD_OBJECTS_BUILDINGS->setText(QCoreApplication::translate("FontDialog", "Buildings...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_CLUTTER->setText(QCoreApplication::translate("FontDialog", "Clutter...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_DOOR->setText(QCoreApplication::translate("FontDialog", "Doors...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_LIGHTS->setText(QCoreApplication::translate("FontDialog", "Lights...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_PLAYER->setText(QCoreApplication::translate("FontDialog", "Player Ships...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_POWERUPS->setText(QCoreApplication::translate("FontDialog", "Powerups...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_ROBOTS->setText(QCoreApplication::translate("FontDialog", "Robots...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_SOUND->setText(QCoreApplication::translate("FontDialog", "Sounds...", nullptr));
        ID_TOOLS_WORLD_TEXTURES->setText(QCoreApplication::translate("FontDialog", "Textures...", nullptr));
        ID_TOOLS_WORLD_WEAPONS->setText(QCoreApplication::translate("FontDialog", "Weapons...", nullptr));
        ID_VIEW_CENTERONCUBE->setText(QCoreApplication::translate("FontDialog", "Center on Current &Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_CENTERONCUBE->setShortcut(QCoreApplication::translate("FontDialog", "Home", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_CENTERONMINE->setText(QCoreApplication::translate("FontDialog", "Center on &Mine Origin", nullptr));
        ID_VIEW_CENTERONOBJECT->setText(QCoreApplication::translate("FontDialog", "Center on Current &Object", nullptr));
        ID_VIEW_DELETEVIEWER->setText(QCoreApplication::translate("FontDialog", "Delete Viewer", nullptr));
        ID_VIEW_FLIP->setText(QCoreApplication::translate("FontDialog", "Flip Viewer", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_FLIP->setShortcut(QCoreApplication::translate("FontDialog", "/", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_KEYPAD_TOGGLE->setText(QCoreApplication::translate("FontDialog", "Keypad", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_KEYPAD_TOGGLE->setShortcut(QCoreApplication::translate("FontDialog", "F10", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOCURRENTOBJECT->setText(QCoreApplication::translate("FontDialog", "Move camera to Current Object", nullptr));
        ID_VIEW_MOVECAMERATOSELECTEDFACE->setText(QCoreApplication::translate("FontDialog", "Move camera to &Current Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOSELECTEDFACE->setShortcut(QCoreApplication::translate("FontDialog", "Shift+C", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOSELECTEDROOM->setText(QCoreApplication::translate("FontDialog", "Move camera to &Current Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOSELECTEDROOM->setShortcut(QCoreApplication::translate("FontDialog", "C", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_NEWVIEWER->setText(QCoreApplication::translate("FontDialog", "New Viewer", nullptr));
        ID_VIEW_NEXTVIEWER->setText(QCoreApplication::translate("FontDialog", "Next Viewer", nullptr));
        ID_VIEW_RESETVIEWRADIUS->setText(QCoreApplication::translate("FontDialog", "Reset View Radius", nullptr));
        ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW->setText(QCoreApplication::translate("FontDialog", "Show objects in Wireframe view", nullptr));
        ID_VIEW_SHOWVIEWERFORWARDVECTOR->setText(QCoreApplication::translate("FontDialog", "Show Viewer Forward Vector", nullptr));
        ID_VIEW_STATUS_BAR->setText(QCoreApplication::translate("FontDialog", "&Status Bar", nullptr));
        ID_VIEW_TEXTUREMINE->setText(QCoreApplication::translate("FontDialog", "Te&xtured view", nullptr));
        ID_VIEW_TOOLBAR->setText(QCoreApplication::translate("FontDialog", "&Toolbar", nullptr));
        ID_VIEW_VIEWPROP->setText(QCoreApplication::translate("FontDialog", "Display Viewer Properties", nullptr));
        ID_VIEW_WIREFRAMEMINE->setText(QCoreApplication::translate("FontDialog", "&Wireframe view", nullptr));
        ID_WINDOW_CASCADE->setText(QCoreApplication::translate("FontDialog", "&Cascade", nullptr));
        ID_WINDOW_TILE->setText(QCoreApplication::translate("FontDialog", "&Tile", nullptr));
        ID_ZBUTTON->setText(QCoreApplication::translate("FontDialog", "ID_ZBUTTON", nullptr));
        IDC_STATIC->setTitle(QCoreApplication::translate("FontDialog", "Properties", nullptr));
        IDC_STATIC_2->setText(QCoreApplication::translate("FontDialog", "Min", nullptr));
        IDC_PROP_MONO->setText(QCoreApplication::translate("FontDialog", "Monochrome", nullptr));
        IDC_STATIC_PROP->setText(QCoreApplication::translate("FontDialog", "Static", nullptr));
        IDC_PROP_BITMAPPED->setText(QCoreApplication::translate("FontDialog", "Bitmapped", nullptr));
        IDC_STATIC_3->setText(QCoreApplication::translate("FontDialog", "Max", nullptr));
        IDC_STATIC_MAXASCII->setText(QCoreApplication::translate("FontDialog", "Static", nullptr));
        IDC_PROP_GRADIANT->setText(QCoreApplication::translate("FontDialog", "Grayscale", nullptr));
        IDC_STATIC_4->setText(QCoreApplication::translate("FontDialog", "Brighness", nullptr));
        IDC_STATIC_FONTNAME->setTitle(QCoreApplication::translate("FontDialog", "Untitled", nullptr));
        IDC_STATIC_5->setText(QCoreApplication::translate("FontDialog", "Name", nullptr));
        IDC_OPEN->setText(QCoreApplication::translate("FontDialog", "Open...", nullptr));
        IDC_SAVE->setText(QCoreApplication::translate("FontDialog", "Save", nullptr));
        IDC_SAVEAS->setText(QCoreApplication::translate("FontDialog", "Save as,,,", nullptr));
        IDOK->setText(QCoreApplication::translate("FontDialog", "OK", nullptr));
        IDCANCEL->setText(QCoreApplication::translate("FontDialog", "Cancel", nullptr));
        IDC_NEW->setText(QCoreApplication::translate("FontDialog", "New", nullptr));
        IDC_LOCK->setText(QCoreApplication::translate("FontDialog", "Lock", nullptr));
        IDC_CHECKIN->setText(QCoreApplication::translate("FontDialog", "Check in", nullptr));
        IDC_DELETE->setText(QCoreApplication::translate("FontDialog", "Delete", nullptr));
    } // retranslateUi

};

namespace Ui {
    class FontDialog: public Ui_FontDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FONT_DIALOG_H
