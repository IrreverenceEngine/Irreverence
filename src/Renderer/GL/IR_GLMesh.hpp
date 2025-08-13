#pragma once

#include <IR_Mesh.hpp>
#include <IR_Common.hpp>

namespace IR::Renderer {
    class GLLayout;

    class GLMesh : public Mesh {
    public:
        bool InitPool(const VertexBasic2D* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum) override;
        bool Init(const VertexBasic2D* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum);
        bool InitPool(const VertexStandard* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum) override;
        bool Init(const VertexStandard* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum);
        bool InitPool(const VertexAnimated* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum) override;
        bool Init(const VertexAnimated* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum);
        void Destroy() override;

        void Draw();

        UInt32 GetVertexOffset() const IR_RETURN(m_VertOffset)
        UInt32 GetIndexOffset() const IR_RETURN(m_IndexOffset)
        UInt32 GetVertexNum() const IR_RETURN(m_VertNum)
        UInt32 GetIndexNum() const IR_RETURN(m_IndexNum)
        UInt32 GetLayoutType() const;

    private:
        union {
            struct {
                UInt32 m_VBO = 0;
                UInt32 m_EBO = 0;
            };

            struct {
                UInt32 m_VertOffset;
                UInt32 m_IndexOffset;
            };
        };
        UInt32 m_VertNum = 0;
        UInt32 m_IndexNum = 0;
        GLLayout* m_Layout = nullptr;
        bool m_Pool = true;
    };
}
