#include <Renderer/GL/IR_GLFrame.hpp>
#include <Renderer/GL/IR_GLRenderer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    bool GLFrame::Init(UInt32 width, UInt32 height, UInt8 samples, const GLAttachment (&colors)[MAX_COLOR_ATTACHS], const GLAttachment& depth)
    {
        m_Width = width;
        m_Height = height;
        m_Samples = samples;

        glCreateFramebuffers(1, &m_ID);

        UInt8 colorAttCount = 0;
        for (UInt8 i = 0; i < MAX_COLOR_ATTACHS; i++) {
            const GLAttachment& att = colors[i];
            if (att.type == 0) {
                continue;
            }

            colorAttCount = i;

            GLTexture* tex = (GLTexture*)s_GL->MakeTexture();
            tex->InitColorAttachment(*this, colorAttCount, width, height, samples, att.format, att.type, att.maxMips);

            m_Colors[colorAttCount] = tex;
            m_ColorInfos[colorAttCount] = att;
        }
        colorAttCount++;

        if (depth.format != 0) {
            GLTexture* tex = (GLTexture*)s_GL->MakeTexture();
            tex->InitDepthAttachment(*this, width, height, samples, depth.format, depth.type);
            m_Depth = tex;
            m_DepthInfo = depth;
        } else {
            glCreateRenderbuffers(1, &m_RBO);

            if (samples == 0) glNamedRenderbufferStorage(m_RBO, GL_DEPTH24_STENCIL8, width, height);
            else glNamedRenderbufferStorageMultisample(m_RBO, samples, GL_DEPTH24_STENCIL8, width, height);

            glNamedFramebufferRenderbuffer(m_ID, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
        }

        if (glCheckNamedFramebufferStatus(m_ID, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            IR_MSG(ERROR, "GL Frame couldn't init. Something fucked up.");
            Destroy();
            return false;
        }

        GLenum colorEnums[MAX_COLOR_ATTACHS] = { 0 };
        for (UInt8 i = 0; i < colorAttCount; i++) {
            colorEnums[i] = GL_COLOR_ATTACHMENT0 + i;
        }

        if (colorAttCount > 0) {
            glNamedFramebufferDrawBuffers(m_ID, colorAttCount, colorEnums);
        }
        else {
            glNamedFramebufferDrawBuffer(m_ID, 0);
            glNamedFramebufferReadBuffer(m_ID, 0);
        }

        return true;
    }

    void GLFrame::Destroy()
    {
        glDeleteFramebuffers(1, &m_ID);

        if (m_RBO > 0) {
            glDeleteRenderbuffers(1, &m_RBO);
        } else {
            m_Depth->Destroy();
        }

        for (UInt8 i = 0; i < MAX_COLOR_ATTACHS; i++) {
            GLTexture* tex = m_Colors[i];
            if (tex) {
                tex->Destroy();
            }
        }

        m_ID = 0;
    }

    void GLFrame::Resize(UInt32 width, UInt32 height)
    {
        m_Width = width;
        m_Height = height;

        for (UInt32 i = 0; i < MAX_COLOR_ATTACHS; i++) {
            GLTexture* col = m_Colors[i];
            GLAttachment info = m_ColorInfos[i];

            if (col) {
                col->Destroy();
                col->InitColorAttachment(*this, i, width, height, m_Samples, info.format, info.type, info.maxMips);
            }
        }

        if (m_Depth) {
            m_Depth->Destroy();
            m_Depth->InitDepthAttachment(*this, width, height, m_Samples, m_DepthInfo.format, m_DepthInfo.type);
        } else {
            glDeleteRenderbuffers(1, &m_RBO);
            glCreateRenderbuffers(1, &m_RBO);

            if (m_Samples == 0) glNamedRenderbufferStorage(m_RBO, GL_DEPTH24_STENCIL8, width, height);
            else glNamedRenderbufferStorageMultisample(m_RBO, m_Samples, GL_DEPTH24_STENCIL8, width, height);

            glNamedFramebufferRenderbuffer(m_ID, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
        }
    }

    void GLFrame::CopyTo(GLFrame* to, UInt8 colorIndex)
    {
        glNamedFramebufferReadBuffer(m_ID, GL_COLOR_ATTACHMENT0 + colorIndex);

        if (!to) {
            glBlitNamedFramebuffer(m_ID, 0, 0, 0, m_Width, m_Height, 0, 0, Globals.Width(), Globals.Height(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
        } else {
            glNamedFramebufferDrawBuffer(to->GetID(), GL_COLOR_ATTACHMENT0 + colorIndex);
            glBlitNamedFramebuffer(m_ID, to->GetID(), 0, 0, m_Width, m_Height, 0, 0, to->GetWidth(), to->GetHeight(), GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }
    }

    void GLFrame::ClearColor(UInt8 colorIndex, Color color)
    {
        const glm::vec4 col = {
            (Float32)color.r / 255.0,
            (Float32)color.g / 255.0,
            (Float32)color.b / 255.0,
            (Float32)color.a / 255.0
        };
        glClearNamedFramebufferfv(m_ID, GL_COLOR, colorIndex, (GLfloat*)&col);
    }

    void GLFrame::RelinkAttachment(UInt8 colorIndex, UInt8 miplevel)
    {
        glNamedFramebufferTexture(m_ID, GL_COLOR_ATTACHMENT0 + colorIndex, m_Colors[colorIndex]->GetID(), miplevel);
    }

    void GLFrame::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, GetID());
        glViewport(0, 0, GetWidth(), GetHeight());
    }

    void GLFrame::UnBind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Globals.Width(), Globals.Height());
    }

}
