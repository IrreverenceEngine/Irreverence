#pragma once

#include <Renderer/IR_GLMesh.hpp>
#include <IR_Common.hpp>

#include <string>

namespace IR::Renderer {

    class GLShader {
    public:
        bool InitRasterPath(const char* vspath, const char* fspath);
        bool InitRasterMemory(const char* vscode, const char* fscode);
        void Destroy();

        void Bind();

        UInt32 GetID() const IR_RETURN(m_ID)

    private:
        UInt32 m_ID = 0;
        std::string m_Name;
    };

}