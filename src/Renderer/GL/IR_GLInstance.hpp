#pragma once

#include <Renderer/GL/IR_GLBuffer.hpp>
#include <IR_Common.hpp>

#include <glm.hpp>

namespace IR::Renderer {
    class GLInstanceList {
    public:
        void Init(UInt32 stride, UInt8 location);
        void Destroy();

        UInt32 Add(const void* data);
        void Upload();
        void Flush();

    private:
        GLBuffer m_Buffer;
        std::vector<UInt8> m_List;
        UInt32 m_Stride = 0;
    };

    struct GLInstanceStandard {
        glm::mat4 model;
        Color color;
        UInt32 matIndex;
        UInt32 _p[2];
    };

    struct GLInstanceMap {
        UInt32 matIndex;
    };
}