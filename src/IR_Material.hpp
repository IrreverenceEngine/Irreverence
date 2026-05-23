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
            MAP_SDE,
            MAP__COUNT
        };

        bool Init(const char* path);
        virtual bool Init(const Texture*(&textures)[MAP__COUNT], Shader* shader) = 0;
        virtual void Destroy() {}

        virtual void Bind() {};

        virtual void Reset() = 0;

        virtual void SetTexture(Map map, Texture* texture) = 0;
        virtual void SetShader(Shader* shader) = 0;

        Float64 GetKVNumber(const char* key, Float64 def = 0.0) const IR_RETURN(m_KeyValues ? m_KeyValues->FindChildNumber(key, def) : def)
        std::string GetKVString(const char* key, const char* def = "") const IR_RETURN(m_KeyValues ? m_KeyValues->FindChildString(key, def) : def)

    protected:
        KeyValue* m_KeyValues = nullptr;

    private:
        bool SetTextureFromPath(Map map, const std::string& path, bool linearize, bool mipmaps, Texture* def);
    };
}
