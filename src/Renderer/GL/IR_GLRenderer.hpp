#pragma once

#include <IR_Renderer.hpp>
#include <Renderer/GL/IR_GLShader.hpp>
#include <Renderer/GL/IR_GLTexture.hpp>
#include <Renderer/GL/IR_GLMaterial.hpp>
#include <Renderer/GL/IR_GLLayout.hpp>
#include <Renderer/GL/IR_GLMesh.hpp>
#include <Renderer/GL/IR_GLModel.hpp>
#include <Renderer/GL/IR_GLUniform.hpp>
#include <Renderer/GL/IR_GLBuffer.hpp>
#include <Renderer/GL/IR_GLMeshPool.hpp>
#include <Renderer/GL/IR_GLCmdList.hpp>
#include <Renderer/GL/IR_GLInstance.hpp>
#include <Renderer/GL/IR_GLLighting.hpp>
#include <Renderer/GL/IR_GLFrame.hpp>

#include <SDL3/SDL.h>

#include <forward_list>

namespace IR::Renderer {
    class GL : public APIHandle {
    public:
        IR_RENDERER_FUNCS(, override)

        enum UniformLocation {
            UNIFORMLOC_COMMON,
            UNIFORMLOC_ANIMATION,
        };

        enum ShaderStorageLocation {
            SSLOC_MATERIALINFO = 0,
            SSLOC_TEXTUREHANDLE,
            SSLOC_POINTLIGHTS,
            SSLOC_SPOTLIGHTS,
            SSLOC_ILSTANDARD = 16,
            SSLOC_ILMAP,
        };

        UInt32 UseTexture(const GLTexture& texture);
        UInt32 UseMaterialInfo(const GLMaterial::Info& info);
        void RemoveTextureHandle(UInt32 index);
        void RemoveMaterialInfo(UInt32 index);
        bool AddShaderInclude(const char* name);

        const std::vector<std::string>& GetPatchIncludes() const IR_RETURN(m_PatchIncludes)
        GLLayout* GetLayout(GLLayout::Type type);
        GLTexture* GetDefaultTexture(Material::Map map);

    private:
        bool m_InitialPrepare = false;

        std::vector<std::string> m_PatchIncludes;

        // Vars
        std::forward_list<GLModel> m_Models;
        std::forward_list<GLMaterial> m_Materials;
        std::forward_list<GLTexture> m_Textures;
        std::forward_list<GLShader> m_Shaders;
        std::forward_list<GLMesh> m_Meshes;

        std::vector<GLMaterial::Info> m_MaterialInfos;
        std::vector<UInt64> m_TextureHandles;

        GLLighting m_Lighting;

        // --- [VERTEX LAYOUTS] ---
        GLLayout m_LayoutBasic2D;
        GLLayout m_LayoutStandard;
        GLLayout m_LayoutAnimated;

        // --- [COMMAND LISTS] ---
        GLCmdList m_CmdsStaticOpaque;
        GLCmdList m_CmdsStaticTrans;
        GLCmdList m_CmdsDynamicOpaque;
        GLCmdList m_CmdsDynamicTrans;

        // --- [UNIFORMS] ---
        struct CommonData {
            UInt32 width;
            UInt32 height;
            Float32 curtime;
            Float32 frametime;
            glm::mat4 view;
            glm::mat4 projection;
            glm::vec3 viewPos;
        } m_CommonData;
        GLUniform m_UniformCommon;

        // --- [BUFFERS] ---
        GLBuffer m_SBMaterialInfos;
        GLBuffer m_SBTextureHandles;

        // --- [INSTANCE LISTS] ---
        // - Dynamics
        GLInstanceList m_ILDStandard;

        // - Statics
        GLInstanceList m_ILSMap;

        // --- [FRAMEBUFFERS] ---
        GLFrame m_FrameMain;

        // --- [TEXTURES] ---
        GLTexture m_TextureWhite;
        GLTexture m_TextureBlack;
        GLTexture m_TextureError;
        GLTexture m_TextureNormal;

        // --- [SHADERS] ---
        GLShader* m_ShaderOpaqueMap = nullptr;
        GLShader* m_ShaderTransMap = nullptr;
        GLShader* m_ShaderScreen = nullptr;
        GLShader* m_ShaderScreenTrans = nullptr;
        GLShader* m_ShaderScreenFinal = nullptr;

        // --- [MESHES] ---
        GLMesh m_MeshCube;
        GLMesh m_MeshPlane;
        GLMesh m_MeshScreen;

        // --- [MATERIALS] ---

        SDL_GLContext m_GLContext;
    };

    extern GL* s_GL;
}
