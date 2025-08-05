#include <Renderer/GL/IR_GLMaterial.hpp>
#include <Renderer/GL/IR_GLRenderer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    bool GLMaterial::Init(const Texture*(&textures)[MAP__COUNT], Shader* shader)
    {
        if (!shader) {
            IR_MSG(ERROR, "Material has no shader");
            return false;
        }

        for (UInt8 i = 0; i < MAP__COUNT; i++) {
            GLTexture* tex = (GLTexture*)textures[i];
            m_Textures[i] = tex ? tex : s_GL->GetDefaultTexture((Material::Map)i);
        }

        return true;
    }

    void GLMaterial::Destroy()
    {
        if (m_InfoIndex) {
            s_GL->RemoveMaterialInfo(m_InfoIndex);
        }
    }

    void GLMaterial::SetTexture(Map map, Texture* texture)
    {
        GLTexture* gltexture = (GLTexture*)texture;

        m_Textures[map] = gltexture;

        if (m_InfoIndex != UINT32_MAX) {
            Reset();
        }
    }

    void GLMaterial::SetShader(Shader* shader)
    {
        m_Shader = (GLShader*)shader;
    }

    void GLMaterial::Bind()
    {
        for (UInt32 i = 0; i < MAP__COUNT; i++) {
            m_Textures[i]->Bind(i);
        }
    }

    void GLMaterial::Use()
    {
        if (m_InfoIndex != UINT32_MAX) {
            return;
        }

        GLMaterial::Info info;
        for (UInt8 i = 0; i < MAP__COUNT; i++) {
            GLTexture* texture = m_Textures[i];
            texture->Use();

            // TODO: Use Error Texture if not existing.
            info.handleIndexes[i] = texture->GetBTIndex();
        }

        m_InfoIndex = s_GL->UseMaterialInfo(info);
    }

    void GLMaterial::Reset()
    {
        GLMaterial::Info info;
        for (UInt8 i = 0; i < MAP__COUNT; i++) {
            GLTexture* texture = m_Textures[i];
            texture->Reset();

            info.handleIndexes[i] = texture->GetBTIndex();
        }

        if (m_InfoIndex != UINT32_MAX) {
            s_GL->RemoveMaterialInfo(m_InfoIndex);
        }

        m_InfoIndex = s_GL->UseMaterialInfo(info);
    }

}