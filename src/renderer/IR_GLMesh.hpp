#pragma once

#include <IR_LayoutType.hpp>
#include <IR_Common.hpp>

namespace IR::Renderer {
    struct GLLayout;

    struct GLMesh {
        bool Init(const GLLayout& layout, const VertexStandard* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum);
        bool Init(const GLLayout& layout, const VertexAnimated* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum);
        void Destroy();

        void Draw(const void* instance_data);

        UInt32 id = 0;
        UInt32 ibo = 0;
        UInt32 vbo = 0;
        UInt32 ebo = 0;
        UInt32 vertCount = 0;
        UInt32 indexCount = 0;
        UInt32 vertStride = 0;
        UInt32 instStride = 0;
    };
}