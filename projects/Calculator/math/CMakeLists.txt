add_library(CalculatorMath STATIC)

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
