#include <Renderer/IR_GLMeshPool.hpp>

#include <GL/glew.h>

namespace IR::Renderer {
    bool GLMeshPool::Init(UInt32 layoutId, UInt64 stride)
    {
        vertexBuffer.Init(GL_ARRAY_BUFFER, nullptr, stride, UINT8_MAX, true);
        glVertexArrayVertexBuffer(layoutId, 0, vertexBuffer.id, 0, stride);

        indexBuffer.Init(GL_ELEMENT_ARRAY_BUFFER, nullptr, sizeof(UInt32), UINT8_MAX, true);
        glVertexArrayElementBuffer(layoutId, indexBuffer.id);

        vstride = stride;
        layout = layoutId;

        return true;
    }

    void GLMeshPool::Destroy()
    {
        vertexBuffer.Destroy();
        indexBuffer.Destroy();

        vertexData.clear();
        indexData.clear();
    }

    std::pair<UInt32, UInt32> GLMeshPool::Add(const UInt8* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indexnum, bool immediate)
    {
        UInt32 vId = vertexData.size();
        vertexData.reserve(vertexData.size() + vertnum * vstride);
        vertexData.insert(vertexData.end(), vertices, vertices + vertnum * vstride);

        UInt32 iId = indexData.size();
        indexData.reserve(indexData.size() + indexnum);
        indexData.insert(indexData.end(), indices, indices + indexnum);

        if (immediate) {
            UInt8* voff = vertexData.data() + vId;
            vertexBuffer.Update(voff, vstride * vertnum, vId);

            UInt32* ioff = indexData.data() + iId;
            indexBuffer.Update(ioff, sizeof(UInt32) * indexnum, iId * sizeof(UInt32));
        }

        return { vId / vstride, iId };
    }

    void GLMeshPool::Upload()
    {
        vertexBuffer.Update(vertexData.data(), vertexData.size(), 0);
        indexBuffer.Update(indexData.data(), indexData.size() * sizeof(UInt32), 0);
    }

    void GLMeshPool::Bind()
    {
        glBindVertexArray(layout);
        glVertexArrayVertexBuffer(layout, 0, vertexBuffer.id, 0, vstride);
        glVertexArrayElementBuffer(layout, indexBuffer.id);
    }
}