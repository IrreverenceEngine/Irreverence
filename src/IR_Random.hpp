#pragma once

#include <IR_Common.hpp>
#include <glm.hpp>

namespace IR::Random {
    void Seed(UInt64 seed);
    void SeedRandom();

    Int64 Int(Int64 min, Int64 max);
    Float64 Float(Float64 min, Float64 max);
}