#ifndef INCLUDE_COMMON
#error You need to include common.glsl before lighting.glsl
#endif

const float L_PI = 3.14159265359;

float DistributionGGX(vec3 normal, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(normal, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = L_PI * denom * denom;

	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float nom   = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 normal, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(normal, V), 0.0);
	float NdotL = max(dot(normal, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

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
	vec3 albedo, float metallic, float roughness, vec3 F0
)
{
    vec3 lightToPos = lightPos - fragPos;
    float dist = length(lightToPos);
    if (dist > lightOuter) {
        return vec3(0.0);
    }

    vec3 L = normalize(lightToPos);
    float NdotL = max(dot(N, L), 0.0);
    if (NdotL <= 0.0) {
        return vec3(0.0);
    }
    vec3 H = normalize(V + L);

    float attenuation = LightAttenuation(dist, lightInner, lightOuter);
    vec3 radiance = lightCol * attenuation;

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    return (kD * albedo / L_PI + specular) * radiance * NdotL;
}

vec3 CalcSpotlight(
	vec3 N, vec3 V,
	vec3 lightPos, vec3 lightCol, float lightInRad, float lightOutRad, vec3 lightDir, float lightInCut, float lightOutCut,
	vec3 fragPos,
	vec3 albedo, float metallic, float roughness, vec3 F0
)
{
    vec3 lightToPos = lightPos - fragPos;
    float dist = length(lightToPos);
    if (dist > lightOutRad) {
        return vec3(0.0);
    }

    vec3 L = normalize(lightToPos);
    float NdotL = max(dot(N, L), 0.0);
    if (NdotL <= 0.0) {
        return vec3(0.0);
    }
    vec3 H = normalize(V + L);

	float theta = dot(L, normalize(-lightDir));
    if (theta <= lightOutCut) {
        return vec3(0.0);
    }

	float epsilon = lightInCut - lightOutCut;
	float intensity = clamp((theta - lightOutCut) / epsilon, 0.0, 1.0);

    float attenuation = LightAttenuation(dist, lightInRad, lightOutRad) * intensity;
    vec3 radiance = lightCol * attenuation;

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    return (kD * albedo / L_PI + specular) * radiance * NdotL;
}

vec3 CalcAllLights(vec3 albedoCol, vec3 fragPos, vec3 normal, float ao, float metallic, float roughness)
{
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedoCol, metallic);

    vec3 V = normalize(uCommon.ViewPosition - fragPos);

    vec3 lightTotal = vec3(0);
    for (uint i = 0; i < uPointlightNum; i++) {
        Pointlight light = uPointlights[i];
        vec4 col = GetColorRGBA8(light.color);

        lightTotal += CalcPointlight(
            normal, V,
            light.position, col.rgb * col.a * 8.0, light.innerRadius, light.outerRadius,
            fragPos,
            albedoCol, metallic, roughness, F0
        );
    }

    for (uint i = 0; i < uSpotlightNum; i++) {
        Spotlight light = uSpotlights[i];
        vec4 col = GetColorRGBA8(light.color);

        lightTotal += CalcSpotlight(
            normal, V,
            light.position, col.rgb * col.a * 8.0, light.innerRadius, light.outerRadius,
            light.direction, light.innerCutoff, light.outerCutoff,
            fragPos,
            albedoCol, metallic, roughness, F0
        );
    }

    return lightTotal;
}
