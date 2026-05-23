#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_FRAG

#include "common.glsl"
#include "lighting.glsl"

layout (location = 0) out vec3 oPosition;
layout (location = 1) out vec3 oNormal;
layout (location = 2) out vec3 oColor;
layout (location = 3) out vec4 oSDE;
layout (location = 4) out vec2 oSSAO;
layout (location = 5) out vec4 oTransColors;
layout (location = 6) out float oTransReveal;

in VP_Shared {
	vec3 pFragPos;
    vec3 pNormal;
    vec2 pUV;
	flat uint pMaterialIndex;
};

void main()
{
    vec4 albedo = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_ALBEDO), pUV);
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
