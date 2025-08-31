#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_VERT

#include "common.glsl"

layout(location = 0) in vec3 aPos;

out VP_Shared {
    vec3 pUV;
};

void main()
{
	vec4 pos = uCommon.Perspective * mat4(mat3(uCommon.View)) * vec4(aPos * 512.0, 1.0);
    gl_Position = pos.xyww;
	pUV = aPos;
}
