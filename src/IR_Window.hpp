#pragma once

namespace IR::Window {
    bool Init();
    void Shutdown();

    void Close();

    bool ShouldClose();
    void* GetHandle();
}