#pragma once

#include <IR_Common.hpp>
#include <IR_LayoutType.hpp>

namespace IR::Renderer {
    struct Model {
        virtual ~Model() {}

        bool Init(const char* path);
        virtual bool Init(const void* meshes, UInt32 count) = 0;

        virtual void Draw(const void* data) = 0;

        virtual void MakeMesh(const VertexStandard* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum) = 0;
        virtual void MakeMesh(const VertexAnimated* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum) = 0;
        virtual void ReserveMesh(UInt32 count) = 0;
    };
}