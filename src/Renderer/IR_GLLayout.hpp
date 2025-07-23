#pragma once

#include <IR_Common.hpp>
#include <IR_LayoutType.hpp>
#include <Renderer/IR_GLMeshPool.hpp>

#include <glm.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    struct GLLayout {
        enum Type : UInt8 {
            TYPE_STANDARD,
            TYPE_ANIMATED,
            TYPE__COUNT
        };

        bool InitStandard();
        bool InitAnimated();

        void Destroy();

        UInt32 id = 0;
        GLMeshPool meshpool;
        Type type = TYPE__COUNT;
    };


}