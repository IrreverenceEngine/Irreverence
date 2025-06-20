#pragma once

#include <IR_CTypes.hpp>

namespace IR {
    struct GlobalsData {
        Int32 width, height;
        Float64 curtime, frametime;
    };

    extern GlobalsData Globals;
}