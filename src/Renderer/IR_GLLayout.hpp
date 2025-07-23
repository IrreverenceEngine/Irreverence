#pragma once

#include <IR_Common.hpp>
#include <IR_LayoutType.hpp>
#include <Renderer/IR_GLMeshPool.hpp>

#include <glm.hpp>

namespace IR::Renderer {

    class GLLayout {
    public:
        enum Type : UInt8 {
            TYPE_STANDARD,
            TYPE_ANIMATED,
            TYPE__COUNT
        };

        bool InitStandard();
        bool InitAnimated();

        void Destroy();

        GLMeshPool& GetMeshPool() IR_RETURN(m_MeshPool)
        Type GetType() IR_RETURN(m_Type)

    private:
        UInt32 m_ID = 0;
        GLMeshPool m_MeshPool;
        Type m_Type = TYPE__COUNT;
    };

}