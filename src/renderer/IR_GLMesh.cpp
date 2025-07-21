#include <renderer/IR_GLMesh.hpp>
#include <renderer/IR_GLLayout.hpp>

#include <GL/glew.h>

namespace IR::Renderer {

    bool GLMesh::Init(const GLLayout& layout, const VertexStandard* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
    {
        enum {
            MODELMESH_POSITION,
            MODELMESH_NORMAL,
            MODELMESH_UV,
            MODELMESH_COLOR,
            MODELMESH_MODELMATRIX
        };

        id = layout.id;
        ibo = layout.ibo;

        vertCount = vertexnum;
        indexCount = indexnum;

        vertStride = sizeof(VertexStandard);
        instStride = sizeof(InstanceStandard);

        size_t vertexSize = vertexnum * sizeof(VertexStandard);
        size_t indexSize = indexnum * sizeof(UInt32);

        glCreateBuffers(1, &vbo);
        glNamedBufferStorage(vbo, vertexSize, vertices, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayVertexBuffer(id, 0, vbo, 0, sizeof(VertexStandard));

        glCreateBuffers(1, &ebo);
        glNamedBufferStorage(ebo, indexSize, indices, GL_DYNAMIC_STORAGE_BIT);
        glVertexArrayElementBuffer(id, ebo);

        return true;
    }

    bool GLMesh::Init(const GLLayout& layout, const VertexAnimated* vertices, UInt32 vertexnum, const UInt32* indices, UInt32 indexnum)
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

        id = layout.id;
        ibo = layout.ibo;

        vertCount = vertexnum;
        indexCount = indexnum;

        vertStride = sizeof(VertexAnimated);
        instStride = sizeof(InstanceStandard);

        size_t vertexSize = vertexnum * sizeof(VertexAnimated);
        size_t indexSize = indexnum * sizeof(UInt32);

        glCreateBuffers(1, &vbo);
        glNamedBufferStorage(vbo, vertexSize, vertices, GL_DYNAMIC_STORAGE_BIT);

        glCreateBuffers(1, &ebo);
        glNamedBufferStorage(ebo, indexSize, indices, GL_DYNAMIC_STORAGE_BIT);

        glVertexArrayElementBuffer(id, ebo);
        glVertexArrayVertexBuffer(id, 0, vbo, 0, sizeof(VertexAnimated));

        return true;
    }

    void GLMesh::Destroy()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteBuffers(1, &ebo);

        id = 0;
    }

    void GLMesh::Draw(const void* instance_data)
    {
        glNamedBufferData(ibo, instStride, instance_data, GL_DYNAMIC_DRAW);

        glBindVertexArray(id);
        glVertexArrayElementBuffer(id, ebo);
        glVertexArrayVertexBuffer(id, 0, vbo, 0, vertStride);
        if (ebo > 0) {
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        } else {
            glDrawArrays(GL_TRIANGLES, 0, vertCount);
        }
    }

}