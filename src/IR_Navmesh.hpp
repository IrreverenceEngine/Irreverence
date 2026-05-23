#pragma once

#include <IR_Common.hpp>

namespace IR {
    class Navmesh {
    public:
        static constexpr UInt32 MAX_QUERYNODES = 2 << 12;
        static constexpr glm::vec3 FIND_EXTENDS = { 512.0f, 512.0f, 512.0f };
        static constexpr UInt32 MAX_POLYS = 2 << 9;
        static constexpr UInt32 MAX_PATHPOINTS = 2 << 10;

        bool Init(const glm::vec3& origin, Int32 maxPolys, Int32 maxTiles, const glm::vec2& tileSize);
        void Destroy();
        ~Navmesh();

        bool AddTile(UInt8* data, UInt32 size);

        bool FindPath(const glm::vec3& start, const glm::vec3& end, std::vector<glm::vec3>& path);

        bool IsValid() const IR_RETURN(m_Mesh)

    private:
        void* m_Mesh = nullptr;
        void* m_Query = nullptr;
        void* m_Filter = nullptr;
    };
}
