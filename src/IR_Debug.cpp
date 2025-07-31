#include <IR_Debug.hpp>
#include <IR_Input.hpp>

#include <glm/gtc/type_ptr.hpp>

namespace IR::Debug
{

    void FlyCam(glm::mat4& outView, glm::mat4& outProjection)
    {
        static glm::vec3 pos = glm::vec3(0, 64, 0);
        static glm::vec3 dir = glm::vec3(0, 0, 1);
        static glm::vec3 angles = glm::vec3(0, 0, 0);
        static float fov = 70.0f;
        static float nearz = 10.0f;
        static float farz = 8192.0f;

        // glm::ivec2 mdelta = Input::MouseDelta();

        // angles.y += mdelta.x;
        // angles.x -= mdelta.y;

		if (Input::IsKeyDown(Input::Key::LEFT)) {
			angles.y -= 200.0f * Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::RIGHT)) {
			angles.y += 200.0f * Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::DOWN)) {
			angles.x -= 200.0f * Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::UP)) {
			angles.x += 200.0f * Globals.frametime;
		}

		dir.x = cosf(glm::radians(angles.y)) * cosf(glm::radians(angles.x));
		dir.y = sinf(glm::radians(angles.x));
		dir.z = sinf(glm::radians(angles.y)) * cosf(glm::radians(angles.x));

		glm::vec3 side = glm::normalize(glm::cross(dir, { 0.0f, 1.0f, 0.0f }));
		if (Input::IsKeyDown(Input::Key::W)) {
			pos += dir * 100.0f * (Float32)Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::S)) {
			pos -= dir * 100.0f * (Float32)Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::A)) {
			pos -= side * 100.0f * (Float32)Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::D)) {
			pos += side * 100.0f * (Float32)Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::SPACE)) {
			pos.y += 100.0f * Globals.frametime;
		}

		if (Input::IsKeyDown(Input::Key::LCTRL)) {
			pos.y -= 100.0f * Globals.frametime;
		}

        outView = glm::lookAt(pos, pos + dir, glm::vec3(0, 1, 0));
        outProjection = glm::perspective(glm::radians(fov), (Float32)Globals.width / Globals.height, nearz, farz);
    }

}