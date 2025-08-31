#pragma once

#include <glm.hpp>

#include <IR_Macro.hpp>

namespace IR {
    struct AABB {
        glm::vec3 min = {};
        glm::vec3 max = {};

        glm::vec3 GetCenter() const IR_RETURN((max + min) / 2.0f)
        glm::vec3 GetHalfExtents() const IR_RETURN(glm::abs(max - min) / 2.0f)
        bool Intersects(const AABB& other) const IR_RETURN(
            (min.x <= other.max.x && max.x >= other.min.x) &&
            (min.y <= other.max.y && max.y >= other.min.y) &&
            (min.z <= other.max.z && max.z >= other.min.z)
        )
    };
}
