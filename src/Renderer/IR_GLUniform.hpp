#pragma once

#include <IR_Common.hpp>

namespace IR::Renderer {
    class GLUniform {
    public:
        bool Init(const void* data, UInt64 size, UInt8 loc);
        void Destroy();

        void Update(const void* data, UInt64 size, UInt64 offset);
        void Bind();
        void Relocate(UInt8 loc);

    private:
        UInt32 m_ID = 0;
        UInt8 m_Location = 0;
        UInt64 m_CurrentSize = 0;
    };
}