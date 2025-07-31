#pragma once

#include <IR_Texture.hpp>
#include <IR_Shader.hpp>
#include <IR_Material.hpp>

namespace IR {

	class Assets {
	public:
		static Renderer::Texture* Texture(const char* path, bool linearize, bool mipmaps);
		static Renderer::Shader* Shader(const char* path);
		static Renderer::Material* Material(const char* path);
	};

}
