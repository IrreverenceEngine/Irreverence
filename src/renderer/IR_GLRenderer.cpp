#include <renderer/IR_GLRenderer.hpp>
#include <renderer/IR_GLShader.hpp>
#include <IR_Window.hpp>
#include <IR_Common.hpp>

#include <GL/glew.h>
#include <SDL3/SDL.h>

namespace IR::Renderer {

    SDL_GLContext s_GLContext;

    const char* GL::GetName() IR_RETURN("OpenGL")

    bool GL::Init()
    {
        s_GLContext = SDL_GL_CreateContext((SDL_Window*)Window::GetHandle());
        SDL_GL_MakeCurrent((SDL_Window*)Window::GetHandle(), s_GLContext);
        SDL_GL_SetSwapInterval(1);

        GLenum gcode = glewInit();
        if (gcode != GLEW_OK) {
            IR_MSG(ERROR, "Failed to init GL Renderer: failed to load GLEW \"%s\"", glewGetErrorString(gcode));
            return false;
        }

        return true;
    }

    void GL::Shutdown()
    {
        SDL_GL_DestroyContext(s_GLContext);
    }

    void GL::Present()
    {
        glClearColor(0.4f, 0.7f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow((SDL_Window*)Window::GetHandle());
    }

}