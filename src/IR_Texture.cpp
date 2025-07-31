#include <IR_Texture.hpp>

#include <Thirdparty/stb_image.h>

#include <string>

namespace IR::Renderer {

    bool Texture::Init(const char* path, bool linearize, bool mipmaps, bool handle)
    {
        Int32 width = 0, height = 0, channels = 0;
        UInt8* data = stbi_load(path, &width, &height, &channels, 0);

        if (!data) {
            IR_MSG(ERROR, "Texture failed to open image \"%s\", reason: %s", path, stbi_failure_reason());
            return false;
        }

        bool success = InitMemory(data, width, height, channels, linearize, mipmaps, handle);
        stbi_image_free(data);

        return success;
    }

}