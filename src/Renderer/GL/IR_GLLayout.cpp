#include <Renderer/GL/IR_GLLayout.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    bool GLLayout::InitBasic2D()
    {
        enum {
            POSITION,
            UV
        };

        m_Type = Type::BASIC2D;

        glCreateVertexArrays(1, &m_ID);

        glEnableVertexArrayAttrib(m_ID, POSITION);
        glVertexArrayAttribFormat(m_ID, POSITION, 2, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(m_ID, POSITION, 0);

        glEnableVertexArrayAttrib(m_ID, UV);
        glVertexArrayAttribFormat(m_ID, UV, 2, GL_FLOAT, GL_FALSE, offsetof(VertexBasic2D, texcoord));
        glVertexArrayAttribBinding(m_ID, UV, 0);

        m_Stride = sizeof(VertexBasic2D);
        m_MeshPool.Init(m_ID, m_Stride);

        return true;
    }

    bool GLLayout::InitStandard()
    {
        enum {
            POSITION,
            NORMAL,
            UV
        };

        m_Type = Type::STANDARD;

        glCreateVertexArrays(1, &m_ID);

        glEnableVertexArrayAttrib(m_ID, POSITION);
        glVertexArrayAttribFormat(m_ID, POSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(m_ID, POSITION, 0);

        glEnableVertexArrayAttrib(m_ID, NORMAL);
        glVertexArrayAttribFormat(m_ID, NORMAL, 3, GL_FLOAT, GL_FALSE, offsetof(VertexStandard, normal));
        glVertexArrayAttribBinding(m_ID, NORMAL, 0);

        glEnableVertexArrayAttrib(m_ID, UV);
        glVertexArrayAttribFormat(m_ID, UV, 2, GL_FLOAT, GL_FALSE, offsetof(VertexStandard, texcoord));
        glVertexArrayAttribBinding(m_ID, UV, 0);

        m_Stride = sizeof(VertexStandard);
        m_MeshPool.Init(m_ID, m_Stride);

        return true;
    }

    bool GLLayout::InitAnimated()
    {
        enum {
            POSITION,
            NORMAL,
            UV,
            BONES,
            WEIGHTS
        };

        m_Type = Type::ANIMATED;

        glCreateVertexArrays(1, &m_ID);

        glEnableVertexArrayAttrib(m_ID, POSITION);
        glVertexArrayAttribFormat(m_ID, POSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(m_ID, POSITION, 0);

        glEnableVertexArrayAttrib(m_ID, NORMAL);
        glVertexArrayAttribFormat(m_ID, NORMAL, 3, GL_FLOAT, GL_FALSE, offsetof(VertexAnimated, normal));
        glVertexArrayAttribBinding(m_ID, NORMAL, 0);

        glEnableVertexArrayAttrib(m_ID, UV);
        glVertexArrayAttribFormat(m_ID, UV, 2, GL_FLOAT, GL_FALSE, offsetof(VertexAnimated, texcoord));
        glVertexArrayAttribBinding(m_ID, UV, 0);

        glEnableVertexArrayAttrib(m_ID, BONES);
        glVertexArrayAttribIFormat(m_ID, BONES, 1, GL_UNSIGNED_INT, offsetof(VertexAnimated, bones));
        glVertexArrayAttribBinding(m_ID, BONES, 0);

        glEnableVertexArrayAttrib(m_ID, WEIGHTS);
        glVertexArrayAttribFormat(m_ID, WEIGHTS, 4, GL_FLOAT, GL_FALSE, offsetof(VertexAnimated, weights));
        glVertexArrayAttribBinding(m_ID, WEIGHTS, 0);

        m_Stride = sizeof(VertexAnimated);
        m_MeshPool.Init(m_ID, m_Stride);

        return true;
    }

    void GLLayout::Destroy()
    {
        glDeleteVertexArrays(1, &m_ID);

        m_MeshPool.Destroy();

        m_ID = 0;
    }

}
