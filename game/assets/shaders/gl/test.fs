#version 450
#extension GL_ARB_bindless_texture : require

out vec4 FRAG_COLOR;

layout(binding = 0) uniform sampler2D uTexture;

in VP_Shared {
	vec3 pFragPos;
	float pDepth;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
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
    MaterialInfo mat = uMaterial[0];
    FRAG_COLOR = pInstanceColor * vec4(texture(GetMaterialSampler(0, 0), pUV).rgb, 1.0);
}