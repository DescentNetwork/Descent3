# QtEditor Decoupling Refactor Plan

## Overview

This document outlines the plan to decouple the `qteditor` subdirectory project from all other subdirectory projects in the Descent3 codebase, creating an independent, self-contained project in `qteditor/mini/`.

**Goal**: Create a standalone `qteditor/mini/` directory containing only the minimal necessary code and headers required for the editor to function, with all external dependencies either:
1. Copied and trimmed to include only what's needed
2. Replaced with stub implementations
3. Removed entirely if not essential

**Exclusions**: Files in the `descar` directory are explicitly excluded from this decoupling task.

---

## Current State Analysis

### qteditor Source Files
The qteditor directory contains approximately 200+ source files (.cpp/.h):
- Main application: `main.cpp`, `main_window.cpp/h`, `d3edit.h`
- Editor core: `editor_view.cpp/h`, `editor_lighting.cpp/h`, `level_io.cpp/h`
- Dialogs: 80+ dialog classes for various editor functions
- Game integration: `d3_editor_init.cpp/h`, `room_ops.cpp/h`, `object_ops.cpp/h`
- Lighting system: `rad_cast.cpp`, `rad_hemicube.cpp`, `radiosity.h`
- UI components: Various keypad and property dialogs

### Existing mini/ Directory Structure
```
qteditor/mini/
├── fix/
│   └── fix.h              (FIXED: Basic fixed-point math types)
├── lib/
│   ├── 3d.h               (3D rendering structures)
│   ├── pstypes.h          (Platform-specific types)
│   ├── vecmat.h           (Vector/matrix operations - declarations)
│   └── vecmat_external.h  (Vector/matrix operations - implementations)
├── logger/                (Logging utilities)
├── gr.h                   (Graphics definitions)
└── grdefs.h               (Graphics constants/enum)
```

---

## Dependency Analysis

### External Headers Currently Referenced in qteditor

Based on analysis of #include statements, qteditor depends on headers from these directories:

#### From /workspace/lib/ (Core Engine Types)
- `AIMain.h` - AI system main interface
- `BOA.h` - Bitmap Object Array
- `bnode.h` - BSP node structures
- `doorpage.h` - Door page management
- `doorway.h` - Doorway structures
- `findintersection.h` - Collision detection
- `gamepath.h` - Path finding
- `genericpage.h` - Generic page management
- `hogfile.h` - Archive file format
- `lightmap.h` - Lightmap structures
- `LoadLevel.h` - Level loading interface
- `Mission.h` - Mission structures
- `objpage.h` - Object page management
- `polymodel.h` - Polygon model structures
- `room.h` - Room structures
- `TelComEfxStructs.h` - Telecom effects
- `terrain.h` - Terrain structures
- `texcache.h` - Texture caching
- `texture.h` - Texture management
- `trigger.h` - Trigger structures
- `viseffect.h` - Visual effects
- `weapon.h` - Weapon definitions

#### From /workspace/editor/ (Editor-Specific Code)
- `aiambient.h` - Ambient AI sounds
- `appdatabase.h` - Application database
- `application.h` - Application state
- `ebnode.h` - Extended BSP node (already partially in mini/)
- `editor_room_state.h` - Room state management
- `med*.h` - Various editor middleware headers

#### From /workspace/Descent3/ (Game Code)
- `descent.h` - Main game structures
- `game.h` - Game state
- `gameevent.h` - Event system
- `gamefont.h` - Font management
- `object.h` - Object management
- `player.h` - Player structures
- `ship.h` - Ship/vehicle definitions

#### From /workspace/ddio/ (Device I/O)
- `ddio.h` - Device-independent I/O
- `DDgr.h` - Graphics device interface

#### From /workspace/cfile/ (File System)
- `cfile.h` - Custom file handling

#### From /workspace/fix/ (Fixed Point Math)
- `fix.h` - Fixed point arithmetic (already copied to mini/fix/)

#### From /workspace/grtext/ (Graphics Text)
- Various text rendering headers

#### From /workspace/vecmat/ (Vector Math)
- Vector/matrix operations (partially copied to mini/lib/)

#### From /workspace/renderer/ (Rendering)
- Rendering pipeline headers

#### From /workspace/ui/ (User Interface)
- UI component definitions

#### From /workspace/bitmap/ (Bitmap Handling)
- `bitmap.h` - Bitmap structures

#### From /workspace/manage/ (Resource Management)
- Resource management headers

#### From /workspace/sndlib/ (Sound Library)
- Sound handling headers

#### From /workspace/networking/ (Network Code)
- Network-related headers (likely can be removed)

#### From /workspace/netgames/ (Multiplayer Games)
- Multiplayer code (likely can be removed)

#### From /workspace/third_party/ (External Libraries)
- SDL3 headers
- OpenGL headers
- Other third-party libraries

---

## Phase 1: Core Infrastructure Setup

### 1.1 Directory Structure
Create the following directory structure in `qteditor/mini/`:

```
qteditor/mini/
├── CMakeLists.txt          # Standalone build configuration
├── README.md               # Mini project documentation
├── fix/                    # Fixed-point math (DONE)
│   └── fix.h
├── lib/                    # Core engine types
│   ├── pstypes.h          (DONE)
│   ├── vecmat.h           (DONE)
│   ├── vecmat_external.h  (DONE)
│   ├── 3d.h               (DONE)
│   ├── bitmap.h
│   ├── bnode.h
│   ├── room.h
│   ├── object.h
│   ├── door.h
│   ├── trigger.h
│   ├── terrain.h
│   ├── weapon.h
│   ├── viseffect.h
│   ├── polymodel.h
│   ├── lightmap.h
│   ├── texture.h
│   ├── objpage.h
│   ├── doorpage.h
│   ├── doorway.h
│   ├── hogfile.h
│   ├── BOA.h
│   ├── AIMain.h
│   ├── findintersection.h
│   ├── gamepath.h
│   ├── genericpage.h
│   ├── LoadLevel.h
│   ├── Mission.h
│   └── TelComEfxStructs.h
├── editor/                 # Editor-specific code
│   ├── ebnode.h           (May need copying from /workspace/editor/)
│   ├── aiambient.h
│   ├── appdatabase.h
│   ├── application.h
│   └── editor_room_state.h
├── ddio/                   # Device I/O layer
│   └── ddio.h
├── cfile/                  # File system abstraction
│   └── cfile.h
├── grtext/                 # Graphics text rendering
├── ui/                     # UI components (non-Qt)
├── logger/                 # Logging (DONE - verify contents)
├── renderer/               # Minimal rendering interface
├── stubs/                  # Stub implementations for removed functionality
├── qt/                     # Qt-specific wrappers/adapters
└── src/                    # Copied qteditor source files (optional)
```

### 1.2 Copy Essential Headers

Priority order for copying headers to mini/:

#### Tier 1: Fundamental Types (Must Have)
These are referenced directly in qteditor source files and are fundamental to compilation:

1. **From /workspace/lib/**:
   - [ ] `bitmap.h` - Bitmap structures used throughout
   - [ ] `room.h` - Room data structures
   - [ ] `object.h` - Object management
   - [ ] `door.h` - Door structures
   - [ ] `trigger.h` - Trigger definitions
   - [ ] `terrain.h` - Terrain data
   - [ ] `bsp.h` - BSP tree structures
   - [ ] `cfile.h` - File handling
   - [ ] `args.h` - Command line arguments
   - [ ] `config.h` - Configuration
   - [ ] `ddio.h` - Device I/O
   - [ ] `gametexture.h` - Texture handling
   - [ ] `gamefont.h` - Font handling

2. **From /workspace/editor/**:
   - [ ] `ebnode.h` - Extended BSP (verify if already sufficient in mini/)
   - [ ] `editor_room_state.h` - Room state for editor

3. **From /workspace/fix/**:
   - [x] `fix.h` - Already copied

4. **From /workspace/vecmat/**:
   - [x] `vecmat.h` - Already copied
   - [x] `vecmat_external.h` - Already copied

5. **From /workspace/2dlib/** or **grtext/**:
   - [x] `gr.h` - Already copied
   - [x] `grdefs.h` - Already copied

#### Tier 2: Game Structures (Needed for Level Editing)
These are needed for the editor to understand and manipulate game data:

6. **From /workspace/lib/** or **/workspace/Descent3/**:
   - [ ] `weapon.h` - Weapon definitions
   - [ ] `powerup.h` - Power-up definitions
   - [ ] `ai.h` or `AIMain.h` - AI definitions
   - [ ] `physics.h` - Physics properties
   - [ ] `viseffect.h` - Visual effects
   - [ ] `polymodel.h` - 3D model structures
   - [ ] `lightmap.h` - Lightmap data
   - [ ] `material.h` - Material definitions
   - [ ] `sound.h` - Sound references
   - [ ] `anim.h` - Animation data
   - [ ] `fireball.h` - Explosion effects
   - [ ] `debris.h` - Debris objects
   - [ ] `particle.h` - Particle systems
   - [ ] `interp.h` - Interpolation
   - [ ] `outline.h` - Outline rendering
   - [ ] `objinfo.h` - Object information
   - [ ] `objnum.h` - Object numbering
   - [ ] `ship.h` - Ship/vehicle definitions
   - [ ] `player.h` - Player structures
   - [ ] `levelgoal.h` - Level objectives

7. **From /workspace/editor/**:
   - [ ] `medextern.h` - Editor externals
   - [ ] `levelgoal.h` - Level goals (editor version)
   - [ ] `objexternal.h` - Object externals

#### Tier 3: Supporting Systems (Can Be Stubbed)
These can potentially be replaced with minimal stub implementations:

8. **System Interfaces**:
   - [ ] `pserror.h` - Error handling (can use standard exceptions)
   - [ ] `pstring.h` - String utilities (can use std::string)
   - [ ] `ptrarray.h` - Pointer arrays (can use std::vector)
   - [ ] `mem.h` - Memory management (can use standard allocators)
   - [ ] `mono.h` - Debug output (redirect to Qt debug)

9. **File and Resource Management**:
   - [ ] `manage.h` - Resource management
   - [ ] `paging.h` - Memory paging
   - [ ] `registry.h` - Configuration registry

10. **Audio**:
    - [ ] `audiolib.h` - Audio library interface
    - [ ] `soundload.h` - Sound loading

11. **Input**:
    - [ ] `input.h` - Input handling
    - [ ] `controls.h` - Control mappings
    - [ ] `mouselib.h` - Mouse handling

12. **Networking** (Likely Removable):
    - [ ] `network.h` - Networking (stub or remove)
    - [ ] `multi.h` - Multiplayer (stub or remove)
    - [ ] All netgames/ headers (remove)

---

## Phase 2: Header Trimming Strategy

### 2.1 Analysis Process
For each header file to be copied:

1. **Identify Dependencies**: Parse the header to find what it includes
2. **Check Usage in qteditor**: Search qteditor source for actual usage of symbols
3. **Trim Unused Content**: Remove symbols, functions, and includes not used by qteditor
4. **Update Include Paths**: Change `#include "foo.h"` to `#include "mini/foo.h"` or appropriate path
5. **Replace External Dependencies**: 
   - Game runtime code → Stubs
   - Platform-specific code → Cross-platform alternatives
   - Third-party libs → Keep as external dependencies

### 2.2 Symbol Removal Guidelines

**Keep**:
- Type definitions (structs, classes, enums) used in qteditor
- Function declarations called from qteditor
- Constants/macros referenced in qteditor
- Inline functions used by qteditor

**Remove/Stub**:
- Function implementations (move to .cpp if needed)
- Game runtime logic (AI updates, physics simulation)
- Rendering code (keep interfaces, remove implementations)
- Network/multiplayer code
- Sound playback code (keep structures, remove playback)
- Platform-specific implementations

**Example Transformation**:

Original (`/workspace/lib/object.h`):
```cpp
struct tObject {
    int id;
    vector pos;
    // ... many fields ...
};

void ObjDoFrame(int objnum);  // Game runtime - REMOVE
void ObjRender(int objnum);   // Rendering - STUB
int ObjCreate(int type, ...); // Creation - KEEP
void ObjDelete(int objnum);   // Deletion - KEEP
```

Trimmed (`qteditor/mini/lib/object.h`):
```cpp
struct tObject {
    int id;
    vector pos;
    // ... only fields used by editor ...
};

// Removed: ObjDoFrame (runtime logic)
// Removed: ObjRender (rendering - handled by editor separately)
int ObjCreate(int type, ...);   // Kept: Used for placing objects
void ObjDelete(int objnum);     // Kept: Used for removing objects
// Stub implementations in stubs/object_stubs.cpp
```

---

## Phase 3: Stub Implementation Strategy

### 3.1 Categories of Stubs Needed

1. **Runtime System Stubs** (`stubs/runtime_stubs.cpp`):
   ```cpp
   // Game loop functions that don't apply to editor
   void GameDoFrame() {}
   void RenderFrame() {}
   void DoSoundFrame() {}
   ```

2. **AI Stubs** (`stubs/ai_stubs.cpp`):
   ```cpp
   // AI behavior that doesn't apply in editor context
   void AIUpdateFrame() {}
   int AIFindPath(...) { return -1; }
   ```

3. **Physics Stubs** (`stubs/physics_stubs.cpp`):
   ```cpp
   // Physics simulation not needed in editor
   void PhysicsSimulateFrame() {}
   void DoCollisionDetection() {}
   ```

4. **Network Stubs** (`stubs/network_stubs.cpp`):
   ```cpp
   // Multiplayer not needed in editor
   bool NetworkConnected() { return false; }
   void NetworkSend(...) {}
   ```

5. **Audio Stubs** (`stubs/audio_stubs.cpp`):
   ```cpp
   // Sound playback not critical for editor
   void PlaySound(int handle) {}
   int LoadSound(const char* name) { return -1; }
   ```

### 3.2 Stub Implementation Guidelines

- Return safe default values (nullptr, -1, false, 0)
- Log warnings when stubs are called (helps identify missing functionality)
- Use `#ifdef EDITOR_MODE` to conditionally compile stubs
- Document why each stub exists

---

## Phase 4: Build System Setup

### 4.1 CMakeLists.txt for mini/

Create `/workspace/qteditor/mini/CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(qteditor-mini VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets OpenGLWidgets)

# Find SDL3
find_package(SDL3 REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Collect source files
set(MINI_SOURCES
    # Core types
    lib/vecmat_external.cpp
    fix/fix.cpp
    
    # Editor sources (copied from parent)
    ../main.cpp
    ../main_window.cpp
    ../d3_editor_init.cpp
    # ... add all qteditor .cpp files ...
    
    # Stub implementations
    stubs/runtime_stubs.cpp
    stubs/ai_stubs.cpp
    stubs/physics_stubs.cpp
    stubs/network_stubs.cpp
    stubs/audio_stubs.cpp
)

set(MINI_HEADERS
    fix/fix.h
    lib/pstypes.h
    lib/vecmat.h
    lib/3d.h
    lib/bitmap.h
    # ... all mini headers ...
)

# Create executable
add_executable(qteditor-mini ${MINI_SOURCES} ${MINI_HEADERS})

# Include directories
target_include_directories(qteditor-mini PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# Link libraries
target_link_libraries(qteditor-mini PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::OpenGLWidgets
    SDL3::SDL3
    OpenGL::GL
)

# Compiler definitions
target_compile_definitions(qteditor-mini PRIVATE
    EDITOR_MODE
    MINI_BUILD
    # Add other necessary defines from original build
)
```

### 4.2 Include Path Updates

All source files in qteditor need their include paths updated:

**Before**:
```cpp
#include "lib/room.h"
#include "editor/ebnode.h"
#include "fix.h"
```

**After**:
```cpp
#include "mini/lib/room.h"
#include "mini/editor/ebnode.h"
#include "mini/fix/fix.h"
```

Or use a base include path and keep relative paths the same.

---

## Phase 5: Testing and Validation

### 5.1 Compilation Tests

1. **Syntax Check**: Ensure all headers compile independently
   ```bash
   cd qteditor/mini
   cmake -B build -S .
   cmake --build build
   ```

2. **Link Test**: Verify all symbols resolve
   - Watch for undefined reference errors
   - Add stubs for missing symbols

3. **Include Order**: Test different include orders don't break builds

### 5.2 Functional Tests

1. **Basic Launch**: Can the editor start?
2. **Level Loading**: Can existing levels be loaded?
3. **Object Placement**: Can objects be placed and manipulated?
4. **Texture Application**: Can textures be applied to surfaces?
5. **Lighting Calculation**: Does radiosity lighting work?
6. **Save/Export**: Can levels be saved correctly?

### 5.3 Regression Tests

Compare behavior between:
- Original qteditor build
- New mini/ build

Look for:
- Crashes or assertion failures
- Missing features
- Performance degradation
- Incorrect data serialization

---

## Phase 6: Cleanup and Documentation

### 6.1 Remove Redundant Code

After verifying mini/ works:
1. Remove unused stubs
2. Consolidate duplicate type definitions
3. Remove deprecated compatibility layers
4. Clean up TODO comments

### 6.2 Documentation

Create:
1. **README.md**: Overview of mini/ project
2. **BUILDING.md**: Build instructions
3. **DEPENDENCIES.md**: List of external dependencies
4. **MIGRATION.md**: Guide for moving from full qteditor to mini/
5. **API_CHANGES.md**: Document any API changes from trimming

### 6.3 Update Main Project

If mini/ replaces original qteditor:
1. Update root CMakeLists.txt
2. Update BUILD.md and USAGE.md
3. Update CI/CD configurations
4. Update developer documentation

---

## Implementation Checklist

### Phase 1: Infrastructure
- [ ] Create directory structure
- [ ] Copy Tier 1 headers (fundamental types)
- [ ] Verify basic compilation of headers
- [ ] Create stub framework

### Phase 2: Header Migration
- [ ] Copy and trim Tier 2 headers (game structures)
- [ ] Copy and trim Tier 3 headers (supporting systems)
- [ ] Update include paths in all qteditor source files
- [ ] Resolve circular dependencies

### Phase 3: Stub Implementation
- [ ] Implement runtime stubs
- [ ] Implement AI stubs
- [ ] Implement physics stubs
- [ ] Implement network stubs
- [ ] Implement audio stubs
- [ ] Add logging to stub calls

### Phase 4: Build System
- [ ] Create CMakeLists.txt for mini/
- [ ] Configure Qt dependencies
- [ ] Configure SDL/OpenGL dependencies
- [ ] Test build process
- [ ] Fix link errors

### Phase 5: Testing
- [ ] Compile test
- [ ] Link test
- [ ] Launch test
- [ ] Feature tests
- [ ] Regression tests

### Phase 6: Finalization
- [ ] Remove unused code
- [ ] Write documentation
- [ ] Update main project references
- [ ] Code review
- [ ] Merge to main branch

---

## Risk Assessment

### High Risk Items
1. **Circular Dependencies**: Headers that depend on each other across directories
   - Mitigation: Careful analysis and forward declarations

2. **Hidden Dependencies**: Symbols used indirectly through macros or templates
   - Mitigation: Comprehensive grep/search and compile testing

3. **Platform-Specific Code**: Windows/Linux/Mac differences
   - Mitigation: Use cross-platform abstractions already in place

### Medium Risk Items
1. **Qt Integration**: Ensuring Qt widgets work with trimmed backend
   - Mitigation: Incremental testing of each dialog/widget

2. **File Format Compatibility**: Ensuring saved levels remain compatible
   - Mitigation: Extensive save/load testing with original game

3. **Performance**: Trimmed code might have performance implications
   - Mitigation: Profile before/after comparison

### Low Risk Items
1. **Missing Features**: Some editor features may be lost
   - Mitigation: Document and prioritize restoration if needed

2. **Build Complexity**: New build system may confuse developers
   - Mitigation: Clear documentation and examples

---

## Timeline Estimate

| Phase | Estimated Time | Dependencies |
|-------|---------------|--------------|
| Phase 1: Infrastructure | 2-4 hours | None |
| Phase 2: Header Migration | 8-16 hours | Phase 1 |
| Phase 3: Stub Implementation | 4-8 hours | Phase 2 |
| Phase 4: Build System | 2-4 hours | Phase 3 |
| Phase 5: Testing | 4-8 hours | Phase 4 |
| Phase 6: Finalization | 2-4 hours | Phase 5 |
| **Total** | **22-44 hours** | |

Note: This is a rough estimate. Actual time may vary based on:
- Complexity of dependencies discovered
- Number of compilation errors encountered
- Amount of game-specific code that needs stubbing
- Testing and bug fixing required

---

## Success Criteria

The refactor is considered successful when:

1. ✅ `qteditor/mini/` compiles independently without referencing parent directories
2. ✅ No dependencies on `descar/` directory (explicit exclusion)
3. ✅ Editor launches and displays main window
4. ✅ Existing levels can be loaded without errors
5. ✅ Basic editing operations work (place objects, apply textures, etc.)
6. ✅ Levels can be saved and loaded in original game
7. ✅ Build time is reduced compared to full qteditor
8. ✅ Documentation is complete and accurate

---

## Notes

- **descar Exclusion**: Remember that files in the `descar` directory are explicitly excluded from this refactoring. Any dependencies on descar must be either stubbed or replaced.

- **Incremental Approach**: It's recommended to proceed incrementally, testing after each major change rather than attempting all changes at once.

- **Version Control**: Make frequent commits during the refactor to allow easy rollback if issues are discovered.

- **Parallel Development**: If possible, maintain both the original and mini versions in parallel until mini is proven stable.

---

*Document created: $(date)*
*Last updated: $(date)*
*Status: Planning Phase*
