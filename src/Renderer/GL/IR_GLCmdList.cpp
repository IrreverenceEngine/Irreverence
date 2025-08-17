#include <Renderer/GL/IR_GLCmdList.hpp>
#include <Renderer/GL/IR_GLRenderer.hpp>

#include <GL/glew.h>
#include <unistd.h>

namespace IR::Renderer {

    constexpr UInt64 CMDLIST_RESERVE = 1024;

    void GLCmdList::Init()
    {
        m_Buffer.Init(GL_DRAW_INDIRECT_BUFFER, nullptr, CMDLIST_RESERVE * sizeof(GLCmdElements), UINT8_MAX, true);
    }

    void GLCmdList::Destroy()
    {
        m_Buffer.Destroy();
    }

    void GLCmdList::Submit(const GLMesh* mesh, const GLShader* shader, UInt32 instanceId)
    {
        if (!shader) {
            IR_MSG(FATAL, "Trying to submit null shader to CmdList, probably due to Material missing shader, pls fix.");
        }

        GLCmdElements cmd;
        cmd.instanceCount = 1;
        cmd.count = mesh->GetIndexNum();
        cmd.firstIndex = mesh->GetIndexOffset();
        cmd.baseVertex = mesh->GetVertexOffset();
        cmd.baseInstance = instanceId;

        UInt32 shaderId = shader->GetID();
        UInt8 layoutType = mesh->GetLayoutType();

        LayoutInfo& layoutInfo = m_LayoutInfos[layoutType];

        auto& shaderMap = layoutInfo.shaderLists;
        if (shaderMap.find(shaderId) == shaderMap.end()) {
            layoutInfo.shaderOrders.push_back(shaderId);
        }
        shaderMap[shaderId].push_back(cmd);

        m_Outdated = true;
    }

    void GLCmdList::Flush()
    {
        for (UInt8 i = 0; i < IR_ARRLEN(m_LayoutInfos); i++) {
            LayoutInfo& layoutInfo = m_LayoutInfos[i];
            layoutInfo.shaderLists.clear();
            layoutInfo.shaderOrders.clear();
        }
    }

    void GLCmdList::Draw()
    {
        if (m_Outdated) {
            m_Cmds.clear();

            for (UInt8 lType = 0; lType < IR_ARRLEN(m_LayoutInfos); lType++) {
                LayoutInfo& layoutInfo = m_LayoutInfos[lType];

                for (UInt32 id : layoutInfo.shaderOrders) {
                    auto& list = layoutInfo.shaderLists[id];

                    m_Cmds.reserve(m_Cmds.size() + list.size());
                    m_Cmds.insert(m_Cmds.end(), list.begin(), list.end());
                }
            }

            m_Buffer.Update(m_Cmds.data(), m_Cmds.size() * sizeof(GLCmdElements), 0);
            m_Outdated = false;
        }

        m_Buffer.Bind();

        UInt64 offset = 0;
        for (UInt8 lType = 0; lType < IR_ARRLEN(m_LayoutInfos); lType++) {
            LayoutInfo& layoutInfo = m_LayoutInfos[lType];

            GLLayout* layout = s_GL->GetLayout((GLLayout::Type)lType);
            layout->GetMeshPool().Bind();

            for (UInt32 id : layoutInfo.shaderOrders) {
                auto& list = layoutInfo.shaderLists[id];
                if (list.empty()) {
                    continue;
                }

                glUseProgram(id);
                glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (const void*)offset, list.size(), 0);

                offset += list.size() * sizeof(GLCmdElements);
            }
        }
    }

}
