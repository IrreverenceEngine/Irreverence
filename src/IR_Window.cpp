#include <IR_Window.hpp>
#include <IR_Common.hpp>
#include <IR_CVar.hpp>
#include <IR_Input.hpp>

#include <SDL3/SDL.h>

namespace IR::Window {
    
    static SDL_Window* s_Window = nullptr;
    static UInt64 s_StartTime = 0;
    static UInt64 s_FTNowTime = 0;
    static UInt64 s_FTLastTime = 0;
    static bool s_Close = false;

    bool Init(Renderer::API api)
    {
        SDL_Init(SDL_INIT_VIDEO);

        Globals.width = 1280;
        Globals.height = 720;

        SDL_WindowFlags flags = Renderer::PreInit(api);

        s_Window = SDL_CreateWindow("Irreverence", Globals.width, Globals.height, flags);
        if (!s_Window) {
            IR_MSG(ERROR, "Failed to init Window: Failed to create window");
            return false;
        }

        s_StartTime = SDL_GetPerformanceCounter();

        IR_SCRMSG(WARN, "Welcome to Irreverence!");

        s_FTNowTime = SDL_GetPerformanceCounter();
        s_FTLastTime = 0;
        
        IR_MSG(INFO, "Successfully initialized Window");

        return true;
    }

    void Shutdown()
    {
        SDL_DestroyWindow(s_Window);
        SDL_Quit();
    }

    void Close()
    {
        s_Close = true;
    }

    bool ShouldClose()
    {
        Input::Update();

        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_EVENT_QUIT:
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED: 
                return true;
            case SDL_EVENT_WINDOW_RESIZED:
                SDL_GetWindowSize(s_Window, &Globals.width, &Globals.height);
                break;
            case SDL_EVENT_KEY_DOWN:
                Input::KeyEvent((Input::Key)event.key.key, true);
                break;
            case SDL_EVENT_KEY_UP:
                Input::KeyEvent((Input::Key)event.key.key, false);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                Input::MButtonEvent((Input::MButton)event.button.button, true);
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                Input::MButtonEvent((Input::MButton)event.button.button, false);
                break;
            case SDL_EVENT_MOUSE_MOTION:
                Input::MMotionEvent({ event.motion.x, event.motion.y });
                break;
            }
        }

        Globals.curtime = (SDL_GetPerformanceCounter() - s_StartTime) / (Float64)SDL_GetPerformanceFrequency();

        s_FTLastTime = s_FTNowTime;
        s_FTNowTime = SDL_GetPerformanceCounter();
        Globals.frametime = (s_FTNowTime - s_FTLastTime) / (Float64)SDL_GetPerformanceFrequency();

        return s_Close;
    }

    void* GetHandle()
    {
        return s_Window;
    }
}