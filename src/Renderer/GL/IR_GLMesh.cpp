#include <Renderer/GL/IR_GLMesh.hpp>
#include <Renderer/GL/IR_GLLayout.hpp>
#include <Renderer/GL/IR_GLRenderer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

   bool GLMesh::InitPool(const VertexBasic2D* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
    {
        m_Layout = s_GL->GetLayout(GLLayout::Type::BASIC2D);
        const auto [vId, iId] = m_Layout->GetMeshPool().Add(vertices, vertexnum, indices, indexnum);

        m_VertNum = vertexnum;
        m_IndexNum = indexnum;
        m_VertOffset = vId;
        m_IndexOffset = iId;

        return true;
    }

    bool GLMesh::Init(const VertexBasic2D* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
    {
        m_Layout = s_GL->GetLayout(GLLayout::Type::BASIC2D);

        m_VertNum = vertexnum;
        m_IndexNum = indexnum;

        glCreateBuffers(1, &m_VBO);
        glNamedBufferStorage(m_VBO, vertexnum * m_Layout->GetStride(), vertices, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayVertexBuffer(m_Layout->GetID(), 0, m_VBO, 0, m_Layout->GetStride());

        glCreateBuffers(1, &m_EBO);
        glNamedBufferStorage(m_EBO, indexnum * sizeof(UInt32), indices, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayElementBuffer(m_Layout->GetID(), m_EBO);

        m_Pool = false;

        return true;
    }

    bool GLMesh::InitPool(const VertexStandard* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
    {
        m_Layout = s_GL->GetLayout(GLLayout::Type::STANDARD);
        const auto [vId, iId] = m_Layout->GetMeshPool().Add(vertices, vertexnum, indices, indexnum);

        m_VertNum = vertexnum;
        m_IndexNum = indexnum;
        m_VertOffset = vId;
        m_IndexOffset = iId;

        return true;
    }

    bool GLMesh::Init(const VertexStandard* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
    {
        m_Layout = s_GL->GetLayout(GLLayout::Type::STANDARD);

        m_VertNum = vertexnum;
        m_IndexNum = indexnum;

        glCreateBuffers(1, &m_VBO);
        glNamedBufferStorage(m_VBO, vertexnum * m_Layout->GetStride(), vertices, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayVertexBuffer(m_Layout->GetID(), 0, m_VBO, 0, m_Layout->GetStride());

        glCreateBuffers(1, &m_EBO);
        glNamedBufferStorage(m_EBO, indexnum * sizeof(UInt32), indices, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayElementBuffer(m_Layout->GetID(), m_EBO);

        m_Pool = false;

        return true;
    }

    bool GLMesh::InitPool(const VertexAnimated* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
    {
        m_Layout = s_GL->GetLayout(GLLayout::Type::ANIMATED);
        const auto [vId, iId] = m_Layout->GetMeshPool().Add(vertices, vertexnum, indices, indexnum);

        m_VertNum = vertexnum;
        m_IndexNum = indexnum;
        m_VertOffset = vId;
        m_IndexOffset = iId;

        return true;
    }

    bool GLMesh::Init(const VertexAnimated* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
    {
        m_Layout = s_GL->GetLayout(GLLayout::Type::ANIMATED);

        m_VertNum = vertexnum;
        m_IndexNum = indexnum;

        glCreateBuffers(1, &m_VBO);
        glNamedBufferStorage(m_VBO, vertexnum * m_Layout->GetStride(), vertices, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayVertexBuffer(m_Layout->GetID(), 0, m_VBO, 0, m_Layout->GetStride());

        glCreateBuffers(1, &m_EBO);
        glNamedBufferStorage(m_EBO, indexnum * sizeof(UInt32), indices, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayElementBuffer(m_Layout->GetID(), m_EBO);

        m_Pool = false;

        return true;
    }

    void GLMesh::Destroy()
    {
        if (m_Pool) {
            return;
        }
    }

    void GLMesh::Draw()
    {
        if (m_Pool) {
            return;
        }

        UInt32 layoutId = m_Layout->GetID();
        glBindVertexArray(layoutId);
        glVertexArrayVertexBuffer(layoutId, 0, m_VBO, 0, m_Layout->GetStride());
        glVertexArrayElementBuffer(layoutId, m_EBO);

        glDrawElements(GL_TRIANGLES, m_IndexNum, GL_UNSIGNED_INT, nullptr);
    }

    UInt32 GLMesh::GetLayoutType() const IR_RETURN((UInt32)m_Layout->GetType())

}
