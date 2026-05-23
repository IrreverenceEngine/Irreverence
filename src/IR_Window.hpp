#pragma once

#include <IR_Renderer.hpp>

namespace IR {
    struct Window {
        static bool Init(Renderer::API api);
        static void Shutdown();

        static void Close();

        static bool ShouldClose();
        static void* GetHandle();

        static bool IsMouseLocked();
        static void LockMouse(bool lock);
        static void ToggleMouseLock();

        static bool IsFullscreen();
        static void SetFullscreen(bool fullscreen);
        static void ToggleFullscreen();
    };
}
