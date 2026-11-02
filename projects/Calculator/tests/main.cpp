
#include <array>
#include <iostream>

#include "calculator_math.h"

int main()
{
    if (add(2, 3) != 5)
    {
        std::cerr << "Test failed: add()\n";
        return 1;
    }

    const std::array<int, 4> values{1, 2, 3, 4};

    if (add_many(values) != 10)
    {
        std::cerr << "Test failed: add_many()\n";
        return 1;
    }

    std::cout << "All math tests passed.\n";
    return 0;
}
