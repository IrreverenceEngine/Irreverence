#pragma once

#include <Renderer/GL/IR_GLTexture.hpp>
#include <IR_Common.hpp>

namespace IR::Renderer {
    struct GLAttachment {
        GLAttachment(UInt32 format, UInt32 type = 0, UInt8 maxMips = 0)
            : format(format), type(type), maxMips(maxMips) {}

        GLAttachment()
            : format(0), type(0), maxMips(0) {}

        UInt32 format;
        UInt32 type;
        UInt8 maxMips; // Only for Color attachments
    };

    class GLFrame {
    public:
        static constexpr UInt8 MAX_COLOR_ATTACHS = 16;

        bool Init(UInt32 width, UInt32 height, UInt8 samples, const GLAttachment (&colors)[MAX_COLOR_ATTACHS], const GLAttachment& depth);
        void Destroy();

        void Resize(UInt32 width, UInt32 height);

        void CopyTo(GLFrame* to, UInt8 colorIndex);
        void ClearColor(UInt8 colorIndex, Color color);
        void RelinkAttachment(UInt8 colorIndex, UInt8 miplevel);

        void Bind();
        void UnBind();

        UInt32 GetID() const IR_RETURN(m_ID)
        UInt32 GetWidth() const IR_RETURN(m_Width)
        UInt32 GetHeight() const IR_RETURN(m_Height)
        GLTexture* GetColorTexture(UInt8 index) const IR_RETURN(index < MAX_COLOR_ATTACHS ? m_Colors[index] : nullptr)
        GLTexture* GetDepthTexture() const IR_RETURN(m_Depth)

    private:
        UInt32 m_ID = 0;
        UInt32 m_RBO = 0;
        UInt32 m_Width = 0;
        UInt32 m_Height = 0;
        UInt8 m_Samples = 0;

        GLTexture* m_Colors[MAX_COLOR_ATTACHS] = {};
        GLAttachment m_ColorInfos[MAX_COLOR_ATTACHS] = {};

        GLTexture* m_Depth = nullptr;
        GLAttachment m_DepthInfo = {};
    };
}
