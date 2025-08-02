#include "IR_Log.hpp"
#include <IR_Common.hpp>
#include <IR_Random.hpp>
#include <IR_Window.hpp>
#include <IR_Input.hpp>
#include <IR_BinaryMap.hpp>
#include <IR_Assets.hpp>

#include <glm/gtc/quaternion.hpp>

#include <IR_KeyValue.hpp>

using namespace IR;

int main(int argc, char** argv)
{
	Random::SeedRandom();

	if (!Window::Init(Renderer::API::OPENGL)) {
		IR_MSG(FATAL, "Failed to init Window, shutting down!");
		return 1;
	}

	if (!Renderer::Init()) {
		IR_MSG(FATAL, "Failed to init Renderer, shutting down!");
		return 1;
	}

    IR_MSG(INFO, "Successfully initialized Irreverence");

	BinaryMap mapfile;
	if (!mapfile.Load("concept.irbm")) {
		return 1;
	}

	struct MeshMaterial { Renderer::Mesh* mesh; Renderer::Material* mat; };
	std::vector<MeshMaterial> meshes;
	for (BinaryMap::EntityData& ent : mapfile.GetEntityDatas()) {
		if (ent.keyvalues["classname"] == "worldspawn") {
			for (const auto& brush : ent.brushes) {
				for (const auto& face : brush.faces) {
					if (face.flags & 1) {
						continue;
					}

					Renderer::Mesh* mesh = Renderer::MakeMesh();
					mesh->Init(face.vertices.data(), face.vertices.size(), face.indices.data(), face.indices.size());
					meshes.push_back({ mesh, Assets::Material(face.materialName.c_str()) });
				}
			}
		}
	}

	for (UInt32 i = 0; i < meshes.size(); i++) {
		Renderer::SubmitMapMesh(meshes[i].mesh, meshes[i].mat);
	}

	while(!Window::ShouldClose()) {

		Renderer::Present();

		if (Input::IsKeyPressed(Input::Key::X)) {
			IR_MSG(INFO, "Destroyed door1");
			Renderer::Texture* tex = Assets::Texture("door1.png", false, false);
			tex->Destroy();

			Renderer::Material* mat = Assets::Material("door1.shader");
			mat->AddTexture(Renderer::Material::MAP_ALBEDO, Assets::Texture("gem.png", true, true));
			mat->Reset();
		}

		if (Input::IsKeyPressed(Input::Key::F1)) {
			Window::ToggleMouseLock();
		}

		if (Input::IsKeyPressed(Input::Key::ESCAPE)) {
			Window::Close();
		}
    }

	Renderer::Shutdown();
	Window::Shutdown();

    return 0;
}
