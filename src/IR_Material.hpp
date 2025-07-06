#pragma once

#include <IR_Common.hpp>

#include <bgfx/bgfx.h>

#include <string>

namespace IR {
    class Material {
    public:
        enum class Map {
            COLOR,   // Base Color
            METAL,   // Metalness
            ROUGH,   // Roughness
            NORMALS, // Normals
            AO,      // Ambient Occlusion
            _COUNT
        };

        Material(const bgfx::TextureHandle* (&maps)[(UInt32)Map::_COUNT], bgfx::ShaderHandle shader);

        void Bind();

    private:
        bgfx::TextureHandle m_Maps[(UInt32)Map::_COUNT];
        bgfx::ShaderHandle m_Shader;
        std::string m_Name;
    };
}