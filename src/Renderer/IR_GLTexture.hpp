#pragma once

#include <IR_Common.hpp>

namespace IR::Renderer {
    class GLTexture {
    public:
        bool InitPath(const char* path, bool linearize, bool mipmaps, bool handle = false);
        bool InitMemory(const UInt8* data, UInt32 width, UInt32 height, UInt8 channelnum, bool linearize, bool mipmaps, bool handle = false);
        void Destroy();

        void Bind(UInt8 loc);
        void Use();
        void Reset();

        UInt32 GetID() const IR_RETURN(m_ID);
        UInt32 GetBTHandle() const IR_RETURN(m_BTHandle);
        UInt32 GetBTIndex() const IR_RETURN(m_BTIndex);

    private:
        UInt32 m_ID = 0;
        UInt64 m_BTHandle = 0; // Bindless Texture handle
        UInt32 m_BTIndex = UINT32_MAX; // Location in the BT Shader Storage
        UInt32 m_Width = 0;
        UInt32 m_Height = 0;
        UInt8 m_ChannelCount = 0;
        UInt8 m_MipCount = 0;
    };

}