#ifndef INCLUDE_COMMON
#error You need to include common.glsl before lighting.glsl
#endif

float LightAttenuation(float dist, float innerRadius, float outerRadius)
{
    float t = clamp((dist - innerRadius) / (outerRadius - innerRadius), 0.0, 1.0);
    float invSq = 1.0 / (1.0 + 25.0 * t * t);
    return invSq * (1.0 - t);
}

vec3 CalcPointlight(
	vec3 N, vec3 V,
	vec3 lightPos, vec3 lightCol, float lightInner, float lightOuter,
	vec3 fragPos,
	vec3 albedo
)
{
    vec3 lightToPos = lightPos - fragPos;
    float dist = length(lightToPos);
    if (dist > lightOuter) {
        return vec3(0.0);
    }

    vec3 L = normalize(lightToPos);
    float NdotL = max(dot(N, L), 0.0);

    float attenuation = LightAttenuation(dist, lightInner, lightOuter);
    vec3 radiance = lightCol * attenuation;

    return radiance * NdotL;
}

vec3 CalcSpotlight(
	vec3 N, vec3 V,
	vec3 lightPos, vec3 lightCol, float lightInRad, float lightOutRad, vec3 lightDir, float lightInCut, float lightOutCut,
	vec3 fragPos,
	vec3 albedo
)
{
    vec3 lightToPos = lightPos - fragPos;
    float dist = length(lightToPos);
    if (dist > lightOutRad) {
        return vec3(0.0);
    }

    vec3 L = normalize(lightToPos);
    float NdotL = max(dot(N, L), 0.0);

	float theta = dot(L, normalize(-lightDir));
    if (theta <= lightOutCut) {
        return vec3(0.0);
    }

	float epsilon = lightInCut - lightOutCut;
	float intensity = clamp((theta - lightOutCut) / epsilon, 0.0, 1.0);

    float attenuation = LightAttenuation(dist, lightInRad, lightOutRad) * intensity;
    vec3 radiance = lightCol * attenuation;

    return radiance * NdotL;
}

vec3 CalcAllLights(vec3 albedoCol, vec3 fragPos, vec3 normal)
{
    vec3 V = normalize(uCommon.ViewPosition - fragPos);

    vec3 lightTotal = vec3(0);
    for (uint i = 0; i < uPointlightNum; i++) {
        Pointlight light = uPointlights[i];
        vec4 col = UnpackColor(light.color);

        lightTotal += CalcPointlight(
            normal, V,
            light.position, col.rgb * col.a * 8.0, light.innerRadius, light.outerRadius,
            fragPos,
            albedoCol
        );
    }

    for (uint i = 0; i < uSpotlightNum; i++) {
        Spotlight light = uSpotlights[i];
        vec4 col = UnpackColor(light.color);

        lightTotal += CalcSpotlight(
            normal, V,
            light.position, col.rgb * col.a * 8.0, light.innerRadius, light.outerRadius,
            light.direction, light.innerCutoff, light.outerCutoff,
            fragPos,
            albedoCol
        );
    }

    return lightTotal;
}
