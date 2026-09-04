// Stub implementations for all symbols referenced by qteditor
// but not provided by any source in mini/.

#include "pstypes.h"
#include "fix.h"
#include "vecmat.h"
#include "gr.h"
#include "object.h"
#include "room.h"
#include "terrain.h"
#include "gametexture.h"
#include "polymodel.h"
#include "lightmap_info.h"
#include "cfile.h"
#include "dedicated_server.h"
#include "3d.h"
#include "grtext.h"
#include "findintersection.h"
#include "Mission.h"
#include "aiambient.h"
#include "matcen.h"
#include "gamepath.h"
#include "soundload.h"
#include "doorway.h"
#include "weapon.h"
#include "ship.h"
#include "megacell.h"
#include "trigger.h"
#include "BOA.h"
#include "bnode.h"
#include "bsp.h"
#include "manage.h"
#include "mem.h"
#include "game.h"
#include "physics.h"
#include "renderer.h"
#include "lightmap.h"
#include "objinfo.h"
#include "vclip.h"
#include "bitmap.h"
#include "special_face.h"
#include "ssl_lib.h"
#include "hlsoundlib.h"
#include "levelgoal.h"
#include "d3edit.h"
#include "lnxdatabase.h"
#include "door.h"
#include "descent.h"
#include "doorpage.h"
#include "genericpage.h"
#include "shippage.h"
#include "soundpage.h"
#include "texpage.h"
#include "weaponpage.h"
#include "object_external_struct.h"
#include "joystick.h"
#include "chrono_timer.h"
#include "args.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>
#include <filesystem>
#include <cstdarg>
#include <cmath>

#include <QDebug>
#include <QtGlobal>
#include <cstdio>
#include <cstdlib>

//#include "default_base_directories.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define PRINT_STUB(x) \
{ \
  static bool printed_##__FUNCTION__ = false; \
  if( !printed_##__FUNCTION__ ) \
  { \
    printed_##__FUNCTION__ = true; \
    qDebug() << "Function \"" << __FUNCTION__ << "\" is a stub!"; \
  } \
}


// ==================== 3D functions (exact signatures from 3d.h) ====================
uint8_t g3_RotatePoint(g3Point *dest, vector3 *src) { PRINT_STUB(__FUNCTION__); return 0; }
void g3_ProjectPoint(g3Point *point) { PRINT_STUB(__FUNCTION__); }
bool g3_CheckNormalFacing(vector3 *v, vector3 *norm) { PRINT_STUB(__FUNCTION__); return false; }
g3Point **g3_ClipPolygon(g3Point **pointlist, int *nv, g3Codes *cc) { PRINT_STUB(__FUNCTION__); return pointlist; }
void g3_FreeTempPoints(g3Point **pointlist, int nv) { PRINT_STUB(__FUNCTION__); }

// ==================== grSurface / grViewport / grHardwareSurface ====================
grSurface::grSurface() { PRINT_STUB(__FUNCTION__); }
grSurface::~grSurface() { PRINT_STUB(__FUNCTION__); }
void grSurface::free() { PRINT_STUB(__FUNCTION__); }
grViewport::grViewport(grSurface *s) { PRINT_STUB(__FUNCTION__); }
grViewport::~grViewport() { PRINT_STUB(__FUNCTION__); }
// grHardwareSurface::create returns bool, takes (int, int, int, unsigned, const char*)
bool grHardwareSurface::create(int w, int h, int bpp, unsigned flags, const std::string& name) { PRINT_STUB(__FUNCTION__); return false; }
//float Float_to_ubyte(float f) { PRINT_STUB(__FUNCTION__); return f; }

// ==================== Object ====================
object Objects[MAX_OBJECTS];
int Highest_object_index = -1;
object *Viewer_object = &Objects[0];
object *Player_object = &Objects[0];
object_info Object_info[MAX_OBJECTS];

// Walks the object parent handle chain to the ultimate ancestor.  The mini
// port has no parented-object state, so every object is treated as its own
// root.
object *ObjGetUltimateParent(object *child) { PRINT_STUB(__FUNCTION__); return child; }

// ==================== Game / Descent ====================
int FrameCount = 0;
bool Katmai = false;                    // whether or not katmai CPU is detected
bool Dedicated_server = false;
function_mode View_mode = EDITOR_MODE;

// ==================== Terrain ====================
terrain_sound_band Terrain_sound_bands[256] = {};
/*
int Terrain_checksum = 0;
uint8_t Terrain_dynamic_table[16] = {};
terrain_segment Terrain_seg[(TERRAIN_WIDTH+1) * (TERRAIN_DEPTH+1)] = {};
terrain_tex_segment Terrain_tex_seg[TERRAIN_TEX_WIDTH * TERRAIN_TEX_DEPTH] = {};
terrain_sky Terrain_sky = {};
terrain_normals *TerrainNormals[MAX_TERRAIN_LOD] = {};
float Terrain_texture_distance = 0;
int16_t Terrain_seg_render_objs[(TERRAIN_WIDTH+1) * (TERRAIN_DEPTH+1)] = {};
uint8_t Fast_terrain = 0;
uint8_t Flat_terrain = 0;
uint8_t Show_invisible_terrain = 0;

int GetTerrainRoomFromPos(vector3 *pos) { PRINT_STUB(__FUNCTION__); return -1; }
void ComputeTerrainSegmentCenter(vector3 *center, int seg) { PRINT_STUB(__FUNCTION__); *center = {}; }
float GetTerrainGroundPoint(vector3 *in, vector3 *out) { PRINT_STUB(__FUNCTION__); *out = *in; return 0; }
void BuildMinMaxTerrain() { PRINT_STUB(__FUNCTION__); }
void BuildTerrainNormals() { PRINT_STUB(__FUNCTION__); }
void ResetTerrain(int terrain_size) { PRINT_STUB(__FUNCTION__); }
void SetupSky(float t, int tmap, unsigned char layer) { PRINT_STUB(__FUNCTION__); }
void UpdateTerrainLightmaps() { PRINT_STUB(__FUNCTION__); }
*/
void ClearTerrainSound() { PRINT_STUB(__FUNCTION__); }

// ==================== Terrain search / render ====================
int Check_terrain_portal = 0;
uint8_t Terrain_from_mine = 1;
void rend_GetProjectionParameters(int *width, int *height) { PRINT_STUB(__FUNCTION__); *width = 0; *height = 0; }
uint8_t g3_CodePoint(g3Point *point) { PRINT_STUB(__FUNCTION__); return 0; }
uint8_t CodeTerrainPoint(g3Point *p) { PRINT_STUB(__FUNCTION__); return 0; }

// ==================== BOA ====================
/*
int BOA_num_terrain_regions = 0;
connect_data BOA_connect[MAX_BOA_TERRAIN_REGIONS][MAX_PATH_PORTALS] = {};
int BOA_num_connect[MAX_BOA_TERRAIN_REGIONS] = {};
int BOA_AABB_checksum = 0;
int BOA_AABB_ROOM_checksum[MAX_ROOMS + MAX_BOA_TERRAIN_REGIONS] = {};
int BOA_mine_checksum = 0;
int BOA_vis_checksum = 0;
bool BOA_vis_valid = false;
void MakeBOA() { PRINT_STUB(__FUNCTION__); }
bool BOA_IsVisible(int start_room, int end_room) { PRINT_STUB(__FUNCTION__); return false; }
*/
// ==================== BSP ====================
void InitBSP() { PRINT_STUB(__FUNCTION__); }
int BSPRayOccluded(vector3 *a, vector3 *b, bspnode *n) { PRINT_STUB(__FUNCTION__); return 0; }
bsptree MineBSP = {};
uint8_t UseBSP = 0;
void BuildBSPTree() { PRINT_STUB(__FUNCTION__); }
void BuildSingleBSPTree(int n) { PRINT_STUB(__FUNCTION__); }

// ==================== Game globals ====================
float Frametime = 0.0f;

int Num_textures = 0;
int Num_sounds = 0;
int Num_weapons = 0;
//int Num_doors = 0;
int Num_ships = 0;
int Num_megacells = 0;
int Num_objects = 0;
int Num_triggers = 0;
int Num_game_paths = 0;
int Num_matcens = 0;
int Num_object_ids[1500] = {};
int Network_up = 0;
float Gravity_strength = 9.8f;
int Cinematics_enabled = 0;
renderer_type PreferredRenderer = {};
int Detail_settings = 0;
float force_field_bounce_multiplier[MAX_FORCE_FIELD_BOUNCE_TEXTURES] = {};
int force_field_bounce_texture[MAX_FORCE_FIELD_BOUNCE_TEXTURES] = {};
//float Ceiling_height = 100.0f;
renderer_preferred_state Render_preferred_state = {};
int sound_override_force_field = -1;
int sound_override_glass_breaking = -1;
bool Level_powerups_ignore_wind = false;

QString InfoString;
QString ErrorString;
QString TableUser;
std::string Default_pilot;
std::filesystem::path LocalMiscDir;
std::filesystem::path LocalModelsDir;
std::filesystem::path LocalSoundsDir;
std::filesystem::path NetMiscDir;
std::filesystem::path NetModelsDir;
std::filesystem::path LocalLevelsDir;
std::filesystem::path LocalScriptDir;

// ==================== Game data arrays ====================
bms_bitmap GameBitmaps[MAX_BITMAPS];
bms_lightmap GameLightmaps[MAX_LIGHTMAPS];
texture GameTextures[MAX_TEXTURES];
//vclip GameVClips[MAX_VCLIPS];
//poly_model Poly_models[500];
game_path GamePaths[MAX_GAME_PATHS];
trigger Triggers[500];
special_face SpecialFaces[MAX_SPECIAL_FACES];

// ==================== Sound ====================
hlsSystem Sound_system;
void hlsSystem::BeginSoundFrame(bool f_force) { PRINT_STUB(__FUNCTION__); }
void hlsSystem::EndSoundFrame() { PRINT_STUB(__FUNCTION__); }
void hlsSystem::StopAllSounds() { PRINT_STUB(__FUNCTION__); }
sound_info Sounds[MAX_SOUNDS] = {};
sound_file_info SoundFiles[MAX_SOUND_FILES] = {};

// ==================== Lighting ====================
lightmap_info LightmapInfoStore[MAX_LIGHTMAP_INFOS] = {};
lightmap_info *LightmapInfo = LightmapInfoStore;
int Num_of_lightmap_info = 0;
int Num_lightmap_infos_read = 0;
void FreeLightmapMemory() { PRINT_STUB(__FUNCTION__); }
void FreeLightmapInfo(int handle) { PRINT_STUB(__FUNCTION__); }
void ClearAllObjectLightmaps(int n) { PRINT_STUB(__FUNCTION__); }
void ClearObjectLightmaps(object *obj) { PRINT_STUB(__FUNCTION__); }
void BlurLightmapInfos(int n) { PRINT_STUB(__FUNCTION__); }
void ShadeLightmapInfoEdges(int n) { PRINT_STUB(__FUNCTION__); }
void EnableLightmapGen() { PRINT_STUB(__FUNCTION__); }
int lm_AllocLightmap(int w, int h) { PRINT_STUB(__FUNCTION__); return -1; }
void lm_FreeLightmap(int n) { PRINT_STUB(__FUNCTION__); }
uint16_t *lm_data(int handle) { PRINT_STUB(__FUNCTION__); return nullptr; }
int lm_w(int n) { PRINT_STUB(__FUNCTION__); return 0; }
int lm_h(int n) { PRINT_STUB(__FUNCTION__); return 0; }
int lmi_h(int n) { PRINT_STUB(__FUNCTION__); return 0; }
int lmi_w(int n) { PRINT_STUB(__FUNCTION__); return 0; }
void SetupObjectLightmapMemory(object *obj) { PRINT_STUB(__FUNCTION__); }
//void ClearAllVolumeLights() { PRINT_STUB(__FUNCTION__); }

// ==================== Player/ship ====================
player Players[MAX_PLAYERS] = {};
int Num_player_shapes = 0;
ship Ships[MAX_SHIPS] = {};

// ==================== Door ====================
//door Doors[MAX_DOORS];
//doorway *DoorwayAdd(room *rp, int doornum) { PRINT_STUB(__FUNCTION__); return nullptr; }

// ==================== Weapon ====================
weapon Weapons[MAX_WEAPONS] = {};
// Sentinel-terminated list (empty) so callers strcmp("", name[i]) cleanly.
const char *const Static_weapon_names[] = {""};
const int Static_weapon_names_msg[] = {0};
void WBClearInfo(otype_wb_info *wb) { PRINT_STUB(__FUNCTION__); }

// ==================== Megacell ====================
megacell Megacells[MAX_MEGACELLS] = {};

// ==================== Matcen ====================
matcen *Matcen[MAX_MATCENS] = {};

// ==================== Level ====================
levelgoals Level_goals;
level_info Level_info;
void levelgoals::CleanupAfterLevel() { PRINT_STUB(__FUNCTION__); }

// ==================== Ambient ====================
ambient_life a_life;
void ambient_life::ALReset() { PRINT_STUB(__FUNCTION__); }

// ==================== Manage ====================
mngs_track_lock GlobalTrackLocks[MAX_TRACKLOCKS] = {};
int mng_AllocTrackLock(const std::string &a, int b) { PRINT_STUB(__FUNCTION__); return -1; }
void mng_FreeTrackLock(int n) { PRINT_STUB(__FUNCTION__); }
int mng_FindTrackLock(const std::string &a, int b) { PRINT_STUB(__FUNCTION__); return -1; }
int mng_CheckIfPageLocked(mngs_Pagelock *p) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_CheckIfPageOwned(mngs_Pagelock *p, const std::string &a) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_DeletePage(const std::string &a, int b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_DeletePagelock(const std::string &a, int b) { PRINT_STUB(__FUNCTION__); return 0; }
void mng_EraseLocker() { PRINT_STUB(__FUNCTION__); }
int mng_MakeLocker() { PRINT_STUB(__FUNCTION__); return 0; }
void mng_OverrideToUnlocked(mngs_Pagelock *p) { PRINT_STUB(__FUNCTION__); }
int mng_RenamePage(const std::string &a, const std::string &b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_ReplacePage(const std::string &a, const std::string &b, int c, int d, int e) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_ReplacePagelock(const std::string &a, mngs_Pagelock *b) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_FindSpecificDoorPage(const std::string &a, mngs_door_page *b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_FindSpecificGenericPage(const std::string &a, mngs_generic_page *b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_FindSpecificShipPage(const std::string &a, mngs_ship_page *b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_FindSpecificSoundPage(const std::string &a, mngs_sound_page *b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_FindSpecificTexPage(const std::string &a, mngs_texture_page *b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_FindSpecificWeaponPage(const std::string &a, mngs_weapon_page *b, int c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_AssignDoorPageToDoor(mngs_door_page *a, int b) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_AssignGenericPageToObjInfo(mngs_generic_page *a, int b, struct CFILE* c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_AssignShipPageToShip(mngs_ship_page *a, int b, struct CFILE* c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_AssignSoundPageToSound(mngs_sound_page *a, int b) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_AssignTexPageToTexture(mngs_texture_page *a, int b, struct CFILE* c) { PRINT_STUB(__FUNCTION__); return 0; }
int mng_AssignWeaponPageToWeapon(mngs_weapon_page *a, int b, struct CFILE* c) { PRINT_STUB(__FUNCTION__); return 0; }

// ==================== CFILE (implemented in cfile/cfile.cpp) ====================

// ==================== Room ====================
//void ClearAllRoomLightmaps(int n) { PRINT_STUB(__FUNCTION__); }
//void ClearRoomLightmaps(int n) { PRINT_STUB(__FUNCTION__); }
//void ClearAllRoomSpecmaps(int n) { PRINT_STUB(__FUNCTION__); }

//room Rooms[MAX_ROOMS] = {};
//int Highest_room_index = -1;

// ==================== DDIO ====================
void ddio_SplitPath(const char *path, char *dir, char *fname, char *ext) { PRINT_STUB(__FUNCTION__); if (dir) dir[0] = 0; if (fname) fname[0] = 0; if (ext) ext[0] = 0; }
int ddio_MouseGetState(int *x, int *y, int *dx, int *dy, int *l, int *r) { PRINT_STUB(__FUNCTION__); if (x) *x = 0; if (y) *y = 0; if (dx) *dx = 0; if (dy) *dy = 0; if (l) *l = 0; if (r) *r = 0; return 0; }
float ddio_KeyDownTime(int key) { PRINT_STUB(__FUNCTION__); return 0; }
void joy_GetPos(tJoystick joy, tJoyPos *pos) { PRINT_STUB(__FUNCTION__); if (pos) { pos->x = 0; pos->y = 0; pos->z = 0; } }
bool joy_IsValid(tJoystick id) { PRINT_STUB(__FUNCTION__); return false; }
void ddio_CopyFileTime(const std::filesystem::path& dest, const std::filesystem::path& src) { PRINT_STUB(__FUNCTION__); (void)dest; (void)src; }


namespace D3 {
const std::vector<std::filesystem::path> Default_read_only_base_directories;
}

// ==================== ChronoTimer ====================


// ==================== Init ====================
// Command-line argument store. GatherArgs collects the argv tokens (including
// argv[0] as index 0) so FindArg/GetArg can be used to pass options such as
// "-datadir <path>" to locate the game data files.
char GameArgs[MAX_ARGS][MAX_CHARS_PER_ARG] = {};

// Second-phase system init. In the Win32 editor this is where the gamedata
// table files are loaded (mng_LoadTableFiles) so levels opened later can
// reference object/ship/weapon/sound/texture data. The decoupled editor does
// the same in initD3Core via loadGameDataTable() (the data dir must be
// resolved there first); this hook preserves the original two-phase shape.
void InitD3Systems2(bool /*editor*/) { PRINT_STUB(__FUNCTION__); }
void InitGamePaths() { PRINT_STUB(__FUNCTION__); }
void ProgramVersion(int a, unsigned char b, unsigned char c, unsigned char d) { PRINT_STUB(__FUNCTION__); }

// ==================== Application ====================
static tLnxAppInfo _default_app_info;

// ==================== Mem ====================
bool Mem_low_memory_mode = false;
bool Mem_superlow_memory_mode = false;
void mem_Init() { PRINT_STUB(__FUNCTION__); }
void mem_Shutdown() { PRINT_STUB(__FUNCTION__); }
int mem_GetTotalMemoryUsed() { PRINT_STUB(__FUNCTION__); return 0; }



// ==================== grtext ====================
void grtext_Init() { PRINT_STUB(__FUNCTION__); }

// ==================== Application ====================
oeLnxApplication::oeLnxApplication(tLnxAppInfo *info) { PRINT_STUB(__FUNCTION__); }
oeLnxApplication::~oeLnxApplication() { PRINT_STUB(__FUNCTION__); }
void oeLnxApplication::init() { PRINT_STUB(__FUNCTION__); }
void oeLnxApplication::get_info(void *appinfo) { PRINT_STUB(__FUNCTION__); }
unsigned oeLnxApplication::defer() { PRINT_STUB(__FUNCTION__); return 0; }
const char *oeLnxApplication::get_window_name() { PRINT_STUB(__FUNCTION__); return "D3Editor"; }
void oeLnxApplication::clear_window() { PRINT_STUB(__FUNCTION__); }
void oeLnxApplication::set_defer_handler(void (*func)(bool isactive)) { PRINT_STUB(__FUNCTION__); }
void oeLnxApplication::delay(float secs) { PRINT_STUB(__FUNCTION__); }
int oeLnxApplication::flags() const { PRINT_STUB(__FUNCTION__); return 0; }

oeLnxAppDatabase::oeLnxAppDatabase() { PRINT_STUB(__FUNCTION__); }
oeLnxAppDatabase::~oeLnxAppDatabase() { PRINT_STUB(__FUNCTION__); }
bool oeLnxAppDatabase::create_record(const char *pathname) { PRINT_STUB(__FUNCTION__); return false; }
bool oeLnxAppDatabase::lookup_record(const char *pathname) { PRINT_STUB(__FUNCTION__); return false; }
bool oeLnxAppDatabase::read(const char *label, char *entry, int *entrylen) { PRINT_STUB(__FUNCTION__); return false; }
bool oeLnxAppDatabase::read(const char *label, void *entry, int wordsize) { PRINT_STUB(__FUNCTION__); return false; }
bool oeLnxAppDatabase::read(const char *label, bool *entry) { PRINT_STUB(__FUNCTION__); return false; }
bool oeLnxAppDatabase::write(const char *label, const char *entry, int entrylen) { PRINT_STUB(__FUNCTION__); return false; }
bool oeLnxAppDatabase::write(const char *label, int entry) { PRINT_STUB(__FUNCTION__); return false; }
void oeLnxAppDatabase::get_user_name(char *buffer, size_t *size) { PRINT_STUB(__FUNCTION__); if (buffer) buffer[0] = 0; }

oeApplication *Descent = nullptr;
oeAppDatabase *Database = nullptr;

// ==================== Find* ====================

int FindDoorName(const std::string &name) { PRINT_STUB(__FUNCTION__); return -1; }
int FindShipName(const std::string &name) { PRINT_STUB(__FUNCTION__); return -1; }

// ==================== Alloc/Free ====================
//int AllocDoor() { PRINT_STUB(__FUNCTION__); return -1; }
//void FreeDoor(int n) { PRINT_STUB(__FUNCTION__);}
int AllocObjectID(int id, bool a, bool b, bool c) { PRINT_STUB(__FUNCTION__); return -1; }
void FreeObjectID(int n) { PRINT_STUB(__FUNCTION__); }
int GetObjectID(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int AllocShip() { PRINT_STUB(__FUNCTION__); return -1; }
void FreeShip(int n) { PRINT_STUB(__FUNCTION__); }
int AllocSound() { PRINT_STUB(__FUNCTION__); return -1; }
void FreeSound(int n) { PRINT_STUB(__FUNCTION__); }
int AllocSpecialFace(int a, int b, bool c, int d) { PRINT_STUB(__FUNCTION__); return -1; }
void FreeSpecialFace(int n) { PRINT_STUB(__FUNCTION__); }
int AllocTexture() {  PRINT_STUB(__FUNCTION__); return -1; }
void FreeTexture(int n) { PRINT_STUB(__FUNCTION__); }
int AllocWeapon() { PRINT_STUB(__FUNCTION__); return -1; }
void FreeWeapon(int n) { PRINT_STUB(__FUNCTION__); }
int AllocLightmapInfo(int a, int b, int c, bool d) { PRINT_STUB(__FUNCTION__); return -1; }

// ==================== GetNext/Prev ====================
//int GetNextDoor(int n) { PRINT_STUB(__FUNCTION__); return -1; }
//int GetPrevDoor(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetNextMegacell(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetPrevMegacell(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetNextShip(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetPrevShip(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetNextSound(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetPrevSound(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetNextTexture(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetPreviousTexture(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetNextWeapon(int n) { PRINT_STUB(__FUNCTION__); return -1; }
int GetPrevWeapon(int n) { PRINT_STUB(__FUNCTION__); return -1; }

// ==================== Remap ====================
void RemapShips() { PRINT_STUB(__FUNCTION__); }
void RemapSounds() { PRINT_STUB(__FUNCTION__); }
void RemapStaticIDs() { PRINT_STUB(__FUNCTION__); }
void RemapWeapons() { PRINT_STUB(__FUNCTION__); }

// ==================== Load ====================
//int LoadDoorImage(const std::filesystem::path& name, int n) { PRINT_STUB(__FUNCTION__); return -1; }
//int LoadPolyModel(const std::filesystem::path& name, int f_module) { PRINT_STUB(__FUNCTION__); return -1; }
int LoadShipImage(const std::filesystem::path& name) { PRINT_STUB(__FUNCTION__); return -1; }
int LoadSoundFile(const char *name, float vol, bool b) { PRINT_STUB(__FUNCTION__); return -1; }
int LoadTextureImage(const std::filesystem::path& name, int *handle, int a, int b, int c, int d) { PRINT_STUB(__FUNCTION__); return -1; }
//int GetDoorImage(int n) { PRINT_STUB(__FUNCTION__); return -1; }

// ==================== Polymodel ====================
//poly_model *GetPolymodelPointer(int n) { PRINT_STUB(__FUNCTION__); return nullptr; }
//int CountFacesInPolymodel(poly_model *pm) { PRINT_STUB(__FUNCTION__); return 0; }
//float ComputeDefaultSize(int type, int handle, float *size) { PRINT_STUB(__FUNCTION__); return 0; }
//void PageInPolymodel(int model_num, int f_damage, float *size) { PRINT_STUB(__FUNCTION__); }
//void SetModelAnglesAndPos(poly_model *pm, float *anim, unsigned int flags) { PRINT_STUB(__FUNCTION__); }
//int IsNonRenderableSubmodel(poly_model *pm, int index) { PRINT_STUB(__FUNCTION__); return 0; }
void ChangeOldModelsForObjects(int a, int b) { PRINT_STUB(__FUNCTION__); }
//std::filesystem::path ChangePolyModelName(const std::filesystem::path& name) { PRINT_STUB(__FUNCTION__); return name; }
//int FindPolyModelName(const std::filesystem::path& name) { PRINT_STUB(__FUNCTION__); return -1; }
//void FreePolyModel(int n) { PRINT_STUB(__FUNCTION__); }
//void SetNormalizedTimeObj(object *obj, float *normalized_time) { PRINT_STUB(__FUNCTION__); }

// ==================== Misc ====================
int CreateMatcen(const char *name, bool *flag) { PRINT_STUB(__FUNCTION__); return -1; }
void DestroyAllMatcens() { PRINT_STUB(__FUNCTION__); }
void FreeAllGamePaths() { PRINT_STUB(__FUNCTION__); }
//int FindPointRoom(vector3 *pnt) { PRINT_STUB(__FUNCTION__); return -1; }
int GetTerrainRoomFromPos_ret(vector3 *pos) { PRINT_STUB(__FUNCTION__); return -1; }
int AIMakeNextRoomList(int roomnum, int *next_rooms, int max_rooms) { PRINT_STUB(__FUNCTION__); return 0; }
bool PhysCalcGround(vector3 *ground_point, vector3 *ground_normal, object *obj, int ground_num) { PRINT_STUB(__FUNCTION__); return false; }
void ClearAllEvents() { PRINT_STUB(__FUNCTION__); }

// ==================== FVI ====================
/*
int fvi_QuickDistFaceList(int init_room_index, vector3 *pos, float rad, fvi_face_room_list *quick_fr_list, int max_elements) { PRINT_STUB(__FUNCTION__); return 0; }
bool fvi_QuickRoomCheck(vector3 *pos, room *cur_room, bool try_again) { PRINT_STUB(__FUNCTION__); return false; }
int fvi_QuickDistObjectList(vector3 *pos, int init_roomnum, float rad, int16_t *object_index_list, int max_elements,
                            bool f_lightmap_only, bool f_only_players_and_ais,
                            bool f_include_non_collide_objects, bool f_stop_at_closed_doors) { PRINT_STUB(__FUNCTION__); return 0; }
int fvi_QuickDistCellList(int init_cell_index, vector3 *pos, float rad, int *quick_cell_list, int max_elements) { PRINT_STUB(__FUNCTION__); return 0; }
uint32_t check_point_to_face(vector3 *colp, vector3 *face_normal, int nv, vector3 **vertex_ptr_list) { PRINT_STUB(__FUNCTION__); return 0; }
bool FVI_always_check_ceiling = false;
*/
// ==================== OSIRIS ====================

// ==================== Pilot ====================
void PltClearList() { PRINT_STUB(__FUNCTION__); }


// ==================== Bitmap ====================
// The mini tree has no InitBitmaps()/Bitmaps_initted global; GameBitmaps is a
// zero-initialised static array so allocation is always considered ready.

// ==================== Inventory ====================
Inventory::Inventory() { PRINT_STUB(__FUNCTION__); }
Inventory::~Inventory() { PRINT_STUB(__FUNCTION__); }

// ==================== grFont ====================
grFont::grFont() { PRINT_STUB(__FUNCTION__); }
grFont::~grFont() { PRINT_STUB(__FUNCTION__); }

// ==================== hlsSystem ====================
hlsSystem::hlsSystem() { PRINT_STUB(__FUNCTION__); }
void hlsSystem::KillSoundLib(bool f_kill_sound_list) { PRINT_STUB(__FUNCTION__); }
int hlsSystem::Play2dSound(int sound_index, float volume, float pan, unsigned short flags) { PRINT_STUB(__FUNCTION__); return 0; }
bool hlsSystem::CheckAndForceSoundDataAlloc(int sound_file_index) { PRINT_STUB(__FUNCTION__); return false; }

// ==================== matcen methods ====================
std::string matcen::GetName(void) { PRINT_STUB(__FUNCTION__); return std::string(); }
bool matcen::SetName(const std::string& name) { PRINT_STUB(__FUNCTION__); return false; }
