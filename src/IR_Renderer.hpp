#pragma once

#include <glm.hpp>
#include <IR_Common.hpp>

#define IR_RENDERER_FUNCS(_x, _y) \
    _x const char* GetName() _y; \
    _x bool Init() _y; \
    _x void Shutdown() _y; \
    _x void Present() _y;

namespace IR::Renderer {
    struct APIHandle {
        IR_RENDERER_FUNCS(virtual, = 0)
    };

    enum class API {
        OpenGL,
        DirectX
    };

    UInt64 PreInit(API api);
    bool Init();
    void Shutdown();
    void Present();

    void DebugCamera(glm::mat4& outView, glm::mat4& outProjection);
}