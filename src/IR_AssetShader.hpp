#pragma once

#include <bgfx/bgfx.h>

// It is such an bitch to load shaders using BGFX that we need utils for this.

namespace IR::Asset::Shader {
    void CompileRaster(const char* name);
    bgfx::ProgramHandle LoadRaster(const char* name, bool force_compile = false);

    void Cleanup();
}