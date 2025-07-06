#include <IR_AssetTexture.hpp>

#include <Thirdparty/stb_image.h>

#include <map>
#include <string>

namespace IR::Asset::Texture {

    static std::map<std::string, bgfx::TextureHandle> s_Textures;

    bgfx::TextureHandle Load(UInt8* data, Int32 width, Int32 height, UInt8 channels, bool mipmap)
    {
        UInt64 dataSize = width * height * channels;
        const bgfx::Memory* mem = bgfx::alloc(dataSize);
        memcpy(mem->data, data, dataSize);

        return bgfx::createTexture2D(width, height, mipmap, 1, bgfx::TextureFormat::RGBA16, 0, mem);
    }

    bgfx::TextureHandle Load(const char* path, bool mipmap)
    {
        std::string cppPath = path;

        Int32 width, height, channels;
        UInt8* data = stbi_load(("assets/textures/" + cppPath).c_str(), &width, &height, &channels, 4);
        if (!data) {
            IR_MSG(ERROR, "Couldn't load image: %s", stbi_failure_reason());
            return *(bgfx::TextureHandle*)&bgfx::kInvalidHandle; // ummm... ok
        }

        IR_DEFER({ stbi_image_free(data); });

        return Load(data, width, height, channels, mipmap);
    }

    bgfx::TextureHandle Get(const char* path, bool mipmap, bool load)
    {
        auto it = s_Textures.find(path);
        if (it != s_Textures.end()) {
            return it->second;
        }

        if (load) {
            return Load(path, mipmap);
        } else {
            IR_MSG(ERROR, "Could not get Texture: %s", path);
            return *(bgfx::TextureHandle*)&bgfx::kInvalidHandle;
        }
    }

    void Cleanup()
    {
        for (const auto& kv : s_Textures) {
            bgfx::destroy(kv.second);
        }
    }

}