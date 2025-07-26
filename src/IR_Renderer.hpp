#pragma once

#include <IR_Common.hpp>
#include <IR_Model.hpp>
#include <IR_Texture.hpp>
#include <IR_Shader.hpp>
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
    _x Texture* MakeTexture() _y; \
    _x Shader* MakeShader() _y; \

namespace IR::Renderer {
    class APIHandle {
    public:
        IR_RENDERER_FUNCS(virtual, IR_UNIMPLEMENTED)
    };

    enum class API {
        OPENGL,
        DIRECTX
    };

    UInt64 PreInit(API api);
    bool Init();
    void Shutdown();
    void Present();

    Model* MakeModel();
    Material* MakeMaterial();
    Texture* MakeTexture();
    Shader* MakeShader();
}