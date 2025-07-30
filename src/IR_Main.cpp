#include "IR_Log.hpp"
#include <IR_Common.hpp>
#include <IR_Random.hpp>
#include <IR_Window.hpp>
#include <IR_Input.hpp>
#include <IR_BinaryMap.hpp>

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
	if (!mapfile.Load("simple.irbm")) {
		return 1;
	}

	KeyValue* kv = KeyValue::Load("assets/materials/CLIP.shader");

	if (!kv) {
		IR_MSG(FATAL, "Failed to load keyvalue file, shutting down!");
		return 1;
	}

	KeyValue* shader = kv->GetChild(0);

	if (!shader) {
		IR_MSG(FATAL, "Failed to get shader keyvalue, shutting down!");
		return 1;
	}

	IR_MSG(INFO, "Shader name: %s", shader->GetKey().c_str());

	KeyValue* params = shader->GetChild(0);

	if (!params) {
		IR_MSG(FATAL, "Failed to get shader parameters, shutting down!");
		return 1;
	}

	auto map = params->FindChildString("map");

	IR_MSG(INFO, "Shader map: %s", map.c_str());

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

	for (UInt32 i = 0; i < meshes.size(); i++) {
		Renderer::Mesh* mesh = meshes[i];

		Renderer::SubmitMapMesh(mesh, Renderer::GetMaterialError());
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
