#include <renderer/IR_GLLayout.hpp>
#include <IR_LayoutType.hpp>

#include <GL/glew.h>

#define SetMatrixLayout(vertexarray, location, offset) for (UInt32 i = 0; i < 4; i++) { UInt32 mat_offset = location + i; glEnableVertexArrayAttrib(vertexarray, mat_offset); glVertexArrayAttribFormat(vertexarray, mat_offset, 4, GL_FLOAT, GL_FALSE, offset + sizeof(float) * 4 * i); glVertexArrayAttribBinding(vertexarray, mat_offset, 1); }

namespace IR::Renderer {

    bool GLLayout::InitStandard()
    {
        enum {
            MODELMESH_POSITION,
            MODELMESH_NORMAL,
            MODELMESH_UV,
            MODELMESH_COLOR,
            MODELMESH_MODELMATRIX
        };

        glCreateBuffers(1, &ibo);

        glCreateVertexArrays(1, &id);

        glEnableVertexArrayAttrib(id, MODELMESH_POSITION);
        glVertexArrayAttribFormat(id, MODELMESH_POSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(id, MODELMESH_POSITION, 0);

        glEnableVertexArrayAttrib(id, MODELMESH_NORMAL);
        glVertexArrayAttribFormat(id, MODELMESH_NORMAL, 3, GL_FLOAT, GL_FALSE, offsetof(VertexStandard, normal));
        glVertexArrayAttribBinding(id, MODELMESH_NORMAL, 0);

        glEnableVertexArrayAttrib(id, MODELMESH_UV);
        glVertexArrayAttribFormat(id, MODELMESH_UV, 2, GL_FLOAT, GL_FALSE, offsetof(VertexStandard, texcoord));
        glVertexArrayAttribBinding(id, MODELMESH_UV, 0);

        glVertexArrayBindingDivisor(id, 1, 1);

        glEnableVertexArrayAttrib(id, MODELMESH_COLOR);
        glVertexArrayAttribFormat(id, MODELMESH_COLOR, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(id, MODELMESH_COLOR, 1);

        SetMatrixLayout(id, MODELMESH_MODELMATRIX, offsetof(InstanceStandard, model));

        glVertexArrayVertexBuffer(id, 1, ibo, 0, sizeof(InstanceStandard));

        return true;
    }

    bool GLLayout::InitAnimated()
    {
        enum {
            MODELMESH_POSITION,
            MODELMESH_NORMAL,
            MODELMESH_UV,
            MODELMESH_BONES,
            MODELMESH_WEIGHTS,
            MODELMESH_COLOR,
            MODELMESH_MODELMATRIX
        };

        glCreateBuffers(1, &ibo);

        glCreateVertexArrays(1, &id);

        glEnableVertexArrayAttrib(id, MODELMESH_POSITION);
        glVertexArrayAttribFormat(id, MODELMESH_POSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(id, MODELMESH_POSITION, 0);

        glEnableVertexArrayAttrib(id, MODELMESH_NORMAL);
        glVertexArrayAttribFormat(id, MODELMESH_NORMAL, 3, GL_FLOAT, GL_FALSE, offsetof(VertexAnimated, normal));
        glVertexArrayAttribBinding(id, MODELMESH_NORMAL, 0);

        glEnableVertexArrayAttrib(id, MODELMESH_UV);
        glVertexArrayAttribFormat(id, MODELMESH_UV, 2, GL_FLOAT, GL_FALSE, offsetof(VertexAnimated, texcoord));
        glVertexArrayAttribBinding(id, MODELMESH_UV, 0);

        glEnableVertexArrayAttrib(id, MODELMESH_BONES);
        glVertexArrayAttribIFormat(id, MODELMESH_BONES, 1, GL_UNSIGNED_INT, offsetof(VertexAnimated, bones));
        glVertexArrayAttribBinding(id, MODELMESH_BONES, 0);

        glEnableVertexArrayAttrib(id, MODELMESH_WEIGHTS);
        glVertexArrayAttribFormat(id, MODELMESH_WEIGHTS, 4, GL_FLOAT, GL_FALSE, offsetof(VertexAnimated, weights));
        glVertexArrayAttribBinding(id, MODELMESH_WEIGHTS, 0);

        glVertexArrayBindingDivisor(id, 1, 1);

        glEnableVertexArrayAttrib(id, MODELMESH_COLOR);
        glVertexArrayAttribFormat(id, MODELMESH_COLOR, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(id, MODELMESH_COLOR, 1);

        SetMatrixLayout(id, MODELMESH_MODELMATRIX, offsetof(InstanceStandard, model));

        glVertexArrayVertexBuffer(id, 1, ibo, 0, sizeof(InstanceStandard));

        return true;
    }

    void GLLayout::Destroy()
    {
        glDeleteVertexArrays(1, &id);
        glDeleteBuffers(1, &ibo);

        id = 0;
    }

}