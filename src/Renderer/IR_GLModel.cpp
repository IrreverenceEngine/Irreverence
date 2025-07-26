#include <Renderer/IR_GLModel.hpp>
#include <Renderer/IR_GLMesh.hpp>
#include <Renderer/IR_GLRenderer.hpp>

namespace IR::Renderer {

    bool GLModel::Init(const void* meshes, UInt32 count)
    {
        if (!meshes) {
            return false;
        }

        m_Meshes.reserve(count);

        GLMesh* glmeshes = (GLMesh*)meshes;
        for (UInt32 i = 0; i < count; i++) {
            m_Meshes.emplace_back(glmeshes[i]);
        }

        return true;
    }

    void GLModel::Draw(const void* data)
    {

    }

    void GLModel::MakeMesh(const VertexStandard* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum)
    {
        GLMesh mesh;
        mesh.Init(vertices, vertnum, indices, indnum);

        m_Meshes.emplace_back(mesh);
    }

    void GLModel::MakeMesh(const VertexAnimated* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum)
    {
        GLMesh mesh;
        mesh.Init(vertices, vertnum, indices, indnum);

        m_Meshes.emplace_back(mesh);
    }

    void GLModel::ReserveMesh(UInt32 count)
    {
        m_Meshes.reserve(count);
    }

}