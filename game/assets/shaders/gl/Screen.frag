#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_FRAG

#include "common.glsl"
#include "lighting.glsl"

out vec4 FRAG_COLOR;

layout(binding = 0) uniform sampler2D oPosition;
layout(binding = 1) uniform sampler2D oNormal;
layout(binding = 2) uniform sampler2D oColor;
layout(binding = 3) uniform sampler2D oAMRE;

in VP_Shared {
    vec2 pUV;
};

void main()
{
    vec3 fragPos = texture(oPosition, pUV).rgb;
    vec3 albedoCol = pow(texture(oColor, pUV).rgb, vec3(2.2));
    vec3 normal = texture(oNormal, pUV).rgb;
    vec4 amre = texture(oAMRE, pUV);

    float ao = amre.r;
    float metallic = amre.g;
    float roughness = amre.b;
    float emissive = amre.a;

    vec3 lightTotal = CalcAllLights(albedoCol, fragPos, normal, ao, metallic, roughness);

    vec3 color = mix(lightTotal, albedoCol, emissive);
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FRAG_COLOR = vec4(color, 1);
}
