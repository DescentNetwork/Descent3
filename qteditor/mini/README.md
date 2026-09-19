# qteditor_mini — Overview

A standalone, self-contained port of the Descent 3 level editor that decouples
the Qt editor from the rest of the Descent3 codebase. It builds only the code
required for the editor to function, replacing engine runtime systems with
small `stub` implementations.

## What's here

```
qteditor/mini/
├── CMakeLists.txt      Standalone build config (Qt + OpenGL only)
├── BUILDING.md         Build & test instructions
├── *.cpp / *.h         Qt editor sources (dialogs, main window, editor core)
├── lib/                Decoupled engine headers (types, structures, math)
├── editor/             Editor-specific headers/code
├── game/               Game data structures (trimmed)
├── fix/                Fixed-point math types
├── logger/             Logging
├── mem/                Memory allocation interface
├── ui/                 .ui widget definitions
├── images/             Toolbar / icon images
├── stubs.cpp           Stub implementations for removed engine functionality
├── editor_test.cpp     QTest suite entering every dialog + editor ops
└── dialog_test.{h,cpp} Lightweight dialog-construction checker
```

## External dependencies

Only Qt, OpenGL, and the **`descar`** submodule (HOG archive I/O — explicitly
excluded from decoupling). No other directory in the repo is referenced at
compile time.

## Key files

- `main.cpp` — application entry point.
- `main_window.cpp/h` — the editor's main window.
- `level_io.cpp` — level load/save and `CreateNewMine`.
- `room_ops.cpp` — room geometry/texture manipulation.
- `stubs.cpp` — functional stubs for engine systems the editor no longer needs.