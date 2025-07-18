#pragma once

#include <IR_Material.hpp>
#include <renderer/IR_GLTexture.hpp>

namespace IR::Renderer {
    struct GLMaterial : public Material {
        ~GLMaterial();

        void Bind() override;

        void MakeTexture(Map map, const char* path, bool linearize, bool gen_mipmaps) override;
        void MakeTexture(Map map, const UInt8* data, UInt32 width, UInt32 height, UInt8 channelnum, bool linearize, bool gen_mipmaps) override;
        void AddTexture(Map map, const void* texture) override;

        GLTexture m_Textures[Map::MAP__COUNT] = {};
    };

}