#include <Renderer/GL/IR_GLMeshPool.hpp>

#include <GL/glew.h>

namespace IR::Renderer {
    bool GLMeshPool::Init(UInt32 layoutId, UInt32 stride)
    {
        m_VertexBuffer.Init(GL_ARRAY_BUFFER, nullptr, stride, UINT8_MAX, true);
        glVertexArrayVertexBuffer(layoutId, 0, m_VertexBuffer.GetID(), 0, stride);

        m_IndexBuffer.Init(GL_ELEMENT_ARRAY_BUFFER, nullptr, sizeof(UInt32), UINT8_MAX, true);
        glVertexArrayElementBuffer(layoutId, m_IndexBuffer.GetID());

        m_Stride = stride;
        m_Layout = layoutId;

        return true;
    }

    void GLMeshPool::Destroy()
    {
        m_VertexBuffer.Destroy();
        m_IndexBuffer.Destroy();
    }

    std::pair<UInt32, UInt32> GLMeshPool::Add(const void* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indexnum)
    {
        UInt32 vId = m_VertexOffset;
        UInt32 iId = m_IndexOffset;

        m_VertexBuffer.Update(vertices, vertnum * m_Stride, vId * m_Stride);
        m_IndexBuffer.Update(indices, indexnum * sizeof(UInt32), iId * sizeof(UInt32));

        m_VertexOffset += vertnum;
        m_IndexOffset += indexnum;

        return { vId, iId };
    }

    void GLMeshPool::Bind() const
    {
        glBindVertexArray(m_Layout);
        glVertexArrayVertexBuffer(m_Layout, 0, m_VertexBuffer.GetID(), 0, m_Stride);
        glVertexArrayElementBuffer(m_Layout, m_IndexBuffer.GetID());
    }
}
