#pragma once

#include <IR_Model.hpp>
#include <Renderer/IR_GLMesh.hpp>

namespace IR::Renderer {
    class GLModel : public Model {
    public:
        bool Init(const void* meshes, UInt32 count) override;

        void Draw(const void* data) override;

        void MakeMesh(const VertexStandard* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum) override;
        void MakeMesh(const VertexAnimated* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum) override;
        void ReserveMesh(UInt32 count) override;

        const std::vector<GLMesh>& GetMeshes() const IR_RETURN(m_Meshes)

    private:
        std::vector<GLMesh> m_Meshes;
    };
}