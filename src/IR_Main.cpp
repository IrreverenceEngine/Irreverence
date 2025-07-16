#include <IR_Window.hpp>

#include <IR_Input.hpp>
#include <IR_AssetBMap.hpp>
#include <IR_Common.hpp>

using namespace IR;

int main(int argc, char** argv)
{
	if (!Window::Init(Renderer::API::OpenGL)) {
		IR_MSG(FATAL, "Failed to init Window, shutting down!");
		return 1;
	}

	if (!Renderer::Init()) {
		IR_MSG(FATAL, "Failed to init Renderer, shutting down!");
		return 1;
	}

    IR_MSG(INFO, "Successfully initialized Irreverence");

	Asset::BMap::Load("simple.irbm");

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