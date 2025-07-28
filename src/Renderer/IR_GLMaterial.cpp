#include <Renderer/IR_GLMaterial.hpp>
#include <Renderer/IR_GLRenderer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    void GLMaterial::Bind()
    {
        for (UInt32 i = 0; i < MAP__COUNT; i++) {
            m_Textures[i]->Bind(i);
        }
    }

    void GLMaterial::Use()
    {
        if (m_BTIndex != UINT32_MAX) {
            return;
        }

        GLMaterial::Info info;
        for (UInt8 i = 0; i < MAP__COUNT; i++) {
            GLTexture* texture = m_Textures[i];
            texture->Use();

            // TODO: Use Error Texture if not existing.
            info.handleIndexes[i] = texture->GetBTIndex();
        }

        m_BTIndex = s_GL->UseMaterialInfo(info);
    }

    void GLMaterial::Reset()
    {
        m_BTIndex = UINT32_MAX;
    }

    void GLMaterial::MakeTexture(Map map, const char* path, bool linearize, bool mipmaps)
    {
        GLTexture* texture = (GLTexture*)s_GL->MakeTexture();
        texture->InitPath(path, linearize, mipmaps);

        m_Textures[map] = texture;
    }

    void GLMaterial::MakeTexture(Map map, const UInt8* data, UInt32 width, UInt32 height, UInt8 channelnum, bool linearize, bool mipmaps)
    {
        GLTexture* texture = (GLTexture*)s_GL->MakeTexture();
        texture->InitMemory(data, width, height, channelnum, linearize, mipmaps);

        m_Textures[map] = texture;
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

}