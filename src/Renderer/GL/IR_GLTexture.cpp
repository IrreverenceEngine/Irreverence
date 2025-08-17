#include <Renderer/GL/IR_GLTexture.hpp>
#include <Renderer/GL/IR_GLRenderer.hpp>

#include <GL/glew.h>

#include <cmath>

namespace IR::Renderer {

    bool GLTexture::InitMemory(const UInt8* data, UInt32 width, UInt32 height, UInt8 channel_count, bool linearize, bool mipmaps, bool handle)
    {
        width = width;
        height = height;
        m_ChannelCount = channel_count;
        m_MipCount = 1;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_REPEAT);

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

        if (mipmaps && data) {
            GLenum minFilEnum = linearize ? GL_LINEAR_MIPMAP_NEAREST : GL_NEAREST_MIPMAP_NEAREST;
            GLenum minFilEnumMag = linearize ? GL_LINEAR : GL_NEAREST;

            glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, minFilEnum);
            glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, minFilEnumMag);

            Float32 maxAllowAniso = 0.0f;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAllowAniso);
            glTextureParameterf(m_ID, GL_TEXTURE_MAX_ANISOTROPY, Math::Clamp(maxAllowAniso, 0.0f, 16.0f));

            m_MipCount = 1 + floorf(log2f(Math::Max(width, height)));
            glTextureStorage2D(m_ID, m_MipCount, glFormat, width, height);
            glTextureSubImage2D(m_ID, 0, 0, 0, width, height, glFormatAlt, GL_UNSIGNED_BYTE, data);

            glGenerateTextureMipmap(m_ID);
        } else {
            GLenum minFilEnum = linearize ? GL_LINEAR : GL_NEAREST;
            glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, minFilEnum);
            glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, minFilEnum);

            glTextureStorage2D(m_ID, 1, glFormat, width, height);

            if (data) {
                glTextureSubImage2D(m_ID, 0, 0, 0, width, height, glFormatAlt, GL_UNSIGNED_BYTE, data);
            }
        }

        if (handle) {
            m_BTHandle = glGetTextureHandleARB(m_ID);
            if (m_BTHandle == 0) {
                IR_MSG(FATAL, "GLTexture failed to make texture handle... this may be due to not having enough VRAM or a driver issue\n");
            }

            glMakeTextureHandleResidentARB(m_BTHandle);
        }

        return true;
    }

    void GLTexture::InitColorAttachment(const GLFrame& frame, UInt8 location, UInt32 width, UInt32 height, UInt8 samples, UInt32 format, UInt32 type, UInt8 maxMips)
    {
        m_Width = width;
        m_Height = height;
        m_MipCount = 1;

        if (samples == 0 || maxMips > 1) {
            glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);
            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            m_MipCount = 1 + floorf(log2f(Math::Max(width, height)));
            glTextureStorage2D(m_ID, m_MipCount, format, width, height);
        } else {
            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_ID);
            glTextureStorage2DMultisample(m_ID, samples, type, width, height, GL_TRUE);
        }

        glNamedFramebufferTexture(frame.GetID(), GL_COLOR_ATTACHMENT0 + location, m_ID, 0);
    }

    void GLTexture::InitDepthAttachment(const GLFrame& frame, UInt32 width, UInt32 height, UInt8 samples, UInt32 format, UInt32 type)
    {
        m_Width = width;
        m_Height = height;

        if (samples == 0) {
            glCreateTextures(GL_TEXTURE_2D, 1, &m_ID);

            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_ID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTextureParameteri(m_ID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(m_ID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureStorage2D(m_ID, 1, format, width, height);
        } else {
            glCreateTextures(GL_TEXTURE_2D_MULTISAMPLE, 1, &m_ID);
            glTextureStorage2DMultisample(m_ID, (GLsizei)samples, type, width, height, GL_TRUE);
        }

        glNamedFramebufferTexture(frame.GetID(), GL_DEPTH_ATTACHMENT, m_ID, 0);
    }

    void GLTexture::Destroy()
    {
        if (m_ID == 0) {
            return;
        }

        if (m_BTHandle != UINT64_MAX) {
            glMakeTextureHandleNonResidentARB(m_BTHandle);
            s_GL->RemoveTextureHandle(m_BTIndex);
        }

        glDeleteTextures(1, &m_ID);
        m_ID = 0;
    }

    void GLTexture::Bind(UInt8 loc)
    {
        glBindTextureUnit(loc, m_ID);
    }

    void GLTexture::Use()
    {
        if (m_BTIndex != UINT32_MAX) {
            return;
        }

        m_BTIndex = s_GL->UseTexture(*this);
    }

    void GLTexture::Reset()
    {
        if (m_BTIndex != UINT32_MAX) {
            s_GL->RemoveTextureHandle(m_BTIndex);
            m_BTIndex = UINT32_MAX;
        }

        m_BTIndex = s_GL->UseTexture(*this);
    }

}
