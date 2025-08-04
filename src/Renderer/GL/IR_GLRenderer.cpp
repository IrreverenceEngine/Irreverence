#include <Renderer/GL/IR_GLRenderer.hpp>
#include <IR_Common.hpp>
#include <IR_Random.hpp>
#include <IR_Window.hpp>
#include <IR_Debug.hpp>
#include <IR_Assets.hpp>
#include <IR_File.hpp>
#include <IR_CVar.hpp>

#include <GL/glew.h>
#include <Thirdparty/stb_image.h>

#include <glm.hpp>
#include <glm/gtc/type_ptr.hpp>

void glMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

/*
TODO: Fix possible problems mentioned below
Should Renderer be able to use the Asset Manager? If yes then use it to load required Assets, on fail, return false.
    - Solution: Just fuckin use em who cares

If we delete an Texture, how does the other objects know whether it doesnt exist anymore?
    - Solution: ?
*/

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

    const char* GL::GetName() IR_RETURN("OpenGL")
    const char* GL::GetDirectory() IR_RETURN("gl/")

    bool GL::Init()
    {
        s_GL = this; // Maybe not the best way to do this but it works perfectly fine.

        stbi_set_flip_vertically_on_load(true);

        m_GLContext = SDL_GL_CreateContext((SDL_Window*)Window::GetHandle());
        SDL_GL_MakeCurrent((SDL_Window*)Window::GetHandle(), m_GLContext);

		SDL_GL_SetSwapInterval(0);

        GLenum gcode = glewInit();
        if (gcode != GLEW_OK) {
            IR_MSG(ERROR, "Failed to init GL Renderer: failed to load OpenGL \"%s\"", glewGetErrorString(gcode));
            return false;
        }

        bool hasBT = GLEW_ARB_bindless_texture;
        bool hasMDI = GLEW_ARB_multi_draw_indirect;
        bool hasSLI = GLEW_ARB_shading_language_include;
        if (!hasBT || !hasMDI || !hasSLI ) {
            std::string errMsg = "Failed to init GL Renderer: user's hardware doesn't support ";
            if (!hasBT) {
                errMsg += "\"Bindless Textures\" ";
            }

            if (!hasMDI) {
                errMsg += "\"Multi-Draw Indirect\" ";
            }

            if (!hasSLI) {
                errMsg += "\"Shading Language Include\"";
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

        File CommonIncl("assets/shaders/gl/include/common.glsl", "r");
        if (!CommonIncl.IsOpen()) {
            IR_MSG(INFO, "Failed to init GL Renderer: couldn't open include common.glsl");
            return false;
        }

        UInt64 CommonInclSize;
        char* CommonInclData = CommonIncl.ReadAll(&CommonInclSize);
        IR_DEFER({ if (CommonInclData) delete[] CommonInclData; });
        glNamedStringARB(GL_SHADER_INCLUDE_ARB, -1, "/common.glsl", CommonInclSize, CommonInclData);
        m_PatchIncludes.push_back("common.glsl");

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
        m_ILDStandard.Init(sizeof(GLInstanceStandard), SSLOC_ILSTANDARD);
        m_ILSMap.Init(sizeof(GLInstanceMap), SSLOC_ILMAP);

        // --- [TEXTURES] ---
        const struct glm::vec<3, UInt8> missingColor1 = { 0, 0, 0 };
        const struct glm::vec<3, UInt8> missingColor2 = { 251, 62, 249 };
        const struct glm::vec<3, UInt8> whiteColor = { 255, 255, 255 };

        std::vector<glm::vec<3, UInt8>> missingPixels;
        missingPixels.reserve(64 * 64);

        for (UInt32 y = 0; y < 64; y++) {
            for (UInt32 x = 0; x < 64; x++) {
                if ((x / 16 + y / 16) % 2) missingPixels.push_back(missingColor1);
                else missingPixels.push_back(missingColor2);
            }
        }

        m_TextureError.InitMemory((const UInt8*)missingPixels.data(), 64, 64, 3, false, false, true);
        m_TextureBlack.InitMemory((const UInt8*)&missingColor1, 1, 1, 3, false, false, true);
        m_TextureWhite.InitMemory((const UInt8*)&whiteColor, 1, 1, 3, false, false, true);

        // --- [SHADERS] ---
        m_ShaderMapFaceLit = (GLShader*)Assets::Shader("MapFaceLit.irs");

        if (!m_ShaderMapFaceLit) {
            IR_MSG(ERROR, "Failed to init GL Renderer: couldn't load irs file \"MapFaceLit.irs\"");
            return false;
        }

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
        m_MaterialWhite.SetShader(m_ShaderMapFaceLit);
        m_MaterialBlack.SetShader(m_ShaderMapFaceLit);
        m_MaterialError.SetShader(m_ShaderMapFaceLit);

        m_MaterialWhite.AddTexture(Material::MAP_ALBEDO, &m_TextureWhite);
        m_MaterialBlack.AddTexture(Material::MAP_ALBEDO, &m_TextureBlack);
        m_MaterialError.AddTexture(Material::MAP_ALBEDO, &m_TextureError);

        return true;
    }

    void GL::Shutdown()
    {
        m_LayoutStandard.Destroy();
        m_LayoutAnimated.Destroy();

        m_CmdListStatic.Destroy();
        m_CmdListDynamic.Destroy();

        m_UniformCommon.Destroy();

        m_SBMaterialInfos.Destroy();
        m_SBTextureHandles.Destroy();

        m_ILDStandard.Destroy();
        m_ILSMap.Destroy();

        for (auto& shader : m_Shaders) {
            shader.Destroy();
        }
        m_Shaders.clear();

        m_TextureWhite.Destroy();
        m_TextureBlack.Destroy();
        m_TextureError.Destroy();
        for (auto& texture : m_Textures) {
            texture.Destroy();
        }
        m_Textures.clear();

        // These three don't actually have a destroy func
        m_Meshes.clear();
        m_Models.clear();
        m_Materials.clear();

        SDL_GL_DestroyContext(m_GLContext);
    }

	void GL::Resize(UInt32 width, UInt32 height)
	{
		glViewport(0, 0, width, height);
	}

    void GL::Present()
    {
        if (!m_InitialPrepare) {
            m_ILSMap.Upload();
            m_InitialPrepare = true;
        }

        m_CommonData.curtime = Globals.curtime;
        m_CommonData.frametime = Globals.frametime;
        m_CommonData.width = Globals.width;
        m_CommonData.height = Globals.height;
        Debug::FlyCam(m_CommonData.view, m_CommonData.projection);

        m_UniformCommon.Update(&m_CommonData, sizeof(m_CommonData), 0);

        m_SBMaterialInfos.Update(m_MaterialInfos.data(), m_MaterialInfos.size() * sizeof(GLMaterial::Info), 0);
        m_SBTextureHandles.Update(m_TextureHandles.data(), m_TextureHandles.size() * sizeof(UInt64), 0);

        m_ILDStandard.Upload();

        glm::vec4 funSky = {
            abs(sinf(Globals.curtime * 0.5f + 2.0f)) * 0.9 + Random::Float(0.0f, 0.1f),
            abs(sinf(Globals.curtime + 5.0f * 5.0f)) * 0.9 + Random::Float(0.0f, 0.1f),
            abs(sinf(Globals.curtime * 2.0f)) * 0.9 + Random::Float(0.0f, 0.1f),
            1.0f };

		static CVar* r_clear_color = CVar::Get("r_clear_color");
		glm::vec4 clearColor = glm::vec4(0.0f);

		if (r_clear_color) {

			// Color is stored as a 0xBBGGRR format
			Int64 colorValue  = r_clear_color->GetInt64();
			clearColor.b = ((colorValue >> 16) & 0xFF) / 255.0f; // Red
			clearColor.g = ((colorValue >> 8) & 0xFF) / 255.0f;  // Green
			clearColor.r = (colorValue & 0xFF) / 255.0f;           // Blue
			clearColor.a = 1.0f; // Alpha
		}

        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // - Static
        m_CmdListStatic.Draw();

        // - Dynamic
        m_CmdListDynamic.Draw();
        m_CmdListDynamic.Flush();

        m_ILDStandard.Flush();

        SDL_GL_SwapWindow((SDL_Window*)Window::GetHandle());
    }

    void GL::SubmitModel(const Model* model, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& size, const glm::vec4& col, UInt8 skin)
    {
        GLModel* glmodel = (GLModel*)model;

        GLInstanceStandard standard;
        standard.model = glm::translate(glm::mat4(1.0f), pos) * glm::mat4(rot) * glm::scale(glm::mat4(1.0f), size);
        standard.color = col;

        for (UInt32 i = 0; i < glmodel->GetMeshNum(); i++) {
            GLMesh& mesh = glmodel->GetMesh(i);
            GLMaterial* material = glmodel->GetMeshMaterial(i, skin);

            material->Use();
            standard.matIndex = material->GetInfoIndex();
            m_CmdListDynamic.Submit(&mesh, material->GetShader(), m_ILDStandard.Add(&standard));
        }
    }

    void GL::SubmitMesh(const Mesh* mesh, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& size, const glm::vec4& col, const Material* material)
    {
        GLMesh* glmesh = (GLMesh*)mesh;
        GLMaterial* glmaterial = (GLMaterial*)material;

        glmaterial->Use();

        GLInstanceStandard inst;
        inst.model = glm::translate(glm::mat4(1.0f), pos) * glm::mat4_cast(rot) * glm::scale(glm::mat4(1.0f), size);
        inst.color = col;
        inst.matIndex = glmaterial->GetInfoIndex();

        m_CmdListDynamic.Submit(glmesh, glmaterial->GetShader(), m_ILDStandard.Add(&inst));
    }

    void GL::SubmitMapMesh(const Mesh* mesh, const Material* material)
    {
        GLMesh* glmesh = (GLMesh*)mesh;
        GLMaterial* glmaterial = (GLMaterial*)material;

        glmaterial->Use();

        GLInstanceMap inst;
        inst.matIndex = glmaterial->GetInfoIndex();

        m_CmdListStatic.Submit(glmesh, glmaterial->GetShader(), m_ILSMap.Add(&inst));
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

    Texture* GL::MakeTexture()
    {
        m_Textures.push_front({});
        return &m_Textures.front();
    }

    Shader* GL::MakeShader()
    {
        m_Shaders.push_front({});
        return &m_Shaders.front();
    }

    Mesh* GL::MakeMesh()
    {
        m_Meshes.push_front({});
        return &m_Meshes.front();
    }

    Texture* GL::GetTextureWhite() IR_RETURN(&m_TextureWhite);
    Texture* GL::GetTextureBlack() IR_RETURN(&m_TextureBlack);
    Texture* GL::GetTextureError() IR_RETURN(&m_TextureError);

    Material* GL::GetMaterialWhite() IR_RETURN(&m_MaterialWhite);
    Material* GL::GetMaterialBlack() IR_RETURN(&m_MaterialBlack);
    Material* GL::GetMaterialError() IR_RETURN(&m_MaterialError);

    Mesh* GL::GetMeshCube() IR_RETURN(&m_MeshCube);

    UInt32 GL::UseTexture(const GLTexture& texture)
    {
        for (UInt32 i = 0; i < m_TextureHandles.size(); i++) {
            UInt64& mhandle = m_TextureHandles[i];
            if (mhandle == UINT64_MAX) {
                mhandle = texture.GetBTHandle();
                return i;
            }
        }

        UInt32 btindex = m_TextureHandles.size();
        m_TextureHandles.emplace_back(texture.GetBTHandle());

        return btindex;
    }

    UInt32 GL::UseMaterialInfo(const GLMaterial::Info& info)
    {
        for (UInt32 i = 0; i < m_MaterialInfos.size(); i++) {
            GLMaterial::Info& minfo = m_MaterialInfos[i];
            if (minfo.handleIndexes[0] == UINT32_MAX) {
                minfo = info;
                return i;
            }

        }

        UInt32 btindex = m_MaterialInfos.size();
        m_MaterialInfos.emplace_back(info);

        return btindex;
    }

    void GL::RemoveTextureHandle(UInt32 index)
    {
        if (index >= m_TextureHandles.size()) {
            return;
        }

        m_TextureHandles[index] = UINT64_MAX;
    }

    void GL::RemoveMaterialInfo(UInt32 index)
    {
        if (index >= m_MaterialInfos.size()) {
            return;
        }

        m_MaterialInfos[index].handleIndexes[0] = UINT32_MAX;
    }

    GLLayout* GL::GetLayout(GLLayout::Type type)
    {
        switch (type) {
        case GLLayout::Type::STANDARD: return &m_LayoutStandard;
        case GLLayout::Type::ANIMATED: return &m_LayoutAnimated;
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
