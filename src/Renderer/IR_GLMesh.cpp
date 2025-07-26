#include <Renderer/IR_GLMesh.hpp>
#include <Renderer/IR_GLLayout.hpp>
#include <Renderer/IR_GLRenderer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    bool GLMesh::Init(const VertexStandard* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
    {
        const auto [vId, iId] = s_GL->GetLayout(GLLayout::Type::STANDARD)->GetMeshPool().Add(vertices, vertexnum, indices, indexnum);

        m_VertNum = vertexnum;
        m_IndexNum = indexnum;
        m_VertOffset = vId;
        m_IndexOffset = iId;

        m_LayoutType = (UInt8)GLLayout::Type::STANDARD;

        return true;
    }

    bool GLMesh::Init(const VertexAnimated* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
    {
        const auto [vId, iId] = s_GL->GetLayout(GLLayout::Type::ANIMATED)->GetMeshPool().Add(vertices, vertexnum, indices, indexnum);

        m_VertNum = vertexnum;
        m_IndexNum = indexnum;
        m_VertOffset = vId;
        m_IndexOffset = iId;

        m_LayoutType = (UInt8)GLLayout::Type::ANIMATED;

        return true;
    }

}