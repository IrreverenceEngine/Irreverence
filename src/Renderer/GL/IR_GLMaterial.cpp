#include <Renderer/GL/IR_GLMaterial.hpp>
#include <Renderer/GL/IR_GLRenderer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    void GLMaterial::Destroy()
    {
        if (m_InfoIndex) {
            s_GL->RemoveMaterialInfo(m_InfoIndex);
        }
    }

    void GLMaterial::AddTexture(Map map, Texture* texture)
    {
        GLTexture* gltexture = (GLTexture*)texture;

        m_Textures[map] = gltexture;
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