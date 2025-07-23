#pragma once

#include <IR_Renderer.hpp>
#include <Renderer/IR_GLShader.hpp>
#include <Renderer/IR_GLTexture.hpp>
#include <Renderer/IR_GLMaterial.hpp>
#include <Renderer/IR_GLLayout.hpp>
#include <Renderer/IR_GLMesh.hpp>
#include <Renderer/IR_GLModel.hpp>
#include <Renderer/IR_GLUniform.hpp>
#include <Renderer/IR_GLBuffer.hpp>
#include <Renderer/IR_GLMeshPool.hpp>
#include <Renderer/IR_GLCmdList.hpp>
#include <Renderer/IR_GLInstance.hpp>

#include <SDL3/SDL.h>

#include <forward_list>

namespace IR::Renderer {
    struct GL : public APIHandle {
        IR_RENDERER_FUNCS(, override)

        enum UniformLocation {
            UNIFORMLOC_COMMON,
            UNIFORMLOC_ANIMATION,
        };

        enum ShaderStorageLocation {
            SSLOC_MATERIALINFO,
            SSLOC_TEXTUREHANDLE,
            SSLOC_ILSTANDARD,
        };

        struct MaterialInfo {
            UInt32 handleIndexes[Material::MAP__COUNT];
        };

        GLTexture* MakeTexture();

        GLLayout* GetLayout(GLLayout::Type type);

        // Vars
        std::forward_list<GLModel> m_Models;
        std::forward_list<GLMaterial> m_Materials;
        std::forward_list<GLTexture> m_Textures;

        std::vector<MaterialInfo> m_MaterialInfos;
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
        GLInstanceList m_ILStandard;

        // --- [TEXTURES] ---
        GLTexture m_TextureWhite;
        GLTexture m_TextureBlack;
        GLTexture m_TextureError;

        // --- [MESHES] ---
        GLMesh m_MeshCube;
        GLMesh m_MeshPlane;

        // --- [MATERIALS] ---
        GLMaterial m_MaterialError;

        SDL_GLContext m_GLContext;
    };

    extern GL* s_GL;
}