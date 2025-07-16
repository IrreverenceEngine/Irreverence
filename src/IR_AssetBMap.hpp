#pragma once

#include <IR_Common.hpp>
#include <IR_AABB.hpp>

namespace IR {

    struct BMap {

    };
}

namespace IR::Asset::BMap {
    IR::BMap Load(const char* path);
}