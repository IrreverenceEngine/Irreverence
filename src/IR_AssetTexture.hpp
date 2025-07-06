#pragma once

#include <IR_Common.hpp>

#include <bgfx/bgfx.h>

namespace IR::Asset::Texture {
    bgfx::TextureHandle Load(UInt8* data, Int32 width, Int32 height, UInt8 channels, bool mipmap);
    bgfx::TextureHandle Load(const char* path, bool mipmap);

    bgfx::TextureHandle Get(const char* path, bool mipmap = true, bool load = false); // Name is often the path

    void Cleanup();
}