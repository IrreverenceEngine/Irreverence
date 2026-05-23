#include <IR_QUtil.hpp>
#include <IR_Common.hpp>

namespace IR::QUtil {
    glm::vec3 QVec3ToVec3(const glm::vec3& qvec3) IR_RETURN(glm::vec3(qvec3.x, qvec3.z, -qvec3.y))
    glm::vec4 QVec4ToVec4(const glm::vec4& qvec4) IR_RETURN(glm::vec4(qvec4.x, qvec4.z, -qvec4.y, qvec4.w))

    glm::vec3 StrToVec3(const char* qstr)
    {
        Float32 vals[3] = { 0 };
        vals[0] = atof(qstr);
        UInt8 cntr = 1;
        for (UInt64 i = 0; i < strlen(qstr); i++) {
            if (cntr > 2) {
                break;
            }

            if (qstr[i] == ' ') {
                vals[cntr++] = atof(qstr + i + 1);
            }
        }

        return { vals[0], vals[1], vals[2] };
    }

    glm::vec4 StrToVec4(const char* qstr)
    {
        Float32 vals[4] = { 0 };
        vals[0] = atof(qstr);
        UInt8 cntr = 1;
        for (UInt64 i = 0; i < strlen(qstr); i++) {
            if (cntr > 3) {
                break;
            }

            if (qstr[i] == ' ') {
                vals[cntr++] = atof(qstr + i + 1);
            }
        }

        return { vals[0], vals[1], vals[2], vals[3] };
    }

}
