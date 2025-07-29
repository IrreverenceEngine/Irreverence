#pragma once

#include <Renderer/GL/IR_GLMesh.hpp>

#include <IR_Shader.hpp>
#include <IR_Common.hpp>

#include <string>

namespace IR::Renderer {
    class GLShader : public Shader {
    public:
        bool InitRasterMemory(const char* vscode, const char* fscode) override;
        bool InitComputeMemory(const char* cscode) override;
        void Destroy() override;

        void Bind();

        UInt32 GetID() const IR_RETURN(m_ID)

    private:
        UInt32 m_ID = 0;
        std::string m_Name;
    };
}