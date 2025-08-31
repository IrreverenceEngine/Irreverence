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
	flat uint pMaterialIndex;
};

void main()
{
	InstanceMap instData = uInstanceMap[gl_BaseInstance];

	pFragPos = aPos;

	vec4 viewPos = uCommon.View * vec4(pFragPos, 1.0);
	gl_Position = uCommon.Perspective * viewPos;

	pNormal = aNormal;
	pUV = aUV;
	pMaterialIndex = instData.matIndex;
}
