#pragma once

#include <span>
#include "calculator_math_export.h"

CALCULATOR_MATH_EXPORT int add(int a, int b);
CALCULATOR_MATH_EXPORT int add_many(std::span<const int> values);