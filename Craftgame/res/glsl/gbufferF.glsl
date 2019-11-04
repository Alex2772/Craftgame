#version 330 core

#define SHADOW_SMOOTH 2
#define PI 3.141592

struct LightSource {
	vec3 pos;
	vec3 color;
	float distance;
};
in vec2 pass_uv;
in vec4 gl_FragCoord;

uniform int addtive;
uniform vec2 screen;
uniform vec3 viewPos;
uniform mat4 ViewProjection;
uniform mat4 InverseViewProjection;
uniform sampler2D depth;
uniform sampler2D diffuse;
uniform sampler2D normal;
uniform sampler2D f0;
uniform sampler2DArray shadow1;
uniform mat4 lightSpaceMatrix1;
uniform mat4 lightSpaceMatrix2;
uniform int sun;
uniform int shadows;

uniform LightSource light;

out vec4 color;



float GetAttenuation(float distance, float lightInnerR, float invLightOuterR)
{
	float d = max(distance, lightInnerR);
	return clamp(1.0 - pow(d / invLightOuterR, 4.0), 0, 1) / (d * d + 1.0);
}

vec3 ReconstructPosition(vec2 uv, float _depth) {
	vec4 pos = InverseViewProjection * vec4(uv * 2 - 1, _depth * 2 - 1, 1.0);
	pos /= pos.w;
	return pos.xyz;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float a) // D
{
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}
float GeometrySchlickGGX(float NdotV, float k)
{
	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float k)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx1 = GeometrySchlickGGX(NdotV, k);
	float ggx2 = GeometrySchlickGGX(NdotL, k);

	return ggx1 * ggx2;
}

void main() {
	vec2 uv = gl_FragCoord.xy / screen;


	vec4 diff = texture(diffuse, uv);
	diff.xyz = pow(diff.xyz, vec3(2.2));

	if (addtive == 0) {
		if (diff == vec4(0, 0, 0, 0)) {
			color = vec4(0, 0, 0, 0);
		}
		else {
			color = vec4(diff.xyz, 1);
		}
	}
	else {
		if (diff.a == 0) {
			color = vec4(1, 1, 1, 0);
			return;
		}
#if GRAPHICS == 2
		float _depth = texture(depth, uv).r;

		// Расчёт координат по глубине

		vec3 pos = ReconstructPosition(uv, _depth);


		vec3 _f0;
		float roughness;
		{
			vec4 temp = texture(f0, uv);
			_f0 = temp.xyz;
			roughness = temp.a;
		}

		// =============

		vec3 n = normalize(texture(normal, uv).xyz * 2 - 1);
		vec3 toCamera = normalize(viewPos - pos.xyz);

		// Освещение

		float NdotV = max(dot(n.xyz, toCamera), 0);
		vec3 toLight = (light.pos - pos.xyz);


		float lightLen = length(toLight);
		float rangeCheck = 0;
		bool sunCheck = sun == 1 && light.pos.y > 0;
		if (sunCheck)
		{
			if (shadows == 1) {
				vec4 aPos1 = lightSpaceMatrix1 * vec4(pos, 1);
				vec3 projCoords1 = aPos1.xyz / aPos1.w;
				projCoords1 = projCoords1 * 0.5 + 0.5;
				float currentDepth1 = projCoords1.z;

				vec4 aPos2 = lightSpaceMatrix2 * vec4(pos, 1);
				vec3 projCoords2 = aPos2.xyz / aPos2.w;
				projCoords2 = projCoords2 * 0.5 + 0.5;
				float currentDepth2 = projCoords2.z;

				vec2 texSize = 1.f / vec2(4096, 4096);
				for (int x = -SHADOW_SMOOTH; x <= SHADOW_SMOOTH; ++x)
				{
					for (int y = -SHADOW_SMOOTH; y <= SHADOW_SMOOTH; ++y)
					{
						vec2 uv = projCoords1.xy + vec2(x, y) * texSize;
						bool first = uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1;
						float closestDepth = first ? texture(shadow1, vec3(uv, 0)).r : texture(shadow1, vec3(projCoords2.xy + vec2(x, y) * texSize, 1)).r;
						float currentDepth = first ? currentDepth1 : currentDepth2;
						rangeCheck += (currentDepth <= closestDepth ? 1.f : 0.f);
					}
				}

				rangeCheck /= (SHADOW_SMOOTH * 2 + 1) * (SHADOW_SMOOTH * 2 + 1);
			}
			else {
				rangeCheck = 1.f;
			}
		}
		else {
			rangeCheck = GetAttenuation(lightLen, 1.f, light.distance);
		}
		if (sun == 1) {
			rangeCheck *= clamp(dot(normalize(toLight), vec3(0, 1, 0)) * 10, 0, 1);
		}
		if (rangeCheck <= 0) {
			color = vec4(1, 1, 1, 1);
			return;
		}
		toLight = normalize(toLight);
		vec3 halfVector = normalize(toLight + toCamera);
		vec3 _frsSchlick = fresnelSchlick(NdotV, _f0); // F
		vec3 color3, vSpecular;

		color3 = (light.color * max(dot(n.xyz, toLight), 0) * rangeCheck) * 3;
		vec3 CookTorrance;
		float G = GeometrySmith(n, toCamera, toLight, roughness);
		float D = DistributionGGX(n, halfVector, roughness);
		CookTorrance = max((G * D * _frsSchlick) / (4 * NdotV * dot(n, toLight)), 0);
		if (sunCheck)
			vSpecular = light.color * rangeCheck * CookTorrance;
		else
			vSpecular = light.color * min(rangeCheck / 0.05, 1) * CookTorrance;
		color3 = max(color3, vec3(0, 0, 0));
		color3 += max(vSpecular, vec3(0, 0, 0)) + vec3(1, 1, 1);
		color = vec4(color3, 1);
#else
		color = vec4(1, 1, 1, 1);
#endif
	}
}
