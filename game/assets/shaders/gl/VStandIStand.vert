#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_VERT

#include "common.glsl"

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

out VP_Shared {
	vec3 pFragPos;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
	flat uint pMaterialIndex;
};

void main()
{
	InstanceStandard instData = uInstanceStandard[gl_BaseInstance];

	pFragPos = vec3(instData.modelMatrix * vec4(aPos, 1.0));

	vec4 viewPos = uCommon.View * vec4(pFragPos, 1.0);
	gl_Position = uCommon.Projection * viewPos;

	pNormal = transpose(mat3(inverse(instData.modelMatrix))) * aNormal;
	pUV = aUV;

	pInstanceColor = GetColorRGBA8(instData.color);
	pMaterialIndex = instData.matIndex;
}
