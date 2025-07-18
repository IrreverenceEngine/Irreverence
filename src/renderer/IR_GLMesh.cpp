#include <renderer/IR_GLMesh.hpp>

#include <GL/glew.h>

#define SetMatrixLayout(vertexarray, location, offset) for (UInt32 i = 0; i < 4; i++) { UInt32 mat_offset = location + i; glEnableVertexArrayAttrib(vertexarray, mat_offset); glVertexArrayAttribFormat(vertexarray, mat_offset, 4, GL_FLOAT, GL_FALSE, offset + sizeof(float) * 4 * i); glVertexArrayAttribBinding(vertexarray, mat_offset, 1); }

namespace IR::Renderer {

    bool GLMesh::Init(const VertexStandard* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
    {
        enum {
            MODELMESH_POSITION,
            MODELMESH_NORMAL,
            MODELMESH_UV,
            MODELMESH_COLOR,
            MODELMESH_MODELMATRIX
        };

        vertCount = vertexnum;
        indexCount = indexnum;

        instStride = sizeof(InstanceStandard);

        size_t vertexSize = vertexnum * sizeof(VertexStandard);
        size_t indexSize = indexnum * sizeof(UInt32);

        // Create OpenGL Objects
        glCreateBuffers(1, &vbo);
        glNamedBufferStorage(vbo, vertexSize, vertices, GL_DYNAMIC_STORAGE_BIT);

        glCreateBuffers(1, &ibo);

        glCreateBuffers(1, &ebo);
        glNamedBufferStorage(ebo, indexSize, indices, GL_DYNAMIC_STORAGE_BIT);

        glCreateVertexArrays(1, &id);

        // Link Element Buffer
        glVertexArrayElementBuffer(id, ebo);

        // Link Vertex Buffer and Set Vertex Buffer Layout
        glVertexArrayVertexBuffer(id, 0, vbo, 0, sizeof(VertexStandard));

        glEnableVertexArrayAttrib(id, MODELMESH_POSITION);
        glVertexArrayAttribFormat(id, MODELMESH_POSITION, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(id, MODELMESH_POSITION, 0);

        glEnableVertexArrayAttrib(id, MODELMESH_NORMAL);
        glVertexArrayAttribFormat(id, MODELMESH_NORMAL, 3, GL_FLOAT, GL_FALSE, offsetof(VertexStandard, normal));
        glVertexArrayAttribBinding(id, MODELMESH_NORMAL, 0);

        glEnableVertexArrayAttrib(id, MODELMESH_UV);
        glVertexArrayAttribFormat(id, MODELMESH_UV, 2, GL_FLOAT, GL_FALSE, offsetof(VertexStandard, texcoord));
        glVertexArrayAttribBinding(id, MODELMESH_UV, 0);

        // Link Instance Buffer and Set Instance Buffer Layout
        glVertexArrayVertexBuffer(id, 1, ibo, 0, sizeof(InstanceStandard));
        glVertexArrayBindingDivisor(id, 1, 1);

        glEnableVertexArrayAttrib(id, MODELMESH_COLOR);
        glVertexArrayAttribFormat(id, MODELMESH_COLOR, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(id, MODELMESH_COLOR, 1);

        SetMatrixLayout(id, MODELMESH_MODELMATRIX, offsetof(InstanceStandard, model));

        return true;
    }

    bool GLMesh::Init(const VertexAnimated* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
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

        vertCount = vertexnum;
        indexCount = indexnum;

        instStride = sizeof(InstanceStandard);

        size_t vertexSize = vertexnum * sizeof(VertexAnimated);
        size_t indexSize = indexnum * sizeof(UInt32);

        // Create OpenGL Objects
        glCreateBuffers(1, &vbo);
        glNamedBufferStorage(vbo, vertexSize, vertices, GL_DYNAMIC_STORAGE_BIT);

        glCreateBuffers(1, &ibo);

        glCreateBuffers(1, &ebo);
        glNamedBufferStorage(ebo, indexSize, indices, GL_DYNAMIC_STORAGE_BIT);

        glCreateVertexArrays(1, &id);

        // Link Element Buffer
        glVertexArrayElementBuffer(id, ebo);

        // Link Vertex Buffer and Set Vertex Buffer Layout
        glVertexArrayVertexBuffer(id, 0, vbo, 0, sizeof(VertexAnimated));

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

        // Link Instance Buffer and Set Instance Buffer Layout
        glVertexArrayVertexBuffer(id, 1, ibo, 0, sizeof(InstanceStandard));
        glVertexArrayBindingDivisor(id, 1, 1);

        glEnableVertexArrayAttrib(id, MODELMESH_COLOR);
        glVertexArrayAttribFormat(id, MODELMESH_COLOR, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(id, MODELMESH_COLOR, 1);

        SetMatrixLayout(id, MODELMESH_MODELMATRIX, offsetof(InstanceStandard, model));

        return true;
    }

    void GLMesh::Destroy()
    {
        glDeleteVertexArrays(1, &id);

        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);
        glDeleteBuffers(1, &ibo);

        id = 0;
    }

    void GLMesh::Draw(const void* instance_data)
    {
        glNamedBufferData(ibo, instStride, instance_data, GL_DYNAMIC_DRAW);

        glBindVertexArray(id);
        if (ebo > 0) {
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, vertCount);
        }
    }

}