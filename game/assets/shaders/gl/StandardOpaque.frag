#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_FRAG

#include "common.glsl"
#include "lighting.glsl"

layout (location = 0) out vec3 oPosition;
layout (location = 1) out vec3 oNormal;
layout (location = 2) out vec3 oColor;
layout (location = 3) out vec4 oAMRE;
layout (location = 4) out vec2 oSSAO;
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

    vec3 normal = GetNormalFromMap(pFragPos, pNormal, pUV, GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_NORMAL));
    float metallic = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_METALNESS), pUV).r;
    float roughness = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_ROUGHNESS), pUV).r;
    float ao = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_AMBIENTOCCLUSION), pUV).r;
    float emissive = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_EMISSIVENESS), pUV).r;

    oPosition = pFragPos;
    oNormal = normal;
    oColor = albedo.rgb;
    oAMRE = vec4(ao, metallic, roughness, emissive);
}
