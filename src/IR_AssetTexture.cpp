#include <IR_AssetTexture.hpp>

#include <thirdparty/stb_image.h>

#include <string>

namespace IR::Asset::Texture {

    void Load(UInt8* data, Int32 width, Int32 height, UInt8 channels, bool mipmap)
    {
    }

    void Load(const char* path, bool mipmap)
    {
        std::string cppPath = path;

        Int32 width, height, channels;
        UInt8* data = stbi_load(("assets/textures/" + cppPath).c_str(), &width, &height, &channels, 4);
        if (!data) {
            IR_MSG(ERROR, "Couldn't load image: %s", stbi_failure_reason());
            return;
        }

        IR_DEFER({ stbi_image_free(data); });
    }

    void Get(const char* path, bool mipmap, bool load)
    {
    }

    void Cleanup()
    {
    }

}