# Calculator

A small C++20 application used to learn **Modern CMake** through incremental exercises.

**Current stage: v0.7.0** — a multi-directory project with a shared math library (DLL/SO), a private header-only `INTERFACE` dependency, and explicit build-output directories.

## Project layout

```text
Calculator/
├── CMakeLists.txt
├── README.md
├── app/
│   ├── CMakeLists.txt
│   └── main.cpp
├── math/
│   ├── CMakeLists.txt
│   ├── include/
│   │   └── calculator_math.h
│   └── src/
│       └── math.cpp
└── utils/
    ├── CMakeLists.txt
    └── include/
        └── calculator/
            └── number_utils.h
```

The root `CMakeLists.txt` declares the project (`Calculator`, version `0.7.0`) and includes the `math`, `app`, and `utils` modules using `add_subdirectory()`.

## CMake targets

| Target | Kind | Responsibility |
| --- | --- | --- |
| `CalculatorApp` | `EXECUTABLE` | Compiles `app/main.cpp` and demonstrates the public math API |
| `CalculatorMath` | `SHARED` | Compiles `math/src/math.cpp` and exports `add(int, int)` and `add_many(std::span<const int>)` |
| `CalculatorUtils` | `INTERFACE` | Provides the header-only `calculator::sum(std::span<const int>)` implementation |

### Dependency graph

```text
CalculatorApp (EXECUTABLE)
└── PRIVATE -> CalculatorMath (SHARED)
               ├── PUBLIC include directory: math/include/
               ├── PUBLIC C++20 requirement
               └── PRIVATE -> CalculatorUtils (INTERFACE)
                              ├── INTERFACE include directory: utils/include/
                              └── INTERFACE C++20 requirement
```

`CalculatorApp` depends directly only on `CalculatorMath`. Its source code includes `calculator_math.h`, but does not directly include headers from `CalculatorUtils`.

`CalculatorMath` uses `CalculatorUtils` internally in `math.cpp`. The `CalculatorUtils` include directory is available while compiling `CalculatorMath`, but is **not** propagated as a compile-time usage requirement to `CalculatorApp`.

- **`PRIVATE`** requirements apply to the target itself, without propagating compilation usage requirements to its consumers.
- **`PUBLIC`** requirements apply to the target and are passed to consumers.
- **`INTERFACE`** requirements are passed to consumers without being applied to the target's own compilation.

The shared `CalculatorMath` library compiles the inline utility implementation as needed. It does not require a separate `CalculatorUtils` DLL or SO. The `PRIVATE` dependency does **not** propagate `CalculatorUtils` include directories to the application.

`CalculatorUtils` has no `.cpp` file and produces no `.lib` or `.a` artifact. It is still a first-class CMake target carrying usage requirements.

## Source example

`app/main.cpp` uses only the public `CalculatorMath` API:

```cpp
#include <array>
#include <iostream>

#include "calculator_math.h"

int main()
{
    const std::array<int, 4> values = {1, 2, 3, 4};

    std::cout << "2 + 3 = " << add(2, 3) << '\n';
    std::cout << "Sum = " << add_many(values) << '\n';

    return 0;
}
```

The public `calculator_math.h` declares `add_many(std::span<const int>)` and includes `<span>`, but does **not** include `calculator/number_utils.h`.

Internally, `math/src/math.cpp` implements `add_many()` using `calculator::sum(values)` from `CalculatorUtils`. `std::span` is part of **C++20**; `CalculatorMath` publicly declares its C++20 requirement because its public API uses that feature.

Expected output:

```text
2 + 3 = 5
Sum = 10
```

## Build

Requirements: **CMake 3.25+**, **Ninja**, and a **C++20-capable compiler** (MSVC on Windows or GCC on Ubuntu).

From the `projects/Calculator/` directory:

```bash
cmake -S . -B build/Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Debug
```

Windows (PowerShell):

```powershell
.\build\Debug\bin\CalculatorApp.exe
```

Ubuntu:

```bash
./build/Debug/bin/CalculatorApp
```

To configure Release separately:

```bash
cmake -S . -B build/Release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release
```

When using MSVC and Ninja from a Windows terminal, run in an initialized MSVC development environment. VS Code with CMake Tools can select the MSVC kit for you.

### Shared-library outputs

The application and the math library use explicit output directories set with `set_target_properties()`:

| CMake property | Target(s) | Destination |
| --- | --- | --- |
| `RUNTIME_OUTPUT_DIRECTORY` | `CalculatorApp`, `CalculatorMath` | `${CMAKE_BINARY_DIR}/bin` |
| `LIBRARY_OUTPUT_DIRECTORY` | `CalculatorMath` | `${CMAKE_BINARY_DIR}/lib` |
| `ARCHIVE_OUTPUT_DIRECTORY` | `CalculatorMath` | `${CMAKE_BINARY_DIR}/lib` |

With Ninja and MSVC, the relevant files are:

```text
build/Debug/
├── bin/
│   ├── CalculatorApp.exe
│   └── CalculatorMath.dll
└── lib/
    └── CalculatorMath.lib  # DLL import library
```

On Ubuntu with GCC, expect:

```text
build/Debug/
├── bin/
│   └── CalculatorApp
└── lib/
    └── libCalculatorMath.so
```

On Windows, the DLL is classified by CMake as a **runtime** output, while its `.lib` import library is an **archive** output. Placing the DLL next to the EXE lets the Windows loader find it without a manual `PATH` change. On Linux, CMake normally sets a build-tree RPATH so the executable can locate the SO in the build tree.

Directories such as `build/Debug/app/` and `build/Debug/math/` are **expected**: `add_subdirectory()` creates corresponding build directories, which hold intermediate files (e.g., `.obj`). Output-directory properties do not move those intermediate files.

`CalculatorMath` currently uses `WINDOWS_EXPORT_ALL_SYMBOLS ON` to export its global functions for the MSVC DLL build. This is a convenient learning-stage setting; a later exercise will implement explicit API exports via `GenerateExportHeader`.

### DLL loading experiment (Windows)

Before moving both runtime outputs into `bin/`, `CalculatorApp.exe` could not find `CalculatorMath.dll` in the separate `math/` directory. It exited with `-1073741515` (`0xC0000135`, missing DLL). Adding the DLL directory to `PATH` made the executable work and return exit code `0`. Configuring both runtime targets to output into `bin/` fixed the layout without a manual `PATH` change.

## Exercise history

| Version | Topic |
| --- | --- |
| `0.1.0` | First executable; `project()`, `add_executable()` and `target_sources()` |
| `0.2.0` | `src/` and `include/`; `target_include_directories()` |
| `0.3.0` | Static `CalculatorMath`; `target_link_libraries()` |
| `0.4.0` | Modular build using `add_subdirectory()` |
| `0.5.0` | Header-only `CalculatorUtils` with `add_library(... INTERFACE)` |
| `0.6.0` | `add_many()` and transitive usage requirements; `PRIVATE` vs `PUBLIC` library dependencies |
| `0.7.0` | `SHARED` math library, `WINDOWS_EXPORT_ALL_SYMBOLS`, runtime DLL loading and output directories |

Previous stages are learning milestones; they are not duplicated in separate source directories. Git history starts with the actual first commit rather than reconstructing earlier commits.

## Design notes

- The header is named `calculator_math.h` rather than `math.h` to avoid shadowing the standard C header.
- `CalculatorMath` keeps `CalculatorUtils` as a private implementation detail; its public API does not include utility-library headers.
- Libraries declare the include paths and language features that their consumers require.
- The project uses portable CMake paths, with no hardcoded MSVC or GCC options.
- There are currently no automated tests, external package dependencies or installation rules. Those are topics for later lessons.
