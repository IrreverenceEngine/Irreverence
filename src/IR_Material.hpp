#pragma once

#include <IR_Texture.hpp>
#include <IR_Shader.hpp>
#include <IR_Common.hpp>

namespace IR::Renderer {
    class Material {
    public:
        enum Map {
            MAP_DIFFUSE,
            MAP__COUNT
        };

        bool Init(const char* path);
        virtual void Destroy() {}

        virtual void MakeTexture(Map map, const char* path, bool linearize, bool gen_mipmaps) = 0;
        virtual void MakeTexture(Map map, const UInt8* data, UInt32 width, UInt32 height, UInt8 channel_count, bool linearize, bool gen_mipmaps) = 0;
        virtual void AddTexture(Map map, Texture* texture) = 0;
        virtual void SetShader(Shader* shader) = 0;
    };
}