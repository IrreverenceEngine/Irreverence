#version 460
#extension GL_ARB_bindless_texture : require

out vec4 FRAG_COLOR;

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

in VP_Shared {
	vec3 pFragPos;
	float pDepth;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
	flat uint pMaterialIndex;
};

struct MaterialInfo {
    uint handleIndex[1];
};

layout(std430, binding = 0) readonly buffer MaterialInfos {
    MaterialInfo uMaterial[];
};

layout(std430, binding = 1) readonly buffer TextureHandles {
    uvec2 uTexHandles[];
};

sampler2D GetMaterialSampler(uint matIndex, uint mapIndex)
{
    return sampler2D(uTexHandles[uMaterial[matIndex].handleIndex[mapIndex]]);
}

void main()
{
    FRAG_COLOR = pInstanceColor * texture(GetMaterialSampler(pMaterialIndex, 0), pUV);
}