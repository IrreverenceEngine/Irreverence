#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_FRAG

#include "common.glsl"
#include "lighting.glsl"

out vec4 FRAG_COLOR;

in VP_Shared {
	vec3 pFragPos;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
	flat uint pMaterialIndex;};

void main()
{
    vec4 albedo = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_ALBEDO), pUV) * pInstanceColor;
    vec3 albedoCol = pow(albedo.rgb, vec3(2.2));
    vec3 normal = GetNormalFromMap(pFragPos, pNormal, pUV, GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_NORMAL));
    float metallic = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_METALNESS), pUV).r;
    float roughness = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_ROUGHNESS), pUV).r;
    float ao = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_AMBIENTOCCLUSION), pUV).r;
    float emissive = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_EMISSIVENESS), pUV).r;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedoCol, metallic);

    vec3 V = normalize(uCommon.ViewPosition - pFragPos);

    vec3 lightTotal = vec3(0);
    for (uint i = 0; i < uPointlightNum; i++) {
        Pointlight light = uPointlights[i];
        vec4 col = GetColorRGBA8(light.color);

        lightTotal += CalcPointLight(
            normal, V,
            light.position, col.rgb * col.a * 4.0, light.innerRadius, light.outerRadius,
            pFragPos,
            albedoCol, metallic, roughness, F0
        );
    }

    vec3 color = vec3(0.01) * albedoCol + lightTotal;

    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0 / 2.2));

    FRAG_COLOR = vec4(color, albedo.a);
}