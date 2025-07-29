#include <Renderer/GL/IR_GLCmdList.hpp>
#include <Renderer/GL/IR_GLRenderer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    void GLCmdList::Init()
    {
        m_Buffer.Init(GL_DRAW_INDIRECT_BUFFER, nullptr, 2 * sizeof(GLCmdElements), UINT8_MAX, true);
    }

    void GLCmdList::Destroy()
    {
        m_Buffer.Destroy();
    }

    void GLCmdList::Submit(const GLMesh* mesh, const GLShader* shader, UInt32 instanceId)
    {
        GLCmdElements cmd;
        cmd.instanceCount = 1;
        cmd.count = mesh->GetIndexNum();
        cmd.firstIndex = mesh->GetIndexOffset();
        cmd.baseVertex = mesh->GetVertexOffset();
        cmd.baseInstance = instanceId;

        m_List[mesh->GetLayoutType()][shader->GetID()].push_back(cmd);
    }

    void GLCmdList::Flush()
    {
        for (UInt8 i = 0; i < IR_ARRLEN(m_List); i++) {
            m_List[i].clear();
        }
    }

    void GLCmdList::Draw()
    {
        m_Buffer.Bind();

        for (UInt8 i = 0; i < IR_ARRLEN(m_List); i++) {
            GLLayout* layout = s_GL->GetLayout((GLLayout::Type)i);
            layout->GetMeshPool().Bind();

            for (const auto& [shaderId, list] : m_List[i]) {
                if (list.size() == 0) {
                    continue;
                }

                if (m_Buffer.Update(list.data(), list.size() * sizeof(GLCmdElements), 0)) { // Rebind the buffer if resized.
                    m_Buffer.Bind();
                }

                // TODO: Bind Shader in a better way

                glUseProgram(shaderId);
                glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, list.size(), 0);
            }
        }
    }

    void GLCmdList::DrawShaderless()
    {
        m_Buffer.Bind();

        for (UInt8 i = 0; i < IR_ARRLEN(m_List); i++) {
            GLLayout* layout = s_GL->GetLayout((GLLayout::Type)i);
            layout->GetMeshPool().Bind();

            for (const auto& [shaderId, list] : m_List[i]) {
                if (list.size() == 0) {
                    continue;
                }

                if (m_Buffer.Update(list.data(), list.size() * sizeof(GLCmdElements), 0)) { // Rebind the buffer if resized.
                    m_Buffer.Bind();
                }

                glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, list.size(), 0);
            }
        }
    }

}