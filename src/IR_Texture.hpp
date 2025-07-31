#pragma once

#include <IR_Common.hpp>

namespace IR::Renderer {
    class Texture {
    public:
		virtual ~Texture() = default;

        bool Init(const char* path, bool linearize, bool mipmaps, bool handle = false);
        virtual bool InitMemory(const UInt8* data, UInt32 width, UInt32 height, UInt8 channelnum, bool linearize, bool mipmaps, bool handle = false) = 0;
        virtual void Destroy() {}
    };
}
