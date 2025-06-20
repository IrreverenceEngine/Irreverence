#pragma once

#include <glm.hpp>

namespace IR::Renderer {
    enum class View {
        Player3D // View in which the player usually see from
    };

    bool Init();
    void Shutdown();

    void Present();
    void UpdateCameraMatrices(const glm::mat4& view, const glm::mat4& projection);

    void DebugCamera(glm::mat4& outView, glm::mat4& outProjection);
}