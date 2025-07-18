#include <renderer/IR_GLTexture.hpp>

#include <thirdparty/stb_image.h>
#include <GL/glew.h>

#include <cmath>

#include <string>

namespace IR::Renderer {

    bool GLTexture::InitPath(const char* path, bool linearize, bool gen_mipmaps)
    {
        stbi_set_flip_vertically_on_load(true);

        Int32 width = 0, height = 0, channels = 0;
        UInt8* data = stbi_load(("assets/textures/" + std::string(path)).c_str(), &width, &height, &channels, 0);

        if (!data) {
            IR_MSG(ERROR, "GL Renderer failed to open image \"%s\", reason: %s", path, stbi_failure_reason());
            return false;
        }

        bool success = InitMemory(data, width, height, channels, linearize, gen_mipmaps);

        stbi_image_free(data);

        return success;
    }

    bool GLTexture::InitMemory(const UInt8* data, UInt32 width, UInt32 height, UInt8 channel_count, bool linearize, bool gen_mipmaps)
    {
        width = width;
        height = height;
        channelCount = channel_count;
        mipCount = 0;

        glCreateTextures(GL_TEXTURE_2D, 1, &id);

        glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLenum glFormat = GL_RGBA8;
        GLenum glFormatAlt = GL_RGBA;
        if (channel_count == 4) IR_LIKELY {
            glFormat = GL_RGBA8;
            glFormatAlt = GL_RGBA;
        } else if (channel_count == 3) IR_LIKELY {
            glFormat = GL_RGB8;
            glFormatAlt = GL_RGB;
        } else if (channel_count == 2) IR_UNLIKELY {
            glFormat = GL_RG8;
            glFormatAlt = GL_RG;
        } else if (channel_count == 1) IR_UNLIKELY {
            glFormat = GL_R8;
            glFormatAlt = GL_RED;
        }

        if (gen_mipmaps && data) {
            GLenum minFilEnum = linearize ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
            GLenum minFilEnumMag = linearize ? GL_LINEAR : GL_NEAREST;

            glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, minFilEnum);
            glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, minFilEnumMag);

            Float32 maxAllowAniso = 0.0f;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAllowAniso);
            glTextureParameterf(id, GL_TEXTURE_MAX_ANISOTROPY, Clamp(maxAllowAniso, 0.0f, 16.0f));

            mipCount = (UInt32)(1 + floorf(log2f(Max(width, height))));
            glTextureStorage2D(id, mipCount, glFormat, width, height);
            glTextureSubImage2D(id, 0, 0, 0, width, height, glFormatAlt, GL_UNSIGNED_BYTE, data);

            glGenerateTextureMipmap(id);
        } else {
            GLenum minFilEnum = linearize ? GL_LINEAR : GL_NEAREST;
            glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, minFilEnum);
            glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, minFilEnum);

            glTextureStorage2D(id, 1, glFormat, width, height);

            if (data) {
                glTextureSubImage2D(id, 0, 0, 0, width, height, glFormatAlt, GL_UNSIGNED_BYTE, data);
            }
        }

        return true;
    }

    void GLTexture::Destroy()
    {
        glDeleteTextures(1, &id);
        id = 0;
    }

    void GLTexture::Bind(UInt8 loc)
    {
        glBindTextureUnit(loc, id);
    }

}