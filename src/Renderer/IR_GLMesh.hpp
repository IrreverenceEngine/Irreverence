#pragma once

#include <IR_LayoutType.hpp>
#include <IR_Common.hpp>

namespace IR::Renderer {

    struct GLMesh {
        bool Init(const VertexStandard* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum);
        bool Init(const VertexAnimated* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum);

        UInt32 vertOffset = 0;
        UInt32 indexOffset = 0;
        UInt32 vertCount = 0;
        UInt32 indexCount = 0;
        UInt8 layoutType;
    };
}