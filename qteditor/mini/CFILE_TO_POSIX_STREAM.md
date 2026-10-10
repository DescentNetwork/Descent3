# Replacing CFILE with posix_istream / posix_ostream

The D3 core (and the ported loader code) reads and writes binary data through
the legacy `CFILE` abstraction in `qteditor/mini/cfile/`. This document
describes how to migrate that I/O to the modern `posix_istream` /
`posix_ostream` types provided by the `descar` submodule, which:

- wrap `FILE*` with an endian-aware, RAII-friendly interface,
- decode/encode integer and float values in little-endian (the native Descent 3
  on-disk order) automatically,
- are the long-term replacement for the `cf_*` read/write API.

## Migration status

The primary file I/O consumers have been migrated (verified against the real
`d3.hog` and `thecore.d3l`):

| Area | Status |
|------|--------|
| HOG table read in `cf_OpenLibrary` (`ReadHogHeader`/`ReadHogEntry`) | **Migrated** to `hog2::archive_t`; legacy helpers removed |
| `gamedata_loader.cpp` (Table.gam page readers + HOG open) | **Migrated** to `posix_istream` + in-memory fmemopen |
| `level_loader.cpp` (.d3l chunk read/write) | **Migrated** to `posix_istream` / `posix_ostream` |
| `bitmap/iff.cpp` (OGF/IFF/TGA decode) + texture loading in gamedata | **Migrated** to `posix_istream` over fmemopen payloads |
| `editor_test.cpp` HOG-access test | **Migrated** (renamed `testPosixReadsHogGamedata`) |

The `mini/cfile/` directory still provides the non-I/O helpers used elsewhere
(`cf_CopyFile`, `cf_GetWritableBaseDirectory`, `cf_Diff`), and the stub
`mng_Assign*To*` signatures take `CFILE*` (they are no-ops, not I/O).

> **Important format caveats discovered during migration** (correct the naive
> mappings in section 3):
> - D3 page/level *strings* are **variable-length, NUL-terminated**, not
>   fixed-width padded. Use `psReadString`/`psWriteString` (in
>   `mini/posix_helpers.h`), **not** `fixed_string_t` (which reads a fixed byte
>   count and would corrupt the format).
> - IFF `BMHD` headers are **big-endian**; the posix `>>` operator is
>   little-endian. Read those with a `rdShortBE`/`rdIntBE` helper using
>   `D3::convert_be`.
> - 1-byte `cf_ReadByte` returns a *signed* `int8_t`; preserve signedness for
>   RLE command decoding (`command >= 0` vs `< 0`).
> - `posix_stream.good()` reflects stale `errno`; validate via magic/version
>   tags and chunk-boundary seeks instead.
> - `fixed_string_t` is only appropriate for genuinely fixed-width on-disk
>   fields (e.g. `hog2::entry_t.name`).

## Where the types live

| Type | Header | Source |
|------|--------|--------|
| `posix_istream`, `posix_ostream`, `byte_istream`, `byte_ostream`, `posix_stream_base` | `descar/utils/posix_stream.h` | `descar/utils/posix_stream.cpp` |
| `hog2::header_t`, `hog2::entry_t`, `hog2::archive_t` | `descar/src/hog2_format.h` | `descar/src/hog2_format.cpp` |
| `fixed_string_t<N>` | `descar/utils/fixed_string.h` | header only |
| `le_to_host`, `host_to_le`, `std::byteswap` | `descar/utils/portable_endian.h` | header only |

These are already compiled into `qteditor_mini` via `DESCAR_SOURCES` in
`qteditor/mini/CMakeLists.txt`:

```cmake
set(DESCAR_SOURCES
    ${PROJECT_SOURCE_DIR}/../../descar/src/hog2_format.cpp
    ${PROJECT_SOURCE_DIR}/../../descar/utils/posix_stream.cpp
)
```

and `descar/src` + `descar/utils` are on the include path.

---

## 1. The `posix_stream` API at a glance

### Opening / closing

```cpp
#include <posix_stream.h>

// Input (read-only)
posix_istream in;
if (!in.open("/path/to/file", std::ios_base::in)) { /* failed: in.error() */ }

// Output (truncate/create)
posix_ostream out;
if (!out.open("/path/file", std::ios_base::out | std::ios_base::trunc)) { /* failed */ }

// RAII: out of scope (or moved-from) closes the file.
in.close();
out.close();
```

### Primitive reads / writes

`byte_istream` provides overloads for every arithmetic type `> 1` byte that
**swap to host order from little-endian** automatically. 1-byte types (`int8_t`,
`uint8_t`) are raw.

```cpp
posix_istream in;

uint8_t  b; int8_t  sb;   // raw bytes
uint16_t s; int32_t i; uint32_t u;
int64_t  l; float f; double d;

in  >> b >> sb >> s >> i >> u >> l >> f >> d;   // each converts LE -> host
in.read(&raw_bytes, count);                     // raw block read
uint8_t one = in.get();                         // single byte

posix_ostream out;
out << b << sb << s << i << u << l << f << d;   // each converts host -> LE
out.write(raw_bytes, count);                    // raw block write
out.put(one);                                   // single byte
```

### Positioning / sizes

```cpp
bool  ok    = in.seek(offset, std::ios_base::beg); // beg/cur/end
off_t pos   = in.tell();
size_t size = in.size();
bool  eof   = in.eof();
bool  good  = in.good();
auto  err   = in.error();                           // std::errc
```

### Fixed-size string fields

Descent 3 uses fixed-width character arrays on disk (e.g. `HOG_FILENAME_LEN`,
`PAGENAME_LEN`). `fixed_string_t<N>` models those:

```cpp
#include <fixed_string.h>

fixed_string_t<36> name;        // 36 raw bytes, NUL-padded on disk
in  >> name;                    // read exactly 36 bytes
out << name;                    // write exactly 36 bytes

std::string s = name;  // fixed_string_t converts to std::string transparently (also .string())
name = s;                       // assign a std::string back
```

> Because `posix_stream` already handles endianness, the legacy `INTEL_INT` /
> `INTEL_SHORT` / `D3::convert_le` calls are no longer needed.

---

## 2. Mapping the legacy `CFILE` API

| Legacy `CFILE` | `posix_stream` replacement |
|----------------|-----------------------------|
| `CFILE *cfp = cfopen(path, "rb")` | `posix_istream in; in.open(path, std::ios_base::in)` |
| `CFILE *cfp = cfopen(path, "wb")` | `posix_ostream out; out.open(path, std::ios_base::out \| std::ios_base::trunc)` |
| `cfclose(cfp)` | `in.close()` / `out.close()` (or RAII scope exit) |
| `cf_ReadByte(cfp)` | `uint8_t v; in >> v;` |
| `cf_ReadShort(cfp)` | `int16_t v; in >> v;` |
| `cf_ReadInt(cfp)` | `int32_t v; in >> v;` |
| `cf_ReadFloat(cfp)` | `float v; in >> v;` |
| `cf_ReadDouble(cfp)` | `double v; in >> v;` |
| `cf_ReadBytes(buf, n, cfp)` | `in.read(buf, n)` |
| `cf_ReadString(buf, n, cfp)` | `fixed_string_t<N> fs; in >> fs; std::string s = fs.string();` |
| `cf_WriteByte(cfp, v)` | `out << v;` |
| `cf_WriteShort(cfp, v)` | `out << v;` |
| `cf_WriteInt(cfp, v)` | `out << v;` |
| `cf_WriteFloat(cfp, v)` | `out << v;` |
| `cf_WriteBytes(buf, n, cfp)` | `out.write(buf, n)` |
| `cf_WriteString(cfp, s)` | `fixed_string_t<N> fs; fs = s; out << fs;` |
| `cfseek(cfp, off, SEEK_SET)` | `in.seek(off, std::ios_base::beg)` |
| `cftell(cfp)` | `in.tell()` |
| `cfeof(cfp)` | `in.eof()` |
| `cfilelength(cfp)` | `in.size()` |
| `cf_OpenLibrary(name)` / HOG library lookups | See section 4 (use `hog2::archive_t`) |

`INTEL_INT(x)` / `INTEL_SHORT(x)` / `D3::convert_le(x)` **drop entirely** when
moving to `posix_stream` — byte order is handled by the `operator>>` /
`operator<<` templates.

---

## 3. Migration examples

### 3.1 Before / after: reading a simple record

**Legacy (`cfile`):**

```cpp
// lib/cfile.h + cfile/cfile.cpp
CFILE *f = cfopen("thing.dat", "rb");
if (!f)
  return;

uint8_t      version = cf_ReadByte(f);
int32_t      count   = cf_ReadInt(f);
float        scale   = cf_ReadFloat(f);
char         name[PAGENAME_LEN];
cf_ReadString(name, PAGENAME_LEN, f);
cfclose(f);
```

**Modern (`posix_istream`):**

```cpp
#include <posix_stream.h>
#include <fixed_string.h>

posix_istream in;
if (!in.open("thing.dat", std::ios_base::in))
  return;

uint8_t       version;
int32_t       count;
float         scale;
fixed_string_t<PAGENAME_LEN> fs;
in >> version >> count >> scale >> fs;

std::string name = fs.string();   // NUL-padded on disk -> std::string
```

### 3.2 Before / after: the ship page reader

This is a real example from `qteditor/mini/manage/gamedata_loader.cpp`
(`mng_ReadNewShipPage`). It shows how the `cf_*` reads that currently go
through a temp buffer (to feed a `std::string`) can be replaced directly.

**Legacy:**

```cpp
int mng_ReadNewShipPage(CFILE *infile, mngs_ship_page *shippage) {
  *shippage = mngs_ship_page{};
  int version = cf_ReadShort(infile);

  cf_ReadString(shippage->ship_struct.name, PAGENAME_LEN, infile);
  char cockpit_buf[PAGENAME_LEN];
  char hud_buf[PAGENAME_LEN];
  cf_ReadString(cockpit_buf, PAGENAME_LEN, infile);
  cf_ReadString(hud_buf, PAGENAME_LEN, infile);
  shippage->ship_struct.cockpit_name    = cockpit_buf;
  shippage->ship_struct.hud_config_name = hud_buf;

  shippage->ship_struct.med_lod_distance = cf_ReadFloat(infile);
  shippage->ship_struct.lo_lod_distance  = cf_ReadFloat(infile);
  mng_ReadPhysicsChunk(&shippage->ship_struct.phys_info, infile);
  // ...
}
```

**Modern:**

```cpp
int mng_ReadNewShipPage(posix_istream &in, mngs_ship_page *shippage) {
  *shippage = mngs_ship_page{};

  int16_t version;
  fixed_string_t<PAGENAME_LEN> name_fs, cockpit_fs, hud_fs;

  in >> version
     >> name_fs >> cockpit_fs >> hud_fs;

  shippage->ship_struct.name           = name_fs.string();
  shippage->ship_struct.cockpit_name   = cockpit_fs.string();
  shippage->ship_struct.hud_config_name = hud_fs.string();

  in >> shippage->ship_struct.med_lod_distance
     >> shippage->ship_struct.lo_lod_distance;

  mng_ReadPhysicsChunk(shippage->ship_struct.phys_info, in);
  // ...
}
```

The migration cascades cleanly through the helper functions: `mng_ReadPhysicsChunk`
and `mng_ReadWeaponBatteryChunk` change their `CFILE *` parameter to
`posix_istream &`, and their `cf_ReadInt`/`cf_ReadByte`/`cf_ReadFloat` calls
become `in >> x`.

### 3.3 Chunked level loading (`level_loader.cpp`)

`LL_ReadVector`, `LL_ReadMatrix`, and the chunk scanner in
`qteditor/mini/level_loader.cpp` use `CFILE`. The same mapping applies; the
chunk `(4-byte tag, int32 length)` layout is read as:

```cpp
// Legacy
char tag[4];
cf_ReadBytes((uint8_t*)tag, 4, ifile);
int32_t chunk_size = cf_ReadInt(ifile);

// Modern
char tag[4];
in.read(tag, 4);
int32_t chunk_size;
in >> chunk_size;
```

Seeking to skip unknown chunks:

```cpp
// Legacy
cfseek(ifile, chunk_start + chunk_size, SEEK_SET);

// Modern
in.seek(chunk_start + chunk_size, std::ios_base::beg);
```

---

## 4. Replacing `ReadHogHeader` / `ReadHogEntry` (and HOG libraries)

The legacy cfile layer (currently `qteditor/mini/cfile/cfile.cpp` +
`hogfile.cpp`) opens a `.hog` with `cf_OpenLibrary` and later resolves a file
inside it by walking the library's header/entry table. The functions
`ReadHogHeader(FILE*, tHogHeader*)` and `ReadHogEntry(FILE*, tHogFileEntry*)`
(`qteditor/mini/cfile/hogfile.cpp`) parse that table manually using `fread` +
`INTEL_INT`.

That entire machinery is superseded by `descar`'s **`hog2::archive_t`**, which
already models the HOG2 header + entries and reads/writes them with
`posix_istream` / `posix_ostream`.

### 4.1 Reading a HOG's table with `hog2::archive_t`

```cpp
#include <hog2_format.h>
#include <posix_stream.h>

posix_istream in;
if (!in.open("/path/to/d3.hog", std::ios_base::in))
  return;

hog2::archive_t archive;
try {
  in >> archive;               // reads header_t + all entry_t records
} catch (const std::invalid_argument &) {
  // not a HOG2 file (bad magic)
  return;
}

for (const hog2::entry_t &entry : archive) {
  const std::string filename = entry.name.string();
  const uint32_t    flags     = entry.flags;
  const uint32_t    len       = entry.len;      // size in bytes of the payload
  const uint32_t    ts        = entry.timestamp;
  const size_t      offset    = archive.fileOffset(archive.begin());
  // ...
}
```

No manual `fread`, no `INTEL_INT` — endianness is handled by `operator>>`.

### 4.2 Before / after for the two legacy functions

`ReadHogHeader` + `ReadHogEntry` (legacy, `hogfile.cpp`) produced a `tHogHeader`
+ a `tHogFileEntry` per file:

```cpp
// legacy hogfile.h
struct tHogHeader     { char magic[4]; uint32_t nfiles; uint32_t file_data_offset; };
struct tHogFileEntry  { char name[36]; uint32_t flags; uint32_t len; uint32_t timestamp; };
```

The `posix_stream` / `hog2` equivalents are `hog2::header_t` and `hog2::entry_t`,
and they are read as part of `archive_t`. The direct replacements are:

| Legacy | Replacement |
|--------|-------------|
| `ReadHogHeader(FILE*, tHogHeader*)` | `hog2::header_t` + `hog2::archive_t` reader (`in >> archive`) |
| `ReadHogEntry(FILE*, tHogFileEntry*)` | `hog2::entry_t` (read via the `archive_t` reader loop) |
| `INTEL_INT(entry->flags)` etc. | automatic (`operator>>` handles endianness) |

An explicit per-entry read (if you do not use `archive_t`) is:

```cpp
hog2::entry_t entry;
in >> entry;   // equals: in >> name >> flags >> len >> timestamp
```

### 4.3 Writing a HOG

```cpp
posix_ostream out;
if (!out.open("out.hog", std::ios_base::out | std::ios_base::trunc))
  return;

hog2::archive_t archive;

hog2::entry_t entry;
entry.name = "file.txt";
entry.flags = 0;
entry.len = file_len;
entry.timestamp = 0;
archive.addEntry(entry);

// optionally: write each payload after the table (at archive.fileOffset(...))
out << archive;
```

> Important: `archive_t` only serializes the **header + entry table**. The file
> *payloads* are stored after the table; you must write them at the byte offset
> reported by `archive.fileOffset(entryIt)` and set `entry.len` to match.

---

## 5. Practical migration checklist

1. **Replace open/close first.** Swap `cfopen(path,"rb"/"wb")` for
   `posix_istream` / `posix_ostream` + `.open(...)`, and `cfclose` for `.close()`
   (or rely on RAII).
2. **Convert scalar reads/writes mechanically.** `cf_ReadInt` -> `in >> int32_v`,
   `cf_ReadShort` -> `in >> int16_v`, `cf_ReadByte` -> `in >> uint8_v`, etc.
3. **Eliminate the endian macros.** Remove `INTEL_INT` / `INTEL_SHORT` /
   `D3::convert_le`. `posix_stream` already converts.
4. **String fields -> `fixed_string_t`.** Replace `char buf[N]` +
   `cf_ReadString(buf, N, fp)` with `fixed_string_t<N> fs; in >> fs;` then use
   `fs.string()` for a `std::string`. When writing, `fs = std::string; out << fs;`.
5. **Seek/size/eof calls** map to `seek` / `tell` / `size` / `eof`.
6. **HOG library lookups** (`cf_OpenLibrary` + walking libraries) become
   `hog2::archive_t` reads (section 4), removing `ReadHogHeader` /
   `ReadHogEntry`.
7. **Propagate the parameter type** through helper functions: change
   `CFILE *f` to `posix_istream &in` (for reads) or `posix_ostream &out`
   (for writes) so call sites don't have to keep a `CFILE*`.
8. **Compile against the descar headers.** Add
   `#include <posix_stream.h>`, `#include <fixed_string.h>`, and
   `#include <hog2_format.h>` as needed. The include dirs `descar/src` and
   `descar/utils` are already configured.
9. **Keep error handling.** `posix_istream` exposes `good()` / `error()`. Use
   `in.error()` where the legacy code checked `nullptr` from `cfopen`, and
   `!in.good()` where the legacy code used `cf_ReadByte`-returns-EOF checks.

---

## 6. Expected outcomes

- The `cfile/` directory and its `cf_*` / `ReadHogHeader` / `ReadHogEntry`
  functions become dead once all callers migrate, and can be removed.
- Binary compatibility is preserved: `posix_stream` writes/reads the same
  little-endian byte layout the legacy code produced.
- String handling no longer round-trips through `char[]`; it can be expressed
  as `std::string` directly (see the `ship::cockpit_name` / `hud_config_name`
  migration already done in `world_objects_player_dialog.cpp`).