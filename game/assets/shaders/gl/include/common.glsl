#extension GL_ARB_bindless_texture : require

#define UNIFORM_COMMON 0
#define UNIFORM_ANIMATION 1

#define STORAGE_MATERIALINFO 0
#define STORAGE_TEXTUREHANDLE 1
#define STORAGE_INST_STANDARD 2
#define STORAGE_INST_MAP 3

#define MATERIAL_MAP_ALBEDO 0
#define MATERIAL_MAP_NORMAL 1
#define MATERIAL_MAP_AMRE 2
#define MATERIAL_MAP_COUNT 3

// Structs
struct MaterialInfo {
    uint handleIndex[MATERIAL_MAP_COUNT];
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

// Uniforms
layout(std140, binding = UNIFORM_COMMON) uniform UniformCommon {
    uint Width;
    uint Height;
    float CurTime;
    float FrameTime;
    mat4 View;
    mat4 Projection;
    float Near;
    float Far;
} uCommon;

layout(std140, binding = UNIFORM_ANIMATION) uniform UniformAnimation {
    uint Dummy;
} uAnimation;

// Storages
layout(std430, binding = STORAGE_MATERIALINFO) readonly buffer MaterialInfos {
    MaterialInfo uMaterial[];
};

layout(std430, binding = STORAGE_TEXTUREHANDLE) readonly buffer TextureHandles {
    uvec2 uTexHandles[];
};

layout(std430, binding = STORAGE_INST_STANDARD) readonly buffer InstanceStandards {
    InstanceStandard uInstanceStandard[];
};

layout(std430, binding = STORAGE_INST_MAP) readonly buffer InstanceMaps {
    InstanceMap uInstanceMap[];
};

// Functions
sampler2D GetMaterialSampler(uint matIndex, uint mapIndex)
{
    return sampler2D(uTexHandles[uMaterial[matIndex].handleIndex[mapIndex]]);
}