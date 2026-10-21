#pragma once

#include <span>

int add(int a, int b);
int add_many(std::span<const int> values);