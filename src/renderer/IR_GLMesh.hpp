#pragma once

#include <IR_MeshType.hpp>
#include <IR_Common.hpp>

namespace IR::Renderer {
    struct GLMesh {
        bool Init(const VertexStandard* vertices, UInt32 vertex_count, const UInt32* indices, UInt32 index_count);
        bool Init(const VertexAnimated* vertices, UInt32 vertex_count, const UInt32* indices, UInt32 index_count);
        void Destroy();

        UInt32 id;
        UInt32 vbo;
        UInt32 ibo;
        UInt32 ebo;
        UInt32 vertCount;
        UInt32 indexCount;
    };
}