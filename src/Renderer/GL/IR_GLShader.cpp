#include <Renderer/GL/IR_GLShader.hpp>
#include <Renderer/GL/IR_GLRenderer.hpp>

#include <GL/glew.h>

#include <string.h>

namespace IR::Renderer {

    bool GLShader::InitRasterMemory(const char* vscode, const char* fscode)
    {
        if (!vscode || !fscode) return false;

        std::string vscodeCpy = vscode;
        std::string fscodeCpy = fscode;

        for (const std::string& incl : s_GL->GetPatchIncludes()) {
            UInt64 vsPos = vscodeCpy.find(incl);
            while (vsPos != std::string::npos) {
                vscodeCpy.insert(vscodeCpy.begin() + vsPos, '/');
                vsPos = vscodeCpy.find(incl, vsPos + incl.length());
            }

            UInt64 fsPos = fscodeCpy.find(incl);
            while (fsPos != std::string::npos) {
                fscodeCpy.insert(fscodeCpy.begin() + fsPos, '/');
                fsPos = fscodeCpy.find(incl, fsPos + incl.length());
            }
        }

        char* vscodePtr = vscodeCpy.data();
        char* fscodePtr = fscodeCpy.data();

        char failureLog[1024];
        Int32 bSuccess = true;

        UInt32 v_id = 0;
        UInt32 f_id = 0;

        v_id = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(v_id, 1, (const GLchar* const*)&vscodePtr, NULL);
        glCompileShader(v_id);
        glGetShaderiv(v_id, GL_COMPILE_STATUS, &bSuccess);
        IR_DEFER({ glDeleteShader(v_id); });

        bool bFailedShaderCompilation = false;
        if (!bSuccess) {
            glGetShaderInfoLog(v_id, 1024, NULL, failureLog);
            IR_MSG(ERROR, "GLShader Vertex shader compilation failed:\n %s", failureLog);
            bFailedShaderCompilation = true;
        }

        f_id = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(f_id, 1, (const GLchar* const*)&fscodePtr, NULL);
        glCompileShader(f_id);
        glGetShaderiv(f_id, GL_COMPILE_STATUS, &bSuccess);
        IR_DEFER({ glDeleteShader(f_id); });

        if (!bSuccess) {
            glGetShaderInfoLog(f_id, 1024, NULL, failureLog);
            IR_MSG(ERROR, "GLShader Fragment shader compilation failed:\n %s", failureLog);
            bFailedShaderCompilation = true;
        }

        if (bFailedShaderCompilation) {
            return false;
        }

        UInt32 p_id = glCreateProgram();
        glAttachShader(p_id, v_id);
        glAttachShader(p_id, f_id);
        glLinkProgram(p_id);
        glGetProgramiv(p_id, GL_LINK_STATUS, &bSuccess);

        if (!bSuccess) {
            glGetProgramInfoLog(p_id, 1024, NULL, failureLog);
            IR_MSG(ERROR, "GLShader Linking failed:\n %s", failureLog);
            glDeleteProgram(m_ID);
            return false;
        }

        m_ID = p_id;

        return true;
    }

    bool GLShader::InitComputeMemory(const char* cscode) IR_UNIMPLEMENTED

    void GLShader::Destroy()
    {
        glDeleteProgram(m_ID);

        m_ID = 0;
    }

    void GLShader::Bind()
    {
        glUseProgram(m_ID);
    }

}