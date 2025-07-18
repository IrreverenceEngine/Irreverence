#pragma once

#include <IR_Common.hpp>

namespace IR::Renderer {
    struct Material {
        enum Map {
            MAP_DIFFUSE,
            MAP__COUNT
        };

        virtual ~Material() {}

        bool Init(const char* path);

        virtual void Bind() = 0;

        virtual void MakeTexture(Map map, const char* path, bool linearize, bool gen_mipmaps) = 0;
        virtual void MakeTexture(Map map, const UInt8* data, UInt32 width, UInt32 height, UInt8 channel_count, bool linearize, bool gen_mipmaps) = 0;
        virtual void AddTexture(Map map, const void* texture) = 0; // Destroys the texture (may double destroy, it is fine though!)
    };
}