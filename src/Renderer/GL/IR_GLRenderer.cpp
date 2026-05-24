#include <Renderer/GL/IR_GLRenderer.hpp>
#include <Renderer/GL/IR_GLDefaultMesh.hpp>
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
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/easing.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tracy/Tracy.hpp>
#include <tracy/TracyOpenGL.hpp>

void glMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);

namespace IR::Renderer {

    const char* GL::GetName() IRX_RETURN("OpenGL")
    const char* GL::GetDirectory() IRX_RETURN("gl/")

    bool GL::Init()
    {
        s_GL = this; // Maybe not the best way to do this but it works perfectly fine.

        stbi_set_flip_vertically_on_load(true);

        m_GLContext = SDL_GL_CreateContext((SDL_Window*)Window::GetHandle());
        SDL_GL_MakeCurrent((SDL_Window*)Window::GetHandle(), m_GLContext);

        SDL_GL_SetSwapInterval(0);

        GLenum gcode = glewInit();

        if (gcode != GLEW_OK) {
            IRX_MSG(ERROR, "Failed to init GL Renderer: failed to load OpenGL \"%s\"", glewGetErrorString(gcode));
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

            IRX_MSG(ERROR, "%s\n"
                "    [Windows] Try using the DirectX Renderer.\n"
                "    [Linux] Try using the DirectX Renderer with Wine or Proton.", errMsg.c_str());

            return false;
        }

        // Set up OpenGL
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(glMessageCallback, NULL);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        if (!AddShaderInclude("common.glsl")) {
            return false;
        }

        if (!AddShaderInclude("lighting.glsl")) {
            return false;
        }

        m_Lighting.Init(SSLOC_POINTLIGHTS, SSLOC_SPOTLIGHTS);

        // --- [VERTEX LAYOUTS] ---
        m_LayoutBasic2D.InitBasic2D();
        m_LayoutPosition.InitPosition();
        m_LayoutStandard.InitStandard();
        m_LayoutAnimated.InitAnimated();

        // --- [COMMAND LISTS] ---
        m_CmdsStaticOpaque.Init();
        m_CmdsStaticTrans.Init();
        m_CmdsDynamicOpaque.Init();
        m_CmdsDynamicTrans.Init();

        // --- [UNIFORMS] ---
        m_UniformCommon.Init(nullptr, sizeof(m_CommonData), UNIFORMLOC_COMMON);

        SSAOSamplesData ssaosamples;
        for (UInt32 i = 0; i < SSAO_SAMPLE_NUM; i++) {
            glm::vec3 sample = {
                Random::Float(0.0f, 1.0f) * 2.0f - 1.0f,
                Random::Float(0.0f, 1.0f) * 2.0f - 1.0f,
                Random::Float(0.0f, 1.0f)
            };

            sample = glm::normalize(sample);
            sample *= Random::Float(0.0f, 1.0f);
            Float32 scale = (Float32)i / SSAO_SAMPLE_NUM;

            scale = Math::Lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssaosamples.samples[i] = { sample.x, sample.y, sample.z, 0.0f };
        }

        m_UniformSSAOSamples.Init(&ssaosamples, sizeof(ssaosamples), UNIFORMLOC_SSAOSAMPLES);

        // --- [BUFFERS] ---
        m_SBMaterialInfos.Init(GL_SHADER_STORAGE_BUFFER, nullptr, sizeof(GLMaterial::Info), SSLOC_MATERIALINFO, true);
        m_SBTextureHandles.Init(GL_SHADER_STORAGE_BUFFER, nullptr, sizeof(UInt64), SSLOC_TEXTUREHANDLE, true);

        // --- [INSTANCE LISTS] ---
        m_ILDStandard.Init(sizeof(GLInstanceStandard), SSLOC_ILSTANDARD);
        m_ILSMap.Init(sizeof(GLInstanceMap), SSLOC_ILMAP);

        // --- [FRAMEBUFFERS] ---
        m_FrameMain.Init(Globals.Width(), Globals.Height(), 0, {
            { GL_RGB16F, GL_FLOAT }, // Pos
            { GL_RGB16F, GL_FLOAT }, // Normal
            { GL_RGB8, GL_UNSIGNED_BYTE }, // Color
            { GL_RGBA8, GL_UNSIGNED_BYTE }, // AMRE
            { GL_RG8, GL_UNSIGNED_BYTE }, // SSAO Color + Blur
            { GL_RGBA16F, GL_FLOAT }, // Transparent Colors
            { GL_R16F, GL_FLOAT }, // Transparent Reveal
            { GL_RGB16F, GL_FLOAT } // Screen Color
        }, { GL_DEPTH_COMPONENT24, GL_FLOAT });

        // --- [TEXTURES] ---
        const struct glm::vec<3, UInt8> missingColor1 = { 0, 0, 0 };

        const struct glm::vec<3, UInt8> missingColor2 = { 251, 62, 249 };
        const struct glm::vec<3, UInt8> whiteColor = { 255, 255, 255 };
        const struct glm::vec<3, UInt8> normalColor = { 127, 127, 255 };
        const struct glm::vec<3, UInt8> SDEColor = { 0, 255, 0 };

        std::vector<glm::vec<3, UInt8>> missingPixels;
        missingPixels.reserve(64 * 64);

        for (UInt32 y = 0; y < 64; y++) {
            for (UInt32 x = 0; x < 64; x++) {
                if ((x / 16 + y / 16) % 2) missingPixels.push_back(missingColor1);
                else missingPixels.push_back(missingColor2);
            }
        }

        const UInt32 ssaoNoiseSqr = SSAO_NOISE_LEN * SSAO_NOISE_LEN;
        glm::vec3 noiseData[ssaoNoiseSqr];
        for (UInt32 i = 0; i < ssaoNoiseSqr; i++) {
            glm::vec3 noise = {
                Random::Float(0.0f, 1.0f) * 2.0 - 1.0,
                Random::Float(0.0f, 1.0f) * 2.0 - 1.0,
                0.0f
            };
            noiseData[i] = noise;
        }

        m_TextureError.InitMemory((const UInt8*)missingPixels.data(), 64, 64, 3, false, false, true);
        m_TextureBlack.InitMemory((const UInt8*)&missingColor1, 1, 1, 3, false, false, true);
        m_TextureWhite.InitMemory((const UInt8*)&whiteColor, 1, 1, 3, false, false, true);
        m_TextureNormal.InitMemory((const UInt8*)&normalColor, 1, 1, 3, false, false, true);
        m_TextureSDE.InitMemory((const UInt8*)&SDEColor, 1, 1, 3, false, false, true);
        m_TextureSSAONoise.InitMemory(GL_RGB16F, GL_RGB, GL_FLOAT, (const UInt8*)noiseData, SSAO_NOISE_LEN, SSAO_NOISE_LEN, 3, false, false, false);

        // --- [SHADERS] ---
        m_ShaderMapOpaque = (GLShader*)Assets::Shader("MapOpaque.irs");
        m_ShaderMapTrans = (GLShader*)Assets::Shader("MapTrans.irs");
        m_ShaderSky = (GLShader*)Assets::Shader("Sky.irs");
        m_ShaderCompositeSSAO = (GLShader*)Assets::Shader("CompositeSSAO.irs");
        m_ShaderCompositePBR = (GLShader*)Assets::Shader("CompositePBR.irs");
        m_ShaderCompositeTrans = (GLShader*)Assets::Shader("CompositeTrans.irs");
        m_ShaderCompositeFinal = (GLShader*)Assets::Shader("CompositeFinal.irs");

        if (
            !m_ShaderMapOpaque ||
            !m_ShaderMapTrans ||
            !m_ShaderSky ||
            !m_ShaderCompositeSSAO ||
            !m_ShaderCompositePBR ||
            !m_ShaderCompositeTrans ||
            !m_ShaderCompositeFinal
        ) {
            IRX_MSG(ERROR, "Failed to init GL Renderer: couldn't load internal shaders");
            return false;
        }

        // --- [MESHES] ---
        const VertexStandard planeVerts[] = {
            { { -1.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f } },
            { { 1.0f, 0.0f, -1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f } },
            { { -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
        };

        const VertexBasic2D scrVerts[] = {
            { { 0.0f, 0.0f }, { 0.0f, 0.0f } },
            { { 1.0f, 0.0f }, { 1.0f, 0.0f } },
            { { 1.0f, 1.0f }, { 1.0f, 1.0f } },
            { { 0.0f, 1.0f }, { 0.0f, 1.0f } },
        };

        const UInt32 planeIndices[] = { 2, 1, 0, 0, 3, 2 };
        const UInt32 scrIndices[] = { 0, 1, 2, 0, 2, 3 };

        UInt32 skyboxIndices[IRX_ARRLEN(SKYBOX_VERTICES)];
        for (UInt32 i = 0; i < IRX_ARRLEN(SKYBOX_VERTICES); i++) {
            skyboxIndices[i] = i;
        }

        m_MeshCube.InitPool(CUBE_VERTS, IRX_ARRLEN(CUBE_VERTS), CUBE_INDICES, IRX_ARRLEN(CUBE_INDICES));
        m_MeshPlane.InitPool(planeVerts, IRX_ARRLEN(planeVerts), planeIndices, IRX_ARRLEN(planeIndices));
        m_MeshScreen.Init(scrVerts, IRX_ARRLEN(scrVerts), scrIndices, IRX_ARRLEN(scrIndices));
        m_MeshSkybox.Init(SKYBOX_VERTICES, IRX_ARRLEN(SKYBOX_VERTICES), skyboxIndices, IRX_ARRLEN(SKYBOX_VERTICES));

        // --- [MATERIALS] ---

        TracyGpuContext;

        return true;
    }

    void GL::Shutdown()
    {
        m_LayoutBasic2D.Destroy();
        m_LayoutPosition.Destroy();
        m_LayoutStandard.Destroy();
        m_LayoutAnimated.Destroy();

        m_CmdsStaticOpaque.Destroy();
        m_CmdsStaticTrans.Destroy();
        m_CmdsDynamicOpaque.Destroy();
        m_CmdsDynamicTrans.Destroy();

        m_UniformCommon.Destroy();
        m_UniformSSAOSamples.Destroy();

        m_SBMaterialInfos.Destroy();
        m_SBTextureHandles.Destroy();

        m_ILDStandard.Destroy();
        m_ILSMap.Destroy();

        m_FrameMain.Destroy();

        for (auto& shader : m_Shaders) {
            shader.Destroy();
        }
        m_Shaders.clear();

        m_TextureWhite.Destroy();
        m_TextureBlack.Destroy();
        m_TextureError.Destroy();
        m_TextureNormal.Destroy();
        m_TextureSSAONoise.Destroy();
        for (auto& texture : m_Textures) {
            texture.Destroy();
        }
        m_Textures.clear();

        m_MeshCube.Destroy();
        m_MeshPlane.Destroy();
        m_MeshScreen.Destroy();
        m_MeshSkybox.Destroy();
        for (auto& mesh : m_Meshes) {
            mesh.Destroy();
        }
        m_Meshes.clear();

        // These three don't actually have a destroy func
        m_Models.clear();
        m_Materials.clear();

        m_Lighting.Destroy();

        SDL_GL_DestroyContext(m_GLContext);
    }

    void GL::Resize(UInt32 width, UInt32 height)
    {
        m_FrameMain.Resize(width, height);
    }

    void GL::Present()
    {
            IR_ZONE_NAME("OpenGL Renderer Present");

        if (!m_InitialPrepare) {
            m_ILSMap.Upload();
            m_InitialPrepare = true;
        }

        m_CommonData.curtime = Globals.Curtime();
        m_CommonData.frametime = Globals.Frametime();
        m_CommonData.width = Globals.Width();
        m_CommonData.height = Globals.Height();
        m_CommonData.ortho = glm::ortho(0.0f, (Float32)Globals.Width(), 0.0f, (Float32)Globals.Height(), 0.0f, 1.0f);
        m_CommonData.viewPos = Debug::FlyCam(m_CommonData.view, m_CommonData.perspective);

        m_CommonData.skyHorizonHeight = -0.25f;
        m_CommonData.skyHorizon = Color(50, 33, 33, 255);
        m_CommonData.skyPaint = Color(75, 100, 200, 0);

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
            clearColor.r = (colorValue & 0xFF) / 255.0f;         // Blue
            clearColor.a = 1.0f; // Alpha
        }

        m_FrameMain.Bind();
        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDepthMask(GL_TRUE);

        {
            TracyGpuZone("Clear Color");
            glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }


        // - Opaque
        {
            TracyGpuZone("Draw Static Opaque Objects");
            m_CmdsStaticOpaque.Draw();
        }
        {
            TracyGpuZone("Draw Dynamic Opaque Objects");
            m_CmdsDynamicOpaque.Draw();
            m_CmdsDynamicOpaque.Flush();
        }


        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        Assets::Material("testsky.shader")->Bind();
        m_MeshSkybox.Draw();
        glEnable(GL_CULL_FACE);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        glEnable(GL_BLEND);
        glBlendFunciARB(5, GL_ONE, GL_ONE);
        glBlendFunciARB(6, GL_ZERO, GL_ONE_MINUS_SRC_COLOR);
        glBlendEquation(GL_FUNC_ADD);
        glDepthMask(GL_FALSE);

        m_FrameMain.ClearColor(5, Color(0));
        m_FrameMain.ClearColor(6, Color(255));

        // - Transparent
        {
            TracyGpuZone("Draw Static Transparent Objects");
            m_CmdsStaticTrans.Draw();
        }
        {
            TracyGpuZone("Draw Dynamic Transparent Objects");
            m_CmdsDynamicTrans.Draw();
            m_CmdsDynamicTrans.Flush();
        }

        {
            TracyGpuZone("Flush Dynamic Instance Data");
            m_ILDStandard.Flush();
        }

        glDisable(GL_DEPTH_TEST);

        {
            TracyGpuZone("Compose PBR + Forward WBOIT");

            m_FrameMain.GetColorTexture(0)->Bind(0);
            m_FrameMain.GetColorTexture(1)->Bind(1);
            m_FrameMain.GetColorTexture(2)->Bind(2);
            m_FrameMain.GetColorTexture(3)->Bind(3);
            m_FrameMain.GetColorTexture(4)->Bind(4);
            m_FrameMain.GetColorTexture(5)->Bind(5);
            m_FrameMain.GetColorTexture(6)->Bind(6);
            m_FrameMain.GetColorTexture(7)->Bind(7);

            glDisable(GL_BLEND);

            m_TextureSSAONoise.Bind(16);
            m_ShaderCompositeSSAO->Bind();
            m_MeshScreen.Draw();

            m_ShaderCompositePBR->Bind();
            m_MeshScreen.Draw();

            glDepthFunc(GL_ALWAYS);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            m_ShaderCompositeTrans->Bind();
            m_MeshScreen.Draw();
            m_FrameMain.UnBind();

            m_FrameMain.GetColorTexture(7)->Bind(0);

            m_ShaderCompositeFinal->Bind();
            m_MeshScreen.Draw();
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
            GLCmdList* cmdlist = (bool)material->GetKVNumber("Transparent", 0) ?
                &m_CmdsDynamicTrans :
                &m_CmdsDynamicOpaque;

            material->Use();
            inst.matIndex = material->GetInfoIndex();
            cmdlist->Submit(&mesh, material->GetShader(), m_ILDStandard.Add(&inst));
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

        GLCmdList* cmdlist = (bool)glmaterial->GetKVNumber("Transparent", 0) ?
            &m_CmdsDynamicTrans :
            &m_CmdsDynamicOpaque;

        cmdlist->Submit(glmesh, glmaterial->GetShader(), m_ILDStandard.Add(&inst));
    }

    void GL::SubmitMapMesh(const Mesh* mesh, const Material* material)
    {
        GLMesh* glmesh = (GLMesh*)mesh;
        GLMaterial* glmaterial = (GLMaterial*)material;

        glmaterial->Use();

        GLInstanceMap inst;
        inst.matIndex = glmaterial->GetInfoIndex();

        GLCmdList* cmdlist = (bool)glmaterial->GetKVNumber("Transparent", 0) ?
            &m_CmdsStaticTrans :
            &m_CmdsStaticOpaque;

        cmdlist->Submit(glmesh, glmaterial->GetShader(), m_ILSMap.Add(&inst));
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

    UInt16 GL::MakePLight() IRX_RETURN(m_Lighting.MakePLight())
    void GL::RemovePLight(UInt16 index) { m_Lighting.RemovePLight(index); }
    void GL::SetPLightPosition(UInt16 index, const glm::vec3& pos) { m_Lighting.SetPLightPosition(index, pos); }
    void GL::SetPLightColor(UInt16 index, const Color& col) { m_Lighting.SetPLightColor(index, col); }
    void GL::SetPLightInnerRadius(UInt16 index, const Float32 radius) { m_Lighting.SetPLightInnerRadius(index, radius); }
    void GL::SetPLightOuterRadius(UInt16 index, const Float32 radius) { m_Lighting.SetPLightOuterRadius(index, radius); }

    UInt16 GL::MakeSLight() IRX_RETURN(m_Lighting.MakeSLight())
    void GL::RemoveSLight(UInt16 index) { m_Lighting.RemoveSLight(index); }
    void GL::SetSLightPosition(UInt16 index, const glm::vec3& pos) { m_Lighting.SetSLightPosition(index, pos); }
    void GL::SetSLightDirection(UInt16 index, const glm::vec3& dir) { m_Lighting.SetSLightDirection(index, dir); }
    void GL::SetSLightColor(UInt16 index, const Color& col) { m_Lighting.SetSLightColor(index, col); }
    void GL::SetSLightInnerRadius(UInt16 index, const Float32 radius) { m_Lighting.SetSLightInnerRadius(index, radius); }
    void GL::SetSLightOuterRadius(UInt16 index, const Float32 radius) { m_Lighting.SetSLightOuterRadius(index, radius); }
    void GL::SetSLightInnerCutoff(UInt16 index, const Float32 deg) { m_Lighting.SetSLightInnerCutoff(index, deg); }
    void GL::SetSLightOuterCutoff(UInt16 index, const Float32 deg) { m_Lighting.SetSLightOuterCutoff(index, deg); }

    Texture* GL::GetTextureWhite() IRX_RETURN(&m_TextureWhite);
    Texture* GL::GetTextureBlack() IRX_RETURN(&m_TextureBlack);
    Texture* GL::GetTextureError() IRX_RETURN(&m_TextureError);
    Texture* GL::GetTextureNormal() IRX_RETURN(&m_TextureNormal);
    Texture* GL::GetTextureSDE() IRX_RETURN(&m_TextureSDE);

    Mesh* GL::GetMeshCube() IRX_RETURN(&m_MeshCube);

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
            IRX_MSG(ERROR, "Failed to init GL Renderer: couldn't open include %s", name);
            return false;
        }

        UInt64 inclSize;
        char* inclData = inclFile.ReadAll(&inclSize);
        IRX_DEFER({ if (inclData) delete[] inclData; });

        glNamedStringARB(GL_SHADER_INCLUDE_ARB, aName.size() + 1, ("/" + aName).c_str(), inclSize, inclData);
        m_PatchIncludes.push_back(name);

        return true;
    }

    GLLayout* GL::GetLayout(GLLayout::Type type)
    {
        switch (type) {
        case GLLayout::Type::BASIC2D: return &m_LayoutBasic2D;
        case GLLayout::Type::POSITION: return &m_LayoutPosition;
        case GLLayout::Type::STANDARD: return &m_LayoutStandard;
        case GLLayout::Type::ANIMATED: return &m_LayoutAnimated;
        default: IRX_UNREACHABLE;
        }
    }

    GLTexture* GL::GetDefaultTexture(Material::Map map)
    {
        switch (map) {
        case Material::Map::MAP_ALBEDO: return &m_TextureError;
        case Material::Map::MAP_NORMAL: return &m_TextureNormal;
        case Material::Map::MAP_SDE: return &m_TextureSDE;
        default: IRX_UNREACHABLE;
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
        IRX_MSG(INFO, "GL MSG %i | [%s] [%s] :: %s", id, sourceStr, typeStr, message);
        break;
    case GL_DEBUG_SEVERITY_MEDIUM:
        IRX_MSG(WARN, "GL MSG %i | [%s] [%s] :: %s", id, sourceStr, typeStr, message);
        break;
    case GL_DEBUG_SEVERITY_HIGH:
        IRX_MSG(ERROR, "GL MSG %i | [%s] [%s] :: %s", id, sourceStr, typeStr, message);
        break;
    }
}
