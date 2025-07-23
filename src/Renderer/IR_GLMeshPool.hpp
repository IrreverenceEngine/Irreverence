#pragma once

#include <IR_Common.hpp>
#include <Renderer/IR_GLBuffer.hpp>
#include <Renderer/IR_GLMesh.hpp>
#include <Renderer/IR_GLShader.hpp>

#include <vector>

namespace IR::Renderer {

    struct GLMeshPool { // Mesh Buffer
        bool Init(UInt32 layoutId, UInt64 stride);
        void Destroy();

        std::pair<UInt32, UInt32> Add(const UInt8* vertices, UInt32 vertnum, const UInt32* indices, UInt32 indexnum, bool immediate = true);
        void Upload();
        void Bind();

        GLBuffer vertexBuffer;
        GLBuffer indexBuffer;

        std::vector<UInt8> vertexData;
        std::vector<UInt32> indexData;
        std::vector<GLShader*> shaders;

        UInt32 layout = 0;
        UInt64 vstride = 0;
    };
}