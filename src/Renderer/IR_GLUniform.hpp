#pragma once

#include <IR_Common.hpp>

namespace IR::Renderer {
    struct GLUniform {
        bool Init(const void* data, UInt64 size, UInt8 loc);
        void Destroy();

        void Update(const void* data, UInt64 size, UInt64 offset);
        void Bind();
        void Relocate(UInt8 loc);

        UInt32 id = 0;
        UInt8 location = 0;
        UInt64 currentSize = 0;
    };
}