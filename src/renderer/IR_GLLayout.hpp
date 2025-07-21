#pragma once

#include <IR_Common.hpp>

namespace IR::Renderer {
    struct GLLayout {
        bool InitStandard();
        bool InitAnimated();
        void Destroy();

        UInt32 id = 0;
        UInt32 ibo = 0;
    };
}