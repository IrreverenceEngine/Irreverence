#pragma once

#include <IR_Common.hpp>

namespace IR::Renderer {
    class GLBuffer {
    public:
        bool Init(Int32 type, const void* data, UInt64 size, UInt8 loc = UINT8_MAX, bool realloc = false);
        void Destroy();

        void Bind() const;
        bool Update(const void* data, UInt64 size, UInt64 offset);

        UInt32 GetID() const IR_RETURN(m_ID)

    private:
        UInt32 m_ID = 0;
        UInt8 m_Location = 0;
        UInt64 m_CurrentSize = 0;
        void* m_Map = nullptr;
        bool m_Realloc = false;
        Int32 m_Type = 0;

        // TODO: Implement scheduled bulk updates, uint8 list
        struct ScheduledData {
            std::vector<UInt8> data;
            UInt64 offset;
        };
    };
}