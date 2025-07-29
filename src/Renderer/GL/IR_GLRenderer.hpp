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
            SSLOC_MATERIALINFO,
            SSLOC_TEXTUREHANDLE,
            SSLOC_ILSTANDARD,
            SSLOC_ILMAP,
        };

        UInt32 UseTexture(const GLTexture& texture);
        UInt32 UseMaterialInfo(const GLMaterial::Info& info);

        GLLayout* GetLayout(GLLayout::Type type);

    private:
        bool m_InitialPrepare = false;

        // Vars
        std::forward_list<GLModel> m_Models;
        std::forward_list<GLMaterial> m_Materials;
        std::forward_list<GLTexture> m_Textures;
        std::forward_list<GLShader> m_Shaders;
        std::forward_list<GLMesh> m_Meshes;

        std::vector<GLMaterial::Info> m_MaterialInfos;
        std::vector<UInt64> m_TextureHandles;

        // --- [VERTEX LAYOUTS] ---
        GLLayout m_LayoutStandard;
        GLLayout m_LayoutAnimated;

        // --- [COMMAND LISTS] ---
        GLCmdList m_CmdListStatic;
        GLCmdList m_CmdListDynamic;

        // --- [UNIFORMS] ---
        struct {
            UInt32 width;
            UInt32 height;
            Float32 curtime;
            Float32 frametime;
            glm::mat4 view;
            glm::mat4 projection;
            Float32 near;
            Float32 far;
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

        // --- [TEXTURES] ---
        GLTexture m_TextureWhite;
        GLTexture m_TextureBlack;
        GLTexture m_TextureError;

        // --- [MESHES] ---
        GLMesh m_MeshCube;
        GLMesh m_MeshPlane;

        // --- [MATERIALS] ---
        GLMaterial m_MaterialWhite;
        GLMaterial m_MaterialBlack;
        GLMaterial m_MaterialError;

        SDL_GLContext m_GLContext;
    };

    extern GL* s_GL;
}