#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_FRAG

#include "common.glsl"
#include "lighting.glsl"

layout (location = 7) out vec4 oFinalColor;

layout(binding = 5) uniform sampler2D uTransColor;
layout(binding = 6) uniform sampler2D uTransReveal;

in VP_Shared {
    vec2 pUV;
};

void main()
{
    float revealage = texture(uTransReveal, pUV).r;

    if (approxEq(revealage, 1.0)) {
        discard;
    }

    vec4 accumulation = texture(uTransColor, pUV);

    if (isinf(max3(abs(accumulation.rgb)))) {
        accumulation.rgb = vec3(accumulation.a);
    }

    vec3 avgColor = accumulation.rgb / max(accumulation.a, EPSILON);
    oFinalColor = vec4(avgColor, 1.0 - revealage);
}
