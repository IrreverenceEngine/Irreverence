#pragma once

#include <IR_Material.hpp>
#include <renderer/IR_GLTexture.hpp>

namespace IR::Renderer {
    struct GLMaterial : public Material {
        ~GLMaterial() {}

        void Bind() override;

        void Use();
        void Reset();

        void MakeTexture(Map map, const char* path, bool linearize, bool mipmaps) override;
        void MakeTexture(Map map, const UInt8* data, UInt32 width, UInt32 height, UInt8 channelnum, bool linearize, bool mipmaps) override;
        void AddTexture(Map map, void* texture) override;

        UInt32 m_BTIndex = UINT32_MAX;
        GLTexture* m_Textures[Map::MAP__COUNT] = {};
        UInt32 m_TextureIDs[Map::MAP__COUNT] = {};
    };

}