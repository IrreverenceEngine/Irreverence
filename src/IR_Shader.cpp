#include <IR_Shader.hpp>

#include <fstream>

namespace IR::Renderer {
    bool Shader::InitRaster(const char* vspath, const char* fspath)
    {
        std::ifstream vsfile(("assets/shaders/gl/" + std::string(vspath)).c_str());
        if (!vsfile.is_open()) {
            IR_MSG(ERROR, "Shader failed to open Vertex Shader File \"%s\"", vspath);
            return false;
        }

        std::ifstream fsfile(("assets/shaders/gl/" + std::string(fspath)).c_str());
        if (!fsfile.is_open()) {
            IR_MSG(ERROR, "Shader failed to open Fragment Shader File \"%s\"", fspath);
            return false;
        }

		std::string vscode((std::istreambuf_iterator<char>(vsfile)), std::istreambuf_iterator<char>());
		std::string fscode((std::istreambuf_iterator<char>(fsfile)), std::istreambuf_iterator<char>());

        return InitRasterMemory(vscode.c_str(), fscode.c_str());
    }

    bool Shader::InitCompute(const char* cspath)
    {
        std::ifstream csfile(("assets/shaders/gl/" + std::string(cspath)).c_str());
        if (!csfile.is_open()) {
            IR_MSG(ERROR, "Shader failed to open Compute Shader File \"%s\"", cspath);
            return false;
        }

		std::string cscode((std::istreambuf_iterator<char>(csfile)), std::istreambuf_iterator<char>());

        return InitComputeMemory(cscode.c_str());
    }
}