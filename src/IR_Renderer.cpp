#include <IR_Renderer.hpp>
#include <renderer/IR_GLRenderer.hpp>
#include <renderer/IR_DXRenderer.hpp>
#include <IR_Common.hpp>
#include <IR_Window.hpp>
#include <IR_Input.hpp>
#include <IR_AssetTexture.hpp>

#include <SDL3/SDL.h>
#include <glm/gtc/type_ptr.hpp>

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
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

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

    void DebugCamera(glm::mat4& outView, glm::mat4& outProjection)
    {
        static glm::vec3 pos = glm::vec3(0, 0, -2);
        static glm::vec3 dir = glm::vec3(0, 0, 1);
        static glm::vec3 angles = glm::vec3(0, 0, 0);
        static float fov = 70.0f;
        static float nearz = 0.2f;
        static float farz = 500.0f;

        // glm::ivec2 mdelta = Input::MouseDelta();

        // angles.y += mdelta.x;
        // angles.x -= mdelta.y;

		if (Input::IsKeyDown(Input::Key::LEFT)) {
			angles.y -= 100.0f * Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::RIGHT)) {
			angles.y += 100.0f * Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::DOWN)) {
			angles.x -= 100.0f * Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::UP)) {
			angles.x += 100.0f * Globals.frametime;
		}

		dir.x = cosf(glm::radians(angles.y)) * cosf(glm::radians(angles.x));
		dir.y = sinf(glm::radians(angles.x));
		dir.z = sinf(glm::radians(angles.y)) * cosf(glm::radians(angles.x));

		glm::vec3 side = glm::normalize(glm::cross(dir, { 0.0f, 1.0f, 0.0f }));
		if (Input::IsKeyDown(Input::Key::W)) {
			pos += dir * 2.0f * (Float32)Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::S)) {
			pos -= dir * 2.0f * (Float32)Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::A)) {
			pos -= side * 2.0f * (Float32)Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::D)) {
			pos += side * 2.0f * (Float32)Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::SPACE)) {
			pos.y += 2.0f * Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::LCTRL)) {
			pos.y -= 2.0f * Globals.frametime;
		}

        outView = glm::lookAt(pos, pos + dir, glm::vec3(0, 1, 0));
        outProjection = glm::perspective(glm::radians(fov), (Float32)Globals.width / Globals.height, nearz, farz);
    }

}