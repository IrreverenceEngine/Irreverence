#pragma once

#include <IR_Types.hpp>

namespace IR {
    struct GlobalsData {
        Int32 width, height;
        Float64 curtime, frametime;
    };

    extern GlobalsData Globals;
}