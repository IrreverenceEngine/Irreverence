#pragma once

#include <IR_Renderer.hpp>
#include <renderer/IR_GLShader.hpp>
#include <renderer/IR_GLTexture.hpp>
#include <renderer/IR_GLMaterial.hpp>
#include <renderer/IR_GLMesh.hpp>
#include <renderer/IR_GLModel.hpp>
#include <renderer/IR_GLUniform.hpp>

#include <SDL3/SDL.h>

namespace IR::Renderer {
    struct GL : public APIHandle {
        IR_RENDERER_FUNCS(, override)

        enum UniformLocation {
            UNIFORMLOC_COMMON,
            UNIFORMLOC_ANIMATION,
        };

        // Vars
        std::vector<GLModel*> m_Models;
        std::vector<GLMaterial*> m_Materials;

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

        GLTexture m_TextureWhite;
        GLTexture m_TextureBlack;
        GLTexture m_TextureError;

        GLMaterial m_MaterialError;

        SDL_GLContext m_GLContext;
    };

}