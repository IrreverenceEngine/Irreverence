#version 460
#extension GL_ARB_shading_language_include : require
#define STAGE_VERT

#include "common.glsl"

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

out VP_Shared {
    vec2 pUV;
};

void main()
{
	gl_Position = uCommon.Projection * vec4(aPos * vec2(uCommon.Width, uCommon.Height), 0.0, 1.0);
    pUV = aUV;
}
