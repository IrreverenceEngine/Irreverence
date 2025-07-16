#pragma once

#include <IR_Renderer.hpp>

namespace IR::Window {
    bool Init(Renderer::API api);
    void Shutdown();

    void Close();

    bool ShouldClose();
    void* GetHandle();
}