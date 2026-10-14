#include <array>
#include <iostream>

#include "calculator_math.h"
#include <calculator/number_utils.h>

int main()
{
    const std::array<int, 4> values = {1, 2, 3, 4};

    std::cout << "2 + 3 = " << add(2, 3) << '\n';
    std::cout << "Sum = " << calculator::sum(values) << '\n';

    return 0;
}