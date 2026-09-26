# `slotvec_t` candidates — `std::vector` tables that can adopt the slot container

**Scope:** `repo/qteditor/mini` (independent Qt port).
**Purpose:** `slotvec_t` (`qteditor/mini/slotvec.h`) replaces the
"`std::vector<T>` + separate `Num_*` counter + `used` field inside `T`"
pattern.  A table is a **candidate** when its element type carries a `used`
slot/refcount member that is managed by find-free/append or a free list, and
(optionally) a companion `Num_*` counter exists purely to bound that scanning.

Legend: **primary** = free-list / find-free slot allocator using `used`
(refcount) → cleanest fit.  **table-load** = per-row `used = 1` occupancy flag,
slots are never freed individually → mechanical fit, drops the `used` member.

---

## 1. Primary candidates (slot allocators with a `used` refcount + free list)

| `std::vector<T>` table | element `used` member | `Num_*` counter | free list | alloc/free | status |
|---|---|---|---|---|---|
| `GameLightmaps` (`std::vector<bms_lightmap>`) | `bms_lightmap::used` (lightmap.h:39) | `Num_of_lightmaps` (lightmap.cpp:19) | `Free_lightmap_list` (lightmap.cpp:20) | `lm_AllocLightmap` / `lm_FreeLightmap` | **DONE** |
| `SpecialFaces` (`std::vector<special_face>`) | `special_face::used` (special_face.h:45) | `Num_of_special_faces` (special_face.cpp:20) | `Free_special_face_list` (special_face.cpp:21) | `AllocSpecialFace` / `FreeSpecialFace` | **DONE** |
| `LightmapInfo` (`std::vector<lightmap_info>`) | `lightmap_info::used` (lightmap_info.h:64) | `Num_of_lightmap_info` (lightmap_info.cpp:20) | `Free_lmi_list` (lightmap_info.cpp:23) | `AllocLightmapInfo` / `FreeLightmapInfo` | **DONE** |

`slotvec_t<T>` gives these `num_empty()` for the free-list size and lets
`acquire`/`release` replace the manual `used++/used--`.

## 2. Primary candidates (find-free slot registry, refcount `used`)

| `std::vector<T>` table | element `used` member | allocator |
|---|---|---|
| `GameBitmaps` (`std::vector<bms_bitmap>`) | `bms_bitmap::used` (bitmap.h:58) | `bm_AllocBitmap` scans for `used == 0`; `bm_FreeBitmap` decrements the refcount |

## 3. Table-load candidates (per-row `used = 1`, no per-slot freeing)

| `std::vector<T>` table | element `used` member | where `used` is set |
|---|---|---|
| `GameTextures` (`std::vector<texture>`) | `texture::used` (gametexture.h:352) | texpage.cpp:97 | **DONE** |
| `Weapons` (`std::vector<weapon>`) | `weapon::used` (weapon.h:329) | weaponpage.cpp:270 | **DONE** |
| `Ships` (`std::vector<ship>`) | `ship::used` (ship.h:179) | shippage.cpp:120 | **DONE** |
| `Sounds` (`std::vector<sound_info>`) | `sound_info::used` (ssl_lib.h:363) | soundpage.cpp:77 | **DONE** |
| `GamePaths` (`std::vector<game_path>`) | `game_path::used` (gamepath.h:75) | level loader (`Num_game_paths` in level_loader.cpp:1116) | **DONE** |

For these, `used` was previously believed to be byte-for-byte level/table data
on disk, but serialization audit proved it is **not**: the `used = 1` assignment
happens in-memory after each row is read (`texpage.cpp:97`, `weaponpage.cpp:270`,
`shippage.cpp:120`, `soundpage.cpp:77`), and `GamePaths[i].used` only marks
occupied entries during the PATH chunk read/write rather than being stored.
Nothing serializes `used`.  They therefore drop the `used` member outright and
rely on `slotvec_t` refcounts (`acquire` on load).

## 4. Borderline / excluded

| table | why excluded |
|---|---|
| `Rooms` (`std::vector<room>`) | `room::used` marks on-disk room occupancy; `Rooms` is grown by `RoomsEnsureIndex`, rooms are never individually freed — level-slot index semantics (ROOMNUM), audit keeps it as an indexed registry |
| `Triggers` (`std::vector<trigger>`) | no `used` member; plain list, no slot allocation |
| `Matcen` (`std::vector<matcen *>`) | pointer table; `Num_matcens` only, no `used` member in `matcen` |
| `Objects` (`std::array<object, MAX_OBJECTS>`) | not `std::vector`; handle/slot registry (kept) |
| `GameVClips`, `Doors`, `Megacells`, `Poly_models`, `Room_changes` | plain C arrays / `used` slot tables (see `Num_MAX_audit.md` §4) — `slotvec_t` requires `std::vector` |

## 5. Summary

Adopt `slotvec_t` for these six `std::vector` tables first:

1. `GameLightmaps`
2. `SpecialFaces`
3. `LightmapInfo`
4. `GameBitmaps`
5. `GameTextures` (occupancy only)
6. `Weapons`, `Ships`, `Sounds`, `GamePaths` (occupancy only)

All six are migrated: each drops its `used` member from the element struct and
relies on `slotvec_t::acquire/release` plus `num_empty()` for free-list/scan
behavior found today.  `GamePaths` keeps the `Num_game_paths` counter (used by
the PATH chunk and Osiris predefs).  Tables are loaded slot-zero-relative via
`GamePaths.resize(N)` + `acquire`, and `slotvec_t::resize` maintains the
empty-slot count so a later `next_slot()`/`acquire` accounting stays correct.