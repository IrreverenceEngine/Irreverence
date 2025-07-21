#pragma once

#include <IR_Common.hpp>

namespace IR::Renderer {
    struct GLTexture {
        bool InitPath(const char* path, bool linearize, bool mipmaps, bool handle = false);
        bool InitMemory(const UInt8* data, UInt32 width, UInt32 height, UInt8 channelnum, bool linearize, bool mipmaps, bool handle = false);
        void Destroy();

        void Bind(UInt8 loc);

        UInt32 id = 0;
        UInt64 bthandle = 0; // Bindless Texture handle
        UInt32 btlocation = UINT32_MAX; // Location in the BT SStorage
        UInt32 width = 0;
        UInt32 height = 0;
        UInt8 channelCount = 0;
        UInt8 mipCount = 0;
    };
}