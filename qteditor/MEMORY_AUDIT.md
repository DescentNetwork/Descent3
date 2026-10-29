# Memory-Management Audit — `qteditor/mini/`

Audit date: 2026-09-06
Scope: every OWNED raw pointer used for dynamic storage in the Qt-port
engine-replacement tree `qteditor/mini/` that should become C++ RAII
management (`std::unique_ptr`, `std::shared_ptr`, `std::vector`, `std::string`)
so that deallocation is automatic. Companion to `STL_CONTAINER_AUDIT.md`,
which covers fixed-size arrays / array-of-`char` members. This audit covers the
`pointer + manual alloc/free` storage sites.

Allocator lines: `mini/mem/mem.h` (wrappers) and `mini/lib/mem.cpp`:
`mem_malloc`/`mem_free` → `malloc`/`free`; `mem_rmalloc<T>(n)`/`mem_rmfree` →
`new T[n]`/`delete[]`; `mem_rmalloc<T>()` → `new T[1]`.

Conversion rule of thumb:
- `mem_malloc(n*sizeof(T))` + `mem_free` → `std::vector<T>` or `std::unique_ptr<T[]>`.
- `mem_rmalloc<T>(n)` + `mem_rmfree` → `std::vector<T>` (preferred) or `std::unique_ptr<T[]>`.
- `mem_rmalloc<T>()` single object → `std::unique_ptr<T>` (`room::doorway_data`).
- `new T[n]` / `new T` → `std::unique_ptr<T[]>` / `std::unique_ptr<T>` / containers.
- The reference RAII shape already used in the tree: `mini/bitmap/NewBitmap.h:35`
  `std::unique_ptr<uint8_t[]> _data;` with `getData()` returning `.get()`.

---

## 1. `object` runtime-info fields (`ai_info`, `effect_info`, `dynamic_wb`, `lighting_info`, `osiris_script`, `multi_turret` arrays)

Declarations — `mini/lib/object_external_struct.h`:
- 542 `dynamic_wb_info *dynamic_wb;` (owned)
- 571 `ai_frame *ai_info;` (owned)
- 583 `effect_info_s *effect_info;` (owned)
- 584 `light_info *lighting_info;` (owned, may be NULL)
- 591 `tOSIRISScript *osiris_script;` (owned)
- 228-229 `float *keyframes; float *last_keyframes;` inside `struct multi_turret` (owned)

Init: `mini/game/object.cpp` `ObjInit` — 489 value-init, 498/514-516 set to `nullptr`.
Allocation sites: NONE (mini strips the runtime subsystems that allocated these).
Free sites: `mini/game/object.cpp` `ObjDelete` — 604-605 (`multi_turret` arrays),
631-634 (`effect_info`), 636-639 (`ai_info`), 641-644 (`dynamic_wb`), 650-653 (`lighting_info`).

Crash risk: `mini/lib/osiris_predefs.cpp:2659` dereferences `objp->dynamic_wb[...]`
(always NULL; UB), used live at 2686/2695/2697/2703-2709. `osiris_predefs.cpp:971`
reads `objp->ai_info` without a null check.

Recommendation: all five fields → `std::unique_ptr<...>`; `multi_turret::keyframes`
/`last_keyframes` → `std::vector<float>`. `ObjDelete` becomes `.reset()` / nothing.

## 2. `room::doorway_data`

- Decl `mini/game/room_external.h:286` `doorway* doorway_data = nullptr;`
- Alloc `mini/game/doorway.cpp:649` (`DoorwayAdd`): `mem_rmalloc<doorway>()`; asserts NULL first at 645.
- Free `mini/game/room.cpp:614-615` (`FreeRoom`): `mem_rmfree` — pointer is NOT reset, so a freed+reused room trips the `Q_ASSERT(== NULL)` at doorway.cpp:645 or double-frees.

Recommendation: `std::unique_ptr<doorway>`; `std::make_unique<doorway>()` in `DoorwayAdd`;
drop the free block in `FreeRoom`. Smallest, highest-crash-value fix.

## 3. Terrain globals

Declarations `mini/game/terrain.h` / definitions `mini/game/terrain.cpp`:
- 237/70 `terrain_normals *TerrainNormals[MAX_TERRAIN_LOD];`
- 281/73 `float *TerrainDeltaBlocks[];`
- 282/63 `uint8_t *Terrain_max_height_int[7];`
- 283/62 `uint8_t *Terrain_min_height_int[7];`
- 295/58 `uint16_t *Terrain_rotate_list;`
- 296/59 `g3Point *World_point_buffer;`

Alloc `InitTerrain` — 956, 959, 967, 976, 986, 987. Frees `CloseTerrain` —
620-621, 623-628, 630 (**no null guard**), 631, 633-634. **No pointer is reset
after free → the second `CloseTerrain` double-frees.**

Recommendation: `std::vector<g3Point>`, `std::vector<uint16_t>`,
`std::array<std::vector<float>, MAX_TERRAIN_LOD>`,
`std::array<std::vector<terrain_normals>, MAX_TERRAIN_LOD>`,
`std::array<std::vector<uint8_t>, 7>` x2. `CloseTerrain` empties break the
double-free hazard. Local `lando` (823/849) → `std::vector<uint8_t>`.

## 4. Lighting statics

`mini/game/lighting.cpp`:
- 66 `static uint16_t *Dynamic_lightmap_memory = NULL;`
- 72 `static dynamic_lightmap *Dynamic_lightmaps;`
- Alloc `InitDynamicLighting` 141, 144; memset loop 147-149.
- Free `FreeLighting` 110-113 — neither reset to NULL → double-free on repeated init/free.

`dynamic_lightmap::mem_ptr` (`mini/game/lighting.h:43`) is a **BORROWED** interior
pointer into `Dynamic_lightmap_memory` — do NOT convert to owning.

Recommendation: `std::unique_ptr<uint16_t[]>` for the memory pool,
`std::vector<dynamic_lightmap>(MAX_DYNAMIC_LIGHTMAPS)` for the array (kills the memset loop).

## 5. `bms_bitmap::data16` — leak, no free site

- Decl `mini/lib/bitmap.h:54` `uint16_t *data16;` (in `struct bms_bitmap` 53-64).
- Alloc `mini/lib/bitmap.cpp:36` `bm_AllocBitmap` (`mem_malloc(base+extra)`), memset 41.
- Free sites: **NONE** — `bm_FreeBitmap` (53-56) only decrements refcount;
  `bm_ShutdownBitmaps`/`bm_FreeBitmapData` (bitmap.h:77,147) are not defined.
  Every bitmap reload leaks `w*h*2` bytes.
- Writers for conversion impact: `mini/bitmap/iff.cpp:813,815,835,837,858,860` (via `.get()`).

Recommendation: `std::unique_ptr<uint16_t[]>` (per `NewBitmap.h:35`); `bm_FreeBitmap`
refcount 0 → `.reset()`. `chunked_bitmap::bm_array` (`bitmap.h:69`, never allocated) → `std::vector<int>`.

## 6. `iff.cpp` file-scratch buffers

- `iff_bitmap_header::raw_data` (`uint8_t*`, iff.cpp:108): alloc 385, 401 (+memcpy 406);
  freed at 517 via `mem_free` AND at 523 via **plain `free()`** — allocator-pair mismatch.
  Recommendation: `std::vector<uint8_t>` (also resolves double-owner risk in ANHD case).
- `Tga_file_data` (`static char*`, iff.cpp:534): alloc 877, free 891-894 (well-paired but
  file-static). Recommendation: function-local `std::vector<char>`.

## 7. `osipf_AIGetNearbyObjs` `s_list` (function-local)

`mini/lib/osiris_predefs.cpp:3329` → alloc 3334 `mem_rmalloc<int16_t>(max_elements)`,
free 3345. Correctly paired temp. Recommendation: `std::vector<int16_t>`.

## 8. `object_info::description` (owned C-string)

- Decl `mini/lib/objinfo.h:510` `char *description;`
- Init `mini/manage/genericpage.cpp:158,304,308`; read 646-649.
- Frees `mini/manage/table_manage.cpp:77-79` (`~GenericPageNode`), 298-300 (`SaveTable`),
  swap dance 93-101 (`setDescription`: alloc `next`, copy, then `mem_rmfree(old)`).

Recommendation: `std::string description;` — `setDescription` becomes plain assignment;
`SaveTable` temp-page cleanup disappears.

## 9. `BOA.cpp` AABB-computation locals + global fixed tables

Locals in `compute_BOA_AABB` (`mini/game/BOA.cpp`): alloc 2131 (`num_structs_per_room`),
2135+2141 (`r_struct_list`, `int16_t**` 2D), 2237 (`nfaces`), 2238 (`used`), 2312-2313
(`s_max_xyz`/`s_min_xyz`). Frees 2289-2290, 2370-2371, 2618-2627 (goto-based manual frees;
only `num_structs_per_room`/`r_struct_list` nulled at 2628-2629).

Recommendation: `std::vector<int16_t>`, `std::vector<std::vector<int16_t>>`,
`std::vector<int16_t>`, `std::vector<bool>`, `std::vector<vector3>` x2 — the goto frees vanish.

Global fixed tables `mini/game/BOA.cpp:174-175,187-188` (`BOA_Array`, `BOA_cost_array`,
`BOA_num_connect`, `BOA_connect`) → `std::array` for parity only (no heap).

## 10. `bnode` path-find working arrays

`mini/game/bnode.cpp:225` `pq_item **node_list` → alloc 231-232 (`mem_rmalloc` + memset),
per-node `new pq_item(...)` at 217, 271; frees 283-286 (`delete` loop), 288 (`mem_rmfree`).

Recommendation: `std::vector<std::unique_ptr<pq_item>>`; `start_node` via `std::make_unique`.
Lines 232/283-288 disappear.

## 11. `polymodel` `start_index` temp

`mini/model/polymodel.cpp:1409` `int *start_index` → alloc 1412, free 1466 (conditional).
Correctly paired temp. Recommendation: `std::vector<int> start_index(nfaces);`.

## 12. Fixed-size arrays + `Buddy_handle` link landmine

- `mini/game/gamestub.cpp:27` `AI_RenderedList[MAX_OBJECTS]` (decl AIMain.h:160) — parity only.
- `mini/game/gamestub.cpp:31` `fireball Fireballs[NUM_FIREBALLS]` (decl fireball.h:190) — parity only.
- `mini/game/gamestub.cpp:33` `vis_effect *VisEffects = new vis_effect[MAX_VIS_EFFECTS]();` —
  decl `mini/lib/viseffect.h:118`; raw `new[]`, **never deleted**. Recommendation:
  `std::unique_ptr<vis_effect[]>` via `std::make_unique<vis_effect[]>(MAX_VIS_EFFECTS)`
  (preserves zero-init) or `std::vector<vis_effect>`.
- `mini/game/doorway.cpp:144-145` `Active_doorways[MAX_ACTIVE_DOORWAYS]`
  (decl doorway.h:122-123) — `std::array<int, MAX_ACTIVE_DOORWAYS>` for parity.
- **`Buddy_handle` (`mini/game/AIMain.h:168`) has NO definition in the qteditor tree** —
  only refs at `mini/lib/osiris_predefs.cpp:803,2854`; the sole definition is the engine's
  `repo/Descent3/AImain.cpp:1628` (not linked). Any link pulling those functions fails.
  Fix: define `std::array<int, MAX_PLAYERS> Buddy_handle` in gamestub.cpp, or drop the two refs.

## 13. `pilot` class dormant char arrays

`mini/lib/pilot_class.h`: 278 `char *name`, 279 `char *ship_logo`, 280 `char *ship_model`,
281-284 `char *audio1_file..audio4_file`, 285 `char *guidebot_name`, 298 `tMissionData *mission_data`,
306 `char taunts[MAX_PILOT_TAUNTS][PILOT_TAUNT_SIZE]`. Dormant (pilot.cpp is a 14-line stub) —
converting now removes the future ownership burden. Recommendation: `std::string`,
`std::vector<tMissionData>`, `std::array<std::string, MAX_PILOT_TAUNTS>`.

## 14. `stubs.cpp` globals

- 398 `oeApplication *Descent` — owned, `new`'d at `d3_editor_init.cpp:60` and again `:118`, never deleted.
- 399 `oeAppDatabase *Database` — owned, `new`'d at `:61` and `:119`, never deleted.
  Double-init leaks. Recommendation: `std::unique_ptr<oeApplication>` / `std::unique_ptr<oeAppDatabase>`;
  init via `std::make_unique`.
- 280 `matcen *Matcen[MAX_MATCENS]` — array of null pointers; → `std::array` for parity.
- Borrowed (do NOT convert): 112-113 `Viewer_object`/`Player_object` (into `Objects`),
  240 `LightmapInfo` (into `LightmapInfoStore`).

## Bonus findings

- **B1.** `mini/brief_model.cpp:20-21` `PBlayouts`/`PBnum_layouts` — `new[]`/`delete[]`
  (paired). Recommendation: `std::vector<tLayoutPredef>` and `std::vector<int>`.
- **B2.** `mini/manage/table_manage.h:36-37,61-71` intrusive `GenericPageNode` linked list
  (`prev`/`next`, `m_head`/`m_tail`/`m_SelectedNode`; `new` at table_manage.cpp:216,
  `delete` loop at 120-135). Leak-free today; medium-effort follow-up: `std::list` / smart pointers.
- **B3.** Reference RAII pattern already in tree: `mini/bitmap/NewBitmap.h:35`.

---

## Prioritized summary

Priority = crash/UB risk first, then leaks, then hygiene/parity.

| # | Item | Decl → alloc → free | Issue | Conversion |
|---|---|---|---|---|
| 1 | `room::doorway_data` | room_external.h:286 → doorway.cpp:649 → room.cpp:614-615 | dangling after free; assert/double-free | `std::unique_ptr<doorway>` |
| 2 | `Dynamic_lightmaps`/memory | lighting.cpp:66,72 → 141,144 → 109-114 | double-free, no NULL reset | `std::vector` + `std::unique_ptr<uint16_t[]>` |
| 3 | Terrain globals (6) | terrain.cpp:58-73 → 956-987 → 617-635 | double-free, unguarded 630 | `std::vector` / `std::array<std::vector,…>` |
| 4 | `object` fields | object_external_struct.h:228-229,542,571,583-584,591 → (none) → object.cpp:604-605,632-653 | always-null deref at osiris_predefs.cpp:2659 | `std::unique_ptr` members |
| 5 | iff `raw_data` | iff.cpp:108 → 385,401 → 517 & 523 | allocator mismatch (`free` vs `mem_free`) | `std::vector<uint8_t>` |
| 6 | `Tga_file_data` | iff.cpp:534 → 877 → 891-894 | static scratch | local `std::vector<char>` |
| 7 | `bms_bitmap::data16` | bitmap.h:54 → bitmap.cpp:36 → none | leak on reload; no shutdown | `std::unique_ptr<uint16_t[]>` |
| 8 | `VisEffects` | viseffect.h:118 → gamestub.cpp:33 → never deleted | raw `new` global, leak | `std::unique_ptr<vis_effect[]>` |
| 9 | `Buddy_handle` | AIMain.h:168 decl; refs osiris_predefs.cpp:803,2854 — no def | link landmine | define `std::array` in gamestub / drop refs |
| 10 | `description` char* | objinfo.h:510 → genericpage.cpp:304 → table_manage.cpp:77-79,93-101,298-300 | owned C-string swap dance | `std::string` |
| 11 | BOA AABB locals | BOA.cpp:2131-2313 → frees 2289-2371,2618-2627 | goto-based frees | `std::vector` |
| 12 | BOA globals | BOA.cpp:174-175,187-188 | parity only | `std::array` |
| 13 | bnode `node_list` | bnode.cpp:225 → 231-232,217,271 → 283-288 | new/delete mix | `std::vector<std::unique_ptr<pq_item>>` |
| 14 | polymodel `start_index` | polymodel.cpp:1409 → 1412 → 1466 | paired temp | `std::vector<int>` |
| 15 | `s_list` | osiris_predefs.cpp:3329 → 3334 → 3345 | paired temp | `std::vector<int16_t>` |
| 16 | `lando` | terrain.cpp:823 → 849 | paired temp | `std::vector<uint8_t>` |
| 17 | pilot char arrays | pilot_class.h:278-285,298,306 | dormant — future leak | `std::string`/`std::vector` |
| 18 | `Descent`/`Database` | stubs.cpp:398-399 → d3_editor_init.cpp:60-61,118-119 | double-init leak | `std::unique_ptr` |
| 19 | `bm_array` | bitmap.h:69 | dead member | `std::vector<int>` |
| 20 | `PBlayouts`/`PBnum_layouts` | brief_model.cpp:20-21 → 185-186 → 130-137 | paired new[]/delete[] | `std::vector` |
| 21 | `GenericPageNode` list | table_manage.h:36-37,61-71 → 216 → 120-135 | intrusive linked list | `std::list`/smart ptrs |
| — | Borrowed pointers | stubs.cpp:112-113,240; lighting.h:43 | **do NOT convert** | keep raw |