# Calculator

A small C++20 application used to learn **Modern CMake** through incremental exercises.

**Current stage: v0.8.0** — a multi-directory C++20 project with a `SHARED` math library (DLL/SO), a private header-only `INTERFACE` dependency, explicit symbol exports via `GenerateExportHeader`, and cross-platform Debug/Release CMake Presets.

## Project layout

```text
Calculator/
├── CMakeLists.txt
├── CMakePresets.json
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

The root `CMakeLists.txt` declares the project (`Calculator`, version `0.8.0`) and includes the `math`, `app`, and `utils` modules using `add_subdirectory()`. The project-local `CMakePresets.json` defines repeatable configurations for Windows and Linux.

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
               ├── PUBLIC generated header directory: build/<OS>/<preset>/math/
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

The public `calculator_math.h` declares `add_many(std::span<const int>)` and includes `<span>` and the generated `calculator_math_export.h`, but does **not** include `calculator/number_utils.h`. Both `add()` and `add_many()` are marked with `CALCULATOR_MATH_EXPORT`.

Internally, `math/src/math.cpp` implements `add_many()` using `calculator::sum(values)` from `CalculatorUtils`. `std::span` is part of **C++20**; `CalculatorMath` publicly declares its C++20 requirement because its public API uses that feature.

Expected output:

```text
2 + 3 = 5
Sum = 10
```

## Build

Requirements: **CMake 3.25+**, **Ninja**, and a **C++20-capable compiler** (MSVC on Windows or GCC on Ubuntu). Run commands from the `projects/Calculator/` directory.

### CMake Presets

`CMakePresets.json` provides reusable configuration for both platforms. The current file uses schema `version: 3`, hidden presets, inheritance and OS conditions.

| Configure preset | Host | Compiler | Build type | Build directory |
| --- | --- | --- | --- | --- |
| `win-debug` | Windows | MSVC (`cl`) | Debug | `build/Windows/win-debug/` |
| `win-release` | Windows | MSVC (`cl`) | Release | `build/Windows/win-release/` |
| `linux-debug` | Linux | GCC (`g++`) | Debug | `build/Linux/linux-debug/` |
| `linux-release` | Linux | GCC (`g++`) | Release | `build/Linux/linux-release/` |

Every Configure Preset has a matching Build Preset with the same name. Platform conditions limit which presets are available on each host. The Windows configurations have been verified with MSVC and Ninja; the Linux presets are prepared but have not yet been run on Ubuntu.

#### Preset hierarchy

```text
base (hidden)
├── windows (hidden, inherits base, condition: Windows, compiler: cl)
│   ├── win-debug   + config-debug
│   └── win-release + config-release
└── linux (hidden, inherits base, condition: Linux, compiler: g++)
    ├── linux-debug   + config-debug
    └── linux-release + config-release

config-debug   (hidden): CMAKE_BUILD_TYPE=Debug
config-release (hidden): CMAKE_BUILD_TYPE=Release
```

- `base` defines the `Ninja` generator and `${sourceDir}/build/${hostSystemName}/${presetName}` output path.
- `windows` and `linux` inherit the common configuration, add an operating-system condition and select a C++ compiler.
- `config-debug` and `config-release` define the build type independently from the platform.
- Final Configure Presets use **multiple inheritance** to combine platform and build-type settings.
- Build Presets reference the corresponding Configure Presets and repeat the host condition, because `configurePreset` does not inherit that condition into a Build Preset.

Use `cmake --list-presets=all` to inspect the available configurations; only the presets matching the host are selectable.

#### Windows (PowerShell, MSVC + Ninja)

```powershell
cmake --preset win-debug
cmake --build --preset win-debug
.\build\Windows\win-debug\bin\CalculatorApp.exe

cmake --preset win-release
cmake --build --preset win-release
.\build\Windows\win-release\bin\CalculatorApp.exe
```

The MSVC command-line environment must be initialized (for example, Developer PowerShell for Visual Studio). Declaring `CMAKE_CXX_COMPILER=cl` in a preset does not initialize MSVC by itself.

#### Ubuntu (GCC + Ninja)

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
./build/Linux/linux-debug/bin/CalculatorApp

cmake --preset linux-release
cmake --build --preset linux-release
./build/Linux/linux-release/bin/CalculatorApp
```

Expected output for both build types:

```text
2 + 3 = 5
Sum = 10
```

A **Configure Preset** generates the build system and stores configuration in the corresponding build directory. A **Build Preset** invokes the already configured build; it does not replace the initial configuration step.

#### Manual configuration (without presets)

For comparison with earlier lessons, CMake can still be invoked explicitly:

```bash
cmake -S . -B build/manual-debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/manual-debug
```

This produces a separate output tree and does not change the preset configurations.

### Shared-library outputs

The application and the math library use explicit output directories set with `set_target_properties()`:

| CMake property | Target(s) | Destination |
| --- | --- | --- |
| `RUNTIME_OUTPUT_DIRECTORY` | `CalculatorApp`, `CalculatorMath` | `${CMAKE_BINARY_DIR}/bin` |
| `LIBRARY_OUTPUT_DIRECTORY` | `CalculatorMath` | `${CMAKE_BINARY_DIR}/lib` |
| `ARCHIVE_OUTPUT_DIRECTORY` | `CalculatorMath` | `${CMAKE_BINARY_DIR}/lib` |

With the `win-debug` preset (Ninja + MSVC), the relevant files are:

```text
build/Windows/win-debug/
├── bin/
│   ├── CalculatorApp.exe
│   └── CalculatorMath.dll
└── lib/
    └── CalculatorMath.lib  # DLL import library
```

With the `linux-debug` preset (Ninja + GCC), expect:

```text
build/Linux/linux-debug/
├── bin/
│   └── CalculatorApp
└── lib/
    └── libCalculatorMath.so
```

On Windows, the DLL is classified by CMake as a **runtime** output, while its `.lib` import library is an **archive** output. Placing the DLL next to the EXE lets the Windows loader find it without a manual `PATH` change. On Linux, CMake normally sets a build-tree RPATH so the executable can locate the SO in the build tree.

Directories such as `build/Windows/win-debug/app/` and `build/Windows/win-debug/math/` are **expected**: `add_subdirectory()` creates corresponding build directories, which hold intermediate files (e.g., `.obj`). Output-directory properties do not move those intermediate files.

### Explicit shared-library exports (`GenerateExportHeader`)

The library does **not** use `WINDOWS_EXPORT_ALL_SYMBOLS`. Instead, `math/CMakeLists.txt` loads the built-in CMake module and generates export declarations for the public API:

```cmake
include(GenerateExportHeader)

generate_export_header(CalculatorMath
    BASE_NAME calculator_math
)
```

This produces `calculator_math_export.h` in `${CMAKE_CURRENT_BINARY_DIR}` (e.g., `build/Windows/win-debug/math/`). The header is generated during CMake configuration and should **not** be checked into Git. It is included from the public source header:

```cpp
#pragma once

#include <span>
#include "calculator_math_export.h"

CALCULATOR_MATH_EXPORT int add(int a, int b);
CALCULATOR_MATH_EXPORT int add_many(std::span<const int> values);
```

Both the source include directory and the binary directory containing the generated header are exposed as `PUBLIC` include directories. This is required because consumers include `calculator_math.h`, which in turn includes `calculator_math_export.h`.

The `CalculatorMath` target also sets:

```cmake
CXX_VISIBILITY_PRESET hidden
VISIBILITY_INLINES_HIDDEN YES
```

On compilers that support these visibility settings (such as GCC), symbols are hidden by default and the explicit export macro marks public symbols as visible. On Windows, the generated macro selects `__declspec(dllexport)` when building the DLL and `__declspec(dllimport)` when consuming it. These are **target-local** properties, not global compiler flags.

On Windows with the Visual Studio developer tools, exports can be inspected with:

```powershell
dumpbin /exports .\build\Windows\win-debug\bin\CalculatorMath.dll
```

On Ubuntu, use:

```bash
nm -D --defined-only build/Linux/linux-debug/lib/libCalculatorMath.so | c++filt
```

The exported math API should contain `add(int, int)` and `add_many(std::span<const int>)` (demangled types may display template details). Removing `CALCULATOR_MATH_EXPORT` from the `add_many()` declaration is an instructive negative test: with hidden visibility on GCC, the shared library builds, but linking the application fails because `add_many()` is not exported. Restore the macro after testing.

This project is currently used directly from the build tree. Installing or packaging the library for use elsewhere will require installation rules for both the public source header and the generated export header, together with appropriate build/install include interfaces.

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
| `0.7.0` | `SHARED` math library; DLL loading, output directories and explicit API exports with `GenerateExportHeader` |
| `0.8.0` | Cross-platform `CMakePresets.json`, Debug/Release, hidden presets, conditions and multiple inheritance |

Previous stages are learning milestones; they are not duplicated in separate source directories. Git history starts with the actual first commit rather than reconstructing earlier commits.

## Design notes

- The header is named `calculator_math.h` rather than `math.h` to avoid shadowing the standard C header.
- `CalculatorMath` keeps `CalculatorUtils` as a private implementation detail; its public API does not include utility-library headers.
- Libraries declare the include paths and language features that their consumers require.
- `CalculatorMath` explicitly marks its exported functions; non-API symbols are hidden by default where supported.
- The project uses portable CMake paths; compiler selection (`cl`/`g++`) is declared in host-specific presets rather than hardcoded in `CMakeLists.txt`.
- Share `CMakePresets.json` in Git, exclude `CMakeUserPresets.json` (if used) and generated `build/` trees.
- There are currently no automated tests, external package dependencies or installation rules. Those are topics for later lessons.
