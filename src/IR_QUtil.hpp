#pragma once

#include <IR_Common.hpp>

#include <glm.hpp>

namespace IR::QUtil {
    glm::vec3 QVec3ToVec3(const glm::vec3& qvec3); // Convert Quake (X,Z,-Y) to (X,Y,Z)
    glm::vec4 QVec4ToVec4(const glm::vec4& qvec4); // Convert Quake (X,Z,-Y,W) to (X,Y,Z,W)

    Float32 StrToFloat32(const char* qstr);
    Int32 StrToInt32(const char* qstr);
    glm::vec3 StrToVec3(const char* qstr);
    glm::vec4 StrToVec4(const char* qstr);
}