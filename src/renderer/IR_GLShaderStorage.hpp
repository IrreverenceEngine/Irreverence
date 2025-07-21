#pragma once

#include <IR_Common.hpp>

namespace IR::Renderer {
    struct GLShaderStorage {
        bool Init(const void* data, UInt64 size, UInt8 loc, bool realloc = false);
        void Destroy();

        void Update(const void* data, UInt64 size, UInt64 offset);

        UInt32 id = 0;
        UInt8 location = 0;
        UInt64 currentSize = 0;
        void* map = nullptr;
        bool reallocate = false;
    };
}