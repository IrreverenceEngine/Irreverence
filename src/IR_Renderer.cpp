#include <IR_Renderer.hpp>
#include <Renderer/GL/IR_GLRenderer.hpp>
#include <Renderer/DX/IR_DXRenderer.hpp>
#include <IR_Common.hpp>
#include <IR_Window.hpp>
#include <IR_Input.hpp>

#include <SDL3/SDL.h>

#ifdef SDL_PLATFORM_WIN32
#include <windows.h>
#endif

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

    void Shutdown()
    {
        s_HAPI->Shutdown();
    }

    void Present()
    {
        s_HAPI->Present();
    }

    void SubmitModel(const Model* model, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& size, const glm::vec4& col, UInt8 skin)
    {
        s_HAPI->SubmitModel(model, pos, rot, size, col, skin);
    }

    void SubmitMesh(const Mesh* mesh, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& size, const glm::vec4& col, const Material* material)
    {
        s_HAPI->SubmitMesh(mesh, pos, rot, size, col, material);
    }

    void SubmitMapMesh(const Mesh *mesh, const Material *material)
    {
        s_HAPI->SubmitMapMesh(mesh, material);
    }

    Model* MakeModel() IR_RETURN(s_HAPI->MakeModel())
    Material* MakeMaterial() IR_RETURN(s_HAPI->MakeMaterial())
    Texture* MakeTexture() IR_RETURN(s_HAPI->MakeTexture())
    Shader* MakeShader() IR_RETURN(s_HAPI->MakeShader())
    Mesh* MakeMesh() IR_RETURN(s_HAPI->MakeMesh())

    Texture* GetTextureWhite() IR_RETURN(s_HAPI->GetTextureWhite())
    Texture* GetTextureBlack() IR_RETURN(s_HAPI->GetTextureBlack())
    Texture* GetTextureError() IR_RETURN(s_HAPI->GetTextureError())
    Material* GetMaterialWhite() IR_RETURN(s_HAPI->GetMaterialWhite())
    Material* GetMaterialBlack() IR_RETURN(s_HAPI->GetMaterialBlack())
    Material* GetMaterialError() IR_RETURN(s_HAPI->GetMaterialError())
    Mesh* GetMeshCube() IR_RETURN(s_HAPI->GetMeshCube())
}
