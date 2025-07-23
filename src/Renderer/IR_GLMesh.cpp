#include <Renderer/IR_GLMesh.hpp>
#include <Renderer/IR_GLLayout.hpp>
#include <Renderer/IR_GLRenderer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    bool GLMesh::Init(const VertexStandard* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
    {
        const auto [vId, iId] = s_GL->m_LayoutStandard.meshpool.Add((const UInt8*)vertices, vertexnum, indices, indexnum, true);

        vertCount = vertexnum;
        indexCount = indexnum;
        vertOffset = vId;
        indexOffset = iId;

        layoutType = GLLayout::TYPE_STANDARD;

        return true;
    }

    bool GLMesh::Init(const VertexAnimated* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
    {
        const auto [vId, iId] = s_GL->m_LayoutAnimated.meshpool.Add((const UInt8*)vertices, vertexnum, indices, indexnum, true);

        vertCount = vertexnum;
        indexCount = indexnum;
        vertOffset = vId;
        indexOffset = iId;

        layoutType = GLLayout::TYPE_ANIMATED;

        return true;
    }

}