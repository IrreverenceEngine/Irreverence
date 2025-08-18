#include <IR_Material.hpp>
#include <IR_Assets.hpp>
#include <IR_Renderer.hpp>

namespace IR::Renderer {

    bool Material::Init(const char* path)
    {
        KeyValue* kv = KeyValue::Load(path);

        if (!kv) {
            IR_MSG(ERROR, "Material failed to load KeyValue file \"%s\"", path);
            return false;
        }

        KeyValue* material = kv->GetChild(0);
        if (!material) {
            IR_MSG(ERROR, "Material failed to load get child data in file \"%s\"", path);
            return false;
        }

	    KeyValue* params = material->GetChild(0);
        if (!params) {
            IR_MSG(ERROR, "Material is missing parameters in file \"%s\"", path);
            return false;
        }

        m_KeyValues = params;

        std::string shaderName = params->FindChildString("Shader");
        if (shaderName.empty()) {
            IR_MSG(ERROR, "Material is missing \"Shader\" parameter in file\"%s\"", path);
            return false;
        }

        Shader* shader = Assets::Shader(shaderName.c_str());
        if (!shader) {
            IR_MSG(ERROR, "Material failed to load a shader file \"%s\"", shaderName.c_str());
            return false;
        }

        SetShader(Assets::Shader(shaderName.c_str()));

        bool linearize = (bool)params->FindChildNumber("Linearize", 1);
        bool mipmaps = (bool)params->FindChildNumber("Mipmap", 1);

	    const std::string& albedoPath = params->FindChildString("Albedo");
        if (!SetTextureFromPath(MAP_ALBEDO, params->FindChildString("Albedo"), linearize, mipmaps, Renderer::GetTextureError())) {
            IR_MSG(WARN, "Material is missing \"Albedo\" parameter in file \"%s\"", path);
        }

        SetTextureFromPath(MAP_NORMAL, params->FindChildString("Normal"), linearize, mipmaps, Renderer::GetTextureNormal());
        SetTextureFromPath(MAP_METALNESS, params->FindChildString("Metal"), linearize, mipmaps, Renderer::GetTextureBlack());
        SetTextureFromPath(MAP_ROUGHNESS, params->FindChildString("Rough"), linearize, mipmaps, Renderer::GetTextureBlack());
        SetTextureFromPath(MAP_EMISSIVENESS, params->FindChildString("Emissive"), linearize, mipmaps, Renderer::GetTextureBlack());
        SetTextureFromPath(MAP_AMBIENTOCCLUSION, params->FindChildString("AO"), linearize, mipmaps, Renderer::GetTextureWhite());

        return true;
    }

    bool Material::SetTextureFromPath(Map map, const std::string& path, bool linearize, bool mipmaps, Texture* def)
    {
        Texture* texture = !path.empty() ? Assets::Texture(path.substr(9).c_str(), linearize, mipmaps) : def;
        texture = texture ? texture : def; // funny

        SetTexture(map, texture);

        return texture != def;
    }

}
