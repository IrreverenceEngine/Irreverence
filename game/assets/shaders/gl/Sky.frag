#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_FRAG

#include "common.glsl"

layout (location = 2) out vec3 oColor;
layout (location = 7) out vec4 oFinalColor;

layout(binding = 0) uniform samplerCube uSkyTexture;

in VP_Shared {
    vec3 pUV;
};

void main()
{
    vec4 skyPaint = UnpackColor(uCommon.SkyPaintCol);
    vec4 skyHorizon = UnpackColor(uCommon.SkyHorizonCol);
    vec3 skyDir = normalize(pUV);

    vec3 skyMixPaint = mix(texture(uSkyTexture, pUV).rgb, skyPaint.rgb, skyPaint.a);
    vec3 skyMixHorizon = mix(texture(uSkyTexture, pUV).rgb, skyHorizon.rgb, skyPaint.a);

    oColor = mix(skyMixHorizon, skyMixPaint, clamp(skyDir.y - uCommon.SkyHorizonHeight, 0.0, 1.0));
}
