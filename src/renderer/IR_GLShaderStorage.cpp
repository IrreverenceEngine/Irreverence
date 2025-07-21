#include <renderer/IR_GLShaderStorage.hpp>

#include <GL/glew.h>

namespace IR::Renderer {
    constexpr UInt32 SSBO_FLAGS1 = GL_DYNAMIC_STORAGE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT;
    constexpr UInt32 SSBO_FLAGS2 = GL_MAP_PERSISTENT_BIT | GL_MAP_WRITE_BIT;

    bool GLShaderStorage::Init(const void* data, UInt64 size, UInt8 loc, bool realloc)
    {
        glCreateBuffers(1, &id);
        glNamedBufferStorage(id, size, data, SSBO_FLAGS1);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, loc, id);

        map = glMapNamedBufferRange(id, 0, size, SSBO_FLAGS2);

        reallocate = realloc;
        location = loc;
        currentSize = size;

        return true;
    }

    void GLShaderStorage::Destroy()
    {
        if (map) {
            glUnmapNamedBuffer(id);
        }

        glDeleteBuffers(1, &id);
        id = 0;
    }

    void GLShaderStorage::Update(const void* data, UInt64 size, UInt64 offset)
    {
        if (size == 0) {
            return;
        }

        UInt64 sizeOffset = size + offset;

        if (sizeOffset > currentSize) {
            if (!reallocate) {
                return;
            }

            UInt32 newId = 0;

            while (sizeOffset > currentSize) {
                currentSize *= 2;
            }

            UInt64 oldSize = currentSize;
            glCreateBuffers(1, &newId);
            glNamedBufferStorage(newId, currentSize, nullptr, SSBO_FLAGS1);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, location, newId);

            glCopyNamedBufferSubData(id, newId, 0, 0, oldSize);

            map = glMapNamedBufferRange(newId, 0, currentSize, SSBO_FLAGS2);

            glUnmapNamedBuffer(id);
            glDeleteBuffers(1, &id);

            id = newId;
        }

        memcpy((char*)map + offset, data, size);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

}