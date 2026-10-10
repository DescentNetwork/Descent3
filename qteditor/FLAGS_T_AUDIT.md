# `*_flags_t` Bitflag Struct Audit

Following the project guideline that *"integers that are used as bitflags via C macros
must be converted to structs with bit flags"*, every raw flag field in the port is
being replaced by a packed bitflag struct. This document tracks the state of each
`*_flags_t` type declared in a header and identifies the ones that are **defined but
not yet used**.

## Summary

| `*_flags_t` type | Defined in | Used as field type? |
|------------------|------------|---------------------|
| `face_flags_t`        | `game/room_external.h`          | yes — `face::flags` |
| `portal_flags_t`      | `game/room_external.h`          | yes — `portal::flags` |
| `room_flags_t`        | `game/room_external.h`          | yes — `room::flags` |
| `texture_flags_t`     | `game/gametexture.h`            | yes — `texture::flags` |
| `text_caps_flags_t`   | `game/TelComEfxStructs.h`       | yes — `TelComTextInterface::caps` |
| `bmp_caps_flags_t`    | `game/TelComEfxStructs.h`       | yes — `TelComBMPInterface::caps` |
| `movie_caps_flags_t`  | `game/TelComEfxStructs.h`       | yes — `TelComMovieInterface::caps` |
| `bkg_caps_flags_t`    | `game/TelComEfxStructs.h`       | yes — `TelComBackGroundInterface::caps` |
| `poly_caps_flags_t`   | `game/TelComEfxStructs.h`       | yes — `TelComPolyInterface::caps` |
| `snd_caps_flags_t`    | `game/TelComEfxStructs.h`       | yes — `TelComSoundInterface::caps` |
| `button_flags_t`      | `game/TelComEfxStructs.h`       | yes — `TelComButton::osflags` |
| `terrain_segment_flags_t` | `game/terrain.h`            | yes — `terrain_segment::flags` |
| `terrain_satellite_flags_t` | `game/terrain.h`          | yes — `terrain_satellite::satellite_flags` |
| `terrain_sky_flags_t` | `game/terrain.h`               | yes — `terrain_sky::flags` |
| `death_flags_t`       | `lib/deathinfo_external.h`      | yes — `death_info::flags` |
| `death_spew_flags_t`         | `lib/objinfo.h`                 | yes — `object_info::f_dspew` |
| `light_flags_t`       | `lib/object_external_struct.h`  | yes — `light_info::flags` |
| `object_flags_t`      | `lib/object_external_struct.h`  | yes — `object::flags` |
| `effect_flags_t`      | `lib/object_external_struct.h`  | yes — `effect::type_flags` |
| `physics_flags_t`     | `lib/object_external_struct.h`  | yes — `physics_info::flags` |
| `object_info_flags_t` | `lib/objinfo.h`                | yes — `object_info::flags` |
| `dynamic_wb_info_flags_t` | `lib/robotfirestruct.h`    | yes — `dynamic_wb_info::flags` |
| `otype_wb_info_flags_t`   | `lib/robotfirestruct.h`    | yes — `otype_wb_info::flags` |
| `sound_flags_t`       | `lib/ssl_lib.h`                 | yes — `sound_info::flags` |
| `slf_flags_t`         | `lib/ssl_lib.h`                 | no — sound library flags (stubbed, no consumer yet) |
| `sif_flags_t`         | `lib/ssl_lib.h`                 | yes — `sound_object::m_obj_type_flags` |
| `env3dvalf_flags_t`   | `lib/ssl_lib.h`                 | yes — `t3dEnvironmentValues::flags`, `t3dEnvironmentToggles::flags` |
| `trigger_flags_t`     | `lib/trigger.h`                 | yes — `trigger::flags` |
| `activator_flags_t`   | `lib/trigger.h`                 | yes — `trigger::activator` |
| `weapon_flags_t`      | `lib/weapon.h`                  | yes — `weapon::flags` |
| `g3point_flags_t`     | `lib/3d.h`                      | yes — `g3Point::p3_flags` (replaces `PF_*`) |
| `g3_clip_codes_t`     | `lib/3d.h`                      | yes — `g3Point::p3_codes`, `g3Codes::cc_or/cc_and` (replaces `CC_*`) |
| `surface_flags_t`     | `radiosity.h`                   | yes — `rad_surface::flags` (replaces `SF_*`) |
| `rad_element_flags_t` | `radiosity.h`                   | yes — `rad_element::flags` (replaces `EF_*`) |
| `volume_element_flags_t` | `radiosity.h`                | yes — `volume_element::flags` (replaces `VEF_REVERSE_SHOOT`) |

## Migration Status

The types below are the `*_flags_t` types in the port that were **defined but
never used as a field type** (or that replaced a raw integer field). They have
now been migrated; their raw integer fields use the struct types and the macro
sets (`SOF_*`, `PMF_*`, `PEF_*`, `DF_*`) have been removed from the headers
once nothing referenced them.

| `*_flags_t` type | Macro set | Field migrated |
|------------------|-----------|----------------|
| `subobject_flags_t`        | `SOF_*` | `bsp_info::flags` — **done** |
| `polymodel_flags_t`        | `PMF_*` | `poly_model::flags` — **done** |
| `polymodel_effects_flags_t` | `PEF_*` | `polymodel_effect::type` — **done** |
| `door_flags_t`              | `DF_*`  | `door::flags` — **done** |

### 1. `subobject_flags_t` → `bsp_info::flags`

`bsp_info::flags` was `int` and manipulated with the `SOF_*` macros across:

- `model/newstyle.cpp` (SM_ draw code)
- `model/polymodel.cpp` (parsing, weapon-battery wiring, keyframe animation)
- `physics/newstyle_fi.cpp`
- `room_ops.cpp`, `world_objects_door_dialog.cpp`
- `editor_test.cpp` (synthetic door model)

**Special case — WB index:** bits 16-20 of `bsp_info::flags` do not describe a
boolean flag; they carry the packed weapon-battery index
(`SOF_WB_MASKS 0x01F0000`, `WB_INDEX_SHIFT 16`) used by `RecursiveAssignWB`
and `SetModelAnglesAndPos`. The anonymous 5-bit gap in `subobject_flags_t` was
renamed to `wb_index : 5`, keeping the bit positions and therefore byte layout
identical.

### 2. `polymodel_flags_t` → `poly_model::flags`

`poly_model::flags` was `int` and manipulated with the `PMF_*` macros across
`model/polymodel.cpp`, `model/newstyle.cpp` and `game/doorway.cpp`.

### 3. `polymodel_effects_flags_t` → `polymodel_effect::type`

`polymodel_effect::type` was `int` and manipulated with the `PEF_*` macros
across `model/newstyle.cpp`, `model/polymodel.cpp` and
`game/renderobject.cpp`.

## Conversion Notes

- All four structs are `[[gnu::packed]]` bitfields with a `static_assert` that
  their `sizeof` equals the underlying integer width, so they are byte-compatible
  with the raw flag words. The `door_flags_t` byte is the one exception that is
  read/written directly by the table-file serialization (`mngs_door_page`
  operators), so it goes through `reinterpret_cast<uint8_t&>` exactly like the
  other packed one-byte flag structs.
- Each macro test/set is rewritten to the equivalent bitfield member access,
  e.g. `sm->flags & SOF_ROTATE` → `sm->flags.rotate`,
  `sm->flags |= SOF_ROTATE` → `sm->flags.rotate = true`,
  `sm->flags &= ~SOF_ROTATE` → `sm->flags.rotate = false`.
- `SOF_GLOW | SOF_THRUSTER`-style combined masks become `||` of the members.
- The `wb_index` packed field (bits 16-20) is accessed directly as
  `sm->flags.wb_index` instead of
  `(sm->flags & SOF_WB_MASKS) >> WB_INDEX_SHIFT` / `wb_index << WB_INDEX_SHIFT`.
- Function-scope integers that are unrelated to these flag words stay `int`
  (e.g. the per-subobject render bitmask parameters of `SetModelAnglesAndPosTimed`,
  `DrawPolygonModel`'s `flags` parameter).