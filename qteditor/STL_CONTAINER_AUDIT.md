# STL Container & HOG-I/O Audit — `qteditor/mini/`

Audit date: 2026-09-05
Scope: all headers under `qteditor/mini/` (game/lib/manage/model/bitmap/2dlib/ddio/sndlib/physics/misc/tools), the HOG/level/table loaders, and `descar` I/O usage by `qteditor`.

Two questions answered:

1. Which struct members can be converted to modern STL containers, and to which type?
2. Which types are loaded from HOG archives (and therefore have on-disk-format constraints)?

---

## Executive summary

- **No serialized struct is unpacked from disk as raw memory.** Every loader
  (`mini/level_loader.cpp`, `mini/manage/gamedata_loader.cpp` + the
  `mini/manage/*page.cpp` readers, `mini/model/polymodel.cpp`) deserializes
  **field-by-field** through `posix_istream`/`posix_ostream` (little-endian,
  from `descar`). The only raw-block reads go into local scratch buffers
  (`tag[4]`, `tempbuf`, payload vectors), never into a struct layout.
- Consequently every fixed-array / `char name[N]` / `pointer + count` member in
  a serialized struct can be converted to `std::array` / `std::string` /
  `std::vector` **provided the paired reader/writer is updated to emit the same
  on-disk field order.** The disk format itself does not change.
- Best-value conversions: the `pointer + count` pairs (`room::faces/verts`,
  `poly_model::submodel`, `bsp_info::verts`, `bn_list::nodes`, `game_path::pathnodes`,
  `object::attach_children`, …) — they replace hand-rolled `mem_malloc`/
  `mem_free` heap buffers with RAII containers.
- Sofar the port has already migrated the hot `std::string` members
  (`room::name`, `object::name`, `texture::name`, `ship::name`, `trigger::name`,
  `weapon::name`, `level_info` strings, …). See Part B section 5 for the
  already-done list.

---

## Part A — Types loaded from HOG archives

A Descent 3 `.hog` is a HOG2 archive (`hog2::archive_t`, `descar`): a header +
an entry table, followed at `archive.fileOffset(entry)` by per-file payloads.
The retail game ships everything inside `d3.hog`. The editor port reads:

### A.1 Read out of a HOG **today** by `qteditor`

Only `gamedata_loader.cpp` (`mini/manage/gamedata_loader.cpp`) opens a real HOG
(`loadGameDataTable(d3.hog)`). It locates `table.gam` in the archive, slurps its
payload, and parses the page stream into the per-page reader structs
(`mngs_*_page`) and then into the game-table indexes (`GameTextures[]`,
`Weapons[]`, `Doors[]`, `Ships[]`, `Sounds[]`, `Object_info[]`, `Megacells[]`).
Texture images referenced by `table.gam` are also read **from the same open
archive** and decoded with `bm_LoadBitmapFromMemory`.

| HOG payload | Page/internal types populated | Struct files |
|---|---|---|
| `table.gam` PAGETYPE_TEXTURE | `texture`, `mngs_texture_page` | `game/gametexture.h`, `lib/texpage.h` |
| `table.gam` PAGETYPE_WEAPON | `weapon`, `mngs_weapon_page` | `lib/weapon.h`, `lib/weaponpage.h` |
| `table.gam` PAGETYPE_DOOR | `door`, `mngs_door_page` | `game/door.h`, `lib/doorpage.h` |
| `table.gam` PAGETYPE_SHIP | `ship`, `mngs_ship_page` | `lib/ship.h`, `lib/shippage.h` |
| `table.gam` PAGETYPE_SOUND | `sound_info`, `mngs_sound_page` | `lib/ssl_lib.h`, `lib/soundpage.h` |
| `table.gam` PAGETYPE_GENERIC | `object_info`, `mngs_generic_page` | `lib/objinfo.h`, `lib/genericpage.h` |
| `table.gam` PAGETYPE_MEGACELL | `megacell`, `mngs_megacell_page` | `lib/megacell.h` |
| texture image payloads (OGF/IFF/TGA) named by `bitmap_name` | `GameBitmaps[]` → `bms_bitmap`, `chunked_bitmap` | `lib/bitmap.h` |

Note: the `mngs_*_page` reader structs are duplicated as header copies in both
`mini/lib/*page.h` and `mini/manage/*page.h` for the shared page types
(door/ship/tex/weapon); `genericpage.h` and `soundpage.h` exist under
`mini/lib/` only. Their readers live in `mini/manage/*page.cpp`.

### A.2 HOG-resident types whose loaders this port keeps (read from filesystem, or dead in the build)

These structs are HOG-resident data in the retail engine. The mini keeps
field-compatible deserializers but the current `qteditor` runtime does **not**
read them from a HOG archive (levels come from the open dialog / filesystem;
the `.pof`/`.vcl`/`.ter`/mission loaders are either posix-filesystem based or
left as `CFILE`-stub paths that are not linked).

| Payload | Types | Loader | Current source in port |
|---|---|---|---|
| `.d3l` level file | `room`, `face`, `portal`, `object`, `trigger`, `level_info`, assorted chunk structs | `mini/level_loader.cpp` (`LoadLevel`/`SaveLevel`) | filesystem only |
| `.pof` polymodel | `poly_model`, `bsp_info`, `polyface`, `w_bank`, `a_bank`, `poly_wb_info`, `lightmap_object` | `mini/model/polymodel.cpp` (`LoadModel`) | filesystem (posix) |
| `.vcl` anim clip | `vclip` | `mini/game/vclip.cpp` (`AllocLoadVClip`) | `CFILE`-stub, dead |
| `.ter` terrain | `terrain_sky`, `lodoff`, terrain segment globals | `mini/game/terrain.cpp` | `CFILE`-stub, dead |
| `.snd` sound | `sound_file_info`, `sound_info` | `mini/sndlib/hlsoundlib.h` | `CFILE`-stub, dead |
| `.msn`/`.mn3` mission | `tMission`, `tMissionInfo`, `level_info` | `mini/game/Mission.h` readers | not wired |

Implication: **never** switch an A.1/A.2 struct to raw-`sizeof`/`memcpy`
serialization; always keep the field-level readers in lock-step when members are
converted (see Part B risk notes).

### A.3 HOG/misc I/O structs that are already modern

- `hog2::header_t`, `hog2::entry_t` (`fixed_string_t<36> name`, `uint32_t flags/len/timestamp`),
  `hog2::archive_t` (iterable entry list) — from `descar`, already STL-based.
- Legacy `tHogHeader` / `tHogFileEntry` (`mini/cfile/hogfile.cpp`) were **removed**.
- HOG **writer** (`mini/tools/HogMaker/HogFormat.h`, namespace `D3`): `HogHeader`
  (`std::array<char,4> tag`, `std::array<char,56> reserved`), `HogFileEntry`
  (`std::array<char,36> m_name`, `std::filesystem::path m_real_path`),
  `HogFormat` (`std::vector<HogFileEntry> m_file_entries`) — already STL.
- `docstring` of the migration is in `mini/CFILE_TO_POSIX_STREAM.md`.

---

## Part B — Struct members that can become STL containers

Conventions used below:

- `std::array<T,N>` — the bound is a fixed `MAX_*`/literal constant and the
  member always holds that many elements **and is serialized at that fixed
  width** (the paired reader/writer loop over the full `MAX_*` count).
- `std::vector<T>` — the member is dynamically sized (`pointer + count` idiom,
  or a fixed declared size that is only partially filled at runtime).
  This also covers members whose header bound is a `MAX_*` constant but whose
  on-disk representation is **count-prefixed**: the reader reads a count from
  the stream first and fills only that many elements (clamped to the `MAX_*`
  bound) — e.g. `object_info::death_types[]` and
  `mngs_texture_page::proc_type[]` (verified below in B.2).
- `std::string` — `char x[N]` buffers (serialized on disk via fixed/NUL-terminated
  strings through `posix_istream`/`fixed_string_t`).

Priority: **P0** = high value + low risk, **P1** = moderate,
**P2** = worth doing but unused/dead in the editor today, **skip** = actively
discouraged (hot path / binary-union / trivially serialized).

### B.1 `pointer + count` members → `std::vector<T>` (P0/P1)

| Struct (header) | Members | Container | Notes |
|---|---|---|---|
| `room` (`game/room_external.h:274`) | `face *faces`+`num_faces`; `portal *portals`+`num_portals`; `vector3 *verts`+`num_verts`; `vector4 *verts4` | `std::vector<face>`, `std::vector<portal>`, `std::vector<vector3>`, `std::vector<vector4>` | Serialized field-by-field in `LL_ReadRoom`/`LL_WriteRoom` (`mini/level_loader.cpp:436`). HOG-resident type (A.2) — update loader/saver in the same change. Counts stay as `Size()` accessors or keep the int fields in sync. Render/pick access the members index-wise (`Rooms[i].verts[j]`), which `std::vector::operator[]` preserves. |
| `room` BBF accel data | `int16_t **bbf_list`, `int16_t *num_bbf`, `vector3 *bbf_list_min_xyz`, `vector3 *bbf_list_max_xyz`, `uint8_t *bbf_list_sector` (+`num_bbf_regions`) | `std::vector<std::vector<int16_t>>` + parallel `std::vector<vector3>`/`std::vector<vector3>`/`std::vector<uint8_t>` | Runtime-built/freed in `mini/game/room.cpp` (`mem_free` loops), **not** serialized. P0-safe. |
| `bn_list` (`game/bnode.h:98`), `bn_node` (`:92`) | `bn_node *nodes`+`num_nodes`; `bn_edge *edges`+`num_edges` | `std::vector<bn_node>`, `std::vector<bn_edge>` | Computed by bnode/ebnode code; not part of the `.d3l` chunk stream in this port. |
| `game_path` (`game/gamepath.h:63`) | `node *pathnodes`+`num_nodes` (manual `mem_free` in dtor) | `std::vector<node>` | Search results/manual path editing write these; verify `SaveLevel` does not serialize paths before converting (currently TRIG/rooms/objects only). |
| `object` (`lib/object_external_struct.h:474`) | `int32_t *attach_children`; `char *custom_default_script_name`; `char *custom_default_module_name` | `std::vector<int32_t>`, `std::string` (x2) | `object` serialized by `LL_WriteObjects` in level_loader; field-based, safe. |
| `bsp_info` (`lib/polymodel_external.h:156`) | `vector3 *verts/*vertnorms`, `float *alpha`, `polyface *faces`, `vector3 *face_min/*face_max`, `int16_t *vertnum_memory`, `float *u_memory/*v_memory`, `vector3 *keyframe_axis`, `int *keyframe_angles`, `vector3 *keyframe_pos`, `matrix *keyframe_matrix`, `uint16_t *tick_pos_remap/*tick_ang_remap`, `int *rot_start_time/*pos_start_time` | `std::vector<T>` each | **P2** — model loading (`mini/model/polymodel.cpp:2067`) fills these; the editor does not render models yet. |
| `poly_model` (`lib/polymodel_external.h:231`) | `uint8_t *model_data`+`model_data_size`; `bsp_info *submodel`+`n_models`; `w_bank *gun_slots`+`n_guns`; `w_bank *ground_slots`+`n_ground`; `a_bank *attach_slots`+`n_attach`; `poly_wb_info *poly_wb`+`num_wbs`; `int *render_order` | `std::vector<uint8_t>`, `std::vector<bsp_info>`, `std::vector<w_bank>`, `std::vector<a_bank>`, `std::vector<poly_wb_info>`, `std::vector<int>` | **P2**; `model_data` is a blob many indexes point into — keep it a vector and indexes as they are. |
| `polyface` (`lib/polymodel_external.h:135`) | `int16_t *vertnums`, `float *u`, `float *v` | `std::vector<int16_t>`, `std::vector<float>` | **P2** (with `bsp_info`). |
| `lightmap_object` (`lib/polymodel_external.h:126`) | `int16_t num_faces[MAX_SUBOBJECTS]`; `lightmap_object_face *lightmap_faces[MAX_SUBOBJECTS]` | `std::vector<int16_t>`, `std::vector<std::vector<lightmap_object_face>>` | **P2**; embedded by value in `object::lm_object`. |

### B.2 Fixed-size array members → `std::array<T,N>` (bounds are compile-time `MAX_*`)

All bounds below come from compile-time constants. The members marked `std::array`
were verified against the paired HOG/level readers to be **fixed-width on disk**
(the reader loops over the full constant count with no predecessor count field).
Two rows are marked `std::vector` instead because verification showed their disk
format is count-prefixed despite the `MAX_*` headert bound. The edits only change
the container type; the byte stream written/read is unchanged.

| Struct (header) | Members → `std::array<>` |
|---|---|
| `ai_path_info` (`lib/aistruct.h:488`) | `path_id[MAX_JOINED_PATHS]`, `path_type[]`, `path_start_node[]`, `path_end_node[]`, `path_flags[]` (each `uint8_t`/`uint16_t`) |
| `ai_mem` (`lib/aistruct.h:717`) | `memory[AI_MEM_DEPTH]` → `std::array<ai_memory,AI_MEM_DEPTH>` |
| `ai_frame` (`lib/aistruct.h:738`) | `melee_damage[2]`, `melee_latency[2]`, `sound[MAX_AI_SOUNDS]`, `last_sound_time[MAX_AI_SOUNDS]`, `goals[MAX_GOALS]` |
| `goal` (`lib/aistruct.h:625`) | `ramp_influence_dists[4]`, `enabler[MAX_ENABLERS_PER_GOAL]` |
| `ai_dynamic_path` (`lib/aistruct.h:861`) | `pos[MAX_NODES]` → `std::vector<vector3>` (MAX_NODES is large/runtime-sparse; prefer vector), `roomnum[MAX_NODES]` |
| `object_info` (`lib/objinfo.h:486`) | `sounds[MAX_OBJ_SOUNDS]`, `dspew_percent[MAX_DSPEW_TYPES]`, `dspew_number[MAX_DSPEW_TYPES]` → `std::array<T,N>`. **Bias:** `dspew[MAX_DSPEW_TYPES]` and `death_types[MAX_DEATH_TYPES]`/`death_probabilities[MAX_DEATH_TYPES]` are NOT plain fixed-width `std::array` members — see note. | **A.1 HOG-loaded (Table.gam GENERIC page)** — reader `mng_ReadNewGenericPage` loops the full counts for `sounds`/`dspew_percent`/`dspew_number` (fixed width). **Note:** `dspew[]` itself is *not* serialized at all (the reader/writer only touches the scalar `f_dspew`), and `death_types[]`/`death_probabilities[]` are **count-prefixed** on disk (reader reads `n_death_types`; absent for version <22) → use `std::vector` sized to the count (clamp `MAX_DEATH_TYPES`). |
| `anim_elem` (`lib/objinfo.h:381`) | `elem[NUM_ANIMS_PER_CLASS]` |
| `t_ai_info` (`lib/objinfo.h:394`) | `melee_damage[2]`, `melee_latency[2]`, `sound[MAX_AI_SOUNDS]` |
| `ship` (`lib/ship.h:146`) | `static_wb[MAX_PLAYER_WEAPONS]`, `fire_flags[]`, `max_ammo[]`, `firing_sound[]`, `firing_release_sound[]`, `spew_powerup[]` | **A.1 HOG-loaded** ship page reader fills these; convert with reader. |
| `weapon` (`lib/weapon.h:341`) | `sounds[MAX_WEAPON_SOUNDS]` | A.1 HOG-loaded. |
| `powerup` (`lib/powerup.h:95`) | `sounds[MAX_POWERUP_SOUNDS]` |
| `poly_wb_info` / `otype_wb_info` / `dynamic_wb_info` (`lib/robotfirestruct.h:35,85,143`) | `gp_index[MAX_WB_GUNPOINTS]`, `turret_index[MAX_WB_TURRETS]`, `gp_weapon_index[]`, `gp_fire_masks[MAX_WB_FIRING_MASKS]`, `gp_fire_wait[]`, `gp_level_weapon_index[MAX_WB_UPGRADES]`, `gp_level_fire_sound_index[]`, `anim_start_frame[]/anim_fire_frame[]/anim_end_frame[]/anim_time[]`, `norm_turret_angle[]/turret_next_think_time[]/turret_direction[]` | **P2** (used by A.1 generic/ship pages via `otype_wb_info`). |
| `player` (`lib/player_external_struct.h:104`) | `weapon_ammo[MAX_PLAYER_WEAPONS]`, `weapon[2]`, `ball_r[3]`, `ball_g[3]`, `ball_b[3]`, `tracker_id[TRACKER_ID_LEN]→std::array<char,N>` |
| `pilot` (`lib/pilot_class.h:168`) | `PrimarySelectList[MAX_PRIMARY_WEAPONS]`, `SecondarySelectList[MAX_SECONDARY_WEAPONS]`, `taunts[MAX_PILOT_TAUNTS][PILOT_TAUNT_SIZE]→std::array<std::array<char,PILOT_TAUNT_SIZE>,N>` (or `std::vector<std::string>`), `controls[NUM_CONTROLLER_FUNCTIONS]`, `mouse_sensitivity[N_MOUSE_AXIS]`, `joy_sensitivity[N_JOY_AXIS]` |
| `team` (`lib/player.h:414`) | `name[CALLSIGN_LEN+1]` → `std::string` |
| `tPosHistory` (`lib/object.h:803`) | `pos[MAX_POSITION_HISTORY]` | runtime ring; `std::array` OK |
| `shard_info_s` / `splinter_info_s` (`lib/object_external_struct.h:253,311`) | `points[3]`, `u[3]`, `v[3]`, `verts[MAX_VERTS_PER_SPLINTER]` | `std::array`. |
| `fvi_info` (`physics/findintersection.h:310`) | `hit_type[MAX_HITS]`, `hit_face_pnt[]`, `hit_face_room[]`, `hit_face[]`, `hit_wallnorm[]`, `hit_object[]`, `hit_subobject[]`, `roomlist[MAX_FVI_SEGS]` | Hot pick/FVI path in the editor — `std::array` is layout-unchanged and safe; keep `max_so_far` counter. |
| `ambient_life` (`game/aiambient.h`) | `m_type[MAX_AL_TYPES]`, `m_total[]/m_max[]/m_min[]/m_flags[]/m_cur_num[]`, `m_handle[MAX_AL_TYPES][MAX_ALS_PER_TYPE]→std::array<std::array<int,MAX_ALS_PER_TYPE>,MAX_AL_TYPES>`, `m_next_size[]`, `m_next_do_time[]` |
| `lgoal` / `levelgoals` (`game/levelgoal.h`) | `m_item[MAX_GOAL_ITEMS]`, `m_goal[MAX_LEVEL_GOALS]`, `m_active_primaries[]`, `m_active_secondaries[]` |
| `terrain_sky` / `lodoff` / `terrain_mine_list` (`game/terrain.h`) | `save_delta[MAX_TERRAIN_LOD]`; `horizon_vectors[MAX_HORIZON_PIECES][6]`, `horizon_u[][5]`, `horizon_v[][5]`, `satellite_*[MAX_SATELLITES]`, `star_vectors[MAX_STARS]`, `satellite_vectors[]`, `satellite_flags[]`, `satellite_size[]`, `satellite_texture[]`, `star_color[]`, `mine_segs[50]` | **P2** (terrain loader is CFILE-stub/dead). |
| `megacell` (`lib/megacell.h:34`) | `texture_handles[MAX_MEGACELL_WIDTH*MAX_MEGACELL_HEIGHT]` | `std::array<int16_t, N>`. A.1 HOG-loaded. |
| `mngs_texture_page` (`lib/texpage.h`, `manage/texpage.h:41`) | `proc_palette[256]` → `std::array<uint16_t,255>` (reader loops `i<255`, guarded by `flags.procedural`). **Bias:** `proc_type[MAX_PROC_ELEMENTS]`, `proc_frequency[]`, `proc_speed[]`, `proc_size[]`, `proc_x1[]`, `proc_y1[]`, `proc_x2[]`, `proc_y2[]` are **count-prefixed** on disk (`num_proc_elements` read first, clamped to `MAX_PROC_ELEMENTS`) → `std::vector<...>` sized to the count. | **A.1 HOG-loaded** (reader `mng_ReadNewTexturePage` fills index-by-index). |
| `ct_function` (`lib/controller.h:141`, `ddio/controller.h`) | `ctype[2]`, `value[2]`, `flags[2]` | `std::array`. |
| `tJoyPos` (`lib/joystick.h:137`) | `pov[JOYPOV_NUM]` | `std::array<unsigned,N>`. |
| `sdlgameController` nested tables (`lib/sdlcontroller.h`) | `normalizer[CT_NUM_AXES]`, `sens[]`, `sensmod[]`, `ctl[CTLBINDS_PER_FUNC]`, `value[]`, `ctype[]`, `pov[JOYPOV_NUM]`, `povstarts[][JOYPOV_DIR]`, `povtimes[][JOYPOV_DIR]`, `povpresses[][]`, `btnpresses[CT_MAX_BUTTONS]`, `btnstarts[]`, `btntimes[]`, plus member arrays `m_ControlList[CT_MAX_CONTROLLERS]`, `m_ElementList[CT_MAX_ELEMENTS]`, `m_ExtCtlStates[CT_MAX_EXTCTLS]` | P1 (input subsystem; unused in editor). |
| `matrix`/`matrix4` union overlay (`lib/vecmat_external.h:320,345`) | `a2d[3][3]`/`a1d[9]` (and `a2d[4][4]`/`a1d[16]`) | **skip** — anonymous-union overlay relied on by GL/vector math; `vector3`/`vector4`/`angvec` already derive from `std::array`. |
| `tQueue<T>` (`lib/psclass.h:154`) | `m_items[t_LEN]` | `std::array<T,N>` (template — direct swap). |
| `bms_bitmap` (`lib/bitmap.h:63`) | `name[BITMAP_NAME_LEN]` | `std::string` |

### B.3 `char x[N]` buffers → `std::string` (P0/P1)

Convert with the corresponding reader-writer pair updated (strings on disk are
fixed-width-NUL or `fixed_string_t<N>`; `std::string` writes the same bytes).

| Struct (header) | Members |
|---|---|
| `tMission` / `tMissionInfo` (`game/Mission.h:239,262`) | `name[MSN_NAMELEN]`, `author[]`, `desc[MSN_NAMELEN*4]`, `email[MSN_URLLEN]`, `web[MSN_URLLEN]`; `keywords[MAX_KEYWORDLEN]` | A.2 mission files; engine HOG-resident. |
| `bms_bitmap` (`lib/bitmap.h:63`) | `name[BITMAP_NAME_LEN]` | A.1 textures; `bm_LoadBitmapFromMemory` sets it from filename. |
| `ddgr_surface` (`lib/Ddgr.h:168`) | `name[SURF_NAMELEN]` |
| `tD3XPMap` (`lib/d3x_op.h:293`) | `name[MAX_D3XID_NAME]` |
| `gamemode` (`lib/game.h:294`) | `scriptname[64]` |
| `tDLLOptions` (`lib/game2dll.h:59`) | `game_name[MAX_GAMENAME_LEN]`, `requirements[MAX_REQUIREMENT_LEN]` |
| `modem_list` (`lib/directplay.h:54`) | `name[200]` | low priority (multiplayer). |
| `tFFJoyInfo` (`lib/forcefeedback.h:143`) | `Name[kMAX_Str]` |
| `tJoyInfo` (`lib/joystick.h:109`) | `name[128]` | P1 (input). |
| `netplayer`/`network_game`/`netgame_info` (`lib/multi_external.h:178,213,251`) | `ship_logo[_MAX_PATH]`, `voice_taunt1..4[]`, `name[NETGAME_NAME_LEN]`, `mission[]`, `mission_name[]`, `scriptname[]`; `digest[16]` → `std::array<uint8_t,16>` | multiplayer-only → dead code in editor; P2. |
| `network_address` (`lib/networking.h:330`) | `address[6]`, `net_id[4]` | MP; P2. |
| `hlsSystem::m_error_text[512]` (`sndlib/hlsoundlib.h:472`) | one error buffer → `std::string` |
| `gr_font_record` (`2dlib/gr.h:165`) | `name[32]`, `filename[32]` → `std::string`; `bmps[MAX_FONT_BITMAPS]` → `std::vector<int>` (plus WAS-removed pointer array `surfs[MAX_FONT_BITMAPS]`) |
| `tCharProperties` (`2dlib/gr.h:133`) | `col[4]` → `std::array<ddgr_color,4>` |
| `tFontFileInfo2` (`lib/grtext.h:273`) | `reserved[62]` → `std::array<char,62>` | Pad blob; `skip` unless the whole font IIO is modernized. |
| `TCTEXTDESC`/`TCBMPDESC`/`TCMOVIEDESC`/`TCPOLYDESC`/`TCSNDDESC`/`TCBUTTONDESC` (`game/TelComEfxStructs.h:169-282`) | all `char xxx[MAX_FILELEN]` filenames → `std::string` | TelCom cinematic effects; P2. |

### B.4 Serialization caution flag (most important)

For **every** member above that lives in an A.1/A.2 struct, on-disk layout must
be reproduced exactly by the paired writer. Concretely:

- `texture`, `weapon`, `door`, `ship`, `sound_info`, `object_info`, `megacell`
  and every `mngs_*_page`: their readers live in
  `mini/manage/{generic,ship,weapon,tex,sound,door,mega}page.cpp` and the 
  assignment step in `gamedata_loader.cpp`. Change reader + table-assignment
  together, then re-verify against the **retail `d3.hog`** (there is a
  `testGamedataTableLoads` + `testPosixReadsHogGamedata` in
  `editor_test.cpp` guarding this).
- `room`/`object`/`trigger`: `mini/level_loader.cpp` `LL_Read*`/`LL_Write*`
  (chunked). Extended test `testRoundTripRoom*/testSaveLoadEncodesFields`
  guard the `.d3l` encoding.
- Strings: ship `name`/`cockpit_name`/`hud_config_name` are **fixed-width
  `PAGENAME_LEN` on disk** (already `std::string` — written via `fixed_string_t`);
  room `name` is **variable-length NUL-terminated**. Do not change one to the
  other (`mini/CFILE_TO_POSIX_STREAM.md` §caveats).

### B.5 Already-STL (no further work)

`room::name` (undefined std::string), `object::name`, `texture::name`,
`ship::{name,cockpit_name,hud_config_name}`, `weapon::name`, `door::{name,module_name}`,
`trigger::name`, `vclip::name`, `fireball::name`, `gamepath.name`,
`megacell::name`, `powerup::name`, `bsp_info::name`, `poly_model::name`,
`sound_info::name`, `sound_file_info::name`, `level_info::{name,designer,copyright,notes}`,
`mngs_generic_page` string arrays
(`sound_name[MAX_OBJ_SOUNDS]`, `ai_sound_name[]`, `weapon_name[][MAX_WB_GUNPOINTS]`,
`fire_sound_name[][]`, `anim_sound_name[][]`, `dspew_name[]` — these are
`std::string` **arrays**, the next step is `std::vector<std::string>` with the
readers resizing),
`mngs_*_page` other string fields (ship `weapon_name[][]`/`fire_sound_name[][]`,
weapon `sound_name[]`, sound `raw_name`, door `image_name`/`open_sound_name`/
`close_sound_name`), `pilot` class strings, `chunked_bitmap::data` (`std::vector<uint8_t>`),
`HogFormat`/`HogFileEntry`/`HogHeader` (A.3), `CFileInfo` filename strings,
`vector3`/`vector4`/`angvec` (derive from `std::array<T,N>`).

### B.6 Global fixed indexes (not members — flagged separately)

The editor's world state still uses global fixed arrays with parallel `Num_*`
counters: `room Rooms[MAX_ROOMS]`, `object Objects[MAX_OBJECTS]`,
`trigger Triggers[500]`, `GameTextures[MAX_TEXTURES]`, `Weapons[MAX_WEAPONS]`,
`Ships[MAX_SHIPS]`, `Object_info[MAX_OBJECTS]`, `Sounds[MAX_SOUNDS]`,
`Doors[MAX_DOORS]`, `Megacells[MAX_MEGACELLS]`, `GameBitmaps[]`,
`Poly_models[]`, `GamePaths[MAX_GAME_PATHS]`, `Players[MAX_PLAYERS]`,
terrain segment arrays. Converting these globals to `std::vector<T>` is a
larger architectural change (index stability, `OBJNUM`/`ROOMNUM` macros,
`array-of-object-for-graphics` assumptions). It pairs naturally with the B.1
member conversions but should be a separate work item.

---

## Recommended order of work

1. **`room`** (B.1): `verts`/`faces`/`portals`/`verts4` → `std::vector`, and the
   BBF jagged arrays → `std::vector<std::vector<>>`. Highest editor value;
   guarded by load/save round-trip tests. (P0)
2. **HOG table types** (B.2): `object_info`/`ship`/`weapon`/`sound_info`/
   `megacell` fixed members → `std::array` (bounds fixed on disk); the verified
   **count-prefixed** members (`object_info::death_types`/`death_probabilities`,
   `mngs_texture_page::proc_type`/`proc_frequency`/`proc_speed`/`proc_size`/
   `proc_x1`/`proc_y1`/`proc_x2`/`proc_y2`) → `std::vector` sized to the
   on-disk count; `mngs_*_page` `std::string` arrays →
   `std::vector<std::string>`. Re-run
   `testGamedataTableLoads`/`testPosixReadsHogGamedata` against retail `d3.hog`. (P0)
3. **String cleanup** (B.3): mission, bitmap, font, input tables → `std::string`
   / `std::array`. (P1)
4. **`object`** (B.1 partial): `attach_children` → `std::vector<int32_t>`,
   script name pointers → `std::string`. (P1)
5. **Navigation/AI** (B.1/B.2): `bn_list`/`bn_node`, `game_path`, `fvi_info`
   (`std::array`), then the `aistruct` family. (P1)
6. **Models** (P2): `poly_model`, `bsp_info`, `polyface`, `lightmap_object`
   only when model rendering is (re)enabled.
7. **Dead/MP code** (P2, or delete per "multiplayer dependent code is removed"):
   terrain/vclip/sound `CFILE`-stub loaders, `multi_external`, `networking`,
   `sdlcontroller`, `TelComEfxStructs`.

Every step: convert struct + reader + writer in one commit, then run
`cmake --build builds/linux/ --target qteditor_tests` and the full offscreen
suite (minus the two known crashers), and rebuild `qteditor`.