# Dependencies

This document lists the external dependencies required by `qteditor/mini/`.

## Required libraries

| Dependency | Purpose | Notes |
|------------|---------|-------|
| Qt Core / Gui / Widgets | UI framework | Qt5 (preferred for offscreen) or Qt6 |
| Qt OpenGL / OpenGLWidgets | Editor 3D viewport | Qt5::OpenGL or Qt6::OpenGLWidgets |
| OpenGL (`OpenGL::GL`) | 3D rendering backend | |
| Qt Test | Test framework | Only for `qteditor_mini_tests` |
| `descar` submodule | HOG archive read/write | Under `repo/descar/`, explicitly excluded from decoupling |

## Game data (runtime, not a build dependency)

To open and edit levels the editor must read gamedata from the game's `d3.hog`
(its `Table.gam`). The editor locates it at startup via the `-datadir <path>`
command-line argument, or falls back to a set of common install paths. The
root directory in the repo is not a requirement; only a Descent 3 install is.

## Deliberate non-dependencies

The decoupling removes the following from the build so the editor can compile
independently:

- **Descent3Core** / any other repo directory
- **SDL** — windowing/input handled by Qt
- **sound**, **networking**, **multiplayer** runtime code — stubbed
- **cfile / hogfile** — a read-only HOG-capable cfile is ported in `mini/cfile/`

The game's gamedata tables are parsed from `d3.hog`'s `Table.gam` by
`mini/manage/gamedata_loader.cpp`, which populates the editor's metadata arrays.

## Replaced with stubs

Systems replaced in `stubs.cpp` / `lib/*_stub.h`:

- Runtime game loop (`GameDoFrame`, physics, collision)
- AI behaviour
- Network / multiplayer
- Audio playback (structures retained, playback stubbed)
- Object/room memory management (implemented minimally, not stubbed)

The stubs return safe defaults so ported editor code compiles and runs without
the engine. Core math (vecmat) and room topology are implemented functionally
in `stubs.cpp` because the editor's room/geometry operations require them.

## Getting a Qt5 config

On Debian/Ubuntu:

```bash
sudo apt install qtbase5-dev libqt5opengl5-dev
```

Test module (optional, for the test target):

```bash
sudo apt install libqt5test5
```