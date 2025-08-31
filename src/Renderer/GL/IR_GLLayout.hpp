#pragma once

#include <IR_Common.hpp>
#include <IR_LayoutType.hpp>
#include <Renderer/GL/IR_GLMeshPool.hpp>

#include <glm.hpp>

namespace IR::Renderer {
    class GLLayout {
    public:
        enum class Type : UInt8 {
            BASIC2D,
            POSITION,
            STANDARD,
            ANIMATED,
            _COUNT
        };

        bool InitBasic2D();
        bool InitPosition();
        bool InitStandard();
        bool InitAnimated();

        void Destroy();

        UInt32 GetID() const IR_RETURN(m_ID)
        GLMeshPool& GetMeshPool() IR_RETURN(m_MeshPool)
        Type GetType() const IR_RETURN(m_Type)
        UInt32 GetStride() const IR_RETURN(m_Stride)

    private:
        UInt32 m_ID = 0;
        GLMeshPool m_MeshPool;
        Type m_Type = Type::_COUNT;
        UInt32 m_Stride = 0;
    };
}
