#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_FRAG

#include "common.glsl"

out vec4 FRAG_COLOR;

layout(binding = 0) uniform sampler2D uFinalColor;

in VP_Shared {
    vec2 pUV;
};

void main()
{
    vec3 color = texture(uFinalColor, pUV).rgb;

    color = vec3(1.0) - exp(-color * 2.0);

    FRAG_COLOR = vec4(color, 1.0);
}
