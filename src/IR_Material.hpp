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
            MAP_METALNESS,  // TODO: Combine these into one thing
            MAP_ROUGHNESS,
            MAP_EMISSIVENESS,
            MAP_AMBIENTOCCLUSION,
            MAP__COUNT
        };

        bool Init(const char* path);
        virtual bool Init(const Texture*(&textures)[MAP__COUNT], Shader* shader) = 0;
        virtual void Destroy() {}

        virtual void Reset() = 0;

        virtual void SetTexture(Map map, Texture* texture) = 0;
        virtual void SetShader(Shader* shader) = 0;

    protected:
        KeyValue* m_KeyValues = nullptr;

    private:
        bool SetTextureFromPath(Map map, const std::string& path, bool linearize, bool mipmaps, Texture* def);
    };
}
