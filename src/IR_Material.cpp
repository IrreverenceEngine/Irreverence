#include <IR_Material.hpp>
#include <IR_Assets.hpp>

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

	    const std::string& diffuse = params->FindChildString("map");
        if (diffuse.empty()) {
            IR_MSG(ERROR, "Material is missing \"map\" parameter in file \"%s\"", path);
            return false;
        }

        if (diffuse.substr(0, 9) != "textures/") {
            IR_MSG(ERROR, "Material map parameter is missing \"textures/\" prefix in file \"%s\"", path);
            return false;
        }

        std::string shaderName = params->FindChildString("Shader");
        if (shaderName.empty()) {
            IR_MSG(ERROR, "Material is missing \"Shader\" parameter in file\"%s\"", path);
            return false;
        }

        bool linearize = (bool)params->FindChildNumber("Linearize", 1);
        bool mipmaps = (bool)params->FindChildNumber("Mipmap", 1);
        AddTexture(MAP_DIFFUSE, Assets::Texture(diffuse.substr(9).c_str(), linearize, mipmaps));

        SetShader(Assets::Shader(shaderName.c_str()));

        return true;
    }
}