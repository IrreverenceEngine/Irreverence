#include <Renderer/GL/IR_GLBuffer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {
    constexpr UInt32 BUFFER_FLAGS = GL_DYNAMIC_STORAGE_BIT;

    bool GLBuffer::Init(Int32 type, const void* data, UInt64 size, UInt8 loc, bool realloc)
    {
        glCreateBuffers(1, &m_ID);
        glNamedBufferStorage(m_ID, size, data, BUFFER_FLAGS);

        if (loc != UINT8_MAX) {
            glBindBufferBase(type, loc, m_ID);
        }

        m_Realloc = realloc;
        m_Location = loc;
        m_CurrentSize = size;
        m_Type = type;

        return true;
    }

    void GLBuffer::Destroy()
    {
        if (m_ID == 0) {
            return;
        }

        glDeleteBuffers(1, &m_ID);
        m_ID = 0;
    }

    void GLBuffer::Bind() const
    {
        glBindBuffer(m_Type, m_ID);
    }

    bool GLBuffer::Update(const void* data, UInt64 size, UInt64 offset, bool copy)
    {
        if (size == 0) {
            return false;
        }

        UInt64 sizeOffset = size + offset;
        bool resized = false;

        if (sizeOffset > m_CurrentSize) {
            if (!m_Realloc) {
                return false;
            }

            UInt32 newId = 0;
            UInt64 oldSize = m_CurrentSize;
            while (sizeOffset > m_CurrentSize) {
                m_CurrentSize *= 2;
            }

            glCreateBuffers(1, &newId);
            glNamedBufferStorage(newId, m_CurrentSize, nullptr, BUFFER_FLAGS);

            if (copy) {
                glCopyNamedBufferSubData(m_ID, newId, 0, 0, oldSize);
            }

            if (m_Location != UINT8_MAX) {
                glBindBufferBase(m_Type, m_Location, newId);
            }

            glDeleteBuffers(1, &m_ID);
            m_ID = newId;
            resized = true;
        }

        glNamedBufferSubData(m_ID, offset, size, data);

        return resized;
    }

}
