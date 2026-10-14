#pragma once

#include <span>

namespace calculator
{
    [[nodiscard]]
    inline int sum(std::span<const int> values)
    {
        int result = 0;

        for (const int value : values)
        {
            result += value;
        }

        return result;
    }
}
