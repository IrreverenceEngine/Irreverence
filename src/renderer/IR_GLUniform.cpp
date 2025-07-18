#include <renderer/IR_GLUniform.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    bool GLUniform::Init(const void* data, UInt64 size, UInt8 loc)
    {
        glCreateBuffers(1, &id);
        glNamedBufferData(id, (GLsizeiptr)size, data, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, loc, id);

        currentSize = size;
        location = loc;

        return true;
    }

    void GLUniform::Destroy()
    {
        glDeleteBuffers(1, &id);
        id = 0;
    }

    void GLUniform::Update(const void* data, UInt64 size, UInt64 offset)
    {
        if (size == 0 || size + offset > currentSize) {
            return;
        }

        glNamedBufferSubData(id, offset, size, data);
    }

    void GLUniform::Bind()
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, location, id);
    }

    void GLUniform::Relocate(UInt8 loc)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, loc, id);
        location = loc;
    }

}