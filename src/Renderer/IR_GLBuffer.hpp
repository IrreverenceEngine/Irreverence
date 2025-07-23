#pragma once

#include <IR_Common.hpp>

namespace IR::Renderer {
    struct GLBuffer {
        bool Init(Int32 type, const void* data, UInt64 size, UInt8 loc = UINT8_MAX, bool realloc = false);
        void Destroy();

        void Bind();
        bool Update(const void* data, UInt64 size, UInt64 offset);

        UInt32 id = 0;
        UInt8 location = 0;
        UInt64 currentSize = 0;
        void* map = nullptr;
        bool reallocate = false;
        Int32 btype = 0;
    };

}