#include <renderer/IR_GLModel.hpp>
#include <renderer/IR_GLMesh.hpp>
#include <renderer/IR_GLRenderer.hpp>

namespace IR::Renderer {

    bool GLModel::Init(const void* meshes, UInt32 count)
    {
        if (!meshes) {
            return false;
        }

        m_Meshes.reserve(count);

        GLMesh* glmeshes = (GLMesh*)meshes;
        for (UInt32 i = 0; i < count; i++) {
            m_Meshes.push_back(glmeshes[i]);
        }

        return true;
    }

    GLModel::~GLModel()
    {
        for (GLMesh& mesh : m_Meshes) {
            mesh.Destroy();
        }
    }

    void GLModel::Draw(const void* data)
    {
        for (GLMesh& mesh : m_Meshes) {
            mesh.Draw(data);
        }
    }

    void GLModel::MakeMesh(const VertexStandard* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum)
    {
        GLMesh mesh;
        mesh.Init(s_GL->m_LayoutStandard, vertices, vertnum, indices, indnum);

        m_Meshes.push_back(mesh);
    }

    void GLModel::MakeMesh(const VertexAnimated* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum)
    {
        GLMesh mesh;
        mesh.Init(s_GL->m_LayoutAnimated, vertices, vertnum, indices, indnum);

        m_Meshes.push_back(mesh);
    }

    void GLModel::ReserveMesh(UInt32 count)
    {
        m_Meshes.reserve(count);
    }

}