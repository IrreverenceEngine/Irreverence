#pragma once

// Just bunch of misc stuff I dunno where to put.

#include <IR_Common.hpp>

#include <glm.hpp>

namespace IR::Debug {
    void FlyCam(glm::mat4& outView, glm::mat4& outProjection, glm::vec3& outPos);
};
