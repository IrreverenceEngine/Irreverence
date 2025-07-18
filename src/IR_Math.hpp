#pragma once

#include <IR_Macro.hpp>

namespace IR {
    
    template <typename T>
    inline T Min(T a, T b) IR_RETURN(a < b ? a : b)

    template <typename T>
    inline T Max(T a, T b) IR_RETURN(a > b ? a : b)

    template <typename T>
    inline T Clamp(T val, T min, T max) IR_RETURN(Max(Min(val, max), min))

}