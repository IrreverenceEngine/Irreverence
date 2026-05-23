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
    vec3 sde = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_SDE), pUV).rgb;
    
    vec3 lightTotal = albedoCol * CalcAllLights(albedoCol, pFragPos, normal);

    vec3 color = mix(lightTotal, albedoCol, sde.b);
    color = pow(color, vec3(1.0 / 2.2));
    
    float weight = CalcWBOITWeight(albedo.a);
    oTransColors = CalcWBOITColor(color, albedo.a, weight);
    oTransReveal = albedo.a;
}
