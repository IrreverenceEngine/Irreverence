#pragma once

#include <IR_Common.hpp>

namespace IR::Renderer {
    class Shader {
    public:
		virtual ~Shader() = default;

        bool InitRaster(const char* vspath, const char* fspath);
        bool InitCompute(const char* cspath);
        virtual bool InitComputeMemory(const char* cscode) = 0;
        virtual bool InitRasterMemory(const char* vscode, const char* fscode) = 0;
        virtual void Destroy() {}
    };
}
