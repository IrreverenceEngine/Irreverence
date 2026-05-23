#include <IR_Renderer.hpp>
#include <Renderer/GL/IR_GLRenderer.hpp>
#include <Renderer/DX/IR_DXRenderer.hpp>
#include <IR_Common.hpp>
#include <IR_Window.hpp>
#include <IR_Input.hpp>
#include <IR_CVar.hpp>

#include <SDL3/SDL.h>

#ifdef SDL_PLATFORM_WIN32
#include <windows.h>
#endif

// Renderer cvars
CVAR(r_clear_color, 0, (IR::Int64)0xFFFFFF);

namespace IR::Renderer {

    static glm::mat4 s_View;
    static glm::mat4 s_Projection;

    static API s_API = API::OPENGL;
    static APIHandle* s_HAPI = nullptr;

    API GetAPI() IR_RETURN(s_API)

    UInt64 PreInit(API api)
    {
        UInt64 flags = 0;

        if (s_API == API::DIRECTX) {

        } else {
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

            flags |= SDL_WINDOW_OPENGL;
        }

        flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
    		flags |= SDL_WINDOW_RESIZABLE;

        return flags;
    }

    const char* GetName() IR_RETURN(s_HAPI->GetName())
    const char* GetDirectory() IR_RETURN(s_HAPI->GetDirectory())

    bool Init()
    {
        s_View = glm::mat4(1.0f);
        s_Projection = glm::mat4(1.0f);

        if (s_API == API::DIRECTX) {
            s_HAPI = new DX;
        } else {
            s_HAPI = new GL;
        }

        bool success = s_HAPI->Init();

        if (success) {
            IR_MSG(INFO, "Successfully initialized Renderer (%s)", s_HAPI->GetName());
        }

        return success;
    }

    void Shutdown() { s_HAPI->Shutdown(); delete s_HAPI; }
    void Resize(UInt32 width, UInt32 height) { s_HAPI->Resize(width, height); }
    void Present() { s_HAPI->Present(); }
    void SubmitModel(const Model* model, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& size, const Color& col, UInt8 skin) { s_HAPI->SubmitModel(model, pos, rot, size, col, skin); }
    void SubmitMesh(const Mesh* mesh, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& size, const Color& col, const Material* material) {
        s_HAPI->SubmitMesh(mesh, pos, rot, size, col, material);
    }

    void SubmitMapMesh(const Mesh* mesh, const Material* material) { s_HAPI->SubmitMapMesh(mesh, material); }
    Model* MakeModel() IR_RETURN(s_HAPI->MakeModel())
    Material* MakeMaterial() IR_RETURN(s_HAPI->MakeMaterial())
    Texture* MakeTexture() IR_RETURN(s_HAPI->MakeTexture())
    Shader* MakeShader() IR_RETURN(s_HAPI->MakeShader())
    Mesh* MakeMesh() IR_RETURN(s_HAPI->MakeMesh())

    UInt16 MakePLight() IR_RETURN(s_HAPI->MakePLight())
    void RemovePLight(UInt16 index) { s_HAPI->RemovePLight(index); }
    void SetPLightPosition(UInt16 index, const glm::vec3& pos) { s_HAPI->SetPLightPosition(index, pos); }
    void SetPLightColor(UInt16 index, const Color& col) { s_HAPI->SetPLightColor(index, col); }
    void SetPLightInnerRadius(UInt16 index, const Float32 radius) { s_HAPI->SetPLightInnerRadius(index, radius); }
    void SetPLightOuterRadius(UInt16 index, const Float32 radius) { s_HAPI->SetPLightOuterRadius(index, radius); }

    UInt16 MakeSLight() IR_RETURN(s_HAPI->MakeSLight())
    void RemoveSLight(UInt16 index) { s_HAPI->RemoveSLight(index); }
    void SetSLightPosition(UInt16 index, const glm::vec3& pos) { s_HAPI->SetSLightPosition(index, pos); }
    void SetSLightDirection(UInt16 index, const glm::vec3& dir) { s_HAPI->SetSLightDirection(index, dir); }
    void SetSLightColor(UInt16 index, const Color& col) { s_HAPI->SetSLightColor(index, col); }
    void SetSLightInnerRadius(UInt16 index, const Float32 radius) { s_HAPI->SetSLightInnerRadius(index, radius); }
    void SetSLightOuterRadius(UInt16 index, const Float32 radius) { s_HAPI->SetSLightOuterRadius(index, radius); }
    void SetSLightInnerCutoff(UInt16 index, const Float32 deg) { s_HAPI->SetSLightInnerCutoff(index, deg); }
    void SetSLightOuterCutoff(UInt16 index, const Float32 deg) { s_HAPI->SetSLightOuterCutoff(index, deg); }

    Texture* GetTextureWhite() IR_RETURN(s_HAPI->GetTextureWhite())
    Texture* GetTextureBlack() IR_RETURN(s_HAPI->GetTextureBlack())
    Texture* GetTextureError() IR_RETURN(s_HAPI->GetTextureError())
    Texture* GetTextureNormal() IR_RETURN(s_HAPI->GetTextureNormal())
    Texture* GetTextureSDE() IR_RETURN(s_HAPI->GetTextureSDE())
    Mesh* GetMeshCube() IR_RETURN(s_HAPI->GetMeshCube())
}
