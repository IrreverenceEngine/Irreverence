#pragma once

#include <Renderer/IR_GLBuffer.hpp>
#include <IR_Common.hpp>

#include <glm.hpp>

namespace IR::Renderer {

    struct GLInstanceList {
        void Init(UInt64 stride, UInt8 location);
        void Destroy();

        UInt32 Add(const void* data);
        void Upload();
        void Flush();

        GLBuffer ibuffer;
        std::vector<UInt8> ilist;
        UInt64 istride = 0;
    };

     struct InstanceStandard {
        glm::vec4 color;
        glm::mat4 model;
        UInt32 matLocation;
        UInt32 padding[3];
    };
}