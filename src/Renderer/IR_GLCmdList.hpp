#pragma once

#include <Renderer/IR_GLLayout.hpp>
#include <Renderer/IR_GLBuffer.hpp>
#include <IR_Common.hpp>

#include <unordered_map>

namespace IR::Renderer {
    struct GLCmdElements {
        UInt32 count;
        UInt32 instanceCount;
        UInt32 firstIndex;
        UInt32 baseVertex;
        UInt32 baseInstance;
    };

    class GLCmdList {
    public:
        void Init();
        void Destroy();

        void Submit(const GLMesh* mesh, const GLShader* shader, UInt32 instanceId);
        void Flush();
        void Draw();
        void DrawShaderless();

    private:
        GLBuffer m_Buffer;
        std::unordered_map<UInt32, std::vector<GLCmdElements>> m_List[(UInt8)GLLayout::Type::_COUNT];
    };
}