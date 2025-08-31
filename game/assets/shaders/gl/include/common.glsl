#define INCLUDE_COMMON

#extension GL_ARB_bindless_texture : require

// ----- [DEFINES] -----
const uint UNIFORM_COMMON = 0;
const uint UNIFORM_SSAOSAMPLES = 1;
const uint UNIFORM_ANIMATION = 2;

const uint STORAGE_MATERIALINFO = 0;
const uint STORAGE_TEXTUREHANDLE = 1;
const uint STORAGE_POINTLIGHTS = 2;
const uint STORAGE_SPOTLIGHTS = 3;
const uint STORAGE_INST_STANDARD = 16;
const uint STORAGE_INST_MAP = 17;

const uint MATERIAL_MAP_ALBEDO = 0;
const uint MATERIAL_MAP_NORMAL = 1;
const uint MATERIAL_MAP_METALNESS = 2;
const uint MATERIAL_MAP_ROUGHNESS = 3;
const uint MATERIAL_MAP_EMISSIVENESS = 4;
const uint MATERIAL_MAP_AMBIENTOCCLUSION = 5;
const uint MATERIAL_MAP_COUNT = 6;

const uint MAX_POINTLIGHTS = 2048;
const uint MAX_SPOTLIGHTS = 2048;

const float EPSILON = 0.00001;

// ----- [STRUCTS] -----
struct MaterialInfo {
    uint handleIndex[MATERIAL_MAP_COUNT];
};

struct Pointlight {
	vec3 position;
	float innerRadius;
    uint color;
	float outerRadius;
    uint _p[2];
};

struct Spotlight {
	vec3 position;
	float innerCutoff;
    vec3 direction;
	float outerCutoff;
	uint color;
	float innerRadius;
	float outerRadius;
    uint _p;
};

struct InstanceStandard {
    mat4 modelMatrix;
    uint color;
    uint matIndex;
    uint _p[2];
};

struct InstanceMap {
    uint matIndex;
};

// ----- [UNIFORMS] -----
layout(std140, binding = UNIFORM_COMMON) uniform UniformCommon {
    uint Width;
    uint Height;
    float CurTime;
    float FrameTime;
    mat4 View;
    mat4 Perspective;
    mat4 Ortho;
    vec3 ViewPosition;
} uCommon;

layout(std140, binding = UNIFORM_SSAOSAMPLES) uniform UniformSSAOSamples {
    vec4 samples[64];
} uSSAOSamples;

layout(std140, binding = UNIFORM_ANIMATION) uniform UniformAnimation {
    uint Dummy;
} uAnimation;

// ----- [STORAGES] -----
layout(std430, binding = STORAGE_MATERIALINFO) readonly buffer MaterialInfos {
    MaterialInfo uMaterial[];
};

layout(std430, binding = STORAGE_TEXTUREHANDLE) readonly buffer TextureHandles {
    uvec2 uTexHandles[];
};

layout(std430, binding = STORAGE_POINTLIGHTS) readonly buffer Pointlights {
    Pointlight uPointlights[MAX_POINTLIGHTS];
    uint uPointlightNum;
};

layout(std430, binding = STORAGE_SPOTLIGHTS) readonly buffer Spotlights {
    Spotlight uSpotlights[MAX_SPOTLIGHTS];
    uint uSpotlightNum;
};

layout(std430, binding = STORAGE_INST_STANDARD) readonly buffer InstanceStandards {
    InstanceStandard uInstanceStandard[];
};

layout(std430, binding = STORAGE_INST_MAP) readonly buffer InstanceMaps {
    InstanceMap uInstanceMap[];
};

// ----- [FUNCTIONS] -----

bool approxEq(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

float max3(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

sampler2D GetMaterialSampler(uint matIndex, uint mapIndex)
{
    return sampler2D(uTexHandles[uMaterial[matIndex].handleIndex[mapIndex]]);
}

vec4 GetColorRGBA8(uint rgba8)
{
    return vec4(
        float((rgba8 & 0xFFu)) / 255.0,
        float((rgba8 >> 8) & 0xFFu) / 255.0,
        float((rgba8 >> 16) & 0xFFu) / 255.0,
        float((rgba8 >> 24) & 0xFFu) / 255.0
    );
}

#ifdef STAGE_FRAG
vec3 GetNormalFromMap(vec3 fragPos, vec3 normal, vec2 uv, sampler2D sampler)
{
	vec3 tangentNormal = texture(sampler, uv).xyz * 2.0 - 1.0;

	vec3 Q1  = dFdx(fragPos);
	vec3 Q2  = dFdy(fragPos);
	vec2 st1 = dFdx(uv);
	vec2 st2 = dFdy(uv);

	vec3 N = normalize(normal);
	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

float CalcWBOITWeight(float alpha)
{
    return clamp(pow(min(1.0, alpha * 10.0) + 0.01, 3.0) * 1e8 * pow(1.0 - gl_FragCoord.z * 0.9, 3.0), 1e-2, 3e3);
}

vec4 CalcWBOITColor(vec3 color, float alpha, float weight)
{
    return vec4(color * alpha, alpha) * weight;
}
#endif
