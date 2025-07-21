#pragma once

#include <IR_Common.hpp>
#include <IR_AABB.hpp>
#include <IR_LayoutType.hpp>

#include <unordered_map>
#include <string>
#include <vector>

namespace IR {

    class BinaryMap {
    public:
        struct EntityData;
        struct BrushData;
        struct FaceData;

        // Is given to actual Entities when creating them
        struct EntityData {
            std::unordered_map<std::string, std::string> keyvalues;
            std::vector<BrushData> brushes;
        };

        struct BrushData {
            std::vector<FaceData> faces;
            std::vector<glm::vec3> convexPoints;
            glm::vec3 origin;
        };

        struct FaceData {
            glm::vec4 plane = {};
            UInt32 flags = 0;
            std::vector<Renderer::VertexStandard> vertices;
        };

        bool Load(const char* path);

        std::vector<EntityData>& GetEntityDatas() IR_RETURN(m_EntityDatas)

    private:
        std::vector<EntityData> m_EntityDatas;
    };

}