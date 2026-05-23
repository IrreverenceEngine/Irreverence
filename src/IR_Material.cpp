#include <IR_Material.hpp>
#include <IR_Assets.hpp>
#include <IR_Renderer.hpp>

#include <Thirdparty/stb_image.h>

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

        SetShader(shader);

        bool linearize = (bool)params->FindChildNumber("Linearize", 1);
        bool cubemap = (bool)params->FindChildNumber("Cubemap", 0);

        if (!cubemap) {
            bool mipmaps = (bool)params->FindChildNumber("Mipmap", 1);

            const std::string& albedoPath = params->FindChildString("Albedo");
            if (!SetTextureFromPath(MAP_ALBEDO, params->FindChildString("Albedo"), linearize, mipmaps, Renderer::GetTextureError())) {
                IR_MSG(WARN, "Material is missing \"Albedo\" parameter in file \"%s\"", path);
            }

            SetTextureFromPath(MAP_NORMAL, params->FindChildString("Normal"), linearize, mipmaps, Renderer::GetTextureNormal());
            SetTextureFromPath(MAP_SDE, params->FindChildString("SDE"), linearize, mipmaps, Renderer::GetTextureSDE());
        } else {
            const std::string cubemapParams[6] = {
                params->FindChildString("Right"),
                params->FindChildString("Left"),
                params->FindChildString("Top"),
                params->FindChildString("Bottom"),
                params->FindChildString("Back"),
                params->FindChildString("Front")
            };

            Texture::ImageInfo infos[6] = {};

            Int32 width = 0, height = 0, channels = 0;
            bool success = true;

            stbi_set_flip_vertically_on_load(false);

            for (UInt8 i = 0; i < 6; i++) {
                if (cubemapParams[i].empty()) {
                    success = false;
                    break;
                }

                const std::string& cubepath = cubemapParams[i];
                UInt8* data = stbi_load(("assets/" + cubepath).c_str(), &width, &height, &channels, 4);
                if (!data) {
                    IR_MSG(ERROR, "Material cubemap failed to open image \"%s\", reason: %s", cubepath.c_str(), stbi_failure_reason());
                    success = false;
                    break;
                }

                infos[i] = { data, width, height };
            }

            stbi_set_flip_vertically_on_load(true);

            if (!success) {
                for (UInt8 i = 0; i < 6; i++) {
                    if (infos[i].data) {
                        stbi_image_free((void*)infos[i].data);
                    }
                }
                return false;
            }

            Renderer::Texture* cubemapTex = Renderer::MakeTexture();
            cubemapTex->InitMemoryCubemap(infos, channels, linearize, true);
            SetTexture(MAP_ALBEDO, cubemapTex);
        }

        return true;
    }

    bool Material::SetTextureFromPath(Map map, const std::string& path, bool linearize, bool mipmaps, Texture* def)
    {
        Texture* texture = !path.empty() ? Assets::Texture(path.substr(9).c_str(), linearize, mipmaps, true) : def;
        texture = texture ? texture : def;

        SetTexture(map, texture);

        return texture != def;
    }

}
