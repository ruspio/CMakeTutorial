# Calculator

A small C++20 application used to learn **Modern CMake** through incremental exercises.

**Current stage: v0.6.0** — a multi-directory project demonstrating private implementation dependencies between a static math library and a header-only `INTERFACE` utility library.

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

The root `CMakeLists.txt` declares the project (`Calculator`, version `0.6.0`) and includes the `math`, `app`, and `utils` modules using `add_subdirectory()`.

## CMake targets

| Target | Kind | Responsibility |
| --- | --- | --- |
| `CalculatorApp` | `EXECUTABLE` | Compiles `app/main.cpp` and demonstrates the public math API |
| `CalculatorMath` | `STATIC` | Compiles `math/src/math.cpp` and provides `add(int, int)` and `add_many(std::span<const int>)` |
| `CalculatorUtils` | `INTERFACE` | Provides the header-only `calculator::sum(std::span<const int>)` implementation |

### Dependency graph

```text
CalculatorApp (EXECUTABLE)
└── PRIVATE -> CalculatorMath (STATIC)
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

For static libraries, a private link dependency can still be carried forward as a link-only dependency needed when linking the final executable. This does **not** expose the private dependency's include directories or compile features to the executable.

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
| `0.6.0` | `add_many()` and transitive usage requirements; `PRIVATE` vs `PUBLIC` library dependencies |

Previous stages are learning milestones; they are not duplicated in separate source directories. Git history starts with the actual first commit rather than reconstructing earlier commits.

## Design notes

- The header is named `calculator_math.h` rather than `math.h` to avoid shadowing the standard C header.
- `CalculatorMath` keeps `CalculatorUtils` as a private implementation detail; its public API does not include utility-library headers.
- Libraries declare the include paths and language features that their consumers require.
- The project uses portable CMake paths, with no hardcoded MSVC or GCC options.
- There are currently no automated tests, external package dependencies or installation rules. Those are topics for later lessons.
