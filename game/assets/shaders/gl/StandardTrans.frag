#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_FRAG

#include "common.glsl"
#include "lighting.glsl"

layout (location = 5) out vec4 oTransColors;
layout (location = 6) out float oTransReveal;

in VP_Shared {
	vec3 pFragPos;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
	flat uint pMaterialIndex;
};

void main()
{
    vec4 albedo = pInstanceColor * texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_ALBEDO), pUV);
    if (albedo.a < 0.01) {
        discard;
    }

    vec3 albedoCol = pow(albedo.rgb, vec3(2.2));
    vec3 normal = GetNormalFromMap(pFragPos, pNormal, pUV, GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_NORMAL));
    float metallic = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_METALNESS), pUV).r;
    float roughness = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_ROUGHNESS), pUV).r;
    float ao = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_AMBIENTOCCLUSION), pUV).r;
    float emissive = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_EMISSIVENESS), pUV).r;

    vec3 lightTotal = CalcAllLights(albedoCol, pFragPos, normal, ao, metallic, roughness);

    vec3 color = mix(lightTotal, albedoCol, emissive);
    color = pow(color, vec3(1.0 / 2.2));

    float weight = CalcWBOITWeight(albedo.a);
    oTransColors = CalcWBOITColor(color, albedo.a, weight);
    oTransReveal = albedo.a;
}
