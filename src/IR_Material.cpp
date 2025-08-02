#include <IR_Material.hpp>
#include <IR_Assets.hpp>
#include <IR_Renderer.hpp>

namespace IR::Renderer {

    bool Material::Init(const char* path)
    {
        m_KeyValues = KeyValue::Load(path);

        if (!m_KeyValues) {
            IR_MSG(ERROR, "Material failed to load KeyValue file \"%s\"", path);
            return false;
        }

        KeyValue* material = m_KeyValues->GetChild(0);
        if (!material) {
            IR_MSG(ERROR, "Material failed to load get child data in file \"%s\"", path);
            return false;
        }

	    KeyValue* params = material->GetChild(0);
        if (!params) {
            IR_MSG(ERROR, "Material is missing parameters in file \"%s\"", path);
            return false;
        }

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

        Texture* albedoTex = Renderer::GetTextureError();
	    const std::string& albedoPath = params->FindChildString("Albedo");
        if (!albedoPath.empty()) {
            albedoTex = Assets::Texture(albedoPath.substr(9).c_str(), linearize, mipmaps);
        } else {
            IR_MSG(WARN, "Material is missing \"Albedo\" parameter in file \"%s\"", path);
        }

        Texture* normalTex = Renderer::GetTextureError();
        const std::string& normalPath = params->FindChildString("Normal");
        if (!normalPath.empty()) {
            normalTex = Assets::Texture(normalPath.substr(9).c_str(), linearize, mipmaps);
        }

        Texture* amreTex = Renderer::GetTextureBlack();
        const std::string& amrePath = params->FindChildString("AMRE");
        if (!amrePath.empty()) {
            amreTex = Assets::Texture(amrePath.substr(9).c_str(), linearize, mipmaps);
        }

        AddTexture(MAP_ALBEDO, albedoTex);
        AddTexture(MAP_NORMAL, normalTex);
        AddTexture(MAP_AMRE, amreTex);

        return true;
    }
}