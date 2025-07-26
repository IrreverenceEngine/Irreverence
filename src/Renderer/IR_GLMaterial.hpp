#pragma once

#include <IR_Material.hpp>
#include <Renderer/IR_GLTexture.hpp>
#include <Renderer/IR_GLShader.hpp>

namespace IR::Renderer {
    class GLMaterial : public Material {
    public:
        struct Info {
            UInt32 handleIndexes[Material::MAP__COUNT];
        };

        void Bind();

        void Use();
        void Reset();

        UInt32 GetBTIndex() const IR_RETURN(m_BTIndex)

        void MakeTexture(Map map, const char* path, bool linearize, bool mipmaps) override;
        void MakeTexture(Map map, const UInt8* data, UInt32 width, UInt32 height, UInt8 channelnum, bool linearize, bool mipmaps) override;
        void AddTexture(Map map, Texture* texture) override;
        void SetShader(Shader* shader) override;

    private:
        Shader* m_Shader = nullptr;
        UInt32 m_BTIndex = UINT32_MAX;
        GLTexture* m_Textures[Map::MAP__COUNT] = {};
        UInt32 m_TextureIDs[Map::MAP__COUNT] = {}; // TODO: Remove this, don't need it anymore (cuz MDI & BT)
    };
}