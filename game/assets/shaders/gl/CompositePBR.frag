#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_FRAG

#include "common.glsl"
#include "lighting.glsl"

layout (location = 7) out vec4 oFinalColor;

layout(binding = 0) uniform sampler2D uPosition;
layout(binding = 1) uniform sampler2D uNormal;
layout(binding = 2) uniform sampler2D uColor;
layout(binding = 3) uniform sampler2D uSDE;
layout(binding = 4) uniform sampler2D uSSAO;
layout(binding = 5) uniform sampler2D uTransColor;
layout(binding = 6) uniform sampler2D uTransReveal;

in VP_Shared {
    vec2 pUV;
};

void main()
{
    vec3 fragPos = texture(uPosition, pUV).rgb;
    vec3 albedoCol = pow(texture(uColor, pUV).rgb, vec3(2.2));
    vec3 normal = texture(uNormal, pUV).rgb;
    vec4 asde = texture(uSDE, pUV);

    float emissive = asde.b;

    vec3 lightTotal = albedoCol * CalcAllLights(albedoCol, fragPos, normal);

    vec3 color = mix(lightTotal, albedoCol, emissive);
    color = pow(color, vec3(1.0 / 2.2));

    oFinalColor = vec4(color, 1.0);
}
