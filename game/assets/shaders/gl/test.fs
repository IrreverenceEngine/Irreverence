#version 450

out vec4 FRAG_COLOR;

layout(binding = 0) uniform sampler2D uTexture;

in VP_Shared {
	vec3 pFragPos;
	float pDepth;
    vec3 pNormal;
    vec2 pUV;
	vec4 pInstanceColor;
};

void main()
{
    FRAG_COLOR = pInstanceColor * texture(uTexture, pUV);
}