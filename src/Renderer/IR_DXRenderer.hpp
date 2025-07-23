#pragma once

#include <IR_Renderer.hpp>

namespace IR::Renderer {
    struct DX : public APIHandle {
        IR_RENDERER_FUNCS(, override)
    };
}