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