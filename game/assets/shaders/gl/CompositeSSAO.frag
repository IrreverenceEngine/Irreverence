#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_FRAG

#include "common.glsl"
#include "lighting.glsl"

layout (location = 4) out vec2 oSSAO;

layout(binding = 0) uniform sampler2D uPosition;
layout(binding = 1) uniform sampler2D uNormal;
layout(binding = 16) uniform sampler2D uSSAONoise;

const int kernelSize = 64;
const float radius = 1.0;
const float bias = 0.025;

in VP_Shared {
    vec2 pUV;
};

void main()
{
    vec2 noiseScale = vec2(float(uCommon.Width) / 4.0, float(uCommon.Height) / 4.0);
}
