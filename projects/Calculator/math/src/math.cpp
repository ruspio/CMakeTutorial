#include "calculator_math.h"
#include <calculator/number_utils.h>

int add(int a, int b)
{
    return a + b;
}

int add_many(std::span<const int> values)
{
    return calculator::sum(values);
}