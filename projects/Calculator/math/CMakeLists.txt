add_library(CalculatorMath SHARED)

target_sources(CalculatorMath
    PRIVATE
        src/math.cpp
)

target_compile_features(CalculatorMath
    PUBLIC
        cxx_std_20
)

target_include_directories(CalculatorMath
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(CalculatorMath
    PRIVATE
        CalculatorUtils
)

set_target_properties(CalculatorMath
    PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS ON
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)