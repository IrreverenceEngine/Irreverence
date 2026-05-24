#pragma once

#include <IR_Model.hpp>
#include <Renderer/GL/IR_GLMaterial.hpp>
#include <Renderer/GL/IR_GLMesh.hpp>
#include <Renderer/GL/IR_GLInstance.hpp>

namespace IR::Renderer {
    class GLModel : public Model {
    public:
        bool Init(const void* meshes, UInt32 count) override;

        void MakeMesh(const VertexStandard* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum, const Material** skins, UInt8 skinnum) override;
        void MakeMesh(const VertexAnimated* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indnum, const Material** skins, UInt8 skinnum) override;
        void ReserveMesh(UInt32 count) override;

        GLMesh& GetMesh(UInt32 i) IRX_RETURN(m_Meshes[i].mesh)
        GLMaterial* GetMeshMaterial(UInt32 i, UInt32 skin) IRX_RETURN(skin < MAX_SKINS ? m_Meshes[i].skins[skin] : nullptr)
        UInt32 GetMeshNum() const IRX_RETURN(m_Meshes.size())

    private:
        struct MeshInfo {
            GLMesh mesh;
            GLMaterial* skins[MAX_SKINS] = {};
        };

        std::vector<MeshInfo> m_Meshes;
    };
}