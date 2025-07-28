#pragma once

#include <IR_Mesh.hpp>
#include <IR_Common.hpp>

namespace IR::Renderer {
    class GLMesh : public Mesh {
    public:
        bool Init(const VertexStandard* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum) override;
        bool Init(const VertexAnimated* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum) override;

        UInt32 GetVertexOffset() const IR_RETURN(m_VertOffset)
        UInt32 GetIndexOffset() const IR_RETURN(m_IndexOffset)
        UInt32 GetVertexNum() const IR_RETURN(m_VertNum)
        UInt32 GetIndexNum() const IR_RETURN(m_IndexNum)
        UInt8 GetLayoutType() const IR_RETURN(m_LayoutType)

    private:
        UInt32 m_VertOffset = 0;
        UInt32 m_IndexOffset = 0;
        UInt32 m_VertNum = 0;
        UInt32 m_IndexNum = 0;
        UInt8 m_LayoutType = UINT8_MAX;
    };
}