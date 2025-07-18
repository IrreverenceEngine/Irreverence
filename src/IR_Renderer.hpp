#pragma once

#include <IR_Common.hpp>
#include <IR_Model.hpp>
#include <IR_Material.hpp>

#include <glm.hpp>

// TODO: Make API Agnostic Model, Material, Animation

#define IR_RENDERER_FUNCS(_x, _y) \
    _x const char* GetName() _y; \
    _x bool Init() _y; \
    _x void Shutdown() _y; \
    _x void Present() _y; \
    _x Model* MakeModel() _y; \
    _x Material* MakeMaterial() _y; \

namespace IR::Renderer {
    struct APIHandle {
        IR_RENDERER_FUNCS(virtual, IR_UNIMPLEMENTED)
    };

    enum class API {
        OpenGL,
        DirectX
    };

    UInt64 PreInit(API api);
    bool Init();
    void Shutdown();
    void Present();

    Model* MakeModel(); // Cleaned up by the Renderer
    Material* MakeMaterial(); // Cleaned up by the Renderer
}