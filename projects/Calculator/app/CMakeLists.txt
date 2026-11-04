add_executable(CalculatorApp)

target_sources(CalculatorApp
    PRIVATE
        main.cpp
)

target_link_libraries(CalculatorApp
    PRIVATE
        CalculatorMath
        fmt::fmt
)

set_target_properties(CalculatorApp
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)