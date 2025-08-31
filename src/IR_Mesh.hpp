#pragma once

#include <IR_LayoutType.hpp>
#include <IR_Common.hpp>

namespace IR::Renderer {
    class Mesh {
    public:
        virtual bool InitPool(const VertexBasic2D* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum) = 0;
        virtual bool InitPool(const VertexPosition* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum) = 0;
        virtual bool InitPool(const VertexStandard* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum) = 0;
        virtual bool InitPool(const VertexAnimated* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum) = 0;
        virtual void Destroy() {}
    };
}
