#include <IR_Navmesh.hpp>

#include <DetourNavMesh.h>
#include <DetourNavMeshQuery.h>
#include <DetourCommon.h>
#include <DetourAlloc.h>
#include <DetourNavMeshBuilder.h>

namespace IR {

    bool Navmesh::Init(const glm::vec3& origin, Int32 maxPolys, Int32 maxTiles, const glm::vec2& tileSize)
    {
        dtNavMesh* navmesh = dtAllocNavMesh();
        m_Mesh = navmesh;

        dtNavMeshParams navParams;
        navParams.maxTiles = maxTiles;
        navParams.maxPolys = maxPolys;
        navParams.tileWidth = tileSize.x;
        navParams.tileHeight = tileSize.y;
        navParams.orig[0] = origin.x;
        navParams.orig[1] = origin.y;
        navParams.orig[2] = origin.z;

        if (dtStatusFailed(navmesh->init(&navParams))) {
            return false;
        }

        dtNavMeshQuery* query = dtAllocNavMeshQuery();
        m_Query = query;

        if (dtStatusFailed(query->init(navmesh, MAX_QUERYNODES))) {
            Destroy();
            return false;
        }

        dtQueryFilter* filter = new dtQueryFilter;
        m_Filter = filter;

        filter->setIncludeFlags(0xFFFF);
        filter->setExcludeFlags(0);
        filter->setAreaCost(0, 1.0f);

        return true;
    }

    void Navmesh::Destroy()
    {
        if (m_Mesh) {
            dtFreeNavMesh((dtNavMesh*)m_Mesh);
            m_Mesh = nullptr;
        }

        if (m_Query) {
            dtFreeNavMeshQuery((dtNavMeshQuery*)m_Query);
            m_Query = nullptr;
        }

        if (m_Filter) {
            delete (dtQueryFilter*)m_Filter;
            m_Filter = nullptr;
        }
    }

    Navmesh::~Navmesh()
    {
        Destroy();
    }

    bool Navmesh::AddTile(UInt8* data, UInt32 size)
    {
        dtNavMesh* navmesh = (dtNavMesh*)m_Mesh;

        dtTileRef ref;
        if (dtStatusFailed(navmesh->addTile(data, size, DT_TILE_FREE_DATA, 0, &ref))) {
            return false;
        }

        return true;
    }

    bool Navmesh::FindPath(const glm::vec3& start, const glm::vec3& end, std::vector<glm::vec3>& path)
    {
        if (!m_Mesh || !m_Query) {
            return false;
        }

        glm::vec3 startNavSpace = start;
        glm::vec3 endNavSpace = end;

        dtNavMeshQuery* query = (dtNavMeshQuery*)m_Query;
        dtQueryFilter* filter = (dtQueryFilter*)m_Filter;

        dtPolyRef startRef, endRef;
        glm::vec3 nearStart, nearEnd;

        query->findNearestPoly((Float32*)&startNavSpace, (Float32*)&FIND_EXTENDS, filter, &startRef, (Float32*)&nearStart);
        query->findNearestPoly((Float32*)&endNavSpace, (Float32*)&FIND_EXTENDS, filter, &endRef, (Float32*)&nearEnd);

        if (!startRef || !endRef) {
            IRX_MSG(WARN, "Navmesh: Couldn't find nearest poly");
            return false;
        }

        dtPolyRef polys[MAX_POLYS];
        Int32 polyNum = 0;

        dtStatus status = query->findPath(
            startRef,
            endRef,
            (Float32*)&nearStart,
            (Float32*)&nearEnd,
            filter,
            polys,
            &polyNum,
            MAX_POLYS
        );

        if (dtStatusFailed(status) || polyNum == 0) {
            IRX_MSG(WARN, "Navmesh: Couldn't find path");
            return false;
        }

        Float32 strtPath[MAX_PATHPOINTS * 3];
        UInt8 strtPathFlags[MAX_PATHPOINTS];
        dtPolyRef strtPathPolys[MAX_PATHPOINTS];
        Int32 strtPathNum = 0;

        status = query->findStraightPath(
            (Float32*)&nearStart,
            (Float32*)&nearEnd,
            polys,
            polyNum,
            strtPath,
            strtPathFlags,
            strtPathPolys,
            &strtPathNum,
            MAX_PATHPOINTS
        );

        if (dtStatusFailed(status) || strtPathNum == 0) {
            IRX_MSG(WARN, "Navmesh: Couldn't find straight path");
            return false;
        }

        path.reserve(strtPathNum);
        for (Int32 i = 0; i < strtPathNum * 3; i += 3) {
            Float32* point = &strtPath[i];
            path.push_back({ point[0], point[1], point[2] });
        }

        return true;
    }

}
