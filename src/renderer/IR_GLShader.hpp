#pragma once

#include <IR_Common.hpp>

#include <string>

namespace IR::Renderer {
    struct GLShader {
        bool InitRaster(const char* vscode, const char* fscode);
        void Destroy();

        UInt32 id = 0;
        std::string name;
    };
}