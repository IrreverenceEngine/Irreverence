#include <Renderer/IR_GLCmdList.hpp>
#include <Renderer/IR_GLRenderer.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    void GLCmdList::Init()
    {
        cmdbuffer.Init(GL_DRAW_INDIRECT_BUFFER, nullptr, 2 * sizeof(GLCmdElements), UINT8_MAX, true);
    }

    void GLCmdList::Destroy()
    {
        cmdbuffer.Destroy();
    }

    void GLCmdList::Submit(const GLMesh& mesh, const GLShader& shader, UInt32 instanceId)
    {
        if (mesh.layoutType >= GLLayout::TYPE__COUNT) {
            return;
        }

        GLCmdElements cmd;
        cmd.instanceCount = 1;
        cmd.count = mesh.indexCount;
        cmd.firstIndex = mesh.indexOffset;
        cmd.baseVertex = mesh.vertOffset;
        cmd.baseInstance = instanceId;

        cmdlist[mesh.layoutType][shader.id].push_back(cmd);
    }

    void GLCmdList::Flush()
    {
        for (UInt8 i = 0; i < IR_ARRLEN(cmdlist); i++) {
            cmdlist[i].clear();
        }
    }

    void GLCmdList::Draw()
    {
        cmdbuffer.Bind();

        for (UInt8 i = 0; i < IR_ARRLEN(cmdlist); i++) {
            GLLayout* layout = s_GL->GetLayout((GLLayout::Type)i);
            layout->meshpool.Bind();

            for (const auto& [shaderId, list] : cmdlist[i]) {
                if (list.size() == 0) {
                    continue;
                }

                if (cmdbuffer.Update(list.data(), list.size() * sizeof(GLCmdElements), 0)) { // Rebind the buffer if resized.
                    cmdbuffer.Bind();
                }

                // TODO: Bind Shader in a better way
                glUseProgram(shaderId);
                glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, list.size(), 0);
            }
        }
    }

}