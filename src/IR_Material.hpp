#pragma once

#include <IR_Texture.hpp>
#include <IR_Shader.hpp>
#include <IR_Common.hpp>

#include <IR_KeyValue.hpp>

namespace IR::Renderer {
    class Material {
    public:
		virtual ~Material() = default;

        enum Map {
            MAP_ALBEDO,
            MAP_NORMAL,
            MAP_AMRE, // AO, Metalness, Roughness, Emissive
            MAP__COUNT
        };

        bool Init(const char* path);
        virtual void Destroy() {}

        virtual void Reset() = 0;

        virtual void AddTexture(Map map, Texture* texture) = 0;
        virtual void SetShader(Shader* shader) = 0;

    protected:
        KeyValue* m_KeyValues = nullptr;
    };
}
