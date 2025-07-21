#include <renderer/IR_GLMaterial.hpp>
#include <renderer/IR_GLRenderer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    void GLMaterial::Bind()
    {
        glBindTextures(0, MAP__COUNT, m_TextureIDs);
    }

    void GLMaterial::Use()
    {
        if (m_BTIndex != UINT32_MAX) {
            return;
        }

        GL::MaterialInfo info;

        for (UInt8 i = 0; i < MAP__COUNT; i++) {
            GLTexture* texture = m_Textures[i];

            if (texture->btlocation == UINT32_MAX) {
                texture->btlocation = s_GL->m_TextureHandles.size();
                s_GL->m_TextureHandles.push_back(texture->bthandle);
            }

            // TODO: Use Error Texture if not existing.
            info.handleIndexes[i] = texture->btlocation;
        }

        m_BTIndex = s_GL->m_MaterialInfos.size();
        s_GL->m_MaterialInfos.push_back(info);
    }

    void GLMaterial::Reset()
    {
        m_BTIndex = UINT32_MAX;
    }

    void GLMaterial::MakeTexture(Map map, const char* path, bool linearize, bool mipmaps)
    {
        GLTexture* texture = s_GL->MakeTexture();
        texture->InitPath(path, linearize, mipmaps);

        m_Textures[map] = texture;
        m_TextureIDs[map] = texture->id;
    }

    void GLMaterial::MakeTexture(Map map, const UInt8* data, UInt32 width, UInt32 height, UInt8 channelnum, bool linearize, bool mipmaps)
    {
        GLTexture* texture = s_GL->MakeTexture();
        texture->InitMemory(data, width, height, channelnum, linearize, mipmaps);

        m_Textures[map] = texture;
        m_TextureIDs[map] = texture->id;
    }

    void GLMaterial::AddTexture(Map map, void* texture)
    {
        GLTexture* gltexture = (GLTexture*)texture;

        m_Textures[map] = gltexture;
        m_TextureIDs[map] = gltexture->id;
    }

}