#pragma once

#include <Renderer/IR_GLMesh.hpp>
#include <IR_Common.hpp>

#include <vector>
#include <string>

namespace IR::Renderer {

    struct GLShader {
        bool InitRasterPath(const char* vspath, const char* fspath);
        bool InitRasterMemory(const char* vscode, const char* fscode);
        void Destroy();

        void Bind();

        void SubmitCommands(const GLMesh* mesh, const void* instdata);
        void FlushCommands();

        UInt32 id = 0;
        std::string name;
    };

}