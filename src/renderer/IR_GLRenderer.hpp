#pragma once

#include <IR_Renderer.hpp>
#include <renderer/IR_GLShader.hpp>
#include <renderer/IR_GLTexture.hpp>
#include <renderer/IR_GLMaterial.hpp>
#include <renderer/IR_GLLayout.hpp>
#include <renderer/IR_GLMesh.hpp>
#include <renderer/IR_GLModel.hpp>
#include <renderer/IR_GLUniform.hpp>
#include <renderer/IR_GLShaderStorage.hpp>

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
        };

        struct MaterialInfo {
            UInt32 handleIndexes[Material::MAP__COUNT];
        };

        GLTexture* MakeTexture();

        // Vars
        std::forward_list<GLModel> m_Models;
        std::forward_list<GLMaterial> m_Materials;
        std::forward_list<GLTexture> m_Textures;

        std::vector<MaterialInfo> m_MaterialInfos;
        std::vector<UInt64> m_TextureHandles;

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

        GLShaderStorage m_SSMaterialInfos;
        GLShaderStorage m_SSTextureHandles;

        GLLayout m_LayoutStandard;
        GLLayout m_LayoutAnimated;

        GLTexture m_TextureWhite;
        GLTexture m_TextureBlack;
        GLTexture m_TextureError;

        GLMaterial m_MaterialError;

        SDL_GLContext m_GLContext;
    };

    extern GL* s_GL;
}