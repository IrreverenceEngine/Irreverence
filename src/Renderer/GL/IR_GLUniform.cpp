#include <Renderer/GL/IR_GLUniform.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    bool GLUniform::Init(const void* data, UInt64 size, UInt8 loc)
    {
        glCreateBuffers(1, &m_ID);
        glNamedBufferData(m_ID, (GLsizeiptr)size, data, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, loc, m_ID);

        m_CurrentSize = size;
        m_Location = loc;

        return true;
    }

    void GLUniform::Destroy()
    {
        glDeleteBuffers(1, &m_ID);
        m_ID = 0;
    }

    void GLUniform::Update(const void* data, UInt64 size, UInt64 offset)
    {
        if (size == 0 || size + offset > m_CurrentSize) {
            return;
        }

        glNamedBufferSubData(m_ID, offset, size, data);
    }

    void GLUniform::Bind()
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, m_Location, m_ID);
    }

    void GLUniform::Relocate(UInt8 loc)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, loc, m_ID);
        m_Location = loc;
    }

}