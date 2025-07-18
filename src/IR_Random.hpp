#pragma once

#include <IR_Common.hpp>
#include <glm.hpp>

namespace IR::Random {
    void Seed(UInt64 seed);
    void SeedRandom();

    Int64 RandomInt64(Int64 min, Int64 max);
    Float64 RandomFloat64(Float64 min, Float64 max);
}