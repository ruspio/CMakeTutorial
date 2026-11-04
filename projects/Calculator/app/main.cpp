#include <array>
#include <fmt/format.h>

#include "calculator_math.h"

int main()
{
    const std::array<int, 4> values = {1, 2, 3, 4};

    fmt::print("2 + 3 = {}\n", add(2, 3));
    fmt::print("Sum = {}\n", add_many(values));

    return 0;
}