#pragma once

#include <IR_Common.hpp>

#include <glm.hpp>

namespace IR::Renderer {
    struct VertexStandard {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
    };

    struct VertexAnimated {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;

        UInt8 bones[4];
        Float32 weights[4];
    };
}
