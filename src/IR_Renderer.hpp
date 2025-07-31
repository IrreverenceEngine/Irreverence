#pragma once

#include <IR_Common.hpp>
#include <IR_Model.hpp>
#include <IR_Texture.hpp>
#include <IR_Shader.hpp>
#include <IR_Material.hpp>
#include <IR_Mesh.hpp>

#include <glm.hpp>

#define IR_RENDERER_FUNCS(_x, _y) \
    _x const char* GetName() _y; \
    _x const char* GetDirectory() _y; \
    _x bool Init() _y; \
    _x void Shutdown() _y; \
    _x void Present() _y; \
    _x void SubmitModel(const Model* model, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& size, const glm::vec4& col, UInt8 skin) _y; \
    _x void SubmitMesh(const Mesh* mesh, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& size, const glm::vec4& col, const Material* material) _y; \
    _x void SubmitMapMesh(const Mesh* mesh, const Material* material) _y; \
    _x Model* MakeModel() _y; \
    _x Material* MakeMaterial() _y; \
    _x Texture* MakeTexture() _y; \
    _x Shader* MakeShader() _y; \
    _x Mesh* MakeMesh() _y; \
    _x Material* GetMaterialWhite() _y; \
    _x Material* GetMaterialBlack() _y; \
    _x Material* GetMaterialError() _y; \

namespace IR::Renderer {
    class APIHandle {
    public:
        IR_RENDERER_FUNCS(virtual, IR_UNIMPLEMENTED)
    };

    enum class API {
        OPENGL,
        DIRECTX
    };

    API GetAPI();
    UInt64 PreInit(API api);

    IR_RENDERER_FUNCS(,)
}