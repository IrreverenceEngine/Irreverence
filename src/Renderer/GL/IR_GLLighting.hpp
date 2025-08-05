#pragma once

#include <Renderer/GL/IR_GLBuffer.hpp>
#include <IR_Renderer.hpp>
#include <IR_Common.hpp>

namespace IR::Renderer {
    class GLLighting {
    public:
        bool Init(UInt8 plightsLoc, UInt8 slightsLoc);
        void Destroy();

        void Upload();

        // Pointlight
        UInt16 MakePLight();
        void RemovePLight(UInt16 index);
        void SetPLightPosition(UInt16 index, const glm::vec3& pos);
        void SetPLightColor(UInt16 index, const Color& col);
        void SetPLightInnerRadius(UInt16 index, const Float32 radius);
        void SetPLightOuterRadius(UInt16 index, const Float32 radius);

        // Spotlight
        UInt16 MakeSLight();
        void RemoveSLight(UInt16 index);
        void SetSLightPosition(UInt16 index, const glm::vec3& pos);
        void SetSLightDirection(UInt16 index, const glm::vec3& dir);
        void SetSLightColor(UInt16 index, const Color& col);
        void SetSLightInnerRadius(UInt16 index, const Float32 radius);
        void SetSLightOuterRadius(UInt16 index, const Float32 radius);
        void SetSLightInnerCutoff(UInt16 index, const Float32 deg);
        void SetSLightOuterCutoff(UInt16 index, const Float32 deg);

    private:
        struct PointlightData {
            glm::vec3 position;
            Float32 innerRadius;
            Color color;
            Float32 outerRadius;
            UInt32 _p[2];
        };

        struct Pointlight {
            PointlightData data;
            bool ready;
	        bool outdated;
            bool remove;
        };

        struct SpotlightData {
            glm::vec3 position;
            Float32 innerCutoff;
            glm::vec3 direction;
            Float32 outerCutoff;
            Color color;
            Float32 innerRadius;
            Float32 outerRadius;
            UInt32 _p;
        };

        struct Spotlight {
            SpotlightData data;
            bool ready;
            bool outdated;
            bool remove;
        };

        GLBuffer m_PointlightBuffer;
        GLBuffer m_SpotlightBuffer;

        Pointlight m_Pointlights[Renderer::MAX_POINTLIGHTS] = {};
        Spotlight m_Spotlights[Renderer::MAX_SPOTLIGHTS] = {};
    };
}