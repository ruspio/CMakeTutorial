add_library(CalculatorMath SHARED)
include(GenerateExportHeader)

generate_export_header(CalculatorMath
    BASE_NAME calculator_math
)

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
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(CalculatorMath
    PRIVATE
        CalculatorUtils
)

set_target_properties(CalculatorMath
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
)