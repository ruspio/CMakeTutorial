# CMake Tutorial

A hands-on repository for learning **Modern CMake** with C++20. Each exercise focuses on understanding how to configure a project, define targets, and describe dependencies—rather than copying a ready-made `CMakeLists.txt`.

Development environments:

- **Primary:** Windows + Visual Studio Code + MSVC + Ninja
- **Secondary:** Ubuntu + Visual Studio Code + GCC + Ninja

## Projects

| Project | Topics | Status |
| --- | --- | --- |
| [Calculator](projects/Calculator/README.md) | Executables, `STATIC` / `SHARED` / `INTERFACE` libraries, target dependencies, symbol exports, CMake Presets, CTest, external dependencies (`fmt`) | **v0.10.0 — Windows Debug verified; other paths pending** |
| Logger | Reusable libraries, public APIs, shared libraries | Planned |
| DataProcessor | External dependencies and data processing | Planned |
| PackageDemo | Installation, target exports and packaging | Planned |

## Repository layout

```text
CMakeTutorial/
├── .gitignore
├── README.md
├── .vscode/                    # VS Code workspace configuration
└── projects/
    └── Calculator/
        ├── CMakeLists.txt
        ├── CMakePresets.json
        ├── README.md
        ├── app/
        │   ├── CMakeLists.txt
        │   └── main.cpp
        ├── math/
        │   ├── CMakeLists.txt
        │   ├── include/calculator_math.h
        │   └── src/math.cpp
        ├── utils/
        │   ├── CMakeLists.txt
        │   └── include/calculator/number_utils.h
        └── tests/
            ├── CMakeLists.txt
            └── main.cpp
```

Each project has **its own top-level `CMakeLists.txt`** and can be configured and built independently. The repository root intentionally has **no** `CMakeLists.txt`.

## Requirements

- **CMake 3.25+**
- **Ninja**
- **Git and network access** for first-time `FetchContent` fallback downloads
- A compiler supporting **C++20**: MSVC on Windows or GCC on Ubuntu
- Optional, but recommended: **Visual Studio Code** with **CMake Tools** and **C/C++** extensions

On Ubuntu, install the standard toolchain:

```bash
sudo apt update
sudo apt install build-essential cmake ninja-build gdb
```

Check `cmake --version`: some older Ubuntu releases provide a CMake version below 3.25. Install a newer version if required.

On Windows, install Visual Studio or Visual Studio Build Tools with the **Desktop development with C++** workload and a Windows SDK. For command-line Ninja builds with MSVC, use an **x64 Developer Command Prompt** or another terminal with the MSVC environment initialized.

## Build the Calculator

The Calculator has project-local **CMake Presets** for Windows/MSVC and Linux/GCC. Version 0.10.0 resolves {fmt} 12.2.0 from an installed CMake package when available, or uses `FetchContent` to retrieve its sources. Run the commands below from `projects/Calculator/` (not the repository root):

```bash
cd projects/Calculator
cmake --list-presets=all
```

**Windows (MSVC + Ninja):**

```powershell
cmake --preset win-debug
cmake --build --preset win-debug
.\build\Windows\win-debug\bin\CalculatorApp.exe
```

**Ubuntu (GCC + Ninja):**

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
./build/Linux/linux-debug/bin/CalculatorApp
```

Expected output on either platform:

```text
2 + 3 = 5
Sum = 10
```

Release builds use the analogous presets: `win-release` or `linux-release`. Run the matching configure preset before the build preset on the first build. The Windows Debug fallback was configured, built and tested successfully with fmt 12.2.0; the application was separately run successfully with the original FetchContent-only integration. The FetchContent-only implementation was also verified with Windows Release; the new fallback condition in Release, the installed-package path and Ubuntu have not yet been independently verified in those environments.

Build artifacts live under `projects/Calculator/build/<OS>/<preset>/` and must **not** be committed. See the [Calculator README](projects/Calculator/README.md#cmake-presets) for the preset hierarchy and [external-dependency setup](projects/Calculator/README.md#external-dependency-fmt).

## Test the Calculator

Calculator v0.9.0 adds a separate `CalculatorTests` executable and registers the `MathBasic` test through **CTest**. Run tests *after* configuring and building the matching preset, from `projects/Calculator/`:

**Windows (PowerShell):**

```powershell
cmake --preset win-debug
cmake --build --preset win-debug
ctest --preset win-debug

cmake --preset win-release
cmake --build --preset win-release
ctest --preset win-release
```

**Ubuntu:**

```bash
cmake --preset linux-debug
cmake --build --preset linux-debug
ctest --preset linux-debug
```

The `MathBasic` test checks the results of `add()` and `add_many()`. The test presets display output on failure and report an error if no tests are registered. Use `ctest --preset win-debug -N` to list tests without running them. Test builds can be disabled with `-DBUILD_TESTING=OFF` at configuration time; then no tests are registered. See [Testing with CTest](projects/Calculator/README.md#testing-with-ctest) for details.

## Visual Studio Code

Open the **`CMakeTutorial/` repository root** as your VS Code workspace. The CMake Tools extension can use `projects/Calculator/` as its source directory and read its `CMakePresets.json`:

```json
{
    "cmake.sourceDirectory": [
        "${workspaceFolder}/projects/Calculator"
    ],
    "cmake.useCMakePresets": "always"
}
```

Merge these settings into the existing `.vscode/settings.json`; do not replace other workspace settings. Select the Calculator CMake project if prompted, then use **CMake: Select Configure Preset** (`win-debug`, `win-release`, `linux-debug` or `linux-release`) and **CMake: Build**. With presets enabled, the compiler and build type are defined by the selected preset rather than by **Select a Kit** or **Select Variant**. Do not override `cmake.buildDirectory` or `cmake.generator` separately for this project; those settings already come from the preset.

For MSVC command-line builds, use an initialized Visual Studio developer environment. VS Code's CMake Tools can manage that environment when configured accordingly.

## Learning roadmap

1. **Calculator v0.1.0:** first executable and `CMakeLists.txt`.
2. **v0.2.0:** source and header directories.
3. **v0.3.0:** static library and target linking.
4. **v0.4.0:** multiple `CMakeLists.txt` files using `add_subdirectory()`.
5. **v0.5.0:** header-only `INTERFACE` library and transitive usage requirements.
6. **v0.6.0:** implementation dependencies and `PRIVATE` vs `PUBLIC`.
7. **v0.7.0:** `SHARED` library, DLL loading, output directories and `GenerateExportHeader`.
8. **v0.8.0:** CMake Presets, Debug/Release, platform conditions and multiple inheritance.
9. **v0.9.0:** CTest, a dedicated test executable, positive/negative tests and Test Presets.
10. **v0.10.0:** external {fmt} dependency, `FetchContent`, `find_package(CONFIG)` and fallback selection.
11. **Next:** installing/exporting CMake packages and consuming them from another project.

See the [Calculator README](projects/Calculator/README.md) for its target graph, public API and detailed exercise history.

## Version control

Keep source files, CMake configuration, documentation and shareable editor settings under Git. Exclude generated build trees, local caches and user-specific configuration. Project milestones may be marked with tags such as `calculator-v0.10.0` after the corresponding commit is created. Commit the shared `CMakePresets.json`, but keep user-specific `CMakeUserPresets.json` out of Git.

This repository is an evolving learning environment, not a production-ready C++ framework.
