#include <IR_Shader.hpp>
#include <IR_File.hpp>
#include <IR_KeyValue.hpp>
#include <IR_Renderer.hpp>

namespace IR::Renderer {
    bool Shader::Init(const char* path)
    {
        KeyValue* kv = KeyValue::Load(path);
        if (!kv) {
            IR_MSG(ERROR, "Shader failed to load .irs file \"%s\"", path);
            return false;
        }

        KeyValue* params = kv->GetChild(0);
        if (!params) {
            IR_MSG(ERROR, "Shader is missing parameters in file \"%s\"", path);
            return false;
        }

        const std::string& type = params->FindChildString("Type");

        if (type.empty()) {
            IR_MSG(ERROR, "Shader is missing \"Type\" parameter in file \"%s\"", path);
            return false;
        }

        std::string assetPath = "assets/shaders/" + std::string(Renderer::GetDirectory());

        if (type == "Raster") {
            const std::string& vertPath = params->FindChildString("Vertex");

            if (vertPath.empty()) {
                IR_MSG(ERROR, "Shader is missing \"Vertex\" parameter in file \"%s\"", path);
                return false;
            }

            const std::string& fragPath = params->FindChildString("Fragment");
            if (fragPath.empty()) {
                IR_MSG(ERROR, "Shader is missing \"Fragment\" parameter in file \"%s\"", path);
                return false;
            }

            return InitRaster((assetPath + vertPath).c_str(), (assetPath + fragPath).c_str());

        } else if (type == "Compute") {
            const std::string& compPath = params->FindChildString("Compute");
            if (compPath.empty()) {
                IR_MSG(ERROR, "Shader is missing \"Compute\" parameter in file \"%s\"", path);
                return false;
            }

            return InitCompute((assetPath + compPath).c_str());

        } else {
            IR_MSG(ERROR, "Shader unknown \"Type\" value in file \"%s\"", path);
            return false;
        }
    }

    bool Shader::InitRaster(const char* vspath, const char* fspath)
    {
        File vsfile(vspath, "r");
        if (!vsfile.IsOpen()) {
            IR_MSG(ERROR, "Shader failed to open Vertex Shader file \"%s\"", vspath);
            return false;
        }

        char* vscode = vsfile.ReadAll();
        IR_DEFER({ if (vscode) delete[] vscode; });

        File fsfile(fspath, "r");
        if (!fsfile.IsOpen()) {
            IR_MSG(ERROR, "Shader failed to open Fragment Shader file \"%s\"", fspath);
            return false;
        }

        char* fscode = fsfile.ReadAll();
        IR_DEFER({ if (fscode) delete[] fscode; });

        return InitRasterMemory(vscode, fscode);
    }

    bool Shader::InitCompute(const char* cspath)
    {
        File csfile(cspath, "r");
        if (!csfile.IsOpen()) {
            IR_MSG(ERROR, "Shader failed to open Compute Shader file \"%s\"", cspath);
            return false;
        }

        char* cscode = csfile.ReadAll();
        IR_DEFER({ if (cscode) delete[] cscode; });

        return InitComputeMemory(cscode);
    }
}