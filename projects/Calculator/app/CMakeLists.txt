add_executable(CalculatorApp)

target_sources(CalculatorApp
    PRIVATE
        main.cpp
)

target_link_libraries(CalculatorApp
    PRIVATE
        CalculatorMath
)
