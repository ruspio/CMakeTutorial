cmake_minimum_required(VERSION 3.25)

project(Calculator
    VERSION 0.9.0
    LANGUAGES CXX
)

include(CTest)

add_subdirectory(utils)
add_subdirectory(math)
add_subdirectory(app)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
