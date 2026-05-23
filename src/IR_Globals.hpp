#pragma once

#include <IR_Types.hpp>
#include <IR_Macro.hpp>

namespace IR {
    class Window;

    struct GlobalsData {
        Int32 Width() const IR_RETURN(width)
        Int32 Height() const IR_RETURN(height)
        Float64 Curtime() const IR_RETURN(curtime)
        Float64 Frametime() const IR_RETURN(frametime)

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
