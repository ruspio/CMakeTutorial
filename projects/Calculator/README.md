# Calculator

A small C++20 application used to learn **Modern CMake** through incremental exercises.

**Current stage: v0.5.0** — a multi-directory project with a static math library and a header-only `INTERFACE` utility library.

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

The root `CMakeLists.txt` declares the project (`Calculator`, version `0.5.0`) and includes the `math`, `app`, and `utils` modules using `add_subdirectory()`.

## CMake targets

| Target | Kind | Responsibility |
| --- | --- | --- |
| `CalculatorApp` | `EXECUTABLE` | Compiles `app/main.cpp` and runs the example |
| `CalculatorMath` | `STATIC` | Compiles `math/src/math.cpp` and provides `add(int, int)` |
| `CalculatorUtils` | `INTERFACE` | Provides the header-only `calculator::sum(std::span<const int>)` function |

### Dependency graph

```text
CalculatorApp (executable)
├── PRIVATE -> CalculatorMath (static library)
│              ├── PUBLIC include directory: math/include/
│              └── PUBLIC C++20 requirement
└── PRIVATE -> CalculatorUtils (INTERFACE library)
               ├── INTERFACE include directory: utils/include/
               └── INTERFACE C++20 requirement
```

The application does **not** declare either library's include directory itself. These paths and language requirements are propagated by `target_link_libraries()`.

- **`PRIVATE`** requirements apply to the target itself.
- **`PUBLIC`** requirements apply to the target and are passed to consumers.
- **`INTERFACE`** requirements are passed to consumers without being applied to the target's own compilation.

`CalculatorUtils` has no `.cpp` file and produces no `.lib` or `.a` artifact. It is still a first-class CMake target carrying usage requirements.

## Source example

`app/main.cpp` uses:

```cpp
#include <array>
#include <iostream>

#include "calculator_math.h"
#include <calculator/number_utils.h>

int main()
{
    const std::array<int, 4> values = {1, 2, 3, 4};

    std::cout << "2 + 3 = " << add(2, 3) << '\n';
    std::cout << "Sum = " << calculator::sum(values) << '\n';

    return 0;
}
```

`calculator::sum` accepts `std::span<const int>`, which is part of **C++20**. Its implementation is `inline` in `utils/include/calculator/number_utils.h`.

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
.\build\Debug\app\CalculatorApp.exe
```

Ubuntu:

```bash
./build/Debug/app/CalculatorApp
```

To configure Release separately:

```bash
cmake -S . -B build/Release -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build/Release
```

When using MSVC and Ninja from a Windows terminal, run in an initialized MSVC development environment. VS Code with CMake Tools can select the MSVC kit for you.

## Exercise history

| Version | Topic |
| --- | --- |
| `0.1.0` | First executable; `project()`, `add_executable()` and `target_sources()` |
| `0.2.0` | `src/` and `include/`; `target_include_directories()` |
| `0.3.0` | Static `CalculatorMath`; `target_link_libraries()` |
| `0.4.0` | Modular build using `add_subdirectory()` |
| `0.5.0` | Header-only `CalculatorUtils` with `add_library(... INTERFACE)` |

Previous stages are learning milestones; they are not duplicated in separate source directories. Git history starts with the actual first commit rather than reconstructing earlier commits.

## Design notes

- The header is named `calculator_math.h` rather than `math.h` to avoid shadowing the standard C header.
- Libraries declare the include paths and language features that their consumers require.
- The project uses portable CMake paths, with no hardcoded MSVC or GCC options.
- There are currently no automated tests, external package dependencies or installation rules. Those are topics for later lessons.
