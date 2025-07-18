#include <renderer/IR_GLMaterial.hpp>

namespace IR::Renderer {

    GLMaterial::~GLMaterial()
    {
        for (UInt8 i = 0; i < IR_ARRLEN(m_Textures); i++) {
            m_Textures[i].Destroy();
        }
    }

    void GLMaterial::Bind()
    {
        for (UInt8 i = 0; i < IR_ARRLEN(m_Textures); i++) {
            m_Textures[i].Bind(i);
        }
    }

    void GLMaterial::MakeTexture(Map map, const char* path, bool linearize, bool gen_mipmaps)
    {
        GLTexture texture;
        texture.InitPath(path, linearize, gen_mipmaps);

        m_Textures[map] = texture;
    }

    void GLMaterial::MakeTexture(Map map, const UInt8* data, UInt32 width, UInt32 height, UInt8 channelnum, bool linearize, bool gen_mipmaps)
    {
        GLTexture texture;
        texture.InitMemory(data, width, height, channelnum, linearize, gen_mipmaps);

        m_Textures[map] = texture;
    }

    void GLMaterial::AddTexture(Map map, const void* texture)
    {
        memcpy(&m_Textures[map], texture, IR_ARRSTRIDE(m_Textures));
    }

}