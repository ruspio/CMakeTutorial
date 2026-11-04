cmake_minimum_required(VERSION 3.25)

project(Calculator
    VERSION 0.10.0
    LANGUAGES CXX
)

include(CTest)
include(FetchContent)

find_package(fmt 12.2.0 EXACT CONFIG QUIET)

if(fmt_FOUND)
    message(STATUS "Using installed fmt")
else()
    message(STATUS "fmt not found, trying to get from GIT")
    
    FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 12.2.0
    )

    FetchContent_MakeAvailable(fmt)
endif()

add_subdirectory(utils)
add_subdirectory(math)
add_subdirectory(app)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
