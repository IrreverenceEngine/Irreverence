#pragma once

#include <IRX_Types.hpp>
#include <IRX_Macro.hpp>

using namespace IRX;

namespace IR {
    class Window;

    struct GlobalsData {
        Int32 Width() const IRX_RETURN(width)
        Int32 Height() const IRX_RETURN(height)
        Float64 Curtime() const IRX_RETURN(curtime)
        Float64 Frametime() const IRX_RETURN(frametime)

        GlobalsData() = default;
        GlobalsData(const GlobalsData&) = delete;
        GlobalsData(GlobalsData&&) = delete;
        GlobalsData& operator=(const GlobalsData&) = delete;
        GlobalsData& operator=(GlobalsData&&) = delete;
    private:
        Int32 width = 0, height = 0;
        Float64 curtime = 0, frametime = 0;

        friend Window;
    };

    extern GlobalsData Globals;
}
