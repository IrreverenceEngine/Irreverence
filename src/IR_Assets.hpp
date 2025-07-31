#pragma once

#include <IR_Texture.hpp>
#include <IR_Material.hpp>

namespace IR {

	class Assets {
	public:
		static Renderer::Texture* Texture(const char* path, bool mipmaps, bool linearize = false);
		static Renderer::Material* Material(const char* path);

	};

}
