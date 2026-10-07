#pragma once


#include <optional>
#include <filesystem>

#include "utils.h"
#include "vecmat.h"
#include "terrain.h"
#include "object_external.h" // object_type

// Define group & room structs so we don't have to include group.h & room.h
struct group;
struct room_t;

const int TEXSCREEN_WIDTH = 512, // Texture screen base width and height
    TEXSCREEN_HEIGHT = 384,
          WIRESCREEN_WIDTH = 640, // Wirefrane screen base width and height
    WIRESCREEN_HEIGHT = 480;

const int EDITOR_RESOLUTION_X = 1024, // Used to try to make editor resolution independent
    EDITOR_RESOLUTION_Y = 768;

int CALC_PIXELS_WITH_ASPECTX(int pixels);
int CALC_PIXELS_WITH_ASPECTY(int pixels);

//	keypad constants
enum {
  TAB_TEXTURE_KEYPAD,
  TAB_MEGACELL_KEYPAD,
  TAB_TERRAIN_KEYPAD,
  TAB_OBJECTS_KEYPAD,
  TAB_ROOM_KEYPAD,
  TAB_DOORWAY_KEYPAD,
  TAB_TRIGGER_KEYPAD,
  TAB_LIGHTING_KEYPAD,
  TAB_PATHS_KEYPAD,
  TAB_LEVEL_KEYPAD,
  TAB_MATCEN_KEYPAD
};

// Constants for d3edit_state variables
enum { IN_WINDOW, ACROSS_EDGE };                          // values for box_selection_mode
enum { REL_OBJECT, REL_VIEWER };                          // values for object_move_mode
enum { GM_WINDOWED, GM_FULLSCREEN_SW, GM_FULLSCREEN_HW }; // values for game_render_mode

class grSurface;
class grViewport;

namespace state
{
  // Which mode we're currently in
  enum class viewer : uint32_t
  {
    mine = 0,
    terrain,
    room,
    invalid
  };

}
// A face within a specific room, along with the sub-selection made within that
// face. This mirrors the Win32 editor's Curroomp/Curface/Curedge/Curvert/
// Curportal block (editor/EDVARS.cpp:130), which is always set as a unit: the
// edge and vert are sub-indices of the face, and the portal is a property of
// the face rather than a peer of the room.
//
// The indices are index_t (nullopt maps to the legacy -1 sentinel used by the
// int-keyed engine APIs: ScaleFaceUVs, HTextureSlide, OutlineCurrentFace,
// ComputeCenterPointOnFace, ...). All reads that cross into int-land go
// through index_to_int() so a missed conversion cannot silently become
// UINT32_MAX instead of a guarded -1.
struct face_selection
{
  index_t room;   // index into Rooms[]
  index_t face;   // face index within that room
  index_t edge;   // edge index within that face
  index_t vert;   // vert index within that face
  index_t portal; // portal_num on that face

  void reset()
  {
    room.reset();
    face.reset();
    edge.reset();
    vert.reset();
    portal.reset();
  }
};

// The room currently being positioned for attachment, together with the target
// room:face it will be attached to. Mirrors the Win32 Placed_* block
// (editor/EDVARS.cpp:138-148), which is written as a unit by PlaceRoom() and
// consumed as a unit by AttachRoom().
//
// The base pair is itself a room:face selection, so it reuses face_selection
// (of which only room and face are meaningful here). The origin/attachpoint/
// orient/rotmat values are all derived by ComputePlacedRoomMatrix() from
// placed.base and placed.orient.
struct placed_room_state
{
  // Room being placed, and the face on it that will sit against the mine.
  index_t room;
  index_t room_face;

  // Target room:face in the mine being attached onto.
  face_selection base;

  std::optional<uint32_t> door;
  group *grp = nullptr;

  float angle = 0;
  vector3 origin = {0, 0, 0};
  matrix orient = IDENTITY_MATRIX;
  vector3 attachpoint = {0, 0, 0};
  matrix rotmat = IDENTITY_MATRIX;
};

// Structure to store various editor state & preference values
struct d3edit_state
{
  // Values for current item in the various dialogs
  index_t texdlg_texture; // current texture in texdialog
  index_t current_door; // current door in door page dialog
  index_t current_ship; // current ship in ship page dialog
  index_t current_sound; // current sound in sound page dialog
  index_t current_weapon; // current weapon in weapon page dialog
  index_t current_path; // currently selected path for a robot to follow
  std::optional<uint16_t> current_node; // currently selected node of preceding path
  index_t current_megacell; // currently selected megacell
  index_t current_room; // currently selected room
  index_t current_gamefile; // currently selected gamefile

  // Object keypad: which object page is selected. This selects which of the
  // per-type current_* indices below is authoritative, mirroring the
  // SetCurrentIndex()/GetCurrentIndex() dispatch in editor/ObjectDialog.cpp:350.
  // There is deliberately no single "current object id" field — Win32 keeps one
  // index per page and dispatches on the type, and it self-heals when the
  // remembered index turns out to belong to a different page.
  std::optional<object_type> obj_page;

  // Current object-info id within each object keypad page. Only the one
  // selected by obj_page is meaningful for placement; the others are retained
  // per page so switching back and forth restores the previous selection.
  // (See objectPageCurrentId()/setObjectPageCurrentId() in object_ops.cpp.)
  std::optional<uint16_t> current_robot; // current robot in robot page dialog
  std::optional<uint16_t> current_powerup; // current powerup id
  std::optional<uint16_t> current_building; // currently selected building
  std::optional<uint16_t> current_clutter; // currently selected clutter

  //	Values for the different editor windows
  bool texscr_visible = false;                        // is texture mine view up?
  int texscr_x,
      texscr_y,
      texscr_w,
      texscr_h; // dims of floating texture mine view

  bool wirescr_visible = false;                           // is wireframe model up?
  int wirescr_x,
      wirescr_y,
      wirescr_w,
      wirescr_h; // dims of floating wireframe model

  bool keypad_visible = false;                                                // is keypad visible?
  std::optional<int> keypad_current;                                                 // which keypad tab are we on?
  bool float_keypad_moved = false;                                            // has floating keypad moved?
  int float_keypad_x,
      float_keypad_y,
      float_keypad_w,
      float_keypad_h; // floating keypad width and height, x, y

  int objmodeless_x,
      objmodeless_y;                                   // object modeless list x and y.
  bool objmodeless_on = false;                                                // is modeless on?

  bool tile_views = false; // tile or floating view windows, keypad

  // Values for terrain renderer
  bool terrain_dots = false;       // show terrain dots?
  bool terrain_flat_shade = false; // flat shade terrain?

  // Misc preferences
  std::optional<int> game_render_mode;        // what mode to we play the game in?  See constants above.
  bool randomize_megacell = false;     // randomize when placing a megacell?
  std::optional<int> box_selection_mode;      // How editor box selection works.  See constants above.
  std::optional<int> object_move_mode;        // How object movements works.  See constants above.
  std::optional<int> object_move_axis;        // This is the axis on which objects move with mouse.
  bool fullscreen_debug_state = false; // do we allow for fullscreen debugging?
  bool hemicube_radiosity = false;
  float node_movement_inc = 0.0;
  int texture_display_flags = 0; // which textures to display on the texture tab
  float texscale            = 0.0;            // the scalar for moving texture UVs
  bool joy_slewing = false;          // shall we allow joystick slewing?
  bool objects_in_wireframe = false; // should we draw objects in the wireframe view?


  state::viewer view_mode = state::viewer::mine;

  // flags for the textured views changed
  bool TV_changed = false;

  // Set this flag if a new world is loaded/created
  bool New_mine = false;

  // Set this when the mine has changed
  bool World_changed = false;

  // Set this when the editor state (but not the world itself) has changed
  bool State_changed = false;

  // Set this when the viewer (i.e., player) has moved
  bool Viewer_moved = false;

  // Set this when an object has moved
  bool Object_moved = false;

  // Flag for if mine has changed (& thus needs to be saved)
  bool Mine_changed = false;

  // Current room:face selection (face-level editing target)
  face_selection current;

  // Current object
  std::optional<int> Cur_object_index;

  //	Current trigger in mine displayed in trigger dialog
  std::optional<int> Current_trigger;

  // The ID of the most recent viewer object (not counting room view)
  std::optional<int> Editor_viewer_id;

  // Marked room:face selection (the secondary face selection used by bridge,
  // join, rotate and compare operations)
  face_selection marked;

  // Room currently being positioned for attachment
  placed_room_state placed;

  // The scrap buffer
  group* Scrap = nullptr;

  // Pointer to the scripts for this level
  std::string Current_level_script;


  int paged_in_count = 0;
  int paged_in_num = 0;


  //	object id clipboard.
  int Copied_object_id = -1;

  bool Fast_terrain = true;
  bool Flat_terrain = false; // Render the terrain as flat?
  bool Show_invisible_terrain = false;
  bool Editor_LOD_engine_off = true;
  bool Terrain_LOD_engine_off = true;
  bool Terrain_render_ext_room_objs = true;

  float Terrain_texture_distance = DEFAULT_TEXTURE_DISTANCE; // how far we should texture before going to flat shad
};

//	Editor.cpp:: Current state of the editor UI.
extern d3edit_state app;

//	Editor.cpp:: Surface describing the actual desktop where the editor is running.
extern grSurface *Desktop_surf;

//	FUNCTIONS
void EditorStatus(const char *format, ...);
void SplashMessage(const char *format, ...);
void StartEditorFrame(grViewport *vp, vector3 *view_vec, matrix *id_mat, float zoom);
void EndEditorFrame();

// Set the editor error message.  A function that's going to return a failure
// code should call this with the error message.
void SetErrorMessage(const char *fmt, ...);

// Get the error message from the last function that returned failure
const char *GetErrorMessage();

static inline std::filesystem::path original_pwd(void)
{
  extern std::filesystem::path orig_pwd;
  return orig_pwd;
}

// Initializes the Descent 3 core in editor mode, mirroring the original MFC
// editor's startup sequence (CMainFrame::OnCreateClient). Must be called once
// after the QApplication has been constructed.
void initD3Core(int argc, char *argv[]);
