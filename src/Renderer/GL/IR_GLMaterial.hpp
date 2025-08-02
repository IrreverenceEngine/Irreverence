#pragma once

#include <IR_Material.hpp>
#include <Renderer/GL/IR_GLTexture.hpp>
#include <Renderer/GL/IR_GLShader.hpp>

namespace IR::Renderer {
    class GLMaterial : public Material {
    public:
        struct Info {
            UInt32 handleIndexes[Material::MAP__COUNT];
        };

        void Destroy() override;

        void AddTexture(Map map, Texture* texture) override;
        void SetShader(Shader* shader) override;

        void Bind();
        void Use();
        void Reset() override;

        UInt32 GetInfoIndex() const IR_RETURN(m_InfoIndex)
        GLShader* GetShader() const IR_RETURN(m_Shader)

    private:
        GLShader* m_Shader = nullptr;
        UInt32 m_InfoIndex = UINT32_MAX;
        GLTexture* m_Textures[Map::MAP__COUNT] = {};
    };
}