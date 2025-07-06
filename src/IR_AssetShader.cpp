#include <IR_AssetShader.hpp>
#include <IR_Common.hpp>
#include <bgfx/bgfx.h>

#include <SDL3/SDL.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>

// TODO: This file overall needs to be tidied up and debugged. Also handle a lot of edge cases.

namespace IR::Asset::Shader {

    bgfx::ShaderHandle CreateShaderFromPath(const char* shaderPath)
    {
        // NOTE: I tried doing that but it kept corrupting memory..? Weird shit.

        FILE* file = fopen(shaderPath, "rb");
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        const bgfx::Memory *mem = bgfx::alloc(fileSize + 1);
        fread(mem->data, 1, fileSize, file);
        mem->data[mem->size - 1] = '\0';
        fclose(file);

        return bgfx::createShader(mem);
    }

    static bool RunCompileProcess(const char* name, const char* path, const char* outPath, const char* type, const char* platform, const char* profile)
    {
        const char* args[] = {
            "bin/shaderc",
            "-i",
            "assets/shaders",
            "-f",
            path,
            "-o",
            outPath,
            "--type",
            type,
            "--platform",
            platform,
            "--profile",
            profile,
            nullptr
        };

        SDL_Process* proc = SDL_CreateProcess(&args[0], true);
        IR_DEFER({ SDL_DestroyProcess(proc); });

        Int32 exitcode = UINT32_MAX;
        UInt64 vertOutSize = 0;
        char* vertOut = (char*)SDL_ReadProcess(proc, &vertOutSize, &exitcode);
        IR_DEFER({ SDL_free(vertOut); });

        if (exitcode == UINT32_MAX) {
            IR_MSG(FATAL, "Couldn't compile shader \"%s\": couldn't get exit code of %s compile process", name, type);
            return false;
        }

        if (exitcode == 1) {
            IR_MSG(FATAL, "Couldn't compile shader \"%s\": %s %s", name, type, vertOut);
            return false;
        }

        return true;
    }

    void CompileRaster(const char* name)
    {
        if (!SDL_GetPathInfo("bin/shaderc", nullptr)) {
            IR_MSG(FATAL, "Couldn't compile shader \"%s\": shaderc is missing in bin, try verifying game files", name);
            return;
        }

        std::string allshadersdir = "assets/shaders/";
        std::string shaderdir = allshadersdir + std::string(name);

        std::string vertpath = shaderdir + "/vert.sc";
        std::string fragpath = shaderdir + "/frag.sc";
        std::string varyingpath = shaderdir + "/varying.def.sc";

        if (!SDL_GetPathInfo(shaderdir.c_str(), nullptr)) {
            IR_MSG(FATAL, "Couldn't compile shader \"%s\": directory is missing", name);
            return;
        }

        if (!SDL_GetPathInfo(vertpath.c_str(), nullptr)) {
            IR_MSG(FATAL, "Couldn't compile shader \"%s\": vert.sc is missing", name);
            return;
        }

        if (!SDL_GetPathInfo(fragpath.c_str(), nullptr)) {
            IR_MSG(FATAL, "Couldn't compile shader \"%s\": frag.sc is missing", name);
            return;
        }

        if (!SDL_GetPathInfo(varyingpath.c_str(), nullptr)) {
            IR_MSG(FATAL, "Couldn't compile shader \"%s\": varying.def.sc is missing", name);
            return;
        }

        std::string cachepath = allshadersdir;
        std::string profile;
        switch (bgfx::getRendererType()) {
        case bgfx::RendererType::OpenGL:
            cachepath += ".cache/gl/";
            profile = "440";
            break;
        case bgfx::RendererType::Vulkan:
            cachepath += ".cache/vk/";
            profile = "spirv16-13";
            break;
        case bgfx::RendererType::Metal:
            cachepath += ".cache/metal/";
            profile = "metal";
            break;
        case bgfx::RendererType::Direct3D11:
            cachepath += ".cache/dx11/";
            profile = "s_4_0";
            break;
        case bgfx::RendererType::Direct3D12:
            cachepath += ".cache/dx12/";
            profile = "s_5_0";
            break;
        default: IR_UNREACHABLE;
        }
        cachepath += std::string(name) + "/";

    #if defined(SDL_PLATFORM_WIN32)
        const char* platform = "windows";
    #elif defined(SDL_PLATFORM_MACOS)
        const char* platform = "osx";
    #elif defined(SDL_PLATFORM_LINUX)
        const char* platform = "linux";
    #endif

        SDL_CreateDirectory(cachepath.c_str());

        if (!RunCompileProcess(name, vertpath.c_str(), (cachepath + "vert.bin").c_str(), "vertex", platform, profile.c_str())) {
            return;
        }

        if (!RunCompileProcess(name, fragpath.c_str(), (cachepath + "frag.bin").c_str(), "fragment", platform, profile.c_str())) {
            return;
        }
    }

    bgfx::ProgramHandle LoadRaster(const char* name, bool force_compile)
    {
        std::string cachepath = "assets/shaders/";
        switch (bgfx::getRendererType()) {
        case bgfx::RendererType::OpenGL:
            cachepath += ".cache/gl/";
            break;
        case bgfx::RendererType::Vulkan:
            cachepath += ".cache/vk/";
            break;
        case bgfx::RendererType::Metal:
            cachepath += ".cache/metal/";
            break;
        case bgfx::RendererType::Direct3D11:
            cachepath += ".cache/dx11/";
            break;
        case bgfx::RendererType::Direct3D12:
            cachepath += ".cache/dx12/";
            break;
        default: IR_UNREACHABLE;
        }
        cachepath += std::string(name) + "/";

        std::string vertPath = cachepath + "vert.bin";
        std::string fragPath = cachepath + "frag.bin";

        bool filesMissing = false;

        if (!SDL_GetPathInfo(vertPath.c_str(), nullptr)) {
            if (!force_compile) {
                IR_MSG(FATAL, "Couldn't load shader \"%s\": vert.bin doesn't exist", name);
            }

            filesMissing = true;
        }

        if (!SDL_GetPathInfo(vertPath.c_str(), nullptr)) {
            if (!force_compile) {
                IR_MSG(FATAL, "Couldn't load shader \"%s\": frag.bin doesn't exist", name);
            }

            filesMissing = true;
        }

        if (filesMissing) {
            CompileRaster(name);
        }

        bgfx::ShaderHandle vertShader = CreateShaderFromPath(vertPath.c_str());
        bgfx::ShaderHandle fragShader = CreateShaderFromPath(fragPath.c_str());

        return bgfx::createProgram(vertShader, fragShader, true);
    }

}