#include <IR_Common.hpp>
#include <IR_Random.hpp>
#include <IR_Window.hpp>
#include <IR_Input.hpp>
#include <IR_BinaryMap.hpp>

using namespace IR;

int main(int argc, char** argv)
{
	Random::SeedRandom();

	if (!Window::Init(Renderer::API::OpenGL)) {
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

	Renderer::Model* mdl = Renderer::MakeModel();
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

					mdl->MakeMesh(face.vertices.data(), face.vertices.size(), tempIndices.data(), tempIndices.size());
				}
			}
		}
	}

	while(!Window::ShouldClose()) {
		Renderer::Present();

		if (Input::IsKeyPressed(Input::Key::X)) {
			IR_SCRMSG(INFO, "yippe");
		}

		if (Input::IsKeyPressed(Input::Key::ESCAPE)) {
			Window::Close();
		}
    }

	Renderer::Shutdown();
	Window::Shutdown();

    return 0;
}