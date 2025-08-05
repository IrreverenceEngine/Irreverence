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
	_x void Resize(UInt32 width, UInt32 height) _y; \
    _x void Present() _y; \
    _x void SubmitModel(const Model* model, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& size, const Color& col, UInt8 skin) _y; \
    _x void SubmitMesh(const Mesh* mesh, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& size, const Color& col, const Material* material) _y; \
    _x void SubmitMapMesh(const Mesh* mesh, const Material* material) _y; \
    _x Model* MakeModel() _y; \
    _x Material* MakeMaterial() _y; \
    _x Texture* MakeTexture() _y; \
    _x Shader* MakeShader() _y; \
    _x Mesh* MakeMesh() _y; \
    _x UInt16 MakePLight() _y; \
    _x void RemovePLight(UInt16 index) _y; \
    _x void SetPLightPosition(UInt16 index, const glm::vec3& pos) _y; \
    _x void SetPLightColor(UInt16 index, const Color& col) _y; \
    _x void SetPLightInnerRadius(UInt16 index, const Float32 radius) _y; \
    _x void SetPLightOuterRadius(UInt16 index, const Float32 radius) _y; \
    _x UInt16 MakeSLight() _y; \
    _x void RemoveSLight(UInt16 index) _y; \
    _x void SetSLightPosition(UInt16 index, const glm::vec3& pos) _y; \
    _x void SetSLightDirection(UInt16 index, const glm::vec3& dir) _y; \
    _x void SetSLightColor(UInt16 index, const Color& col) _y; \
    _x void SetSLightInnerRadius(UInt16 index, const Float32 radius) _y; \
    _x void SetSLightOuterRadius(UInt16 index, const Float32 radius) _y; \
    _x void SetSLightInnerCutoff(UInt16 index, const Float32 deg) _y; \
    _x void SetSLightOuterCutoff(UInt16 index, const Float32 deg) _y; \
    _x Texture* GetTextureWhite() _y; \
    _x Texture* GetTextureBlack() _y; \
    _x Texture* GetTextureError() _y; \
    _x Texture* GetTextureNormal() _y; \
    _x Mesh* GetMeshCube() _y;

namespace IR::Renderer {
    constexpr UInt16 MAX_POINTLIGHTS = 1 << 11;
    constexpr UInt16 MAX_SPOTLIGHTS = 1 << 11;

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
