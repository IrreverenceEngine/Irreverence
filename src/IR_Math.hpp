#pragma once

#include <IR_Macro.hpp>

namespace IR::Math {

    template <typename T>
    constexpr inline T Min(T a, T b) IR_RETURN(a < b ? a : b)

	template <typename T, typename... Ts>
	constexpr inline T Min(T a, T b, Ts... ts) IR_RETURN(Min(Min(a, b), ts...))

    template <typename T>
    constexpr inline T Max(T a, T b) IR_RETURN(a > b ? a : b)

	template <typename T, typename... Ts>
	constexpr inline T Max(T a, T b, Ts... ts) IR_RETURN(Max(Max(a, b), ts...))

    template <typename T>
    constexpr inline T Clamp(T val, T min, T max) IR_RETURN(Max(Min(val, max), min))

}
