#include <renderer/IR_GLRenderer.hpp>
#include <IR_Common.hpp>
#include <IR_Random.hpp>
#include <IR_Window.hpp>
#include <IR_Debug.hpp>

#include <GL/glew.h>
#include <thirdparty/stb_image.h>

void glMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

namespace IR::Renderer {

    GLShader shader;

    const char* GL::GetName() IR_RETURN("OpenGL")

    bool GL::Init()
    {
        m_GLContext = SDL_GL_CreateContext((SDL_Window*)Window::GetHandle());
        SDL_GL_MakeCurrent((SDL_Window*)Window::GetHandle(), m_GLContext);
        if (!SDL_GL_SetSwapInterval(-1)) {
            SDL_GL_SetSwapInterval(1);
        }

        GLenum gcode = glewInit();
        if (gcode != GLEW_OK) {
            IR_MSG(ERROR, "Failed to init GL Renderer: failed to load OpenGL \"%s\"", glewGetErrorString(gcode));
            return false;
        }

        bool hasBT = GLEW_ARB_bindless_texture;
        bool hasMDI = GLEW_ARB_multi_draw_indirect;
        bool hasMB = GLEW_ARB_multi_bind;
        if (!hasBT || !hasMDI || !hasMB ) {
            std::string errMsg = "Failed to init GL Renderer: user's hardware doesn't support ";
            if (!hasBT) {
                errMsg += "\"Bindless Textures\" ";
            }

            if (!hasMDI) {
                errMsg += "\"Multi-Draw Indirect\" ";
            }

            if (!hasMB) {
                errMsg += "\"Multi-Bind\"";
            }

            IR_MSG(ERROR, "%s\n    [Windows] Try using the DirectX Renderer.\n    [Linux] Try using the DirectX Renderer with Wine or Proton.", errMsg.c_str());
            return false;
        }

        // Set up OpenGL
        glEnable(GL_DEBUG_OUTPUT);
	    glDebugMessageCallback(glMessageCallback, NULL);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        // --- [UNIFORMS] ---
        m_UniformCommon.Init(nullptr, sizeof(m_CommonData), UNIFORMLOC_COMMON);

        // --- [SHADER STORAGES] ---

        m_SSMaterialInfos.Init(nullptr, sizeof(MaterialInfo), SSLOC_MATERIALINFO);
        m_SSTextureHandles.Init(nullptr, sizeof(UInt64), SSLOC_TEXTUREHANDLE);

        // --- [VERTEX LAYOUTS] ---
        m_LayoutStandard.InitStandard();
        m_LayoutAnimated.InitAnimated();

        // --- [TEXTURES] ---
        const struct glm::vec<3, UInt8> missingColor1 = { 0, 0, 0 };
        const struct glm::vec<3, UInt8> missingColor2 = { 255, 255, 255 }; // { 251, 62, 249 };
        const struct glm::vec<3, UInt8> whiteColor = { 255, 255, 255 };

        std::vector<glm::vec<3, UInt8>> missingPixels;
        missingPixels.reserve(32 * 32);

        for (UInt32 y = 0; y < 32; y++) {
            for (UInt32 x = 0; x < 32; x++) {
                if ((x / 8 + y / 8) % 2) {
                    missingPixels.push_back(missingColor1);
                } else {
                    missingPixels.push_back(missingColor2);
                }
            }
        }

        m_TextureError.InitMemory((const UInt8*)missingPixels.data(), 32, 32, 3, false, false, true);
        m_TextureBlack.InitMemory((const UInt8*)&missingColor1, 1, 1, 3, false, false, true);
        m_TextureWhite.InitMemory((const UInt8*)&whiteColor, 1, 1, 3, false, false, true);

        // --- [MATERIALS] ---
        m_MaterialError.AddTexture(Material::MAP_DIFFUSE, &m_TextureError);

        // testing
        shader.InitRasterPath("test.vs", "test.fs");

        s_GL = this; // Maybe not the best way to do this but it works perfectly fine.
        return true;
    }

    void GL::Shutdown()
    {
        shader.Destroy();

        m_TextureWhite.Destroy();
        m_TextureBlack.Destroy();
        m_TextureError.Destroy();

        m_LayoutStandard.Destroy();
        m_LayoutAnimated.Destroy();

        m_UniformCommon.Destroy();

        m_SSMaterialInfos.Destroy();
        m_SSTextureHandles.Destroy();

        m_Models.clear();
        m_Materials.clear();

        for (auto& tex : m_Textures) {
            tex.Destroy();
        }

        m_Textures.clear();

        SDL_GL_DestroyContext(m_GLContext);
    }

    void GL::Present()
    {
        m_CommonData.curtime = Globals.curtime;
        m_CommonData.frametime = Globals.frametime;
        m_CommonData.width = Globals.width;
        m_CommonData.height = Globals.height;

        m_MaterialError.Use();

        Debug::FlyCam(m_CommonData.view, m_CommonData.projection);

        m_UniformCommon.Update(&m_CommonData, sizeof(m_CommonData), 0);

        m_SSMaterialInfos.Update(m_MaterialInfos.data(), m_MaterialInfos.size() * sizeof(MaterialInfo), 0);
        m_SSTextureHandles.Update(m_TextureHandles.data(), m_TextureHandles.size() * sizeof(UInt64), 0);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Bind();
        m_TextureWhite.Bind(0);
        static std::vector<InstanceStandard> sigma;

        if (sigma.size() == 0) {
            for (auto& v : m_Models) {
                for (const auto& mesh : v.m_Meshes) {
                    sigma.push_back({ {
                            Random::Float(0.2, 1.0),
                            Random::Float(0.2, 1.0),
                            Random::Float(0.2, 1.0),
                            1.0f
                        },
                        glm::mat4(1.0f)
                    });
                }
            }
        }

        for (auto& v : m_Models) {
            for (UInt32 i = 0; i < v.m_Meshes.size(); i++) {
                v.m_Meshes[i].Draw(&sigma[i]);
            }
        }

        SDL_GL_SwapWindow((SDL_Window*)Window::GetHandle());

        for (GLMaterial& mat : m_Materials) {
            mat.Reset();
        }

        for (GLTexture& tex : m_Textures) {
            tex.btlocation = UINT64_MAX;
        }

        m_MaterialInfos.clear();
        m_TextureHandles.clear();
    }

    Model* GL::MakeModel()
    {
        m_Models.push_front({});
        return &m_Models.front();
    }

    Material* GL::MakeMaterial()
    {
        m_Materials.push_front({});
        return &m_Materials.front();
    }

    GLTexture* GL::MakeTexture()
    {
        m_Textures.push_front({});
        return &m_Textures.front();
    }

    GL* s_GL = nullptr;
}

void glMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
    if (type == GL_DEBUG_TYPE_PERFORMANCE) return;

    char sourceStr[16];
    memset(sourceStr, '\0', sizeof(sourceStr));
    switch (source)
    {
    case GL_DEBUG_SOURCE_API: strcpy(sourceStr, "API"); break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM: strcpy(sourceStr, "WINDOW SYSTEM"); break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: strcpy(sourceStr, "SHADER COMPILER"); break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: strcpy(sourceStr, "THIRDPARTY"); break;
    case GL_DEBUG_SOURCE_APPLICATION: strcpy(sourceStr, "APPLICATION"); break;
    case GL_DEBUG_SOURCE_OTHER: strcpy(sourceStr, "OTHER"); break;
    };

    char typeStr[20];
    memset(typeStr, '\0', sizeof(typeStr));

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR: strcpy(typeStr, "ERROR"); break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: strcpy(typeStr, "DEPRECATED BEHAVIOR"); break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: strcpy(typeStr, "UNDEFINED BEHAVIOR"); break;
    case GL_DEBUG_TYPE_PORTABILITY: strcpy(typeStr, "PORTABILITY"); break;
    case GL_DEBUG_TYPE_PERFORMANCE: strcpy(typeStr, "PERFORMANCE"); break;
    case GL_DEBUG_TYPE_MARKER: strcpy(typeStr, "MARKER"); break;
    case GL_DEBUG_TYPE_OTHER: strcpy(typeStr, "OTHER"); break;
    }

    switch (severity) {
    case GL_DEBUG_SEVERITY_LOW:
        IR_MSG(INFO, "GL MSG %i | [%s] [%s] :: %s", id, sourceStr, typeStr, message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        IR_MSG(WARN, "GL MSG %i | [%s] [%s] :: %s", id, sourceStr, typeStr, message);
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        IR_MSG(ERROR, "GL MSG %i | [%s] [%s] :: %s", id, sourceStr, typeStr, message);
        break;
    }
}