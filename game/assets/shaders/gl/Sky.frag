#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_FRAG

#include "common.glsl"

layout (location = 2) out vec3 oColor;
layout (location = 7) out vec4 oFinalColor;

layout(binding = 0) uniform samplerCube uSkyTexture;

in VP_Shared {
    vec3 pUV;
};

void main()
{
    oColor = vec3(abs(cos(uCommon.CurTime * 0.75) + 0.2), abs(sin(uCommon.CurTime * 0.5)), abs(cos(uCommon.CurTime * 0.2)));
}
