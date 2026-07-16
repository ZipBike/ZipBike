# ZipBike

A modular high-performance compression engine
supporting RLE, Huffman and LZ77 algorithms.

## Features
- Modular architecture
- Custom container format (`.zbik`) with file and folder archives
- CLI interface (`zb`)
- Extensible algorithm system
- Auto mode: picks the best algorithm per file (falls back to stored)
- Safe extraction (rejects absolute paths and `..` traversal)
- Atomic archive writes (a failed save never destroys an existing archive)
- CRC32 integrity check per entry (`verify` and `extract` detect corruption)
- Unicode file names
- Cross-platform: Linux, macOS, Windows

## Usage

```
zb <file|folder> [-o <output.zbik>] [-a <algorithm>]
zb extract <archive.zbik> [-o <output-dir>]
zb list <archive.zbik>
zb verify <archive.zbik>
zb help
```

Algorithms: `auto` (default), `rle`, `lz77`, `huffman`, `none`.

Examples:

```
zb report.txt                 # -> report.txt.zbik
zb photos -o backup           # whole folder -> backup.zbik
zb extract backup.zbik -o restored
zb list backup.zbik
zb verify backup.zbik
```

## Building

```
cmake -S . -B build
cmake --build build
```

Requires a C++17 compiler (GCC, Clang or MSVC) on Linux, macOS or Windows.

## Testing on Windows (PowerShell)

From the repository root, with CMake and a C++17 compiler installed:

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

"hello " * 200 | Set-Content test.txt
.\build\zb.exe test.txt
.\build\zb.exe list test.txt.zbik
.\build\zb.exe extract test.txt.zbik -o out
if ((Get-FileHash test.txt).Hash -eq (Get-FileHash out\test.txt).Hash) { "IDENTICAL" }
.\build\zb.exe verify test.txt.zbik
```

With MinGW the executable is statically linked, so `zb.exe` can be copied and
run anywhere. With the Visual Studio generator, build with
`cmake --build build --config Release` and use `.\build\Release\zb.exe`.

## Testing on Linux / macOS

From the repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cd build

echo "hello hello hello hello" > test.txt
./zb test.txt                      # -> test.txt.zbik
./zb list test.txt.zbik
./zb extract test.txt.zbik -o out
diff test.txt out/test.txt && echo IDENTICAL
./zb verify test.txt.zbik

mkdir -p myfolder/sub && echo data > myfolder/sub/f.txt
./zb myfolder -o backup
./zb extract backup.zbik -o restored
diff -r myfolder restored/myfolder && echo FOLDER-IDENTICAL
```

Without CMake, a direct compile works too:

```bash
g++ -std=c++17 -O2 -Icli -Icore cli/*.cpp core/*.cpp -o zb
```

## Archive format (version 3)

All integers little-endian:

```
magic "ZBIK" | version u8 | entryCount u32
per entry: pathLen u16 | path (UTF-8, '/') | isDir u8 | type u8
           originalSize u64 | crc32 u32 | compressedSize u64 | data
```

## Roadmap
- Parallel compression
- AI model compression mode
- Plugin-based algorithm loading
