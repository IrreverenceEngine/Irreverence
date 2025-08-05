#pragma once

#include <IR_Common.hpp>
#include <IR_AABB.hpp>
#include <IR_LayoutType.hpp>

#include <unordered_map>
#include <string>
#include <vector>

namespace IR::BinaryMap {
    struct FaceData {
        glm::vec4 plane = {};
        UInt32 flags = 0;
        std::string materialName;
        std::vector<Renderer::VertexStandard> vertices;
        std::vector<UInt32> indices;
    };

    struct BrushData {
        std::vector<FaceData> faces;
        std::vector<glm::vec3> convexPoints;
        glm::vec3 origin;
    };

    // Is given to actual Entities when creating them
    struct EntityData {
        std::unordered_map<std::string, std::string> keyvalues;
        std::vector<BrushData> brushes;
    };

    bool Load(const char* path, std::vector<EntityData>& entDatas);
}