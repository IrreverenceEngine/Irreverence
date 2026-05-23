#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_FRAG

#include "common.glsl"
#include "lighting.glsl"

layout (location = 0) out vec3 oPosition;
layout (location = 1) out vec3 oNormal;
layout (location = 2) out vec3 oColor;
layout (location = 3) out vec3 oSDE;
layout (location = 4) out vec2 oSSAO;

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

    vec3 normal = GetNormalFromMap(pFragPos, pNormal, pUV, GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_NORMAL));
    vec3 sde = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_SDE), pUV).rgb;

    oPosition = pFragPos;
    oNormal = normal;
    oColor = albedo.rgb;
    oSDE = sde;
}
