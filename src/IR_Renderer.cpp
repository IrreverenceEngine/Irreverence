#include <IR_Renderer.hpp>
#include <IR_Common.hpp>
#include <IR_Window.hpp>
#include <IR_Input.hpp>
#include <IR_AssetShader.hpp>
#include <IR_AssetTexture.hpp>

#include <SDL3/SDL.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <glm/gtc/type_ptr.hpp>

#ifdef SDL_PLATFORM_WIN32
#include <windows.h>
#endif

namespace IR::Renderer {

    struct PosColorVertex
    {
        float m_x;
        float m_y;
        float m_z;
        uint32_t m_abgr;

        static void init()
        {
            ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
                .end();
        };

        static bgfx::VertexLayout ms_layout;
    };

    bgfx::VertexLayout PosColorVertex::ms_layout;

    static PosColorVertex s_cubeVertices[] =
    {
        {-1.0f,  1.0f,  1.0f, 0xff000000 },
        { 1.0f,  1.0f,  1.0f, 0xff0000ff },
        {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
        { 1.0f, -1.0f,  1.0f, 0xff00ffff },
        {-1.0f,  1.0f, -1.0f, 0xffff0000 },
        { 1.0f,  1.0f, -1.0f, 0xffff00ff },
        {-1.0f, -1.0f, -1.0f, 0xffffff00 },
        { 1.0f, -1.0f, -1.0f, 0xffffffff },
    };

    static const uint16_t s_cubeTriList[] =
    {
        0, 1, 2, // 0
        1, 3, 2,
        4, 6, 5, // 2
        5, 6, 7,
        0, 2, 4, // 4
        4, 2, 6,
        1, 5, 3, // 6
        5, 7, 3,
        0, 4, 1, // 8
        4, 5, 1,
        2, 3, 6, // 10
        6, 3, 7,
    };

    static glm::mat4 s_View;
    static glm::mat4 s_Projection;

    static bgfx::VertexBufferHandle s_VertBuffer;
    static bgfx::IndexBufferHandle s_IndexBuffer;
    static bgfx::ProgramHandle s_Program;
    static bgfx::TextureHandle s_Texturah;

    bool Init()
    {
        s_View = glm::mat4(1);
        s_Projection = glm::mat4(1);

        SDL_Window* sdlWindow = (SDL_Window*) Window::GetHandle();

        bgfx::PlatformData pd;
    #if defined(SDL_PLATFORM_WIN32)
        pd.nwh = (HWND) SDL_GetPointerProperty(SDL_GetWindowProperties(sdlWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
        pd.ndt = NULL;
    #elif defined(SDL_PLATFORM_MACOS)
        pd.nwh = (__bridge NSWindow*) SDL_GetPointerProperty(SDL_GetWindowProperties(sdlWindow), SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
        pd.ndt = NULL;
    #elif defined(SDL_PLATFORM_LINUX)
        if(SDL_strcmp(SDL_GetCurrentVideoDriver(), "x11") == 0) {
            pd.nwh = (void*) SDL_GetNumberProperty(SDL_GetWindowProperties(sdlWindow), SDL_PROP_WINDOW_X11_WINDOW_NUMBER, 0);
            pd.ndt = SDL_GetPointerProperty(SDL_GetWindowProperties(sdlWindow), SDL_PROP_WINDOW_X11_DISPLAY_POINTER, NULL);
        } else if(SDL_strcmp(SDL_GetCurrentVideoDriver(), "wayland") == 0) {
            pd.nwh = SDL_GetPointerProperty(SDL_GetWindowProperties(sdlWindow), SDL_PROP_WINDOW_WAYLAND_SURFACE_POINTER, NULL);
            pd.ndt = SDL_GetPointerProperty(SDL_GetWindowProperties(sdlWindow), SDL_PROP_WINDOW_WAYLAND_DISPLAY_POINTER, NULL);
        }
    #endif

        pd.context = NULL;
        pd.backBuffer = NULL;
        pd.backBufferDS = NULL;
        bgfx::setPlatformData(pd);

        bgfx::Init init;
        init.type = bgfx::RendererType::OpenGL;
        init.vendorId = BGFX_PCI_ID_NONE;
        init.platformData.nwh = pd.nwh;
        init.platformData.ndt = pd.ndt;
        init.resolution.width = Globals.width;
        init.resolution.height = Globals.height;
        init.resolution.reset = BGFX_RESET_VSYNC;
        if(!bgfx::init(init)) {
            IR_MSG(ERROR, "Failed to init Renderer: failed to init BGFX");
            return false;
        }

        bgfx::setDebug(BGFX_DEBUG_TEXT);
        bgfx::reset(Globals.width, Globals.height, BGFX_RESET_VSYNC);
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x1b2123ff, 1.0f, 0);

        IR_MSG(INFO, "Successfully initialized Renderer");

        PosColorVertex::init();

        s_VertBuffer = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)), PosColorVertex::ms_layout);
        s_IndexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList)));
        s_Program = Asset::Shader::LoadRaster("cube", true);
        s_Texturah = Asset::Texture::Load("__NODRAW.png", false);

        return true;
    }

    void Shutdown()
    {
        bgfx::destroy(s_VertBuffer);
        bgfx::destroy(s_IndexBuffer);

        bgfx::destroy(s_Program);

        bgfx::shutdown();
    }

    void Present()
    {
		Renderer::DebugCamera(s_View, s_Projection);

        bgfx::setViewTransform(0, glm::value_ptr(s_View), glm::value_ptr(s_Projection));
        bgfx::setViewRect(0, 0, 0, Globals.width, Globals.height);
        bgfx::touch(0);

        bgfx::setVertexBuffer(0, s_VertBuffer);
        bgfx::setIndexBuffer(s_IndexBuffer);

        UInt64 state = 0
            |BGFX_STATE_WRITE_R
            |BGFX_STATE_WRITE_G
            |BGFX_STATE_WRITE_B
            |BGFX_STATE_WRITE_A
            |BGFX_STATE_WRITE_Z
            |BGFX_STATE_DEPTH_TEST_LESS
            |BGFX_STATE_MSAA
            |BGFX_STATE_CULL_CCW;

        bgfx::setState(state);
        bgfx::submit(0, s_Program);

        bgfx::dbgTextClear();
        Log::DrawScrMsgs();

        bgfx::frame();
    }

    void UpdateCameraMatrices(const glm::mat4& view, const glm::mat4& projection)
    {
        s_View = view;
        s_Projection = projection;
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