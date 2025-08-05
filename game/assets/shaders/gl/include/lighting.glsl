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
    return 1.0 - smoothstep(innerRadius, outerRadius, dist);
}

vec3 CalcPointLight(
	vec3 N, vec3 V,
	vec3 lightPos, vec3 lightCol, float lightInner, float lightOuter,
	vec3 fragPos,
	vec3 albedo, float metallic, float roughness, vec3 F0
)
{
    vec3 posDiff = lightPos - fragPos;

    vec3 L = normalize(posDiff);
    vec3 H = normalize(V + L);
    float dist = length(posDiff);
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

    float NdotL = max(dot(N, L), 0.0);

    return (kD * albedo / L_PI + specular) * radiance * NdotL;
}