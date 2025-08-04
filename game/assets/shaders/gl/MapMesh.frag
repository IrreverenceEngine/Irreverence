#version 460
#extension GL_ARB_shading_language_include : require

#include "common.glsl"

out vec4 FRAG_COLOR;

in VP_Shared {
	vec3 pFragPos;
    vec3 pNormal;
    vec2 pUV;
	flat uint pMaterialIndex;
};

void main()
{
    FRAG_COLOR = texture(GetMaterialSampler(pMaterialIndex, MATERIAL_MAP_ALBEDO), pUV);
}
