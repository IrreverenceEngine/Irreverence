#include <Renderer/IR_GLRenderer.hpp>
#include <IR_Common.hpp>
#include <IR_Random.hpp>
#include <IR_Window.hpp>
#include <IR_Debug.hpp>

#include <GL/glew.h>
#include <Thirdparty/stb_image.h>

#include <glm.hpp>
#include <glm/gtc/type_ptr.hpp>

void glMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

namespace IR::Renderer {

	constexpr VertexStandard CUBE_VERTS[] = {
		{ { -1.0f, 1.0f, -1.0f },		{ 0.0f, 1.0f, 0.0f },		{ 1.0f, 1.0f } },
		{ { 1.0f, 1.0f, 1.0f },			{ 0.0f, 1.0f, 0.0f },		{ 0.0f, 0.0f } },
		{ { 1.0f, 1.0f, -1.0f },		{ 0.0f, 1.0f, 0.0f },		{ 0.0f, 1.0f } },
		{ { 1.0f, 1.0f, 1.0f },			{ 0.0f, 0.0f, 1.0f },		{ 1.0f, 1.0f } },
		{ { -1.0f, -1.0f, 1.0f },		{ 0.0f, 0.0f, 1.0f },		{ 0.0f, 0.0f } },
		{ { 1.0f, -1.0f, 1.0f },		{ 0.0f, 0.0f, 1.0f },		{ 0.0f, 1.0f } },
		{ { -1.0f, 1.0f, 1.0f },		{ -1.0f, 0.0f, 0.0f },		{ 1.0f, 1.0f } },
		{ { -1.0f, -1.0f, -1.0f },		{ -1.0f, 0.0f, 0.0f },		{ 0.0f, 0.0f } },
		{ { -1.0f, -1.0f, 1.0f },		{ -1.0f, 0.0f, 0.0f },		{ 0.0f, 1.0f } },
		{ { 1.0f, -1.0f, -1.0f },		{ 0.0f, -1.0f, 0.0f },		{ 1.0f, 1.0f } },
		{ { -1.0f, -1.0f, 1.0f },		{ 0.0f, -1.0f, 0.0f },		{ 0.0f, 0.0f } },
		{ { -1.0f, -1.0f, -1.0f },		{ 0.0f, -1.0f, 0.0f },		{ 0.0f, 1.0f } },
		{ { 1.0f, 1.0f, -1.0f },		{ 1.0f, 0.0f, 0.0f },		{ 1.0f, 1.0f } },
		{ { 1.0f, -1.0f, 1.0f },		{ 1.0f, 0.0f, 0.0f },		{ 0.0f, 0.0f } },
		{ { 1.0f, -1.0f, -1.0f },		{ 1.0f, 0.0f, 0.0f },		{ 0.0f, 1.0f } },
		{ { -1.0f, 1.0f, -1.0f },		{ 0.0f, 0.0f, -1.0f },		{ 1.0f, 1.0f } },
		{ { 1.0f, -1.0f, -1.0f },		{ 0.0f, 0.0f, -1.0f },		{ 0.0f, 0.0f } },
		{ { -1.0f, -1.0f, -1.0f },		{ 0.0f, 0.0f, -1.0f },		{ 0.0f, 1.0f } },
		{ { -1.0f, 1.0f, 1.0f },		{ 0.0f, 1.0f, 0.0f },		{ 1.0f, 0.0f } },
		{ { -1.0f, 1.0f, 1.0f },		{ 0.0f, 0.0f, 1.0f },		{ 1.0f, 0.0f } },
		{ { -1.0f, 1.0f, -1.0f },		{ -1.0f, 0.0f, 0.0f },		{ 1.0f, 0.0f } },
		{ { 1.0f, -1.0f, 1.0f },		{ 0.0f, -1.0f, 0.0f },		{ 1.0f, 0.0f } },
		{ { 1.0f, 1.0f, 1.0f },			{ 1.0f, 0.0f, 0.0f },		{ 1.0f, 0.0f } },
		{ { 1.0f, 1.0f, -1.0f },		{ 0.0f, 0.0f, -1.0f },		{ 1.0f, 0.0f } }
	};

    constexpr UInt32 CUBE_INDICES[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 0, 18, 1, 3, 19, 4, 6, 20, 7, 9, 21, 10, 12, 22, 13, 15, 23, 16 };

    GLShader shader;

    const char* GL::GetName() IR_RETURN("OpenGL")

    bool GL::Init()
    {
        s_GL = this; // Maybe not the best way to do this but it works perfectly fine.

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

            IR_MSG(ERROR, "%s\n"
                "    [Windows] Try using the DirectX Renderer.\n"
                "    [Linux] Try using the DirectX Renderer with Wine or Proton.", errMsg.c_str());

            return false;
        }

        // Set up OpenGL
        glEnable(GL_DEBUG_OUTPUT);
	    glDebugMessageCallback(glMessageCallback, NULL);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);

        // --- [VERTEX LAYOUTS] ---
        m_LayoutStandard.InitStandard();
        m_LayoutAnimated.InitAnimated();

        // --- [COMMAND LISTS] ---
        m_CmdListStatic.Init();
        m_CmdListDynamic.Init();

        // --- [UNIFORMS] ---
        m_UniformCommon.Init(nullptr, sizeof(m_CommonData), UNIFORMLOC_COMMON);

        // --- [BUFFERS] ---
        m_SBMaterialInfos.Init(GL_SHADER_STORAGE_BUFFER, nullptr, sizeof(GLMaterial::Info), SSLOC_MATERIALINFO, true);
        m_SBTextureHandles.Init(GL_SHADER_STORAGE_BUFFER, nullptr, sizeof(UInt64), SSLOC_TEXTUREHANDLE, true);

        // --- [INSTANCE LISTS] ---
        m_ILStandard.Init(sizeof(InstanceStandard), SSLOC_ILSTANDARD);

        // --- [TEXTURES] ---
        const struct glm::vec<3, UInt8> missingColor1 = { 0, 0, 0 };
        const struct glm::vec<3, UInt8> missingColor2 = { 255, 255, 255 }; // { 251, 62, 249 };
        const struct glm::vec<3, UInt8> whiteColor = { 255, 255, 255 };

        std::vector<glm::vec<3, UInt8>> missingPixels;
        missingPixels.reserve(32 * 32);

        for (UInt32 y = 0; y < 32; y++) {
            for (UInt32 x = 0; x < 32; x++) {
                if ((x / 8 + y / 8) % 2) missingPixels.push_back(missingColor1);
                else missingPixels.push_back(missingColor2);
            }
        }

        m_TextureError.InitMemory((const UInt8*)missingPixels.data(), 32, 32, 3, false, false, true);
        m_TextureBlack.InitMemory((const UInt8*)&missingColor1, 1, 1, 3, false, false, true);
        m_TextureWhite.InitMemory((const UInt8*)&whiteColor, 1, 1, 3, false, false, true);

        // --- [MESHES] ---
        const VertexStandard planeVerts[] = {
            { { -1.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
            { { 1.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
            { { -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
        };

        const UInt32 planeIndices[] = {
            2, 1, 0, 0, 3, 2
        };

        m_MeshCube.Init(CUBE_VERTS, IR_ARRLEN(CUBE_VERTS), CUBE_INDICES, IR_ARRLEN(CUBE_INDICES));
        m_MeshPlane.Init(planeVerts, IR_ARRLEN(planeVerts), planeIndices, IR_ARRLEN(planeIndices));

        // --- [MATERIALS] ---
        m_MaterialError.AddTexture(Material::MAP_DIFFUSE, &m_TextureError);
        m_MaterialWhite.AddTexture(Material::MAP_DIFFUSE, &m_TextureWhite);

        // testing
        shader.InitRasterPath("test.vert", "test.frag");

        return true;
    }

    void GL::Shutdown()
    {
        shader.Destroy();

        m_LayoutStandard.Destroy();
        m_LayoutAnimated.Destroy();

        m_CmdListStatic.Destroy();
        m_CmdListDynamic.Destroy();

        m_UniformCommon.Destroy();

        m_SBMaterialInfos.Destroy();
        m_SBTextureHandles.Destroy();

        m_ILStandard.Destroy();

        m_TextureWhite.Destroy();
        m_TextureBlack.Destroy();
        m_TextureError.Destroy();
        for (auto& tex : m_Textures) {
            tex.Destroy();
        }
        m_Textures.clear();

        m_Models.clear();
        m_Materials.clear();

        SDL_GL_DestroyContext(m_GLContext);
    }

    void GL::Present()
    {
        m_MaterialError.Use();
        m_MaterialWhite.Use();

        static std::vector<InstanceStandard> mapInstances;

        if (mapInstances.size() == 0) {
            for (const auto& model : m_Models) {
                for (const auto& mesh : model.GetMeshes()) {
                    mapInstances.push_back({
                            {
                                Random::Float(0.2f, 1.0f),
                                Random::Float(0.2f, 1.0f),
                                Random::Float(0.2f, 1.0f),
                                1.0f
                            },
                            glm::mat4(1.0f),
                            Random::Int(0, 1) == 0 ? m_MaterialError.GetBTIndex() : m_MaterialWhite.GetBTIndex()
                        }
                    );
                }
            }
        }

        for (const auto& model : m_Models) {
            for (UInt32 i = 0; i < model.GetMeshes().size(); i++) {
                m_CmdListDynamic.Submit(model.GetMeshes()[i], shader, m_ILStandard.Add(&mapInstances[i]));
            }
        }

        m_CommonData.curtime = Globals.curtime;
        m_CommonData.frametime = Globals.frametime;
        m_CommonData.width = Globals.width;
        m_CommonData.height = Globals.height;
        Debug::FlyCam(m_CommonData.view, m_CommonData.projection);

        m_UniformCommon.Update(&m_CommonData, sizeof(m_CommonData), 0);

        m_SBMaterialInfos.Update(m_MaterialInfos.data(), m_MaterialInfos.size() * sizeof(GLMaterial::Info), 0);
        m_SBTextureHandles.Update(m_TextureHandles.data(), m_TextureHandles.size() * sizeof(UInt64), 0);

        m_ILStandard.Upload();

        glClearColor(100.0f / 255.0f, 200.0f / 255.0f, 225.0f / 255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_CmdListStatic.Draw();

        m_CmdListDynamic.Draw();
        m_CmdListDynamic.Flush();
        m_ILStandard.Flush();

        SDL_GL_SwapWindow((SDL_Window*)Window::GetHandle());

        for (GLMaterial& mat : m_Materials) {
            mat.Reset();
        }

        for (GLTexture& tex : m_Textures) {
            tex.Reset();
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

    UInt32 GL::UseTexture(const GLTexture& texture)
    {
        UInt32 btindex = s_GL->m_TextureHandles.size();
        s_GL->m_TextureHandles.push_back(texture.GetBTHandle());

        return btindex;
    }

    UInt32 GL::UseMaterialInfo(const GLMaterial::Info& info)
    {
        UInt32 btindex = s_GL->m_MaterialInfos.size();
        s_GL->m_MaterialInfos.push_back(info);

        return btindex;
    }

    GLLayout* GL::GetLayout(GLLayout::Type type)
    {
        switch (type) {
        case GLLayout::TYPE_STANDARD: return &m_LayoutStandard;
        case GLLayout::TYPE_ANIMATED: return &m_LayoutAnimated;
        default: return nullptr;
        }
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