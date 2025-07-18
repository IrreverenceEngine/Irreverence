#pragma once

#include <IR_Common.hpp>

#include <string>

namespace IR::Renderer {
    struct GLShader {
        bool InitRasterPath(const char* vspath, const char* fspath);
        bool InitRasterMemory(const char* vscode, const char* fscode);
        void Destroy();

        void Bind();

        UInt32 id = 0;
        std::string name;
    };
}