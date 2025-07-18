#pragma once

#include <IR_Model.hpp>
#include <renderer/IR_GLMesh.hpp>

namespace IR::Renderer {
    struct GLModel : public Model {
        ~GLModel();

        bool Init(const void* meshes, UInt32 count) override;

        void Draw(const void* data) override;

        void MakeMesh(const VertexStandard* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum) override;
        void MakeMesh(const VertexAnimated* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum) override;
        void ReserveMesh(UInt32 count) override;

        std::vector<GLMesh> m_Meshes;
    };
}