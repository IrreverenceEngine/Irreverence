#include "IR_Log.hpp"
#include <IR_Window.hpp>
#include <IR_Renderer.hpp>
#include <IR_Input.hpp>
#include <IR_Common.hpp>

using namespace IR;

int main(int argc, char** argv)
{
	if (!Window::Init()) {
		return 1;
	}

	if (!Renderer::Init()) {
		return 1;
	}

    IR_MSG(INFO, "Successfully initialized Irreverence");

	glm::mat4 view;
	glm::mat4 proj;

	while(!Window::ShouldClose()) {
		Renderer::DebugCamera(view, proj);
		Renderer::Present();

		if (Input::IsKeyPressed(Input::Key::L)) {
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