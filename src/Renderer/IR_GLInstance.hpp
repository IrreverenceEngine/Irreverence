#pragma once

#include <Renderer/IR_GLBuffer.hpp>
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
        glm::vec4 color;
        glm::mat4 model;
        UInt32 matIndex;
        UInt32 padding[3];
    };

    struct GLInstanceMap {
        UInt32 matIndex;
    };
}