#include <IR_Common.hpp>
#include <IR_Random.hpp>
#include <IR_Window.hpp>
#include <IR_Input.hpp>
#include <IR_BinaryMap.hpp>

#include <glm/gtc/quaternion.hpp>

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
	if (!mapfile.Load("simple.irbm")) {
		return 1;
	}

	std::vector<Renderer::Mesh*> meshes;
	for (BinaryMap::EntityData& ent : mapfile.GetEntityDatas()) {
		if (ent.keyvalues["classname"] == "worldspawn") {
			for (const auto& brush : ent.brushes) {
				for (const auto& face : brush.faces) {
					if (face.flags & 1) {
						continue;
					}

					std::vector<UInt32> tempIndices;
					for (UInt32 i = 0; i < face.vertices.size(); i++) {
						tempIndices.push_back(i);
					}

					Renderer::Mesh* mesh = Renderer::MakeMesh();
					mesh->Init(face.vertices.data(), face.vertices.size(), tempIndices.data(), tempIndices.size());
					meshes.emplace_back(mesh);
				}
			}
		}
	}


	static std::vector<Renderer::Material*> mapMats;

	for (UInt32 i = 0; i < meshes.size(); i++) {
		mapMats.push_back(Random::Int(0, 1) == 0 ? Renderer::GetMaterialError() : Renderer::GetMaterialWhite());
	}

	for (UInt32 i = 0; i < meshes.size(); i++) {
		Renderer::Mesh* mesh = meshes[i];

		Renderer::SubmitMapMesh(mesh, mapMats[i]);
	}

	while(!Window::ShouldClose()) {

		Renderer::Present();

		if (Input::IsKeyPressed(Input::Key::X)) {
			IR_MSG(INFO, "yippe");
		}

		if (Input::IsKeyPressed(Input::Key::ESCAPE)) {
			Window::Close();
		}
    }

	Renderer::Shutdown();
	Window::Shutdown();

    return 0;
}