# Building qteditor_mini

The `qteditor/mini/` directory is a self-contained build of the Descent 3
level editor. It does **not** link against `Descent3Core` or any other
directory in the repo; it only depends on Qt and OpenGL, plus the `descar`
submodule (excluded from the decoupling task) for HOG archive I/O.

## Prerequisites

- CMake 3.16+
- A C++17 compiler (GCC/Clang)
- Qt 5 or Qt 6:
  - Qt5: `Core`, `Gui`, `Widgets`, `OpenGL` (and `Test` for the test binary)
  - Qt6: `Core`, `Gui`, `Widgets`, `OpenGLWidgets` (and `Test`)
- OpenGL development headers
- The `descar` submodule checked out:
  ```bash
  git submodule update --init descar
  ```

## Configure and build

This build is normally produced in `repo/builds/linux_qtmini/`. From that dir:

```bash
cmake -S ../qteditor/mini -D FORCE_QT5_EDITOR=TRUE ..
cmake --build .
```

- `FORCE_QT5_EDITOR=TRUE` forces Qt5 (required for offscreen rendering).
- Without it, Qt6 is used if found, falling back to Qt5.

### Targets

| Target | Description |
|--------|-------------|
| `qteditor_mini` | The editor GUI executable. |
| `qteditor_mini_tests` | QTest suite (see Test section). |

## Run

```bash
# Editor (offscreen platform for headless runs)
QT_QPA_PLATFORM=offscreen ./qteditor_mini

# Without env override, Qt uses the host display.
./qteditor_mini
```

## Test

```bash
cmake --build . --target qteditor_mini_tests
QT_QPA_PLATFORM=offscreen ./qteditor_mini_tests
```

The test binary is independent of `Descent3Core`. It exercises dialog
construction, menu wiring, settings persistence, and room/object/geometry
operations. Run a single test with:

```bash
./qteditor_mini_tests testCombineFacesCoplanar
```

Disable the test target in CMake with `-D BUILD_EDITOR_TESTS=OFF`.