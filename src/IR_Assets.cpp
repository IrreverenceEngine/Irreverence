#include <IR_Assets.hpp>
#include <IR_Renderer.hpp>

#include <string>
#include <unordered_map>

namespace IR {
	static std::unordered_map<std::string, IR::Renderer::Texture*> s_Textures;

	Renderer::Texture* Assets::Texture(const char* path, bool linearize, bool mipmaps)
	{
		std::string name = ("assets/textures/" + std::string(path)).c_str();

		auto it = s_Textures.find(path);
		if (it != s_Textures.end()) {
			return it->second;
		}

		Renderer::Texture* texture = Renderer::MakeTexture();

		if (!texture->Init(name.c_str(), linearize, mipmaps, true)) {
			return nullptr;
		}

		s_Textures[name] = texture;
		return texture;
	}

	static std::unordered_map<std::string, IR::Renderer::Shader*> s_Shaders;
	Renderer::Shader* Assets::Shader(const char* path)
	{
		std::string name = ("assets/shaders/" + std::string(path)).c_str();

		auto it = s_Shaders.find(name);
		if (it != s_Shaders.end()) {
			return it->second;
		}

		Renderer::Shader* shader = Renderer::MakeShader();
		if (!shader->Init(name.c_str())) {
			return nullptr;
		}

		s_Shaders[name] = shader;
		return shader;
	}

	static std::unordered_map<std::string, IR::Renderer::Material*> s_Materials;

	Renderer::Material* Assets::Material(const char* path)
	{
		std::string name = ("assets/materials/" + std::string(path)).c_str();

		auto it = s_Materials.find(name);
		if (it != s_Materials.end()) {
			return it->second;
		}

		Renderer::Material* material = Renderer::MakeMaterial();

		if (!material->Init(name.c_str())) {
			return nullptr;
		}

		s_Materials[name] = material;
		return material;
	}
}
