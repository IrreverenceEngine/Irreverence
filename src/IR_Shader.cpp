#include <IR_Shader.hpp>
#include <IR_File.hpp>

#include <string>

namespace IR::Renderer {
    bool Shader::InitRaster(const char* vspath, const char* fspath)
    {
        File vsfile(("assets/shaders/gl/" + std::string(vspath)).c_str(), "r");
        if (!vsfile.IsOpen()) {
            IR_MSG(ERROR, "Shader failed to open Vertex Shader File \"%s\"", vspath);
            return false;
        }

        char* vscode = vsfile.ReadAll();
        IR_DEFER({ if (vscode) delete[] vscode; });

        File fsfile(("assets/shaders/gl/" + std::string(fspath)).c_str(), "r");
        if (!fsfile.IsOpen()) {
            IR_MSG(ERROR, "Shader failed to open Fragment Shader File \"%s\"", fspath);
            return false;
        }

        char* fscode = fsfile.ReadAll();
        IR_DEFER({ if (fscode) delete[] fscode; });

        return InitRasterMemory(vscode, fscode);
    }

    bool Shader::InitCompute(const char* cspath)
    {
        File csfile(("assets/shaders/gl/" + std::string(cspath)).c_str(), "r");
        if (!csfile.IsOpen()) {
            IR_MSG(ERROR, "Shader failed to open Compute Shader File \"%s\"", cspath);
            return false;
        }

        char* cscode = csfile.ReadAll();
        IR_DEFER({ if (cscode) delete[] cscode; });

        return InitComputeMemory(cscode);
    }
}