#pragma once

#include <glm.hpp>

namespace IR {
    struct AABB {
        glm::vec3 min;
        glm::vec3 max;
    };
}