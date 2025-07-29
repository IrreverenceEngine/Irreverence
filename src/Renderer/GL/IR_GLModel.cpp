#include <Renderer/GL/IR_GLModel.hpp>
#include <Renderer/GL/IR_GLMesh.hpp>
#include <Renderer/GL/IR_GLRenderer.hpp>

namespace IR::Renderer {

    bool GLModel::Init(const void* meshes, UInt32 count)
    {
        if (!meshes) {
            return false;
        }

        m_Meshes.reserve(count);

        GLMesh* glmeshes = (GLMesh*)meshes;
        for (UInt32 i = 0; i < count; i++) {
            m_Meshes.push_back({ glmeshes[i] });
        }

        return true;
    }

    void GLModel::MakeMesh(const VertexStandard* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum, const Material** skins, UInt8 skinnum)
    {
        UInt8 safeSkinNum = Math::Min(Model::MAX_SKINS, skinnum);

        MeshInfo info;

        GLMesh mesh;
        mesh.Init(vertices, vertnum, indices, indnum);
        info.mesh = mesh;

        for (UInt8 i = 0; i < Model::MAX_SKINS; i++) {
            if (i < safeSkinNum) {
                const Material* mat = skins[i];
                if (mat) {
                    info.skins[i] = (GLMaterial*)mat;
                    continue;
                }
            }

            info.skins[i] = (GLMaterial*)s_GL->GetMaterialError();
        }

        m_Meshes.emplace_back(info);
    }

    void GLModel::MakeMesh(const VertexAnimated* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum, const Material** skins, UInt8 skinnum)
    {
        UInt8 safeSkinNum = Math::Min(Model::MAX_SKINS, skinnum);

        MeshInfo info;

        GLMesh mesh;
        mesh.Init(vertices, vertnum, indices, indnum);
        info.mesh = mesh;

        for (UInt8 i = 0; i < Model::MAX_SKINS; i++) {
            if (i < safeSkinNum) {
                const Material* mat = skins[i];
                if (mat) {
                    info.skins[i] = (GLMaterial*)mat;
                    continue;
                }
            }

            info.skins[i] = (GLMaterial*)s_GL->GetMaterialError();
        }

        m_Meshes.emplace_back(info);
    }

    void GLModel::ReserveMesh(UInt32 count)
    {
        m_Meshes.reserve(count);
    }

}