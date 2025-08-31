#pragma once

#include <IR_Common.hpp>
#include <IR_AABB.hpp>
#include <IR_LayoutType.hpp>

#include <DetourNavMesh.h>

#include <unordered_map>
#include <string>
#include <vector>

namespace IR::BinaryMap {
    struct FaceData {
        enum {
            FLAGS_NONE = 0,
            FLAGS_NOMESH = 1 << 0,
            FLAGS_NORENDER = 1 << 1,
            FLAGS_NOSHADOW = 1 << 2,
            FLAGS_UNWALKABLE = 1 << 3,
        };

        glm::vec4 plane = {};
        UInt32 flags = 0;
        std::string materialName;
        std::vector<Renderer::VertexStandard> vertices;
        std::vector<UInt32> indices;
    };

    struct BrushData {
        enum {
            FLAGS_NONE = 0,
            FLAGS_NOCONVEX = 1 << 0,
        };

        UInt32 flags;
        std::vector<FaceData> faces;
        std::vector<glm::vec3> convexPoints;
        glm::vec3 origin;
        AABB aabb;
    };

    // Is given to actual Entities when creating them
    struct EntityData {
        std::unordered_map<std::string, std::string> keyvalues;
        std::vector<BrushData> brushes;
    };

    bool Load(const char* path, std::vector<EntityData>& entDatas, dtNavMesh** navmesh);
}
