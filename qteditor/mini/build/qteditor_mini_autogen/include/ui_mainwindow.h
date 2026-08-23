/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
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
    QAction *ID_TERRAIN_SELECT_RANGE;
    QAction *ID_TERRAIN_SOUNDS;
    QAction *ID_TERRAIN_VIEW;
    QAction *ID_TOOLS_WORLD_OBJECTS_BUILDINGS;
    QAction *ID_TOOLS_WORLD_OBJECTS_CLUTTER;
    QAction *ID_TOOLS_WORLD_OBJECTS_DOOR;
    QAction *ID_TOOLS_WORLD_OBJECTS_LIGHTS;
    QAction *ID_TOOLS_WORLD_OBJECTS_PLAYER;
    QAction *ID_TOOLS_WORLD_OBJECTS_POWERUPS;
    QAction *ID_TOOLS_WORLD_OBJECTS_ROBOTS;
    QAction *ID_TOOLS_WORLD_OBJECTS_SOUND;
    QAction *ID_TOOLS_WORLD_OBJECTS_TEXTURES;
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
    QWidget *centralwidget;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuRoom;
    QMenu *menuFaceEditing;
    QMenu *menuObject;
    QMenu *menuEditors;
    QMenu *menuWindow;
    QMenu *menuHelp;
    QToolBar *maintoolbar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(900, 600);
        IDD_ORPHANHUNTER = new QAction(MainWindow);
        IDD_ORPHANHUNTER->setObjectName(QString::fromUtf8("IDD_ORPHANHUNTER"));
        IDD_REORDER_PAGES = new QAction(MainWindow);
        IDD_REORDER_PAGES->setObjectName(QString::fromUtf8("IDD_REORDER_PAGES"));
        IDD_SHOW_ALL_CHECKED_OUT = new QAction(MainWindow);
        IDD_SHOW_ALL_CHECKED_OUT->setObjectName(QString::fromUtf8("IDD_SHOW_ALL_CHECKED_OUT"));
        IDM_IMPORT_BITMAP = new QAction(MainWindow);
        IDM_IMPORT_BITMAP->setObjectName(QString::fromUtf8("IDM_IMPORT_BITMAP"));
        ID_APP_ABOUT = new QAction(MainWindow);
        ID_APP_ABOUT->setObjectName(QString::fromUtf8("ID_APP_ABOUT"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ui/images/Toolbar/6.png"), QSize(), QIcon::Normal, QIcon::Off);
        ID_APP_ABOUT->setIcon(icon);
        ID_APP_EXIT = new QAction(MainWindow);
        ID_APP_EXIT->setObjectName(QString::fromUtf8("ID_APP_EXIT"));
        ID_BRIEFING_EDITOR = new QAction(MainWindow);
        ID_BRIEFING_EDITOR->setObjectName(QString::fromUtf8("ID_BRIEFING_EDITOR"));
        ID_BUTTON_LIGHTING = new QAction(MainWindow);
        ID_BUTTON_LIGHTING->setObjectName(QString::fromUtf8("ID_BUTTON_LIGHTING"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/ui/images/Toolbar/10.png"), QSize(), QIcon::Normal, QIcon::Off);
        ID_BUTTON_LIGHTING->setIcon(icon1);
        ID_BUTTON_OUTLINE = new QAction(MainWindow);
        ID_BUTTON_OUTLINE->setObjectName(QString::fromUtf8("ID_BUTTON_OUTLINE"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/ui/images/Toolbar/8.png"), QSize(), QIcon::Normal, QIcon::Off);
        ID_BUTTON_OUTLINE->setIcon(icon2);
        ID_BUTTON_WINDOWSELECTION = new QAction(MainWindow);
        ID_BUTTON_WINDOWSELECTION->setObjectName(QString::fromUtf8("ID_BUTTON_WINDOWSELECTION"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/ui/images/Toolbar/9.png"), QSize(), QIcon::Normal, QIcon::Off);
        ID_BUTTON_WINDOWSELECTION->setIcon(icon3);
        ID_D3HELP = new QAction(MainWindow);
        ID_D3HELP->setObjectName(QString::fromUtf8("ID_D3HELP"));
        ID_EDITORS_AIPROPERTIES = new QAction(MainWindow);
        ID_EDITORS_AIPROPERTIES->setObjectName(QString::fromUtf8("ID_EDITORS_AIPROPERTIES"));
        ID_EDITORS_AMBIENTSOUNDS = new QAction(MainWindow);
        ID_EDITORS_AMBIENTSOUNDS->setObjectName(QString::fromUtf8("ID_EDITORS_AMBIENTSOUNDS"));
        ID_EDITORS_DALLAS = new QAction(MainWindow);
        ID_EDITORS_DALLAS->setObjectName(QString::fromUtf8("ID_EDITORS_DALLAS"));
        ID_EDITORS_FILES = new QAction(MainWindow);
        ID_EDITORS_FILES->setObjectName(QString::fromUtf8("ID_EDITORS_FILES"));
        ID_EDITORS_MEGACELLS = new QAction(MainWindow);
        ID_EDITORS_MEGACELLS->setObjectName(QString::fromUtf8("ID_EDITORS_MEGACELLS"));
        ID_EDIT_ADDSELECT = new QAction(MainWindow);
        ID_EDIT_ADDSELECT->setObjectName(QString::fromUtf8("ID_EDIT_ADDSELECT"));
        ID_EDIT_ATTACH = new QAction(MainWindow);
        ID_EDIT_ATTACH->setObjectName(QString::fromUtf8("ID_EDIT_ATTACH"));
        ID_EDIT_CLEARSELECTED = new QAction(MainWindow);
        ID_EDIT_CLEARSELECTED->setObjectName(QString::fromUtf8("ID_EDIT_CLEARSELECTED"));
        ID_EDIT_COPY = new QAction(MainWindow);
        ID_EDIT_COPY->setObjectName(QString::fromUtf8("ID_EDIT_COPY"));
        ID_EDIT_CUT = new QAction(MainWindow);
        ID_EDIT_CUT->setObjectName(QString::fromUtf8("ID_EDIT_CUT"));
        ID_EDIT_DELETE = new QAction(MainWindow);
        ID_EDIT_DELETE->setObjectName(QString::fromUtf8("ID_EDIT_DELETE"));
        ID_EDIT_LOADSCRAP = new QAction(MainWindow);
        ID_EDIT_LOADSCRAP->setObjectName(QString::fromUtf8("ID_EDIT_LOADSCRAP"));
        ID_EDIT_PASTE = new QAction(MainWindow);
        ID_EDIT_PASTE->setObjectName(QString::fromUtf8("ID_EDIT_PASTE"));
        ID_EDIT_PLACE = new QAction(MainWindow);
        ID_EDIT_PLACE->setObjectName(QString::fromUtf8("ID_EDIT_PLACE"));
        ID_EDIT_PLACE_TERRAIN = new QAction(MainWindow);
        ID_EDIT_PLACE_TERRAIN->setObjectName(QString::fromUtf8("ID_EDIT_PLACE_TERRAIN"));
        ID_EDIT_REMOVESELECT = new QAction(MainWindow);
        ID_EDIT_REMOVESELECT->setObjectName(QString::fromUtf8("ID_EDIT_REMOVESELECT"));
        ID_EDIT_SAVESCRAP = new QAction(MainWindow);
        ID_EDIT_SAVESCRAP->setObjectName(QString::fromUtf8("ID_EDIT_SAVESCRAP"));
        ID_EDIT_SELECTATTACHED = new QAction(MainWindow);
        ID_EDIT_SELECTATTACHED->setObjectName(QString::fromUtf8("ID_EDIT_SELECTATTACHED"));
        ID_EDIT_UNDO = new QAction(MainWindow);
        ID_EDIT_UNDO->setObjectName(QString::fromUtf8("ID_EDIT_UNDO"));
        ID_FILE_AUTOSAVE = new QAction(MainWindow);
        ID_FILE_AUTOSAVE->setObjectName(QString::fromUtf8("ID_FILE_AUTOSAVE"));
        ID_FILE_AUTOSAVE->setCheckable(true);
        ID_FILE_CHECKMINE = new QAction(MainWindow);
        ID_FILE_CHECKMINE->setObjectName(QString::fromUtf8("ID_FILE_CHECKMINE"));
        ID_FILE_FIXCRACKS = new QAction(MainWindow);
        ID_FILE_FIXCRACKS->setObjectName(QString::fromUtf8("ID_FILE_FIXCRACKS"));
        ID_FILE_FIXDEGENERATEFACES = new QAction(MainWindow);
        ID_FILE_FIXDEGENERATEFACES->setObjectName(QString::fromUtf8("ID_FILE_FIXDEGENERATEFACES"));
        ID_FILE_IMPORT_ROOM = new QAction(MainWindow);
        ID_FILE_IMPORT_ROOM->setObjectName(QString::fromUtf8("ID_FILE_IMPORT_ROOM"));
        ID_FILE_INFORMATION = new QAction(MainWindow);
        ID_FILE_INFORMATION->setObjectName(QString::fromUtf8("ID_FILE_INFORMATION"));
        ID_FILE_LEAVE_EDITOR = new QAction(MainWindow);
        ID_FILE_LEAVE_EDITOR->setObjectName(QString::fromUtf8("ID_FILE_LEAVE_EDITOR"));
        ID_FILE_LEVELPROPS = new QAction(MainWindow);
        ID_FILE_LEVELPROPS->setObjectName(QString::fromUtf8("ID_FILE_LEVELPROPS"));
        ID_FILE_LEVEL_INFO = new QAction(MainWindow);
        ID_FILE_LEVEL_INFO->setObjectName(QString::fromUtf8("ID_FILE_LEVEL_INFO"));
        ID_FILE_MRU_FILE1 = new QAction(MainWindow);
        ID_FILE_MRU_FILE1->setObjectName(QString::fromUtf8("ID_FILE_MRU_FILE1"));
        ID_FILE_MRU_FILE1->setEnabled(false);
        ID_FILE_NEW = new QAction(MainWindow);
        ID_FILE_NEW->setObjectName(QString::fromUtf8("ID_FILE_NEW"));
        ID_FILE_OPEN = new QAction(MainWindow);
        ID_FILE_OPEN->setObjectName(QString::fromUtf8("ID_FILE_OPEN"));
        ID_FILE_PLAY640X480 = new QAction(MainWindow);
        ID_FILE_PLAY640X480->setObjectName(QString::fromUtf8("ID_FILE_PLAY640X480"));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/ui/images/Toolbar/7.png"), QSize(), QIcon::Normal, QIcon::Off);
        ID_FILE_PLAY640X480->setIcon(icon4);
        ID_FILE_PREFERENCES = new QAction(MainWindow);
        ID_FILE_PREFERENCES->setObjectName(QString::fromUtf8("ID_FILE_PREFERENCES"));
        ID_FILE_PRINT = new QAction(MainWindow);
        ID_FILE_PRINT->setObjectName(QString::fromUtf8("ID_FILE_PRINT"));
        ID_FILE_PRINT_PREVIEW = new QAction(MainWindow);
        ID_FILE_PRINT_PREVIEW->setObjectName(QString::fromUtf8("ID_FILE_PRINT_PREVIEW"));
        ID_FILE_PRINT_SETUP = new QAction(MainWindow);
        ID_FILE_PRINT_SETUP->setObjectName(QString::fromUtf8("ID_FILE_PRINT_SETUP"));
        ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM = new QAction(MainWindow);
        ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM->setObjectName(QString::fromUtf8("ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM"));
        ID_FILE_REMOVEEXTRAPOINTS = new QAction(MainWindow);
        ID_FILE_REMOVEEXTRAPOINTS->setObjectName(QString::fromUtf8("ID_FILE_REMOVEEXTRAPOINTS"));
        ID_FILE_RESTOREGAMESTATE = new QAction(MainWindow);
        ID_FILE_RESTOREGAMESTATE->setObjectName(QString::fromUtf8("ID_FILE_RESTOREGAMESTATE"));
        ID_FILE_SAVE = new QAction(MainWindow);
        ID_FILE_SAVE->setObjectName(QString::fromUtf8("ID_FILE_SAVE"));
        ID_FILE_SAVEGOALTEXT = new QAction(MainWindow);
        ID_FILE_SAVEGOALTEXT->setObjectName(QString::fromUtf8("ID_FILE_SAVEGOALTEXT"));
        ID_FILE_SAVE_AS = new QAction(MainWindow);
        ID_FILE_SAVE_AS->setObjectName(QString::fromUtf8("ID_FILE_SAVE_AS"));
        ID_FILE_SAVE_COPY_AS = new QAction(MainWindow);
        ID_FILE_SAVE_COPY_AS->setObjectName(QString::fromUtf8("ID_FILE_SAVE_COPY_AS"));
        ID_FILE_STATS = new QAction(MainWindow);
        ID_FILE_STATS->setObjectName(QString::fromUtf8("ID_FILE_STATS"));
        ID_FILE_UPDATE = new QAction(MainWindow);
        ID_FILE_UPDATE->setObjectName(QString::fromUtf8("ID_FILE_UPDATE"));
        ID_FILE_VERIFY_LEVEL = new QAction(MainWindow);
        ID_FILE_VERIFY_LEVEL->setObjectName(QString::fromUtf8("ID_FILE_VERIFY_LEVEL"));
        ID_HOTSPOT_TGA = new QAction(MainWindow);
        ID_HOTSPOT_TGA->setObjectName(QString::fromUtf8("ID_HOTSPOT_TGA"));
        ID_MINE_VIEW = new QAction(MainWindow);
        ID_MINE_VIEW->setObjectName(QString::fromUtf8("ID_MINE_VIEW"));
        ID_OBJBUTTON = new QAction(MainWindow);
        ID_OBJBUTTON->setObjectName(QString::fromUtf8("ID_OBJBUTTON"));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/ui/images/Toolbar/12.png"), QSize(), QIcon::Normal, QIcon::Off);
        ID_OBJBUTTON->setIcon(icon5);
        ID_OBJECT_DELETEOBJECT = new QAction(MainWindow);
        ID_OBJECT_DELETEOBJECT->setObjectName(QString::fromUtf8("ID_OBJECT_DELETEOBJECT"));
        ID_OBJECT_MOVEPLAYER = new QAction(MainWindow);
        ID_OBJECT_MOVEPLAYER->setObjectName(QString::fromUtf8("ID_OBJECT_MOVEPLAYER"));
        ID_OBJECT_PLACECAMERAATCURRENTFACE = new QAction(MainWindow);
        ID_OBJECT_PLACECAMERAATCURRENTFACE->setObjectName(QString::fromUtf8("ID_OBJECT_PLACECAMERAATCURRENTFACE"));
        ID_OBJECT_PLACECAMERAATVIEWER = new QAction(MainWindow);
        ID_OBJECT_PLACECAMERAATVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_PLACECAMERAATVIEWER"));
        ID_OBJECT_PLACEOBJECT = new QAction(MainWindow);
        ID_OBJECT_PLACEOBJECT->setObjectName(QString::fromUtf8("ID_OBJECT_PLACEOBJECT"));
        ID_OBJECT_PLACESOUNDSOURCEATVIEWER = new QAction(MainWindow);
        ID_OBJECT_PLACESOUNDSOURCEATVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_PLACESOUNDSOURCEATVIEWER"));
        ID_OBJECT_PLACEWAYPOINTATVIEWER = new QAction(MainWindow);
        ID_OBJECT_PLACEWAYPOINTATVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_PLACEWAYPOINTATVIEWER"));
        ID_OBJECT_SELECTBYNUMBER = new QAction(MainWindow);
        ID_OBJECT_SELECTBYNUMBER->setObjectName(QString::fromUtf8("ID_OBJECT_SELECTBYNUMBER"));
        ID_OBJECT_SETCAMERAFROMVIEWER = new QAction(MainWindow);
        ID_OBJECT_SETCAMERAFROMVIEWER->setObjectName(QString::fromUtf8("ID_OBJECT_SETCAMERAFROMVIEWER"));
        ID_OBJECT_SETVIEWERFROMCAMERA = new QAction(MainWindow);
        ID_OBJECT_SETVIEWERFROMCAMERA->setObjectName(QString::fromUtf8("ID_OBJECT_SETVIEWERFROMCAMERA"));
        ID_OSIRISCOMPILE = new QAction(MainWindow);
        ID_OSIRISCOMPILE->setObjectName(QString::fromUtf8("ID_OSIRISCOMPILE"));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/ui/images/Toolbar/14.png"), QSize(), QIcon::Normal, QIcon::Off);
        ID_OSIRISCOMPILE->setIcon(icon6);
        ID_REINIT_OPENGL = new QAction(MainWindow);
        ID_REINIT_OPENGL->setObjectName(QString::fromUtf8("ID_REINIT_OPENGL"));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/ui/images/Toolbar/13.png"), QSize(), QIcon::Normal, QIcon::Off);
        ID_REINIT_OPENGL->setIcon(icon7);
        ID_ROOM_ADD = new QAction(MainWindow);
        ID_ROOM_ADD->setObjectName(QString::fromUtf8("ID_ROOM_ADD"));
        ID_ROOM_ADDVERTTONEWFACE = new QAction(MainWindow);
        ID_ROOM_ADDVERTTONEWFACE->setObjectName(QString::fromUtf8("ID_ROOM_ADDVERTTONEWFACE"));
        ID_ROOM_ATTACHROOM = new QAction(MainWindow);
        ID_ROOM_ATTACHROOM->setObjectName(QString::fromUtf8("ID_ROOM_ATTACHROOM"));
        ID_ROOM_BUILDBRIDGE = new QAction(MainWindow);
        ID_ROOM_BUILDBRIDGE->setObjectName(QString::fromUtf8("ID_ROOM_BUILDBRIDGE"));
        ID_ROOM_BUILDSMOOTHBRIDGE = new QAction(MainWindow);
        ID_ROOM_BUILDSMOOTHBRIDGE->setObjectName(QString::fromUtf8("ID_ROOM_BUILDSMOOTHBRIDGE"));
        ID_ROOM_CLEARCENTERPOINT = new QAction(MainWindow);
        ID_ROOM_CLEARCENTERPOINT->setObjectName(QString::fromUtf8("ID_ROOM_CLEARCENTERPOINT"));
        ID_ROOM_COMBINE = new QAction(MainWindow);
        ID_ROOM_COMBINE->setObjectName(QString::fromUtf8("ID_ROOM_COMBINE"));
        ID_ROOM_DELETE = new QAction(MainWindow);
        ID_ROOM_DELETE->setObjectName(QString::fromUtf8("ID_ROOM_DELETE"));
        ID_ROOM_DELETECONNECTEDFACES = new QAction(MainWindow);
        ID_ROOM_DELETECONNECTEDFACES->setObjectName(QString::fromUtf8("ID_ROOM_DELETECONNECTEDFACES"));
        ID_ROOM_DELETEFACE = new QAction(MainWindow);
        ID_ROOM_DELETEFACE->setObjectName(QString::fromUtf8("ID_ROOM_DELETEFACE"));
        ID_ROOM_DELETEPORTAL = new QAction(MainWindow);
        ID_ROOM_DELETEPORTAL->setObjectName(QString::fromUtf8("ID_ROOM_DELETEPORTAL"));
        ID_ROOM_DELETEVERT = new QAction(MainWindow);
        ID_ROOM_DELETEVERT->setObjectName(QString::fromUtf8("ID_ROOM_DELETEVERT"));
        ID_ROOM_DROPROOM = new QAction(MainWindow);
        ID_ROOM_DROPROOM->setObjectName(QString::fromUtf8("ID_ROOM_DROPROOM"));
        ID_ROOM_FACE_ADDVERTTOEDGE = new QAction(MainWindow);
        ID_ROOM_FACE_ADDVERTTOEDGE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_ADDVERTTOEDGE"));
        ID_ROOM_FACE_DELETEVERTONEDGE = new QAction(MainWindow);
        ID_ROOM_FACE_DELETEVERTONEDGE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_DELETEVERTONEDGE"));
        ID_ROOM_FACE_MOVEVERTONEDGE = new QAction(MainWindow);
        ID_ROOM_FACE_MOVEVERTONEDGE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_MOVEVERTONEDGE"));
        ID_ROOM_FACE_SPLITFACE = new QAction(MainWindow);
        ID_ROOM_FACE_SPLITFACE->setObjectName(QString::fromUtf8("ID_ROOM_FACE_SPLITFACE"));
        ID_ROOM_FINISHNEWFACE = new QAction(MainWindow);
        ID_ROOM_FINISHNEWFACE->setObjectName(QString::fromUtf8("ID_ROOM_FINISHNEWFACE"));
        ID_ROOM_GRABTEXTURE = new QAction(MainWindow);
        ID_ROOM_GRABTEXTURE->setObjectName(QString::fromUtf8("ID_ROOM_GRABTEXTURE"));
        ID_ROOM_JOINROOMS = new QAction(MainWindow);
        ID_ROOM_JOINROOMS->setObjectName(QString::fromUtf8("ID_ROOM_JOINROOMS"));
        ID_ROOM_JOINROOMSEXACT = new QAction(MainWindow);
        ID_ROOM_JOINROOMSEXACT->setObjectName(QString::fromUtf8("ID_ROOM_JOINROOMSEXACT"));
        ID_ROOM_JOIN_ADJACENT_FACES = new QAction(MainWindow);
        ID_ROOM_JOIN_ADJACENT_FACES->setObjectName(QString::fromUtf8("ID_ROOM_JOIN_ADJACENT_FACES"));
        ID_ROOM_LINKTONEWEXTERNAL = new QAction(MainWindow);
        ID_ROOM_LINKTONEWEXTERNAL->setObjectName(QString::fromUtf8("ID_ROOM_LINKTONEWEXTERNAL"));
        ID_ROOM_MARK = new QAction(MainWindow);
        ID_ROOM_MARK->setObjectName(QString::fromUtf8("ID_ROOM_MARK"));
        ID_ROOM_MERGEOBJECTINTOROOM = new QAction(MainWindow);
        ID_ROOM_MERGEOBJECTINTOROOM->setObjectName(QString::fromUtf8("ID_ROOM_MERGEOBJECTINTOROOM"));
        ID_ROOM_NEXTFACE = new QAction(MainWindow);
        ID_ROOM_NEXTFACE->setObjectName(QString::fromUtf8("ID_ROOM_NEXTFACE"));
        ID_ROOM_PLACEROOM = new QAction(MainWindow);
        ID_ROOM_PLACEROOM->setObjectName(QString::fromUtf8("ID_ROOM_PLACEROOM"));
        ID_ROOM_PLACETERRAINROOM = new QAction(MainWindow);
        ID_ROOM_PLACETERRAINROOM->setObjectName(QString::fromUtf8("ID_ROOM_PLACETERRAINROOM"));
        ID_ROOM_PREVIOUSFACE = new QAction(MainWindow);
        ID_ROOM_PREVIOUSFACE->setObjectName(QString::fromUtf8("ID_ROOM_PREVIOUSFACE"));
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES = new QAction(MainWindow);
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES->setObjectName(QString::fromUtf8("ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES"));
        ID_ROOM_PROPAGATETOALL = new QAction(MainWindow);
        ID_ROOM_PROPAGATETOALL->setObjectName(QString::fromUtf8("ID_ROOM_PROPAGATETOALL"));
        ID_ROOM_RENAMEROOM = new QAction(MainWindow);
        ID_ROOM_RENAMEROOM->setObjectName(QString::fromUtf8("ID_ROOM_RENAMEROOM"));
        ID_ROOM_ROTATEPLACEDROOM45DEGREES = new QAction(MainWindow);
        ID_ROOM_ROTATEPLACEDROOM45DEGREES->setObjectName(QString::fromUtf8("ID_ROOM_ROTATEPLACEDROOM45DEGREES"));
        ID_ROOM_SAVECURRENTROOM = new QAction(MainWindow);
        ID_ROOM_SAVECURRENTROOM->setObjectName(QString::fromUtf8("ID_ROOM_SAVECURRENTROOM"));
        ID_ROOM_SELECTBYNUMBER = new QAction(MainWindow);
        ID_ROOM_SELECTBYNUMBER->setObjectName(QString::fromUtf8("ID_ROOM_SELECTBYNUMBER"));
        ID_ROOM_SELECTFACEBYNUMBER = new QAction(MainWindow);
        ID_ROOM_SELECTFACEBYNUMBER->setObjectName(QString::fromUtf8("ID_ROOM_SELECTFACEBYNUMBER"));
        ID_ROOM_SETCENTERFROMVIEWER = new QAction(MainWindow);
        ID_ROOM_SETCENTERFROMVIEWER->setObjectName(QString::fromUtf8("ID_ROOM_SETCENTERFROMVIEWER"));
        ID_ROOM_SNAPPLACEDROOM = new QAction(MainWindow);
        ID_ROOM_SNAPPLACEDROOM->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPLACEDROOM"));
        ID_ROOM_SNAPPOINTTOEDGE = new QAction(MainWindow);
        ID_ROOM_SNAPPOINTTOEDGE->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPOINTTOEDGE"));
        ID_ROOM_SNAPPOINTTOFACE = new QAction(MainWindow);
        ID_ROOM_SNAPPOINTTOFACE->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPOINTTOFACE"));
        ID_ROOM_SNAPPOINTTOPOINT = new QAction(MainWindow);
        ID_ROOM_SNAPPOINTTOPOINT->setObjectName(QString::fromUtf8("ID_ROOM_SNAPPOINTTOPOINT"));
        ID_ROOM_SPLITFACE = new QAction(MainWindow);
        ID_ROOM_SPLITFACE->setObjectName(QString::fromUtf8("ID_ROOM_SPLITFACE"));
        ID_ROOM_STARTNEWFACE = new QAction(MainWindow);
        ID_ROOM_STARTNEWFACE->setObjectName(QString::fromUtf8("ID_ROOM_STARTNEWFACE"));
        ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE = new QAction(MainWindow);
        ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE->setObjectName(QString::fromUtf8("ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE"));
        ID_ROOM_UNDOSNAP = new QAction(MainWindow);
        ID_ROOM_UNDOSNAP->setObjectName(QString::fromUtf8("ID_ROOM_UNDOSNAP"));
        ID_ROOM_UNPLACEROOM = new QAction(MainWindow);
        ID_ROOM_UNPLACEROOM->setObjectName(QString::fromUtf8("ID_ROOM_UNPLACEROOM"));
        ID_ROOM_VIEW = new QAction(MainWindow);
        ID_ROOM_VIEW->setObjectName(QString::fromUtf8("ID_ROOM_VIEW"));
        ID_SCRIPT_LEVEL_INTERFACE = new QAction(MainWindow);
        ID_SCRIPT_LEVEL_INTERFACE->setObjectName(QString::fromUtf8("ID_SCRIPT_LEVEL_INTERFACE"));
        ID_SUBEDITORS_FONT = new QAction(MainWindow);
        ID_SUBEDITORS_FONT->setObjectName(QString::fromUtf8("ID_SUBEDITORS_FONT"));
        ID_SUBEDITORS_HOGMAKER = new QAction(MainWindow);
        ID_SUBEDITORS_HOGMAKER->setObjectName(QString::fromUtf8("ID_SUBEDITORS_HOGMAKER"));
        ID_SUBEDITORS_TABLEFILEEDIT = new QAction(MainWindow);
        ID_SUBEDITORS_TABLEFILEEDIT->setObjectName(QString::fromUtf8("ID_SUBEDITORS_TABLEFILEEDIT"));
        ID_SUBEDITORS_TABLEFILEFILTER = new QAction(MainWindow);
        ID_SUBEDITORS_TABLEFILEFILTER->setObjectName(QString::fromUtf8("ID_SUBEDITORS_TABLEFILEFILTER"));
        ID_TABLEEDIT_FILE_EXIT = new QAction(MainWindow);
        ID_TABLEEDIT_FILE_EXIT->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_EXIT"));
        ID_TABLEEDIT_FILE_LOAD = new QAction(MainWindow);
        ID_TABLEEDIT_FILE_LOAD->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_LOAD"));
        ID_TABLEEDIT_FILE_SAVE = new QAction(MainWindow);
        ID_TABLEEDIT_FILE_SAVE->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_SAVE"));
        ID_TABLEEDIT_FILE_SAVE_AS = new QAction(MainWindow);
        ID_TABLEEDIT_FILE_SAVE_AS->setObjectName(QString::fromUtf8("ID_TABLEEDIT_FILE_SAVE_AS"));
        ID_TABLEFILTER_FILE_EXIT = new QAction(MainWindow);
        ID_TABLEFILTER_FILE_EXIT->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_EXIT"));
        ID_TABLEFILTER_FILE_LOAD = new QAction(MainWindow);
        ID_TABLEFILTER_FILE_LOAD->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_LOAD"));
        ID_TABLEFILTER_FILE_NEW = new QAction(MainWindow);
        ID_TABLEFILTER_FILE_NEW->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_NEW"));
        ID_TABLEFILTER_FILE_SAVE = new QAction(MainWindow);
        ID_TABLEFILTER_FILE_SAVE->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_SAVE"));
        ID_TABLEFILTER_FILE_SAVEAS = new QAction(MainWindow);
        ID_TABLEFILTER_FILE_SAVEAS->setObjectName(QString::fromUtf8("ID_TABLEFILTER_FILE_SAVEAS"));
        ID_TERRAIN_SELECT_RANGE = new QAction(MainWindow);
        ID_TERRAIN_SELECT_RANGE->setObjectName(QString::fromUtf8("ID_TERRAIN_SELECT_RANGE"));
        ID_TERRAIN_SOUNDS = new QAction(MainWindow);
        ID_TERRAIN_SOUNDS->setObjectName(QString::fromUtf8("ID_TERRAIN_SOUNDS"));
        ID_TERRAIN_VIEW = new QAction(MainWindow);
        ID_TERRAIN_VIEW->setObjectName(QString::fromUtf8("ID_TERRAIN_VIEW"));
        ID_TOOLS_WORLD_OBJECTS_BUILDINGS = new QAction(MainWindow);
        ID_TOOLS_WORLD_OBJECTS_BUILDINGS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_BUILDINGS"));
        ID_TOOLS_WORLD_OBJECTS_CLUTTER = new QAction(MainWindow);
        ID_TOOLS_WORLD_OBJECTS_CLUTTER->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_CLUTTER"));
        ID_TOOLS_WORLD_OBJECTS_DOOR = new QAction(MainWindow);
        ID_TOOLS_WORLD_OBJECTS_DOOR->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_DOOR"));
        ID_TOOLS_WORLD_OBJECTS_LIGHTS = new QAction(MainWindow);
        ID_TOOLS_WORLD_OBJECTS_LIGHTS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_LIGHTS"));
        ID_TOOLS_WORLD_OBJECTS_PLAYER = new QAction(MainWindow);
        ID_TOOLS_WORLD_OBJECTS_PLAYER->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_PLAYER"));
        ID_TOOLS_WORLD_OBJECTS_POWERUPS = new QAction(MainWindow);
        ID_TOOLS_WORLD_OBJECTS_POWERUPS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_POWERUPS"));
        ID_TOOLS_WORLD_OBJECTS_ROBOTS = new QAction(MainWindow);
        ID_TOOLS_WORLD_OBJECTS_ROBOTS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_ROBOTS"));
        ID_TOOLS_WORLD_OBJECTS_SOUND = new QAction(MainWindow);
        ID_TOOLS_WORLD_OBJECTS_SOUND->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_SOUND"));
        ID_TOOLS_WORLD_OBJECTS_TEXTURES = new QAction(MainWindow);
        ID_TOOLS_WORLD_OBJECTS_TEXTURES->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_OBJECTS_TEXTURES"));
        ID_TOOLS_WORLD_TEXTURES = new QAction(MainWindow);
        ID_TOOLS_WORLD_TEXTURES->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_TEXTURES"));
        ID_TOOLS_WORLD_WEAPONS = new QAction(MainWindow);
        ID_TOOLS_WORLD_WEAPONS->setObjectName(QString::fromUtf8("ID_TOOLS_WORLD_WEAPONS"));
        ID_VIEW_CENTERONCUBE = new QAction(MainWindow);
        ID_VIEW_CENTERONCUBE->setObjectName(QString::fromUtf8("ID_VIEW_CENTERONCUBE"));
        ID_VIEW_CENTERONMINE = new QAction(MainWindow);
        ID_VIEW_CENTERONMINE->setObjectName(QString::fromUtf8("ID_VIEW_CENTERONMINE"));
        ID_VIEW_CENTERONOBJECT = new QAction(MainWindow);
        ID_VIEW_CENTERONOBJECT->setObjectName(QString::fromUtf8("ID_VIEW_CENTERONOBJECT"));
        ID_VIEW_DELETEVIEWER = new QAction(MainWindow);
        ID_VIEW_DELETEVIEWER->setObjectName(QString::fromUtf8("ID_VIEW_DELETEVIEWER"));
        ID_VIEW_FLIP = new QAction(MainWindow);
        ID_VIEW_FLIP->setObjectName(QString::fromUtf8("ID_VIEW_FLIP"));
        ID_VIEW_KEYPAD_TOGGLE = new QAction(MainWindow);
        ID_VIEW_KEYPAD_TOGGLE->setObjectName(QString::fromUtf8("ID_VIEW_KEYPAD_TOGGLE"));
        ID_VIEW_MOVECAMERATOCURRENTOBJECT = new QAction(MainWindow);
        ID_VIEW_MOVECAMERATOCURRENTOBJECT->setObjectName(QString::fromUtf8("ID_VIEW_MOVECAMERATOCURRENTOBJECT"));
        ID_VIEW_MOVECAMERATOSELECTEDFACE = new QAction(MainWindow);
        ID_VIEW_MOVECAMERATOSELECTEDFACE->setObjectName(QString::fromUtf8("ID_VIEW_MOVECAMERATOSELECTEDFACE"));
        ID_VIEW_MOVECAMERATOSELECTEDROOM = new QAction(MainWindow);
        ID_VIEW_MOVECAMERATOSELECTEDROOM->setObjectName(QString::fromUtf8("ID_VIEW_MOVECAMERATOSELECTEDROOM"));
        ID_VIEW_NEWVIEWER = new QAction(MainWindow);
        ID_VIEW_NEWVIEWER->setObjectName(QString::fromUtf8("ID_VIEW_NEWVIEWER"));
        ID_VIEW_NEXTVIEWER = new QAction(MainWindow);
        ID_VIEW_NEXTVIEWER->setObjectName(QString::fromUtf8("ID_VIEW_NEXTVIEWER"));
        ID_VIEW_RESETVIEWRADIUS = new QAction(MainWindow);
        ID_VIEW_RESETVIEWRADIUS->setObjectName(QString::fromUtf8("ID_VIEW_RESETVIEWRADIUS"));
        ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW = new QAction(MainWindow);
        ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW->setObjectName(QString::fromUtf8("ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW"));
        ID_VIEW_SHOWVIEWERFORWARDVECTOR = new QAction(MainWindow);
        ID_VIEW_SHOWVIEWERFORWARDVECTOR->setObjectName(QString::fromUtf8("ID_VIEW_SHOWVIEWERFORWARDVECTOR"));
        ID_VIEW_STATUS_BAR = new QAction(MainWindow);
        ID_VIEW_STATUS_BAR->setObjectName(QString::fromUtf8("ID_VIEW_STATUS_BAR"));
        ID_VIEW_TEXTUREMINE = new QAction(MainWindow);
        ID_VIEW_TEXTUREMINE->setObjectName(QString::fromUtf8("ID_VIEW_TEXTUREMINE"));
        ID_VIEW_TOOLBAR = new QAction(MainWindow);
        ID_VIEW_TOOLBAR->setObjectName(QString::fromUtf8("ID_VIEW_TOOLBAR"));
        ID_VIEW_VIEWPROP = new QAction(MainWindow);
        ID_VIEW_VIEWPROP->setObjectName(QString::fromUtf8("ID_VIEW_VIEWPROP"));
        ID_VIEW_WIREFRAMEMINE = new QAction(MainWindow);
        ID_VIEW_WIREFRAMEMINE->setObjectName(QString::fromUtf8("ID_VIEW_WIREFRAMEMINE"));
        ID_WINDOW_CASCADE = new QAction(MainWindow);
        ID_WINDOW_CASCADE->setObjectName(QString::fromUtf8("ID_WINDOW_CASCADE"));
        ID_WINDOW_TILE = new QAction(MainWindow);
        ID_WINDOW_TILE->setObjectName(QString::fromUtf8("ID_WINDOW_TILE"));
        ID_ZBUTTON = new QAction(MainWindow);
        ID_ZBUTTON->setObjectName(QString::fromUtf8("ID_ZBUTTON"));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/ui/images/Toolbar/11.png"), QSize(), QIcon::Normal, QIcon::Off);
        ID_ZBUTTON->setIcon(icon8);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 900, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuRoom = new QMenu(menubar);
        menuRoom->setObjectName(QString::fromUtf8("menuRoom"));
        menuFaceEditing = new QMenu(menuRoom);
        menuFaceEditing->setObjectName(QString::fromUtf8("menuFaceEditing"));
        menuObject = new QMenu(menubar);
        menuObject->setObjectName(QString::fromUtf8("menuObject"));
        menuEditors = new QMenu(menubar);
        menuEditors->setObjectName(QString::fromUtf8("menuEditors"));
        menuWindow = new QMenu(menubar);
        menuWindow->setObjectName(QString::fromUtf8("menuWindow"));
        menuHelp = new QMenu(menubar);
        menuHelp->setObjectName(QString::fromUtf8("menuHelp"));
        MainWindow->setMenuBar(menubar);
        maintoolbar = new QToolBar(MainWindow);
        maintoolbar->setObjectName(QString::fromUtf8("maintoolbar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, maintoolbar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuRoom->menuAction());
        menubar->addAction(menuObject->menuAction());
        menubar->addAction(menuEditors->menuAction());
        menubar->addAction(menuWindow->menuAction());
        menubar->addAction(menuHelp->menuAction());
        menuFile->addAction(ID_FILE_NEW);
        menuFile->addAction(ID_FILE_OPEN);
        menuFile->addAction(ID_FILE_SAVE);
        menuFile->addAction(ID_FILE_SAVE_AS);
        menuFile->addSeparator();
        menuFile->addAction(ID_FILE_LEVEL_INFO);
        menuFile->addAction(ID_FILE_SAVEGOALTEXT);
        menuFile->addAction(ID_FILE_STATS);
        menuFile->addAction(ID_FILE_VERIFY_LEVEL);
        menuFile->addAction(ID_FILE_FIXCRACKS);
        menuFile->addAction(ID_FILE_REMOVEEXTRAPOINTS);
        menuFile->addAction(ID_FILE_FIXDEGENERATEFACES);
        menuFile->addAction(ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM);
        menuFile->addAction(ID_FILE_LEVELPROPS);
        menuFile->addSeparator();
        menuFile->addAction(ID_FILE_IMPORT_ROOM);
        menuFile->addSeparator();
        menuFile->addAction(ID_FILE_PLAY640X480);
        menuFile->addAction(ID_FILE_LEAVE_EDITOR);
        menuFile->addSeparator();
        menuFile->addAction(ID_FILE_AUTOSAVE);
        menuFile->addAction(ID_FILE_RESTOREGAMESTATE);
        menuFile->addAction(ID_FILE_INFORMATION);
        menuFile->addAction(ID_FILE_CHECKMINE);
        menuFile->addSeparator();
        menuFile->addAction(ID_FILE_PRINT);
        menuFile->addAction(ID_FILE_PRINT_PREVIEW);
        menuFile->addAction(ID_FILE_PRINT_SETUP);
        menuFile->addSeparator();
        menuFile->addAction(ID_FILE_PREFERENCES);
        menuFile->addSeparator();
        menuFile->addAction(ID_FILE_MRU_FILE1);
        menuFile->addSeparator();
        menuFile->addAction(ID_APP_EXIT);
        menuEdit->addAction(ID_EDIT_UNDO);
        menuEdit->addSeparator();
        menuEdit->addAction(ID_EDIT_CUT);
        menuEdit->addAction(ID_EDIT_COPY);
        menuEdit->addAction(ID_EDIT_PLACE);
        menuEdit->addAction(ID_EDIT_PLACE_TERRAIN);
        menuEdit->addAction(ID_EDIT_ATTACH);
        menuEdit->addAction(ID_EDIT_DELETE);
        menuEdit->addSeparator();
        menuEdit->addAction(ID_EDIT_LOADSCRAP);
        menuEdit->addAction(ID_EDIT_SAVESCRAP);
        menuEdit->addSeparator();
        menuEdit->addAction(ID_EDIT_ADDSELECT);
        menuEdit->addAction(ID_EDIT_REMOVESELECT);
        menuEdit->addAction(ID_EDIT_SELECTATTACHED);
        menuEdit->addAction(ID_EDIT_CLEARSELECTED);
        menuView->addAction(ID_VIEW_TOOLBAR);
        menuView->addAction(ID_VIEW_KEYPAD_TOGGLE);
        menuView->addAction(ID_VIEW_TEXTUREMINE);
        menuView->addAction(ID_VIEW_WIREFRAMEMINE);
        menuView->addAction(ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW);
        menuView->addSeparator();
        menuView->addAction(ID_VIEW_CENTERONCUBE);
        menuView->addAction(ID_VIEW_CENTERONOBJECT);
        menuView->addAction(ID_VIEW_CENTERONMINE);
        menuView->addAction(ID_VIEW_RESETVIEWRADIUS);
        menuView->addSeparator();
        menuView->addAction(ID_VIEW_MOVECAMERATOSELECTEDROOM);
        menuView->addAction(ID_VIEW_MOVECAMERATOSELECTEDFACE);
        menuView->addAction(ID_VIEW_MOVECAMERATOCURRENTOBJECT);
        menuView->addAction(ID_VIEW_FLIP);
        menuView->addSeparator();
        menuView->addAction(ID_VIEW_SHOWVIEWERFORWARDVECTOR);
        menuView->addSeparator();
        menuView->addAction(ID_MINE_VIEW);
        menuView->addAction(ID_TERRAIN_VIEW);
        menuView->addAction(ID_ROOM_VIEW);
        menuView->addSeparator();
        menuView->addAction(ID_VIEW_NEXTVIEWER);
        menuView->addAction(ID_VIEW_NEWVIEWER);
        menuView->addAction(ID_VIEW_DELETEVIEWER);
        menuView->addSeparator();
        menuView->addAction(ID_VIEW_VIEWPROP);
        menuRoom->addAction(ID_ROOM_ADD);
        menuRoom->addSeparator();
        menuRoom->addAction(ID_ROOM_DELETE);
        menuRoom->addAction(ID_ROOM_DELETEFACE);
        menuRoom->addAction(ID_ROOM_DELETEPORTAL);
        menuRoom->addAction(ID_ROOM_DELETEVERT);
        menuRoom->addAction(ID_ROOM_DELETECONNECTEDFACES);
        menuRoom->addSeparator();
        menuRoom->addAction(ID_ROOM_PLACEROOM);
        menuRoom->addAction(ID_ROOM_PLACETERRAINROOM);
        menuRoom->addAction(ID_ROOM_ROTATEPLACEDROOM45DEGREES);
        menuRoom->addAction(ID_ROOM_SNAPPLACEDROOM);
        menuRoom->addAction(ID_ROOM_ATTACHROOM);
        menuRoom->addAction(ID_ROOM_UNPLACEROOM);
        menuRoom->addAction(ID_ROOM_DROPROOM);
        menuRoom->addSeparator();
        menuRoom->addAction(ID_ROOM_MARK);
        menuRoom->addAction(ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE);
        menuRoom->addSeparator();
        menuRoom->addAction(ID_ROOM_SELECTBYNUMBER);
        menuRoom->addSeparator();
        menuRoom->addAction(ID_ROOM_SAVECURRENTROOM);
        menuRoom->addAction(ID_ROOM_RENAMEROOM);
        menuRoom->addSeparator();
        menuRoom->addAction(ID_ROOM_BUILDBRIDGE);
        menuRoom->addAction(ID_ROOM_BUILDSMOOTHBRIDGE);
        menuRoom->addAction(ID_ROOM_JOINROOMS);
        menuRoom->addAction(ID_ROOM_JOINROOMSEXACT);
        menuRoom->addAction(ID_ROOM_JOIN_ADJACENT_FACES);
        menuRoom->addAction(ID_ROOM_COMBINE);
        menuRoom->addAction(ID_ROOM_LINKTONEWEXTERNAL);
        menuRoom->addAction(ID_ROOM_MERGEOBJECTINTOROOM);
        menuRoom->addSeparator();
        menuRoom->addAction(ID_ROOM_STARTNEWFACE);
        menuRoom->addAction(ID_ROOM_ADDVERTTONEWFACE);
        menuRoom->addAction(ID_ROOM_FINISHNEWFACE);
        menuRoom->addSeparator();
        menuRoom->addAction(ID_ROOM_SNAPPOINTTOEDGE);
        menuRoom->addAction(ID_ROOM_SNAPPOINTTOPOINT);
        menuRoom->addAction(ID_ROOM_SNAPPOINTTOFACE);
        menuRoom->addAction(ID_ROOM_UNDOSNAP);
        menuRoom->addSeparator();
        menuRoom->addAction(ID_ROOM_SETCENTERFROMVIEWER);
        menuRoom->addAction(ID_ROOM_CLEARCENTERPOINT);
        menuRoom->addSeparator();
        menuRoom->addAction(ID_ROOM_NEXTFACE);
        menuRoom->addAction(ID_ROOM_PREVIOUSFACE);
        menuRoom->addAction(ID_ROOM_SELECTFACEBYNUMBER);
        menuRoom->addAction(menuFaceEditing->menuAction());
        menuRoom->addSeparator();
        menuRoom->addAction(ID_ROOM_GRABTEXTURE);
        menuRoom->addAction(ID_ROOM_PROPAGATETOALL);
        menuRoom->addAction(ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES);
        menuFaceEditing->addAction(ID_ROOM_FACE_ADDVERTTOEDGE);
        menuFaceEditing->addAction(ID_ROOM_FACE_DELETEVERTONEDGE);
        menuFaceEditing->addAction(ID_ROOM_FACE_MOVEVERTONEDGE);
        menuFaceEditing->addAction(ID_ROOM_FACE_SPLITFACE);
        menuFaceEditing->addAction(ID_ROOM_SPLITFACE);
        menuObject->addAction(ID_OBJECT_PLACEOBJECT);
        menuObject->addAction(ID_OBJECT_DELETEOBJECT);
        menuObject->addAction(ID_OBJECT_MOVEPLAYER);
        menuObject->addAction(ID_OBJECT_PLACECAMERAATVIEWER);
        menuObject->addAction(ID_OBJECT_PLACECAMERAATCURRENTFACE);
        menuObject->addAction(ID_OBJECT_SETCAMERAFROMVIEWER);
        menuObject->addAction(ID_OBJECT_SETVIEWERFROMCAMERA);
        menuObject->addAction(ID_OBJECT_SELECTBYNUMBER);
        menuObject->addAction(ID_OBJECT_PLACESOUNDSOURCEATVIEWER);
        menuObject->addAction(ID_OBJECT_PLACEWAYPOINTATVIEWER);
        menuEditors->addAction(ID_TOOLS_WORLD_TEXTURES);
        menuEditors->addAction(ID_EDITORS_MEGACELLS);
        menuEditors->addAction(ID_TOOLS_WORLD_OBJECTS_ROBOTS);
        menuEditors->addAction(ID_TOOLS_WORLD_OBJECTS_POWERUPS);
        menuEditors->addAction(ID_TOOLS_WORLD_OBJECTS_BUILDINGS);
        menuEditors->addAction(ID_TOOLS_WORLD_OBJECTS_CLUTTER);
        menuEditors->addAction(ID_TOOLS_WORLD_OBJECTS_PLAYER);
        menuEditors->addAction(ID_TOOLS_WORLD_WEAPONS);
        menuEditors->addAction(ID_TOOLS_WORLD_OBJECTS_DOOR);
        menuEditors->addAction(ID_TOOLS_WORLD_OBJECTS_SOUND);
        menuEditors->addAction(ID_TOOLS_WORLD_OBJECTS_LIGHTS);
        menuEditors->addAction(ID_EDITORS_AMBIENTSOUNDS);
        menuEditors->addAction(ID_SCRIPT_LEVEL_INTERFACE);
        menuEditors->addAction(ID_EDITORS_FILES);
        menuEditors->addAction(ID_EDITORS_AIPROPERTIES);
        menuEditors->addSeparator();
        menuEditors->addAction(IDD_REORDER_PAGES);
        menuEditors->addAction(IDD_SHOW_ALL_CHECKED_OUT);
        menuEditors->addAction(IDD_ORPHANHUNTER);
        menuEditors->addSeparator();
        menuEditors->addAction(IDM_IMPORT_BITMAP);
        menuEditors->addAction(ID_HOTSPOT_TGA);
        menuEditors->addSeparator();
        menuEditors->addAction(ID_BRIEFING_EDITOR);
        menuEditors->addAction(ID_SUBEDITORS_FONT);
        menuEditors->addAction(ID_SUBEDITORS_HOGMAKER);
        menuEditors->addAction(ID_SUBEDITORS_TABLEFILEEDIT);
        menuEditors->addAction(ID_SUBEDITORS_TABLEFILEFILTER);
        menuEditors->addAction(ID_EDITORS_DALLAS);
        menuWindow->addAction(ID_WINDOW_TILE);
        menuWindow->addAction(ID_WINDOW_CASCADE);
        menuHelp->addAction(ID_D3HELP);
        menuHelp->addAction(ID_APP_ABOUT);
        maintoolbar->addAction(ID_FILE_NEW);
        maintoolbar->addAction(ID_FILE_OPEN);
        maintoolbar->addAction(ID_FILE_SAVE);
        maintoolbar->addSeparator();
        maintoolbar->addAction(ID_EDIT_CUT);
        maintoolbar->addAction(ID_EDIT_COPY);
        maintoolbar->addAction(ID_EDIT_PLACE);
        maintoolbar->addSeparator();
        maintoolbar->addAction(ID_APP_ABOUT);
        maintoolbar->addSeparator();
        maintoolbar->addAction(ID_FILE_PLAY640X480);
        maintoolbar->addSeparator();
        maintoolbar->addAction(ID_BUTTON_OUTLINE);
        maintoolbar->addAction(ID_BUTTON_WINDOWSELECTION);
        maintoolbar->addAction(ID_BUTTON_LIGHTING);
        maintoolbar->addAction(ID_ZBUTTON);
        maintoolbar->addSeparator();
        maintoolbar->addAction(ID_OBJBUTTON);
        maintoolbar->addAction(ID_REINIT_OPENGL);
        maintoolbar->addAction(ID_OSIRISCOMPILE);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        IDD_ORPHANHUNTER->setText(QCoreApplication::translate("MainWindow", "Orphan Hunter", nullptr));
        IDD_REORDER_PAGES->setText(QCoreApplication::translate("MainWindow", "Reorder Net Pages", nullptr));
        IDD_SHOW_ALL_CHECKED_OUT->setText(QCoreApplication::translate("MainWindow", "Show All Pages Checked Out", nullptr));
        IDM_IMPORT_BITMAP->setText(QCoreApplication::translate("MainWindow", "Bitmap/Animation Converter...", nullptr));
        ID_APP_ABOUT->setText(QCoreApplication::translate("MainWindow", "&About editor...", nullptr));
        ID_APP_EXIT->setText(QCoreApplication::translate("MainWindow", "E&xit", nullptr));
#if QT_CONFIG(shortcut)
        ID_APP_EXIT->setShortcut(QCoreApplication::translate("MainWindow", "Alt+X", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_BRIEFING_EDITOR->setText(QCoreApplication::translate("MainWindow", "Briefing Editor...", nullptr));
        ID_BUTTON_LIGHTING->setText(QCoreApplication::translate("MainWindow", "Toggle Lighting", nullptr));
#if QT_CONFIG(tooltip)
        ID_BUTTON_LIGHTING->setToolTip(QCoreApplication::translate("MainWindow", "Toggle lighting", nullptr));
#endif // QT_CONFIG(tooltip)
        ID_BUTTON_OUTLINE->setText(QCoreApplication::translate("MainWindow", "Toggle Outline", nullptr));
#if QT_CONFIG(tooltip)
        ID_BUTTON_OUTLINE->setToolTip(QCoreApplication::translate("MainWindow", "Toggle Outline mode in the Texture-mapped view", nullptr));
#endif // QT_CONFIG(tooltip)
        ID_BUTTON_WINDOWSELECTION->setText(QCoreApplication::translate("MainWindow", "Toggle Box Selection", nullptr));
#if QT_CONFIG(tooltip)
        ID_BUTTON_WINDOWSELECTION->setToolTip(QCoreApplication::translate("MainWindow", "Toggle box selection between IN WINDOW and ACROSS EDGE", nullptr));
#endif // QT_CONFIG(tooltip)
        ID_D3HELP->setText(QCoreApplication::translate("MainWindow", "Help on D3 Editor", nullptr));
        ID_EDITORS_AIPROPERTIES->setText(QCoreApplication::translate("MainWindow", "AI Properties...", nullptr));
        ID_EDITORS_AMBIENTSOUNDS->setText(QCoreApplication::translate("MainWindow", "Ambient Sounds...", nullptr));
        ID_EDITORS_DALLAS->setText(QCoreApplication::translate("MainWindow", "Dallas Graphical Script Editor...", nullptr));
        ID_EDITORS_FILES->setText(QCoreApplication::translate("MainWindow", "Files...", nullptr));
        ID_EDITORS_MEGACELLS->setText(QCoreApplication::translate("MainWindow", "Megacells...", nullptr));
        ID_EDIT_ADDSELECT->setText(QCoreApplication::translate("MainWindow", "Add Current Room to Selected List", nullptr));
        ID_EDIT_ATTACH->setText(QCoreApplication::translate("MainWindow", "&Attach", nullptr));
        ID_EDIT_CLEARSELECTED->setText(QCoreApplication::translate("MainWindow", "Clear Selected List", nullptr));
        ID_EDIT_COPY->setText(QCoreApplication::translate("MainWindow", "&Copy", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_COPY->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+Ins", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_EDIT_CUT->setText(QCoreApplication::translate("MainWindow", "Cu&t", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_CUT->setShortcut(QCoreApplication::translate("MainWindow", "Shift+Del", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_EDIT_DELETE->setText(QCoreApplication::translate("MainWindow", "&Delete", nullptr));
        ID_EDIT_LOADSCRAP->setText(QCoreApplication::translate("MainWindow", "Load Scrap from Disk", nullptr));
        ID_EDIT_PASTE->setText(QCoreApplication::translate("MainWindow", "&Paste", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_PASTE->setShortcut(QCoreApplication::translate("MainWindow", "Shift+Ins", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_EDIT_PLACE->setText(QCoreApplication::translate("MainWindow", "&Place in Mine", nullptr));
        ID_EDIT_PLACE_TERRAIN->setText(QCoreApplication::translate("MainWindow", "&Place on Terrain", nullptr));
        ID_EDIT_REMOVESELECT->setText(QCoreApplication::translate("MainWindow", "Remove Current Room from Selected List", nullptr));
        ID_EDIT_SAVESCRAP->setText(QCoreApplication::translate("MainWindow", "Save Scrap to Disk", nullptr));
        ID_EDIT_SELECTATTACHED->setText(QCoreApplication::translate("MainWindow", "Select All Rooms Attached to Current Room", nullptr));
        ID_EDIT_UNDO->setText(QCoreApplication::translate("MainWindow", "&Undo", nullptr));
#if QT_CONFIG(shortcut)
        ID_EDIT_UNDO->setShortcut(QCoreApplication::translate("MainWindow", "Alt+Backspace", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_AUTOSAVE->setText(QCoreApplication::translate("MainWindow", "Autosave", nullptr));
        ID_FILE_CHECKMINE->setText(QCoreApplication::translate("MainWindow", "&Check Mine", nullptr));
        ID_FILE_FIXCRACKS->setText(QCoreApplication::translate("MainWindow", "Fix Level Cracks", nullptr));
        ID_FILE_FIXDEGENERATEFACES->setText(QCoreApplication::translate("MainWindow", "Fix/Remove Degenerate Faces", nullptr));
        ID_FILE_IMPORT_ROOM->setText(QCoreApplication::translate("MainWindow", "Import 3DSMax Room", nullptr));
        ID_FILE_INFORMATION->setText(QCoreApplication::translate("MainWindow", "&Information", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_INFORMATION->setShortcut(QCoreApplication::translate("MainWindow", "Alt+I", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_LEAVE_EDITOR->setText(QCoreApplication::translate("MainWindow", "Go to main menu...", nullptr));
        ID_FILE_LEVELPROPS->setText(QCoreApplication::translate("MainWindow", "Set Selected Room(s) Properties", nullptr));
        ID_FILE_LEVEL_INFO->setText(QCoreApplication::translate("MainWindow", "Edit Level Info...", nullptr));
        ID_FILE_MRU_FILE1->setText(QCoreApplication::translate("MainWindow", "Recent File", nullptr));
        ID_FILE_NEW->setText(QCoreApplication::translate("MainWindow", "&New", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_NEW->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+N", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_OPEN->setText(QCoreApplication::translate("MainWindow", "&Open...", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_OPEN->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+O", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_PLAY640X480->setText(QCoreApplication::translate("MainWindow", "&Play in 640 x 480", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_PLAY640X480->setShortcut(QCoreApplication::translate("MainWindow", "Alt+P", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_PREFERENCES->setText(QCoreApplication::translate("MainWindow", "Preferences", nullptr));
        ID_FILE_PRINT->setText(QCoreApplication::translate("MainWindow", "&Print...", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_PRINT->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+P", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_PRINT_PREVIEW->setText(QCoreApplication::translate("MainWindow", "Print Pre&view", nullptr));
        ID_FILE_PRINT_SETUP->setText(QCoreApplication::translate("MainWindow", "P&rint Setup...", nullptr));
        ID_FILE_REMOVEDUPLICATEFACESFROMCURRENTROOM->setText(QCoreApplication::translate("MainWindow", "Remove Duplicate Faces from Current Room", nullptr));
        ID_FILE_REMOVEEXTRAPOINTS->setText(QCoreApplication::translate("MainWindow", "Remove Extra Points", nullptr));
        ID_FILE_RESTOREGAMESTATE->setText(QCoreApplication::translate("MainWindow", "Restore Gamestate", nullptr));
        ID_FILE_SAVE->setText(QCoreApplication::translate("MainWindow", "&Save", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_SAVE->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_SAVEGOALTEXT->setText(QCoreApplication::translate("MainWindow", "Save Goal Text to File", nullptr));
        ID_FILE_SAVE_AS->setText(QCoreApplication::translate("MainWindow", "Save &As...", nullptr));
        ID_FILE_SAVE_COPY_AS->setText(QCoreApplication::translate("MainWindow", "Save Copy &As...", nullptr));
        ID_FILE_STATS->setText(QCoreApplication::translate("MainWindow", "Show Level Stats", nullptr));
        ID_FILE_UPDATE->setText(QCoreApplication::translate("MainWindow", "&Update", nullptr));
#if QT_CONFIG(shortcut)
        ID_FILE_UPDATE->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+S", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_FILE_VERIFY_LEVEL->setText(QCoreApplication::translate("MainWindow", "Verify Level", nullptr));
        ID_HOTSPOT_TGA->setText(QCoreApplication::translate("MainWindow", "HotSpot TGA Extractor...", nullptr));
        ID_MINE_VIEW->setText(QCoreApplication::translate("MainWindow", "Mine View", nullptr));
#if QT_CONFIG(shortcut)
        ID_MINE_VIEW->setShortcut(QCoreApplication::translate("MainWindow", "F2", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_OBJBUTTON->setText(QCoreApplication::translate("MainWindow", "Object List", nullptr));
#if QT_CONFIG(tooltip)
        ID_OBJBUTTON->setToolTip(QCoreApplication::translate("MainWindow", "Display object list", nullptr));
#endif // QT_CONFIG(tooltip)
        ID_OBJECT_DELETEOBJECT->setText(QCoreApplication::translate("MainWindow", "Delete Object", nullptr));
#if QT_CONFIG(shortcut)
        ID_OBJECT_DELETEOBJECT->setShortcut(QCoreApplication::translate("MainWindow", "Del", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_OBJECT_MOVEPLAYER->setText(QCoreApplication::translate("MainWindow", "Move Player 0 to Viewer", nullptr));
        ID_OBJECT_PLACECAMERAATCURRENTFACE->setText(QCoreApplication::translate("MainWindow", "Create New Camera at Current Face", nullptr));
        ID_OBJECT_PLACECAMERAATVIEWER->setText(QCoreApplication::translate("MainWindow", "Create New Camera at Viewer", nullptr));
        ID_OBJECT_PLACEOBJECT->setText(QCoreApplication::translate("MainWindow", "Place Object", nullptr));
#if QT_CONFIG(shortcut)
        ID_OBJECT_PLACEOBJECT->setShortcut(QCoreApplication::translate("MainWindow", "Ins", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_OBJECT_PLACESOUNDSOURCEATVIEWER->setText(QCoreApplication::translate("MainWindow", "Create SoundSource at Viewer", nullptr));
        ID_OBJECT_PLACEWAYPOINTATVIEWER->setText(QCoreApplication::translate("MainWindow", "Place Waypoint at Viewer", nullptr));
        ID_OBJECT_SELECTBYNUMBER->setText(QCoreApplication::translate("MainWindow", "Select Object By Number", nullptr));
        ID_OBJECT_SETCAMERAFROMVIEWER->setText(QCoreApplication::translate("MainWindow", "Move Camera to Viewer", nullptr));
        ID_OBJECT_SETVIEWERFROMCAMERA->setText(QCoreApplication::translate("MainWindow", "Move Viewer to Camera", nullptr));
        ID_OSIRISCOMPILE->setText(QCoreApplication::translate("MainWindow", "Compile Scripts", nullptr));
#if QT_CONFIG(tooltip)
        ID_OSIRISCOMPILE->setToolTip(QCoreApplication::translate("MainWindow", "Compile level scripts", nullptr));
#endif // QT_CONFIG(tooltip)
        ID_REINIT_OPENGL->setText(QCoreApplication::translate("MainWindow", "Reinit OpenGL", nullptr));
#if QT_CONFIG(tooltip)
        ID_REINIT_OPENGL->setToolTip(QCoreApplication::translate("MainWindow", "Reinitialize OpenGL renderer", nullptr));
#endif // QT_CONFIG(tooltip)
        ID_ROOM_ADD->setText(QCoreApplication::translate("MainWindow", "&Add Room at Current Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_ADD->setShortcut(QCoreApplication::translate("MainWindow", "Shift+A", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_ADDVERTTONEWFACE->setText(QCoreApplication::translate("MainWindow", "Add Vert to New Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_ADDVERTTONEWFACE->setShortcut(QCoreApplication::translate("MainWindow", "Shift+V", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_ATTACHROOM->setText(QCoreApplication::translate("MainWindow", "Attach Room", nullptr));
        ID_ROOM_BUILDBRIDGE->setText(QCoreApplication::translate("MainWindow", "&Build Bridge", nullptr));
        ID_ROOM_BUILDSMOOTHBRIDGE->setText(QCoreApplication::translate("MainWindow", "Build Smooth Bridge", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_BUILDSMOOTHBRIDGE->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+B", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_CLEARCENTERPOINT->setText(QCoreApplication::translate("MainWindow", "Clear Center Point", nullptr));
        ID_ROOM_COMBINE->setText(QCoreApplication::translate("MainWindow", "Combine Rooms", nullptr));
        ID_ROOM_DELETE->setText(QCoreApplication::translate("MainWindow", "Delete Current Room", nullptr));
        ID_ROOM_DELETECONNECTEDFACES->setText(QCoreApplication::translate("MainWindow", "Delete Connected Faces", nullptr));
        ID_ROOM_DELETEFACE->setText(QCoreApplication::translate("MainWindow", "Delete Current Face", nullptr));
        ID_ROOM_DELETEPORTAL->setText(QCoreApplication::translate("MainWindow", "Delete Current Portal", nullptr));
        ID_ROOM_DELETEVERT->setText(QCoreApplication::translate("MainWindow", "Delete Current Vert From Face", nullptr));
        ID_ROOM_DROPROOM->setText(QCoreApplication::translate("MainWindow", "Drop Room", nullptr));
        ID_ROOM_FACE_ADDVERTTOEDGE->setText(QCoreApplication::translate("MainWindow", "Add Vert To Edge", nullptr));
        ID_ROOM_FACE_DELETEVERTONEDGE->setText(QCoreApplication::translate("MainWindow", "Delete Vert on Edge", nullptr));
        ID_ROOM_FACE_MOVEVERTONEDGE->setText(QCoreApplication::translate("MainWindow", "Move Vert on Edge", nullptr));
        ID_ROOM_FACE_SPLITFACE->setText(QCoreApplication::translate("MainWindow", "Split Face", nullptr));
        ID_ROOM_FINISHNEWFACE->setText(QCoreApplication::translate("MainWindow", "Add Last Point to New Face (Finish Face)", nullptr));
        ID_ROOM_GRABTEXTURE->setText(QCoreApplication::translate("MainWindow", "Grab Texture", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_GRABTEXTURE->setShortcut(QCoreApplication::translate("MainWindow", "G", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_JOINROOMS->setText(QCoreApplication::translate("MainWindow", "Join Rooms", nullptr));
        ID_ROOM_JOINROOMSEXACT->setText(QCoreApplication::translate("MainWindow", "Join Rooms Exact", nullptr));
        ID_ROOM_JOIN_ADJACENT_FACES->setText(QCoreApplication::translate("MainWindow", "Join Adjacent Faces", nullptr));
        ID_ROOM_LINKTONEWEXTERNAL->setText(QCoreApplication::translate("MainWindow", "Link Room to New External Room", nullptr));
        ID_ROOM_MARK->setText(QCoreApplication::translate("MainWindow", "&Mark Room/Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_MARK->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+M", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_MERGEOBJECTINTOROOM->setText(QCoreApplication::translate("MainWindow", "Merge Object into Room", nullptr));
        ID_ROOM_NEXTFACE->setText(QCoreApplication::translate("MainWindow", "Next Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_NEXTFACE->setShortcut(QCoreApplication::translate("MainWindow", "F", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_PLACEROOM->setText(QCoreApplication::translate("MainWindow", "Place Room at Current Room", nullptr));
        ID_ROOM_PLACETERRAINROOM->setText(QCoreApplication::translate("MainWindow", "Place Room at Current Terrain Cell", nullptr));
        ID_ROOM_PREVIOUSFACE->setText(QCoreApplication::translate("MainWindow", "Previous Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_PREVIOUSFACE->setShortcut(QCoreApplication::translate("MainWindow", "Shift+F", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES->setText(QCoreApplication::translate("MainWindow", "Propagate to Adjacent Coplanar Faces", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOADJACENTCOPLANARFACES->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+F", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOALL->setText(QCoreApplication::translate("MainWindow", "Propagate to All Faces in Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_PROPAGATETOALL->setShortcut(QCoreApplication::translate("MainWindow", "Shift+R", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_RENAMEROOM->setText(QCoreApplication::translate("MainWindow", "Rename Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_RENAMEROOM->setShortcut(QCoreApplication::translate("MainWindow", "Ctrl+R", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_ROOM_ROTATEPLACEDROOM45DEGREES->setText(QCoreApplication::translate("MainWindow", "Rotate Placed Room 45 Degrees", nullptr));
        ID_ROOM_SAVECURRENTROOM->setText(QCoreApplication::translate("MainWindow", "Save Current Room", nullptr));
        ID_ROOM_SELECTBYNUMBER->setText(QCoreApplication::translate("MainWindow", "Select Room by Number", nullptr));
        ID_ROOM_SELECTFACEBYNUMBER->setText(QCoreApplication::translate("MainWindow", "Select Face by Number", nullptr));
        ID_ROOM_SETCENTERFROMVIEWER->setText(QCoreApplication::translate("MainWindow", "Set Room Center from Viewer", nullptr));
        ID_ROOM_SNAPPLACEDROOM->setText(QCoreApplication::translate("MainWindow", "Snap Placed Room", nullptr));
        ID_ROOM_SNAPPOINTTOEDGE->setText(QCoreApplication::translate("MainWindow", "Snap Point to Edge", nullptr));
        ID_ROOM_SNAPPOINTTOFACE->setText(QCoreApplication::translate("MainWindow", "Snap Point to Face", nullptr));
        ID_ROOM_SNAPPOINTTOPOINT->setText(QCoreApplication::translate("MainWindow", "Snap Point to Point", nullptr));
        ID_ROOM_SPLITFACE->setText(QCoreApplication::translate("MainWindow", "Triangulate face", nullptr));
        ID_ROOM_STARTNEWFACE->setText(QCoreApplication::translate("MainWindow", "Add First Point to New Face (Start Face)", nullptr));
        ID_ROOM_SWAPMAKEDANDCURRENTROOMFACE->setText(QCoreApplication::translate("MainWindow", "Swap Marked and Current Room/Face", nullptr));
        ID_ROOM_UNDOSNAP->setText(QCoreApplication::translate("MainWindow", "Undo Snap", nullptr));
        ID_ROOM_UNPLACEROOM->setText(QCoreApplication::translate("MainWindow", "Un-place Room/Group/Door", nullptr));
        ID_ROOM_VIEW->setText(QCoreApplication::translate("MainWindow", "Room View", nullptr));
#if QT_CONFIG(shortcut)
        ID_ROOM_VIEW->setShortcut(QCoreApplication::translate("MainWindow", "F4", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_SCRIPT_LEVEL_INTERFACE->setText(QCoreApplication::translate("MainWindow", "Levels and Scripts...", nullptr));
        ID_SUBEDITORS_FONT->setText(QCoreApplication::translate("MainWindow", "Font Editor...", nullptr));
        ID_SUBEDITORS_HOGMAKER->setText(QCoreApplication::translate("MainWindow", "Hog Maker...", nullptr));
        ID_SUBEDITORS_TABLEFILEEDIT->setText(QCoreApplication::translate("MainWindow", "Table File Text Editor...", nullptr));
        ID_SUBEDITORS_TABLEFILEFILTER->setText(QCoreApplication::translate("MainWindow", "Table File Filter...", nullptr));
        ID_TABLEEDIT_FILE_EXIT->setText(QCoreApplication::translate("MainWindow", "&Exit", nullptr));
        ID_TABLEEDIT_FILE_LOAD->setText(QCoreApplication::translate("MainWindow", "&Load...", nullptr));
        ID_TABLEEDIT_FILE_SAVE->setText(QCoreApplication::translate("MainWindow", "&Save", nullptr));
        ID_TABLEEDIT_FILE_SAVE_AS->setText(QCoreApplication::translate("MainWindow", "S&ave as...", nullptr));
        ID_TABLEFILTER_FILE_EXIT->setText(QCoreApplication::translate("MainWindow", "Exit", nullptr));
        ID_TABLEFILTER_FILE_LOAD->setText(QCoreApplication::translate("MainWindow", "Load...", nullptr));
        ID_TABLEFILTER_FILE_NEW->setText(QCoreApplication::translate("MainWindow", "New", nullptr));
        ID_TABLEFILTER_FILE_SAVE->setText(QCoreApplication::translate("MainWindow", "Save", nullptr));
        ID_TABLEFILTER_FILE_SAVEAS->setText(QCoreApplication::translate("MainWindow", "Save as...", nullptr));
        ID_TERRAIN_SELECT_RANGE->setText(QCoreApplication::translate("MainWindow", "Select Range...", nullptr));
        ID_TERRAIN_SOUNDS->setText(QCoreApplication::translate("MainWindow", "Sounds...", nullptr));
        ID_TERRAIN_VIEW->setText(QCoreApplication::translate("MainWindow", "Terrain View", nullptr));
#if QT_CONFIG(shortcut)
        ID_TERRAIN_VIEW->setShortcut(QCoreApplication::translate("MainWindow", "F3", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_TOOLS_WORLD_OBJECTS_BUILDINGS->setText(QCoreApplication::translate("MainWindow", "Buildings...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_CLUTTER->setText(QCoreApplication::translate("MainWindow", "Clutter...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_DOOR->setText(QCoreApplication::translate("MainWindow", "Doors...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_LIGHTS->setText(QCoreApplication::translate("MainWindow", "Lights...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_PLAYER->setText(QCoreApplication::translate("MainWindow", "Player Ships...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_POWERUPS->setText(QCoreApplication::translate("MainWindow", "Powerups...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_ROBOTS->setText(QCoreApplication::translate("MainWindow", "Robots...", nullptr));
        ID_TOOLS_WORLD_OBJECTS_SOUND->setText(QCoreApplication::translate("MainWindow", "Sounds...", nullptr));
        ID_TOOLS_WORLD_TEXTURES->setText(QCoreApplication::translate("MainWindow", "Textures...", nullptr));
        ID_TOOLS_WORLD_WEAPONS->setText(QCoreApplication::translate("MainWindow", "Weapons...", nullptr));
        ID_VIEW_CENTERONCUBE->setText(QCoreApplication::translate("MainWindow", "Center on Current &Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_CENTERONCUBE->setShortcut(QCoreApplication::translate("MainWindow", "Home", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_CENTERONMINE->setText(QCoreApplication::translate("MainWindow", "Center on &Mine Origin", nullptr));
        ID_VIEW_CENTERONOBJECT->setText(QCoreApplication::translate("MainWindow", "Center on Current &Object", nullptr));
        ID_VIEW_DELETEVIEWER->setText(QCoreApplication::translate("MainWindow", "Delete Viewer", nullptr));
        ID_VIEW_FLIP->setText(QCoreApplication::translate("MainWindow", "Flip Viewer", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_FLIP->setShortcut(QCoreApplication::translate("MainWindow", "/", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_KEYPAD_TOGGLE->setText(QCoreApplication::translate("MainWindow", "Keypad", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_KEYPAD_TOGGLE->setShortcut(QCoreApplication::translate("MainWindow", "F10", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOCURRENTOBJECT->setText(QCoreApplication::translate("MainWindow", "Move camera to Current Object", nullptr));
        ID_VIEW_MOVECAMERATOSELECTEDFACE->setText(QCoreApplication::translate("MainWindow", "Move camera to &Current Face", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOSELECTEDFACE->setShortcut(QCoreApplication::translate("MainWindow", "Shift+C", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOSELECTEDROOM->setText(QCoreApplication::translate("MainWindow", "Move camera to &Current Room", nullptr));
#if QT_CONFIG(shortcut)
        ID_VIEW_MOVECAMERATOSELECTEDROOM->setShortcut(QCoreApplication::translate("MainWindow", "C", nullptr));
#endif // QT_CONFIG(shortcut)
        ID_VIEW_NEWVIEWER->setText(QCoreApplication::translate("MainWindow", "New Viewer", nullptr));
        ID_VIEW_NEXTVIEWER->setText(QCoreApplication::translate("MainWindow", "NextViewer", nullptr));
        ID_VIEW_RESETVIEWRADIUS->setText(QCoreApplication::translate("MainWindow", "Reset View Radius", nullptr));
        ID_VIEW_SHOWOBJECTSINWIREFRAMEVIEW->setText(QCoreApplication::translate("MainWindow", "Show objects in Wireframe view", nullptr));
        ID_VIEW_SHOWVIEWERFORWARDVECTOR->setText(QCoreApplication::translate("MainWindow", "Show Viewer Forward Vector", nullptr));
        ID_VIEW_STATUS_BAR->setText(QCoreApplication::translate("MainWindow", "&Status Bar", nullptr));
        ID_VIEW_TEXTUREMINE->setText(QCoreApplication::translate("MainWindow", "Te&xtured view", nullptr));
        ID_VIEW_TOOLBAR->setText(QCoreApplication::translate("MainWindow", "&Toolbar", nullptr));
        ID_VIEW_VIEWPROP->setText(QCoreApplication::translate("MainWindow", "Display Viewer Properties", nullptr));
        ID_VIEW_WIREFRAMEMINE->setText(QCoreApplication::translate("MainWindow", "&Wireframe view", nullptr));
        ID_WINDOW_CASCADE->setText(QCoreApplication::translate("MainWindow", "&Cascade", nullptr));
        ID_WINDOW_TILE->setText(QCoreApplication::translate("MainWindow", "&Tile", nullptr));
        ID_ZBUTTON->setText(QCoreApplication::translate("MainWindow", "Toggle Z-Buffer", nullptr));
#if QT_CONFIG(tooltip)
        ID_ZBUTTON->setToolTip(QCoreApplication::translate("MainWindow", "Toggle software Z-Buffer mode", nullptr));
#endif // QT_CONFIG(tooltip)
        menuFile->setTitle(QCoreApplication::translate("MainWindow", "&File", nullptr));
        menuEdit->setTitle(QCoreApplication::translate("MainWindow", "&Edit", nullptr));
        menuView->setTitle(QCoreApplication::translate("MainWindow", "&View", nullptr));
        menuRoom->setTitle(QCoreApplication::translate("MainWindow", "&Room", nullptr));
        menuFaceEditing->setTitle(QCoreApplication::translate("MainWindow", "Face Editing", nullptr));
        menuObject->setTitle(QCoreApplication::translate("MainWindow", "&Object", nullptr));
        menuEditors->setTitle(QCoreApplication::translate("MainWindow", "Edi&tors", nullptr));
        menuWindow->setTitle(QCoreApplication::translate("MainWindow", "&Window", nullptr));
        menuHelp->setTitle(QCoreApplication::translate("MainWindow", "&Help", nullptr));
        maintoolbar->setWindowTitle(QCoreApplication::translate("MainWindow", "Toolbar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
