#include <Renderer/GL/IR_GLRenderer.hpp>
#include <IR_Common.hpp>
#include <IR_Random.hpp>
#include <IR_Window.hpp>
#include <IR_Debug.hpp>
#include <IR_Assets.hpp>
#include <IR_File.hpp>
#include <IR_CVar.hpp>
#include <IR_Tracy.hpp>

#include <GL/glew.h>
#include <Thirdparty/stb_image.h>

#include <glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tracy/Tracy.hpp>
#include <tracy/TracyOpenGL.hpp>

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

        if (!AddShaderInclude("common.glsl")) {
            return false;
        }

        if (!AddShaderInclude("lighting.glsl")) {
            return false;
        }

        m_Lighting.Init(SSLOC_POINTLIGHTS, SSLOC_SPOTLIGHTS);

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
        const struct glm::vec<3, UInt8> normalColor = { 127, 127, 255 };

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
        m_TextureNormal.InitMemory((const UInt8*)&normalColor, 1, 1, 3, false, false, true);

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

		TracyGpuContext;

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
        m_TextureNormal.Destroy();
        for (auto& texture : m_Textures) {
            texture.Destroy();
        }
        m_Textures.clear();

        // These three don't actually have a destroy func
        m_Meshes.clear();
        m_Models.clear();
        m_Materials.clear();

        m_Lighting.Destroy();

        SDL_GL_DestroyContext(m_GLContext);
    }

	void GL::Resize(UInt32 width, UInt32 height)
	{
		glViewport(0, 0, width, height);
	}

    void GL::Present()
    {
		IR_ZONE_NAME("OpenGL Renderer Present");

        if (!m_InitialPrepare) {
            m_ILSMap.Upload();
            m_InitialPrepare = true;
        }

        m_CommonData.curtime = Globals.curtime;
        m_CommonData.frametime = Globals.frametime;
        m_CommonData.width = Globals.width;
        m_CommonData.height = Globals.height;
        Debug::FlyCam(m_CommonData.view, m_CommonData.projection, m_CommonData.viewPos);

		{
			TracyGpuZone("Update Common Data");
			m_UniformCommon.Update(&m_CommonData, sizeof(m_CommonData), 0);
		}

		{
			TracyGpuZone("Update Material Infos");
			m_SBMaterialInfos.Update(m_MaterialInfos.data(), m_MaterialInfos.size() * sizeof(GLMaterial::Info), 0);
		}

        {
			TracyGpuZone("Update Texture Handles");
			m_SBTextureHandles.Update(m_TextureHandles.data(), m_TextureHandles.size() * sizeof(UInt64), 0);
		}

		{
			TracyGpuZone("Update Instance Data");
			m_ILDStandard.Upload();
		}

		{
			TracyGpuZone("Update Lighting Data");
			m_Lighting.Upload();
		}

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

		{
			TracyGpuZone("Clear Color");
			glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		{
			TracyGpuZone("Draw Static Command Lists");
			// - Static
			m_CmdListStatic.Draw();
		}

        // - Dynamic
		{
			TracyGpuZone("Draw Dynamic Command Lists");
			m_CmdListDynamic.Draw();
			m_CmdListDynamic.Flush();
		}

		{
			TracyGpuZone("Flush Dynamic Instance Data");
			m_ILDStandard.Flush();
		}

		{
			TracyGpuZone("Swap Buffers");
			SDL_GL_SwapWindow((SDL_Window*)Window::GetHandle());
		}

		TracyGpuCollect;
    }

    void GL::SubmitModel(const Model* model, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& size, const Color& col, UInt8 skin)
    {
        GLModel* glmodel = (GLModel*)model;

        GLInstanceStandard inst;
        inst.model = glm::translate(glm::mat4(1.0f), pos) * glm::mat4(rot) * glm::scale(glm::mat4(1.0f), size);
        inst.color.r = col.r;
        inst.color.g = col.g;
        inst.color.b = col.b;
        inst.color.a = col.a;

        for (UInt32 i = 0; i < glmodel->GetMeshNum(); i++) {
            GLMesh& mesh = glmodel->GetMesh(i);
            GLMaterial* material = glmodel->GetMeshMaterial(i, skin);

            material->Use();
            inst.matIndex = material->GetInfoIndex();
            m_CmdListDynamic.Submit(&mesh, material->GetShader(), m_ILDStandard.Add(&inst));
        }
    }

    void GL::SubmitMesh(const Mesh* mesh, const glm::vec3& pos, const glm::quat& rot, const glm::vec3& size, const Color& col, const Material* material)
    {
        GLMesh* glmesh = (GLMesh*)mesh;
        GLMaterial* glmaterial = (GLMaterial*)material;

        glmaterial->Use();

        GLInstanceStandard inst;
        inst.model = glm::translate(glm::mat4(1.0f), pos) * glm::mat4_cast(rot) * glm::scale(glm::mat4(1.0f), size);
        inst.color.r = col.r;
        inst.color.g = col.g;
        inst.color.b = col.b;
        inst.color.a = col.a;
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

    UInt16 GL::MakePLight() IR_RETURN(m_Lighting.MakePLight())
    void GL::RemovePLight(UInt16 index) { m_Lighting.RemovePLight(index); }
    void GL::SetPLightPosition(UInt16 index, const glm::vec3& pos) { m_Lighting.SetPLightPosition(index, pos); }
    void GL::SetPLightColor(UInt16 index, const Color& col) { m_Lighting.SetPLightColor(index, col); }
    void GL::SetPLightInnerRadius(UInt16 index, const Float32 radius) { m_Lighting.SetPLightInnerRadius(index, radius); }
    void GL::SetPLightOuterRadius(UInt16 index, const Float32 radius) { m_Lighting.SetPLightOuterRadius(index, radius); }

    UInt16 GL::MakeSLight() IR_RETURN(m_Lighting.MakeSLight())
    void GL::RemoveSLight(UInt16 index) { m_Lighting.RemoveSLight(index); }
    void GL::SetSLightPosition(UInt16 index, const glm::vec3& pos) { m_Lighting.SetSLightPosition(index, pos); }
    void GL::SetSLightDirection(UInt16 index, const glm::vec3& dir) { m_Lighting.SetSLightDirection(index, dir); }
    void GL::SetSLightColor(UInt16 index, const Color& col) { m_Lighting.SetSLightColor(index, col); }
    void GL::SetSLightInnerRadius(UInt16 index, const Float32 radius) { m_Lighting.SetSLightInnerRadius(index, radius); }
    void GL::SetSLightOuterRadius(UInt16 index, const Float32 radius) { m_Lighting.SetSLightOuterRadius(index, radius); }
    void GL::SetSLightInnerCutoff(UInt16 index, const Float32 deg) { m_Lighting.SetSLightInnerCutoff(index, deg); }
    void GL::SetSLightOuterCutoff(UInt16 index, const Float32 deg) { m_Lighting.SetSLightOuterCutoff(index, deg); }

    Texture* GL::GetTextureWhite() IR_RETURN(&m_TextureWhite);
    Texture* GL::GetTextureBlack() IR_RETURN(&m_TextureBlack);
    Texture* GL::GetTextureError() IR_RETURN(&m_TextureError);
    Texture* GL::GetTextureNormal() IR_RETURN(&m_TextureNormal);

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

    bool GL::AddShaderInclude(const char* name)
    {
        std::string aName = name;

        File inclFile(("assets/shaders/gl/include/" + aName).c_str(), "r");
        if (!inclFile.IsOpen()) {
            IR_MSG(ERROR, "Failed to init GL Renderer: couldn't open include %s", name);
            return false;
        }

        UInt64 inclSize;
        char* inclData = inclFile.ReadAll(&inclSize);
        IR_DEFER({ if (inclData) delete[] inclData; });

        glNamedStringARB(GL_SHADER_INCLUDE_ARB, aName.size() + 1, ("/" + aName).c_str(), inclSize, inclData);
        m_PatchIncludes.push_back(name);

        return true;
    }

    GLLayout* GL::GetLayout(GLLayout::Type type)
    {
        switch (type) {
        case GLLayout::Type::STANDARD: return &m_LayoutStandard;
        case GLLayout::Type::ANIMATED: return &m_LayoutAnimated;
        default: IR_UNREACHABLE;
        }
    }

    GLTexture* GL::GetDefaultTexture(Material::Map map)
    {
        switch (map) {
        case Material::Map::MAP_ALBEDO: return &m_TextureError;
        case Material::Map::MAP_NORMAL: return &m_TextureNormal;
        case Material::Map::MAP_METALNESS: return &m_TextureBlack;
        case Material::Map::MAP_ROUGHNESS: return &m_TextureBlack;
        case Material::Map::MAP_EMISSIVENESS: return &m_TextureBlack;
        case Material::Map::MAP_AMBIENTOCCLUSION: return &m_TextureWhite;
        default: IR_UNREACHABLE;
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
