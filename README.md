# CMake Tutorial

A hands-on repository for learning **Modern CMake** with C++20. Each exercise focuses on understanding how to configure a project, define targets, and describe dependencies—rather than copying a ready-made `CMakeLists.txt`.

Development environments:

- **Primary:** Windows + Visual Studio Code + MSVC + Ninja
- **Secondary:** Ubuntu + Visual Studio Code + GCC + Ninja

## Projects

| Project | Topics | Status |
| --- | --- | --- |
| [Calculator](projects/Calculator/README.md) | Executables, static and header-only libraries, subdirectories, target usage requirements | **v0.5.0 — complete** |
| Logger | Reusable libraries, public APIs, shared libraries | Planned |
| DataProcessor | External dependencies, CTest, automated tests | Planned |
| PackageDemo | CMake presets, installation, target exports, packaging | Planned |

## Repository layout

```text
CMakeTutorial/
├── .gitignore
├── README.md
├── .vscode/                    # VS Code workspace configuration
└── projects/
    └── Calculator/
        ├── CMakeLists.txt
        ├── README.md
        ├── app/
        │   ├── CMakeLists.txt
        │   └── main.cpp
        ├── math/
        │   ├── CMakeLists.txt
        │   ├── include/calculator_math.h
        │   └── src/math.cpp
        └── utils/
            ├── CMakeLists.txt
            └── include/calculator/number_utils.h
```

Each project has **its own top-level `CMakeLists.txt`** and can be configured and built independently. The repository root intentionally has **no** `CMakeLists.txt`.

## Requirements

- **CMake 3.25+**
- **Ninja**
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

From the **repository root**, configure and build the Debug version:

```bash
cmake -S projects/Calculator -B build/Calculator/Debug -G Ninja -DCMAKE_BUILD_TYPE=Debug
cmake --build build/Calculator/Debug
```

Run on Windows (PowerShell):

```powershell
.\build\Calculator\Debug\app\CalculatorApp.exe
```

Run on Ubuntu:

```bash
./build/Calculator/Debug/app/CalculatorApp
```

Expected output:

```text
2 + 3 = 5
Sum = 10
```

For Release, use a separate build directory:

```bash
cmake -S projects/Calculator -B build/Calculator/Release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/Calculator/Release
```

Build directories are generated locally and should **not** be committed.

## Visual Studio Code

Open the **`CMakeTutorial/` repository root** as your VS Code workspace. CMake Tools can use the Calculator directory as its CMake source directory:

```json
{
    "cmake.sourceDirectory": [
        "${workspaceFolder}/projects/Calculator"
    ],
    "cmake.generator": "Ninja",
    "cmake.buildDirectory": "${sourceDirectory}/build/${buildType}"
}
```

This example goes in `.vscode/settings.json`. Select the **MSVC x64 kit** on Windows or **GCC** on Ubuntu, configure, then build using CMake Tools. When more projects are added, their source directories can be added to the array; no top-level CMake project is required.

The VS Code build directory above is separate from the command-line build directory used in the examples. Avoid reusing one build directory across incompatible generators or toolchains.

## Learning roadmap

1. **Calculator v0.1.0:** first executable and `CMakeLists.txt`.
2. **v0.2.0:** source and header directories.
3. **v0.3.0:** static library and target linking.
4. **v0.4.0:** multiple `CMakeLists.txt` files using `add_subdirectory()`.
5. **v0.5.0:** header-only `INTERFACE` library and transitive usage requirements.
6. **Next:** shared libraries, deeper dependency management, presets, tests, external packages and installation.

See the [Calculator README](projects/Calculator/README.md) for the target diagram, current API and project-specific details.

## Version control

Keep source files, CMake configuration, documentation and shareable editor settings under Git. Exclude generated build trees, local caches and user-specific configuration. Project milestones may be marked with tags such as `calculator-v0.5.0` after the corresponding commit is created.

This repository is an evolving learning environment, not a production-ready C++ framework.
