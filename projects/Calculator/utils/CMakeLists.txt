add_library(CalculatorUtils INTERFACE)

target_include_directories(CalculatorUtils
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(CalculatorUtils
    INTERFACE
        cxx_std_20
)