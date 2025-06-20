#pragma once

#include <IR_Common.hpp>

namespace IR {
    
    template <typename T>
    T Min(T a, T b) IR_RETURN(a < b ? a : b)

    template <typename T>
    T Max(T a, T b) IR_RETURN(a > b ? a : b)

    template <typename T>
    T Clamp(T val, T min, T max) IR_RETURN(Max(Min(val, max), min))

}