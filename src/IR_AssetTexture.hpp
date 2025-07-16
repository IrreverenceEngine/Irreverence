#pragma once

#include <IR_Common.hpp>

namespace IR::Asset::Texture {
    void Load(UInt8* data, Int32 width, Int32 height, UInt8 channels, bool mipmap);
    void Load(const char* path, bool mipmap);

    void Get(const char* path, bool mipmap = true, bool load = false); // Name is often the path

    void Cleanup();
}