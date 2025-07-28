#pragma once

#include <IR_Material.hpp>
#include <IR_LayoutType.hpp>
#include <IR_Common.hpp>

namespace IR::Renderer {
    class Model {
    public:
        static constexpr UInt8 MAX_SKINS = 8;

        bool Init(const char* path);
        virtual bool Init(const void* meshes, UInt32 count) = 0;
        virtual void Destroy() {}

        virtual void MakeMesh(const VertexStandard* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum, const Material** skins, UInt8 skinnum) = 0;
        virtual void MakeMesh(const VertexAnimated* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum, const Material** skins, UInt8 skinnum) = 0;
        virtual void ReserveMesh(UInt32 count) = 0;
    };
}