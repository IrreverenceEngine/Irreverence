#pragma once

#include <IR_Texture.hpp>
#include <IR_Shader.hpp>
#include <IR_Material.hpp>

namespace IR::Assets {
    Renderer::Texture* Texture(const char* path, bool linearize, bool mipmaps, bool handle = false);
    Renderer::Shader* Shader(const char* path);
    Renderer::Material* Material(const char* path);
}
