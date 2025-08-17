#pragma once

#include <Renderer/GL/IR_GLLayout.hpp>
#include <Renderer/GL/IR_GLBuffer.hpp>
#include <IR_Common.hpp>

#include <unordered_map>

namespace IR::Renderer {
    struct GLCmdElements {
        UInt32 count;
        UInt32 instanceCount;
        UInt32 firstIndex;
        UInt32 baseVertex;
        UInt32 baseInstance;
    };

    class GLCmdList {
    public:
        void Init();
        void Destroy();

        void Submit(const GLMesh* mesh, const GLShader* shader, UInt32 instanceId);
        void Flush();
        void Draw();

    private:
        struct LayoutInfo {
            std::vector<UInt32> shaderOrders;
            std::unordered_map<UInt32, std::vector<GLCmdElements>> shaderLists;
        };

        GLBuffer m_Buffer;
        LayoutInfo m_LayoutInfos[(UInt8)GLLayout::Type::_COUNT];
        std::vector<GLCmdElements> m_Cmds;
        bool m_Outdated = false;
    };
}
