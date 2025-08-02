#version 460
#extension GL_ARB_shading_language_include : require

#include "common.glsl"

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

out VP_Shared {
	vec3 pFragPos;
	float pDepth;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
	flat uint pMaterialIndex;
};

void main()
{
	InstanceMap instData = uInstanceMap[gl_BaseInstance];

	pFragPos = aPos;

	vec4 viewPos = uCommon.View * vec4(pFragPos, 1.0);
	gl_Position = uCommon.Projection * viewPos;

    pDepth = 1.0 - (-viewPos.z - uCommon.Near) / (uCommon.Far - uCommon.Near);

	pNormal = aNormal;
	pUV = aUV;

	pInstanceColor = vec4(1.0);
	pMaterialIndex = instData.matIndex;
}