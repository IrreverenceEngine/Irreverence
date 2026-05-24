#include <IR_Shader.hpp>
#include <IR_File.hpp>
#include <IR_KeyValue.hpp>
#include <IR_Renderer.hpp>

namespace IR::Renderer {
    bool Shader::Init(const char* path)
    {
        KeyValue* kv = KeyValue::Load(path);
        if (!kv) {
            IRX_MSG(ERROR, "Shader failed to load .irs file \"%s\"", path);
            return false;
        }

        const std::string& type = kv->FindChildString("Type");

        if (type.empty()) {
            IRX_MSG(ERROR, "Shader is missing \"Type\" parameter in file \"%s\"", path);
            return false;
        }

        std::string assetPath = "assets/shaders/" + std::string(Renderer::GetDirectory());

        if (type == "Raster") {
            const std::string& vertPath = kv->FindChildString("Vertex");

            if (vertPath.empty()) {
                IRX_MSG(ERROR, "Shader is missing \"Vertex\" parameter in file \"%s\"", path);
                return false;
            }

            const std::string& fragPath = kv->FindChildString("Fragment");
            if (fragPath.empty()) {
                IRX_MSG(ERROR, "Shader is missing \"Fragment\" parameter in file \"%s\"", path);
                return false;
            }

            return InitRaster((assetPath + vertPath).c_str(), (assetPath + fragPath).c_str());

        } else if (type == "Compute") {
            const std::string& compPath = kv->FindChildString("Compute");
            if (compPath.empty()) {
                IRX_MSG(ERROR, "Shader is missing \"Compute\" parameter in file \"%s\"", path);
                return false;
            }

            return InitCompute((assetPath + compPath).c_str());

        } else {
            IRX_MSG(ERROR, "Shader unknown \"Type\" value in file \"%s\"", path);
            return false;
        }
    }

    bool Shader::InitRaster(const char* vspath, const char* fspath)
    {
        File vsfile(vspath, "r");
        if (!vsfile.IsOpen()) {
            IRX_MSG(ERROR, "Shader failed to open Vertex Shader file \"%s\"", vspath);
            return false;
        }

        char* vscode = vsfile.ReadAll();
        IRX_DEFER({ if (vscode) delete[] vscode; });

        File fsfile(fspath, "r");
        if (!fsfile.IsOpen()) {
            IRX_MSG(ERROR, "Shader failed to open Fragment Shader file \"%s\"", fspath);
            return false;
        }

        char* fscode = fsfile.ReadAll();
        IRX_DEFER({ if (fscode) delete[] fscode; });

		if (!InitRasterMemory(vscode, fscode)) {
			IRX_MSG(ERROR, "Shader failed to initialize Raster Shader from files \"%s\" and \"%s\"", vspath, fspath);
            return false;
        }

        return true;
    }

    bool Shader::InitCompute(const char* cspath)
    {
        File csfile(cspath, "r");
        if (!csfile.IsOpen()) {
            IRX_MSG(ERROR, "Shader failed to open Compute Shader file \"%s\"", cspath);
            return false;
        }

        char* cscode = csfile.ReadAll();
        IRX_DEFER({ if (cscode) delete[] cscode; });

		if (!InitComputeMemory(cscode)) {
			IRX_MSG(ERROR, "Shader failed to initialize Compute Shader from file \"%s\"", cspath);
			return false;
		}

        return true;
    }
}
