#include <IR_Assets.hpp>
#include <IR_Renderer.hpp>

#include <string>
#include <unordered_map>

namespace IR {
	static std::unordered_map<std::string, IR::Renderer::Texture*> s_Textures;

	Renderer::Texture* Assets::Texture(const char* path, bool mipmaps, bool linearize)
	{
		auto it = s_Textures.find(path);

		if (it != s_Textures.end()) {
			return it->second;
		}

		Renderer::Texture* texture = Renderer::MakeTexture();

		if (!texture->InitPath(path, linearize, mipmaps)) {
			texture->Destroy();
			delete texture;
			return nullptr;
		}

		s_Textures[path] = texture;
		return texture;
	}

	static std::unordered_map<std::string, IR::Renderer::Material*> s_Materials;

	Renderer::Material* Assets::Material(const char* path)
	{
		auto it = s_Materials.find(path);
		if (it != s_Materials.end()) {
			return it->second;
		}

		Renderer::Material* material = Renderer::MakeMaterial();

		if (!material->Init(path)) {
			material->Destroy();
			delete material;
			return nullptr;
		}

		s_Materials[path] = material;
		return material;
	}
}
