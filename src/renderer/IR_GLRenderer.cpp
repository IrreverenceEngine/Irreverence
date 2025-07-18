#include <renderer/IR_GLRenderer.hpp>
#include <IR_Common.hpp>
#include <IR_Random.hpp>
#include <IR_Window.hpp>
#include <IR_Debug.hpp>

#include <GL/glew.h>
#include <thirdparty/stb_image.h>

namespace IR::Renderer {

    GLShader shader;

    const char* GL::GetName() IR_RETURN("OpenGL")

    bool GL::Init()
    {
        m_GLContext = SDL_GL_CreateContext((SDL_Window*)Window::GetHandle());
        SDL_GL_MakeCurrent((SDL_Window*)Window::GetHandle(), m_GLContext);
        SDL_GL_SetSwapInterval(1);

        GLenum gcode = glewInit();
        if (gcode != GLEW_OK) {
            IR_MSG(ERROR, "Failed to init GL Renderer: failed to load GLEW \"%s\"", glewGetErrorString(gcode));
            return false;
        }

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glFrontFace(GL_CCW);

        glEnable(GL_DEPTH_TEST);

        m_UniformCommon.Init(nullptr, sizeof(m_CommonData), UNIFORMLOC_COMMON);

        const struct glm::vec<3, UInt8> missingColor1 = { 0, 0, 0 };
        const struct glm::vec<3, UInt8> missingColor2 = { 251, 62, 249 };
        const struct glm::vec<3, UInt8> whiteColor = { 255, 255, 255 };

        std::vector<glm::vec<3, UInt8>> missingPixels;
        for (UInt32 y = 0; y < 32; y++) {
            for (UInt32 x = 0; x < 32; x++) {
                if ((x / 8 + y / 8) % 2) {
                    missingPixels.push_back(missingColor1);
                } else {
                    missingPixels.push_back(missingColor2);
                }
            }
        }

        m_TextureError.InitMemory((const UInt8*)missingPixels.data(), 32, 32, 3, false, false);
        m_TextureBlack.InitMemory((const UInt8*)&missingColor1, 1, 1, 3, false, false);
        m_TextureWhite.InitMemory((const UInt8*)&whiteColor, 1, 1, 3, false, false);

        m_MaterialError.AddTexture(Material::MAP_DIFFUSE, &m_TextureError);

        // testing

        shader.InitRasterPath("test.vs", "test.fs");

        return true;
    }

    void GL::Shutdown()
    {
        shader.Destroy();

        m_UniformCommon.Destroy();

        for (UInt32 i = 0; i < m_Models.size(); i++) {
            GLModel* model = m_Models[i];
            if (model) {
                delete model;
                model = nullptr;
            }
        }

        SDL_GL_DestroyContext(m_GLContext);
    }

    void GL::Present()
    {
        m_CommonData.curtime = Globals.curtime;
        m_CommonData.frametime = Globals.frametime;
        m_CommonData.width = Globals.width;
        m_CommonData.height = Globals.height;

        Debug::FlyCam(m_CommonData.view, m_CommonData.projection);

        m_UniformCommon.Update(&m_CommonData, sizeof(m_CommonData), 0);

        glClearColor(0.4f, 0.7f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Bind();
        m_TextureWhite.Bind(0);
        static std::vector<InstanceStandard> sigma;

        if (sigma.size() == 0) {
            for (auto& v : m_Models) {
                for (const auto& mesh : v->m_Meshes) {
                    sigma.push_back({ {
                            Random::RandomFloat64(0.0f, 1.0f),
                            Random::RandomFloat64(0.0f, 1.0f),
                            Random::RandomFloat64(0.0f, 1.0f),
                            1.0f
                        },
                        glm::mat4(1.0f)
                    });
                }
            }
        }

        for (auto& v : m_Models) {
            for (UInt32 i = 0; i < v->m_Meshes.size(); i++) {
                v->m_Meshes[i].Draw(&sigma[i]);
            }
        }

        SDL_GL_SwapWindow((SDL_Window*)Window::GetHandle());
    }

    Model* GL::MakeModel()
    {
        GLModel* model = new GLModel;
        m_Models.push_back(model);

        return model;
    }

    Material* GL::MakeMaterial()
    {
        GLMaterial* material = new GLMaterial;
        m_Materials.push_back(material);

        return material;
    }

}