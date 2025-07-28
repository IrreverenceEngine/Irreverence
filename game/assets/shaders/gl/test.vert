#version 460

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

layout(std140, binding = 0) uniform CommonUniform
{
	uint Width;
	uint Height;
	float CurTime;
	float FrameTime;
	mat4 View;
	mat4 Projection;
	float Near;
	float Far;
} uCommon;

out VP_Shared {
	vec3 pFragPos;
	float pDepth;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
	flat uint pMaterialIndex;
};

struct InstanceStandard {
	vec4 color;
	mat4 modelMatrix;
	uint matIndex;
	uint padding[3];
};

struct InstanceMap {
	uint matIndex;
};

layout(std430, binding = 3) readonly buffer InstanceStandards {
    InstanceMap uInstanceMap[];
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