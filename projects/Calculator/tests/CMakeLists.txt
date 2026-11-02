add_executable(CalculatorTests)

target_sources(CalculatorTests
    PRIVATE
        main.cpp
)

target_link_libraries(CalculatorTests
    PRIVATE
        CalculatorMath
)

set_target_properties(CalculatorTests
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

add_test(
    NAME MathBasic
    COMMAND CalculatorTests
)
