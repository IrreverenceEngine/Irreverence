#pragma once

#include <IR_Common.hpp>
#include <Renderer/IR_GLBuffer.hpp>
#include <Renderer/IR_GLMesh.hpp>
#include <Renderer/IR_GLShader.hpp>

namespace IR::Renderer {

    class GLMeshPool { // Mesh Buffer
    public:
        bool Init(UInt32 layoutId, UInt64 stride);
        void Destroy();

        std::pair<UInt32, UInt32> Add(const void* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indexnum);
        void Bind() const;

    private:
        GLBuffer m_VertexBuffer;
        GLBuffer m_IndexBuffer;

        UInt64 m_VertexOffset = 0;
        UInt64 m_IndexOffset = 0;

        UInt32 m_Layout = 0;
        UInt64 m_Stride = 0;
    };
}