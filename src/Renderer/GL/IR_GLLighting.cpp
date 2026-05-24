#include <Renderer/GL/IR_GLLighting.hpp>

#include <GL/glew.h>

#include <cstring>

namespace IR::Renderer {

    bool GLLighting::Init(UInt8 plightsLoc, UInt8 slightsLoc)
    {
        m_PointlightBuffer.Init(GL_SHADER_STORAGE_BUFFER, nullptr, sizeof(Pointlight) * MAX_POINTLIGHTS + sizeof(UInt32), plightsLoc);
        m_SpotlightBuffer.Init(GL_SHADER_STORAGE_BUFFER, nullptr, sizeof(Spotlight) * MAX_SPOTLIGHTS + sizeof(UInt32), slightsLoc);

        return true;
    }

    void GLLighting::Destroy()
    {
        m_PointlightBuffer.Destroy();
        m_SpotlightBuffer.Destroy();

        memset(m_Pointlights, 0, sizeof(m_Pointlights));
        memset(m_Spotlights, 0, sizeof(m_Spotlights));
    }

    void GLLighting::Upload()
    {
        UInt32 pointlightCount = 0;
        for (UInt16 i = 0; i < MAX_POINTLIGHTS; i++) {
            Pointlight* light = &m_Pointlights[i];

            if (!light->ready) {
                continue;
            }
            
            pointlightCount = i + 1;

            if (!light->outdated) {
                continue;
            }

            light->outdated = false;

            m_PointlightBuffer.Update(&light->data, sizeof(PointlightData), sizeof(PointlightData) * i);
        }
        m_PointlightBuffer.Update(&pointlightCount, sizeof(UInt32), sizeof(PointlightData) * MAX_POINTLIGHTS);

        UInt32 spotlightCount = 0;
        for (UInt16 i = 0; i < MAX_SPOTLIGHTS; i++) {
            Spotlight* light = &m_Spotlights[i];

            if (!light->ready) {
                continue;
            }

            spotlightCount = i + 1;

            if (!light->outdated) {
                continue;
            }

            light->outdated = false;

            m_SpotlightBuffer.Update(&light->data, sizeof(SpotlightData), sizeof(SpotlightData) * i);
        }
        m_SpotlightBuffer.Update(&spotlightCount, sizeof(UInt32), sizeof(SpotlightData) * MAX_SPOTLIGHTS);
    }

    UInt16 GLLighting::MakePLight()
    {
        UInt16 index = MAX_POINTLIGHTS;
        for (UInt16 i = 0; i < MAX_POINTLIGHTS; i++) {
            Pointlight* plight = &m_Pointlights[i];

            if (plight->ready) continue;

            index = i;
            break;
        }

        if (index >= MAX_POINTLIGHTS) {
            IRX_MSG(ERROR, "Renderer: can't add Pointlight due to reaching limit");
            return MAX_POINTLIGHTS;
        }

        Pointlight& plight = m_Pointlights[index];
        plight.data.color = {};
        plight.outdated = true;
        plight.ready = true;

        return index;
    }

    void GLLighting::RemovePLight(UInt16 index)
    {
        if (index >= MAX_POINTLIGHTS) {
            return;
        }

        Pointlight& light = m_Pointlights[index];
        light.data.color = Color(0);
        light.outdated = false;
        light.ready = false;

        m_PointlightBuffer.Update(&light.data, sizeof(PointlightData), index * sizeof(PointlightData));
    }

    void GLLighting::SetPLightPosition(UInt16 index, const glm::vec3& pos)
    {
        Pointlight& light = m_Pointlights[index];
        if (!light.ready) {
            return;
        }

        light.data.position = pos;
        light.outdated = true;
    }

    void GLLighting::SetPLightColor(UInt16 index, const Color& col)
    {
        Pointlight& light = m_Pointlights[index];
        if (!light.ready) {
            return;
        }

        light.data.color = col;
        light.outdated = true;
    }

    void GLLighting::SetPLightInnerRadius(UInt16 index, const float radius)
    {
        Pointlight& light = m_Pointlights[index];
        if (!light.ready) {
            return;
        }

        light.data.innerRadius = radius;
        light.outdated = true;
    }

    void GLLighting::SetPLightOuterRadius(UInt16 index, const float radius)
    {
        Pointlight& light = m_Pointlights[index];
        if (!light.ready) {
            return;
        }

        light.data.outerRadius = radius;
        light.outdated = true;
    }

    UInt16 GLLighting::MakeSLight()
    {
        UInt16 index = MAX_SPOTLIGHTS;
        for (UInt16 i = 0; i < MAX_SPOTLIGHTS; i++) {
            Spotlight* plight = &m_Spotlights[i];

            if (plight->ready) continue;

            index = i;
            break;
        }

        if (index >= MAX_SPOTLIGHTS) {
            IRX_MSG(ERROR, "Renderer: can't add Spotlight due to reaching limit");
            return MAX_SPOTLIGHTS;
        }

        Spotlight& slight = m_Spotlights[index];
        slight.data = {};
        slight.outdated = true;
        slight.ready = true;

        return index;
    }

    void GLLighting::RemoveSLight(UInt16 index)
    {
        if (index >= MAX_SPOTLIGHTS) {
            return;
        }

        Spotlight& light = m_Spotlights[index];
        light.data.color = Color(0);
        light.outdated = false;
        light.ready = false;

        m_SpotlightBuffer.Update(&light.data, sizeof(SpotlightData), index * sizeof(SpotlightData));
    }

    void GLLighting::SetSLightPosition(UInt16 index, const glm::vec3& pos)
    {
        Spotlight& light = m_Spotlights[index];
        if (!light.ready) {
            return;
        }

        light.data.position = pos;
        light.outdated = true;
    }

    void GLLighting::SetSLightDirection(UInt16 index, const glm::vec3& dir)
    {
        Spotlight& light = m_Spotlights[index];
        if (!light.ready) {
            return;
        }

        light.data.direction = dir;
        light.outdated = true;
    }

    void GLLighting::SetSLightColor(UInt16 index, const Color& col)
    {
        Spotlight& light = m_Spotlights[index];
        if (!light.ready) {
            return;
        }

        light.data.color = col;
        light.outdated = true;
    }

    void GLLighting::SetSLightInnerRadius(UInt16 index, const float radius)
    {
        Spotlight& light = m_Spotlights[index];
        if (!light.ready) {
            return;
        }

        light.data.innerRadius = radius;
        light.outdated = true;
    }

    void GLLighting::SetSLightOuterRadius(UInt16 index, const float radius)
    {
        Spotlight& light = m_Spotlights[index];
        if (!light.ready) {
            return;
        }

        light.data.outerRadius = radius;
        light.outdated = true;
    }

    void GLLighting::SetSLightInnerCutoff(UInt16 index, const float deg)
    {
        Spotlight& light = m_Spotlights[index];
        if (!light.ready) {
            return;
        }

        light.data.innerCutoff = cosf(glm::radians(deg));
        light.outdated = true;
    }

    void GLLighting::SetSLightOuterCutoff(UInt16 index, const float deg)
    {
        Spotlight& light = m_Spotlights[index];
        if (!light.ready) {
            return;
        }

        light.data.outerCutoff = cosf(glm::radians(deg));
        light.outdated = true;
    }

}
