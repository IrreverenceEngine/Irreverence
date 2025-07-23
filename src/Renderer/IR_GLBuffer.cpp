#include <Renderer/IR_GLBuffer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {
    constexpr UInt32 BUFFER_FLAGS1 = GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT;
    constexpr UInt32 BUFFER_FLAGS2 = GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT;

    bool GLBuffer::Init(Int32 type, const void* data, UInt64 size, UInt8 loc, bool realloc)
    {
        glCreateBuffers(1, &id);
        glNamedBufferStorage(id, size, data, BUFFER_FLAGS1);

        if (loc != UINT8_MAX) {
            glBindBufferBase(type, loc, id);
        }

        map = glMapNamedBufferRange(id, 0, size, BUFFER_FLAGS2);

        reallocate = realloc;
        location = loc;
        currentSize = size;
        btype = type;

        return true;
    }

    void GLBuffer::Destroy()
    {
        if (map) {
            glUnmapNamedBuffer(id);
        }

        glDeleteBuffers(1, &id);
        id = 0;
    }

    void GLBuffer::Bind()
    {
        glBindBuffer(btype, id);
    }

    bool GLBuffer::Update(const void* data, UInt64 size, UInt64 offset)
    {
        if (size == 0) {
            return false;
        }

        UInt64 sizeOffset = size + offset;

        bool resized = false;
        if (sizeOffset > currentSize) {
            if (!reallocate) {
                return false;
            }

            UInt32 newId = 0;

            UInt64 oldSize = currentSize;
            while (sizeOffset > currentSize) {
                currentSize *= 2;
            }

            glCreateBuffers(1, &newId);
            glNamedBufferStorage(newId, currentSize, nullptr, BUFFER_FLAGS1);

            if (location != UINT8_MAX) {
                glBindBufferBase(btype, location, newId);
            }

            glCopyNamedBufferSubData(id, newId, 0, 0, oldSize);

            map = glMapNamedBufferRange(newId, 0, currentSize, BUFFER_FLAGS2);

            glUnmapNamedBuffer(id);
            glDeleteBuffers(1, &id);

            id = newId;

            resized = true;
        }

        memcpy((char*)map + offset, data, size);

        return resized;
    }

}