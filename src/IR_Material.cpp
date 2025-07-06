#include <IR_Material.hpp>

namespace IR {

    Material::Material(const bgfx::TextureHandle* (&maps)[(UInt32)Map::_COUNT], bgfx::ShaderHandle shader)
    {
        // TODO: Load default textures (ERROR, BLACK, BLACK, NORMAL, WHITE)

        for (UInt32 i = 0; i < (UInt32)Map::_COUNT; i++) {
            if (!maps[i]) {
                // Load default
                continue;
            }

            m_Maps[i] = *maps[i];
        }

        m_Shader = shader;
    }

    void Material::Bind()
    {

    }

}