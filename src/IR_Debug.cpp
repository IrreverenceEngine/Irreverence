#include <IR_Debug.hpp>
#include <IR_Input.hpp>
#include <IR_Window.hpp>
#include <IR_CVar.hpp>

#include <glm/gtc/type_ptr.hpp>

namespace IR::Debug
{

    glm::vec3 FlyCam(glm::mat4& outView, glm::mat4& outProjection)
    {
        static glm::vec3 pos = glm::vec3(200, 100, -88);
        static glm::vec3 dir = glm::vec3(0, 0, 1);
        static glm::vec3 angles = glm::vec3(0, 270, 0);
        static float fov = 70.0f;
        static float nearz = 10.0f;
        static float farz = 65356.0f;

        static CVar* sensitivity = CVar::Get("in_sensitivity");
        float sensitivityValue = sensitivity ? sensitivity->GetFloat64() : 1.0f;

        if (Window::IsMouseLocked()) {
            glm::ivec2 mdelta = Input::MouseDelta();
            angles.y += mdelta.x * sensitivityValue;
            angles.x -= mdelta.y * sensitivityValue;
        }

        dir.x = cosf(glm::radians(angles.y)) * cosf(glm::radians(angles.x));
        dir.y = sinf(glm::radians(angles.x));
        dir.z = sinf(glm::radians(angles.y)) * cosf(glm::radians(angles.x));

        glm::vec3 side = glm::normalize(glm::cross(dir, { 0.0f, 1.0f, 0.0f }));
        if (Input::IsKeyDown(Input::Key::W)) {
            pos += dir * 400.0f * (Float32)Globals.Frametime();
        }

        if (Input::IsKeyDown(Input::Key::S)) {
            pos -= dir * 400.0f * (Float32)Globals.Frametime();
        }

        if (Input::IsKeyDown(Input::Key::A)) {
            pos -= side * 400.0f * (Float32)Globals.Frametime();
        }

        if (Input::IsKeyDown(Input::Key::D)) {
            pos += side * 400.0f * (Float32)Globals.Frametime();
        }

        if (Input::IsKeyDown(Input::Key::SPACE)) {
            pos.y += 400.0f * Globals.Frametime();
        }

        if (Input::IsKeyDown(Input::Key::LCTRL)) {
            pos.y -= 400.0f * Globals.Frametime();
        }

        outView = glm::lookAt(pos, pos + dir, glm::vec3(0, 1, 0));
        outProjection = glm::perspective(glm::radians(fov), (Float32)Globals.Width() / Globals.Height(), nearz, farz);

        return pos;
    }

}
