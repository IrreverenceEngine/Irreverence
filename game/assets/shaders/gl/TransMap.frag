#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_FRAG

#include "common.glsl"
#include "lighting.glsl"

layout (location = 0) out vec3 oPosition;
layout (location = 1) out vec3 oNormal;
layout (location = 2) out vec3 oColor;
layout (location = 3) out vec4 oAMRE;
layout (location = 4) out vec4 oTransColors;
layout (location = 5) out float oTransReveal;

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
    float metallic = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_METALNESS), pUV).r;
    float roughness = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_ROUGHNESS), pUV).r;
    float ao = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_AMBIENTOCCLUSION), pUV).r;
    float emissive = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_EMISSIVENESS), pUV).r;

    // Transparent Object, do Forward-ish
    emissive += 0.02;
    vec3 albedoCol = pow(albedo.rgb, vec3(2.2));

    vec3 lightTotal = CalcAllLights(albedoCol, pFragPos, normal, ao, metallic, roughness);

    vec3 color = emissive * albedoCol + lightTotal;
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    float weight = clamp(pow(min(1.0, albedo.a * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
    oTransColors = vec4(albedoCol.rgb * albedo.a, albedo.a) * weight;
    oTransReveal = albedo.a;
    
    oPosition = pFragPos;
    oNormal = normal;
    oColor = albedo.rgb;
    oAMRE = vec4(ao, metallic, roughness, emissive);
}
