#include <IR_Renderer.hpp>
#include <Renderer/IR_GLRenderer.hpp>
#include <Renderer/IR_DXRenderer.hpp>
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

    static API s_API = API::OpenGL;
    static APIHandle* s_HAPI = nullptr;

    UInt64 PreInit(API api)
    {
        UInt64 flags = 0;

        if (s_API == API::DirectX) {

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

    bool Init()
    {
        s_View = glm::mat4(1.0f);
        s_Projection = glm::mat4(1.0f);

        if (s_API == API::DirectX) {
            s_HAPI = new DX;
        } else {
            s_HAPI = new GL;
        }

        bool success = s_HAPI->Init();

        if (success) {
            IR_MSG(INFO, "Successfully initialized %s Renderer", s_HAPI->GetName());
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

    Model* MakeModel() IR_RETURN(s_HAPI->MakeModel())
    Material* MakeMaterial() IR_RETURN(s_HAPI->MakeMaterial())

}