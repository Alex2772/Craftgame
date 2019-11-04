#version 330 core

#define PI 3.141592

#define SAMPLE_SIZE 2.5f
#define NEAR  0.05f
#define FAR   500.f
#define iSteps 8
#define jSteps 4

in vec2 pass_uv;
in vec4 gl_FragCoord;
uniform sampler2D diffuse;
uniform sampler2D depth;
uniform sampler2D normal;
uniform sampler2D noise;
uniform sampler2D f0;
uniform vec2 screen;
uniform vec3 viewPos;
uniform vec3 sun;
uniform mat4 ViewProjection;
uniform mat4 InverseViewProjection;
uniform mat4 InverseProjection;
uniform mat4 View;
uniform mat4 vp;
uniform int bskybox;

uniform vec3 kernel[32];

out vec4 _color;

// =================================
//           SKY PASS
// =================================

vec2 rsi(vec3 r0, vec3 rd, float sr) {
	// ray-sphere intersection that assumes
	// the sphere is centered at the origin.
	// No intersection when result.x > result.y
	float a = dot(rd, rd);
	float b = 2.0 * dot(rd, r0);
	float c = dot(r0, r0) - (sr * sr);
	float d = (b*b) - 4.0*a*c;
	if (d < 0.0) return vec2(1e5, -1e5);
	return vec2(
		(-b - sqrt(d)) / (2.0*a),
		(-b + sqrt(d)) / (2.0*a)
	);
}

vec3 atmosphere(vec3 r, vec3 r0, vec3 pSun, float iSun, float rPlanet, float rAtmos, vec3 kRlh, float kMie, float shRlh, float shMie, float g) {
	// Normalize the sun and view directions.
	pSun = normalize(pSun);
	r = normalize(r);

	// Calculate the step size of the primary ray.
	vec2 p = rsi(r0, r, rAtmos);
	if (p.x > p.y) return vec3(0, 0, 0);
	p.y = min(p.y, rsi(r0, r, rPlanet).x);
	float iStepSize = (p.y - p.x) / float(iSteps);

	// Initialize the primary ray time.
	float iTime = 0.0;

	// Initialize accumulators for Rayleigh and Mie scattering.
	vec3 totalRlh = vec3(0, 0, 0);
	vec3 totalMie = vec3(0, 0, 0);

	// Initialize optical depth accumulators for the primary ray.
	float iOdRlh = 0.0;
	float iOdMie = 0.0;

	// Calculate the Rayleigh and Mie phases.
	float mu = dot(r, pSun);
	float mumu = mu * mu;
	float gg = g * g;
	float pRlh = 3.0 / (16.0 * PI) * (1.0 + mumu);
	float pMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));

	// Sample the primary ray.
	for (int i = 0; i < iSteps; i++) {

		// Calculate the primary ray sample position.
		vec3 iPos = r0 + r * (iTime + iStepSize * 0.5);

		// Calculate the height of the sample.
		float iHeight = length(iPos) - rPlanet;

		// Calculate the optical depth of the Rayleigh and Mie scattering for this step.
		float odStepRlh = exp(-iHeight / shRlh) * iStepSize;
		float odStepMie = exp(-iHeight / shMie) * iStepSize;

		// Accumulate optical depth.
		iOdRlh += odStepRlh;
		iOdMie += odStepMie;

		// Calculate the step size of the secondary ray.
		float jStepSize = rsi(iPos, pSun, rAtmos).y / float(jSteps);

		// Initialize the secondary ray time.
		float jTime = 0.0;

		// Initialize optical depth accumulators for the secondary ray.
		float jOdRlh = 0.0;
		float jOdMie = 0.0;

		// Sample the secondary ray.
		for (int j = 0; j < jSteps; j++) {

			// Calculate the secondary ray sample position.
			vec3 jPos = iPos + pSun * (jTime + jStepSize * 0.5);

			// Calculate the height of the sample.
			float jHeight = length(jPos) - rPlanet;
			// Accumulate the optical depth.
			jOdRlh += exp(-jHeight / shRlh) * jStepSize;
			jOdMie += exp(-jHeight / shMie) * jStepSize;

			// Increment the secondary ray time.
			jTime += jStepSize;
		}

		// Calculate attenuation.
		vec3 attn = exp(-(kMie * (iOdMie + jOdMie) + kRlh * (iOdRlh + jOdRlh)));
		// Accumulate scattering.
		totalRlh += odStepRlh * attn * 2;
		totalMie += odStepMie * attn * 2;

		// Increment the primary ray time.
		iTime += iStepSize;

	}
	vec3 night = pow(kRlh * 10000 * (max(1 - r.y, 0.6)), vec3(2));


	// Calculate and return the final color.
	return max(iSun * (pRlh * kRlh * totalRlh + pMie * kMie * totalMie), night);
}
vec4 atmo(vec3 ray) {
#if SKY == 1
	vec3 color = atmosphere(
		ray,           // normalized ray direction
		vec3(0, 6372e3, 0),               // ray origin
		sun,                        // position of the sun
		26.0,                           // intensity of the sun
		6371e3,                         // radius of the planet in meters
		6471e3,                         // radius of the atmosphere in meters
		vec3(5.5e-6, 13.0e-6, 22.4e-6), // Rayleigh scattering coefficient
		21e-6,                          // Mie scattering coefficient
		8e3,                            // Rayleigh scale height
		3.2e3,                          // Mie scale height
		0.758                           // Mie preferred scattering direction
	);
	// Apply exposure.
	color = 1.0 - exp(-1.0 * color);
	return vec4(color, 1);
#else
	return vec4(0.1, 0.2, 1, 1);
#endif
}
vec4 skybox_pass() {
	gl_FragDepth = 0.9999999f;
	vec4 ray = vp * vec4(pass_uv * 2 - 1, 0, 1);
	
	//for (int i = 0; i < starsCount; i++) {
	//	color = star(color, ray.xyz, stars[i], starsSize[i]);
	//}
	//color = star(color, ray.xyz, vec3(-0.123327, 0.899558, -0.419031));
	return atmo(ray.xyz);
}

// =================================
//           SSAO PASS
// =================================

vec2 texCoordScales;

float restoreViewSpaceDistance(in float depth)
{
	return (2.0 * NEAR * FAR) / (depth * (FAR - NEAR) - FAR - NEAR);
}

vec3 restoreViewSpacePosition(in vec2 texCoords, in float depth)
{
	return vec3(texCoords * texCoordScales, 1.0) * restoreViewSpaceDistance(depth);
}

vec3 randomVectorOnHemisphere(vec3 normal, vec3 n)
{
	return n * sign(dot(n, normal));
}
float projectViewSpaceDistance(float z)
{
	return (2.0 * NEAR * FAR / z + FAR + NEAR) / (FAR - NEAR);
}

vec3 projectViewSpacePosition(in vec3 viewSpace)
{
	return 0.5 + 0.5 * vec3((viewSpace.xy / texCoordScales) / viewSpace.z, projectViewSpaceDistance(viewSpace.z));
}

vec4 ssao(vec3 pos, vec3 normal, vec3 noise) {
	vec3 randomNormal = randomVectorOnHemisphere(normal, noise * SAMPLE_SIZE);
	vec3 projected = projectViewSpacePosition(pos + randomNormal);
	float sampledDepth = texture(depth, projected.xy).r;

	if (sampledDepth >= projected.z)
		return vec4(0.0);

	float rangeCheck = .7f / max(pow(abs(projected.z - sampledDepth) * 1000, 2), 1);
	vec4 r;
	r.a = rangeCheck;
	r.xyz = texture(diffuse, projected.xy).xyz * rangeCheck * 0.05f;
	return r;
}

vec4 ssao_pass() {
	float _depth = texture(depth, pass_uv).r * 2 - 1;
	texCoordScales = vec2(-InverseProjection[0][0], -InverseProjection[1][1]);
	vec3 n = (View * vec4(texture(normal, pass_uv).xyz * 2 - 1, 0)).xyz;
	//vec3 n = normalize(texture(normal, pass_uv).xyz * 2 - 1);

	vec3 pos = restoreViewSpacePosition(pass_uv * 2 - 1, _depth);
	vec4 environment = vec4(0);
	vec2 base_noise_uv = pass_uv * screen / 4;

	vec3 randomVec = texture(noise, base_noise_uv).xyz;

	vec3 tangent = normalize(randomVec - n * dot(randomVec, n));
	vec3 bitangent = cross(n, tangent);
	mat3 TBN = mat3(tangent, bitangent, n);

	for (int i = 0; i < SSAO_SAMPLES; ++i) {
		environment += ssao(pos, n, TBN * kernel[i]);
	}
	return vec4(environment.xyz / SSAO_SAMPLES, 1.f - environment.a / SSAO_SAMPLES);
}

// =================================
//          REFLECT PASS
// =================================

vec3 ReconstructPosition(vec2 uv, float _depth) {
	vec4 pos = InverseViewProjection * vec4(uv * 2 - 1, _depth * 2 - 1, 1.0);
	pos /= pos.w;
	return pos.xyz;
}
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 reflection_pass() {
	vec2 uv = gl_FragCoord.xy / screen;
	vec4 diff = texture(diffuse, uv);
	// Отражение

	vec3 color3 = vec3(0, 0, 0);


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
	if (roughness >= 1) {
		//return vec3(0, 0, 0);
	}

	// =============

	vec3 n = normalize(texture(normal, uv).xyz * 2 - 1);
	vec3 toCamera = normalize(viewPos - pos.xyz);

	vec3 _frsSchlick = fresnelSchlick(dot(n, toCamera), _f0); // F

	vec3 fromCamera = -toCamera;
	vec3 reflectDir = normalize(reflect(fromCamera, n));

	vec3 currentRay = vec3(0, 0, 0);

	vec3 nuv = vec3(0, 0, 0);
	float L = .1f;

	for (int i = 0; i < REFLECTIONS; ++i)
	{
		currentRay = pos + reflectDir * L;
		vec4 kek = vec4(ViewProjection * vec4(currentRay, 1));
		nuv = kek.xyz / kek.w; // проецирование позиции на экран
		nuv.xy = (nuv.xy + 1.0) / 2.0;
		float n = texture(depth, nuv.xy).r; // чтение глубины из DepthMap по UV

		vec3 newPosition = ReconstructPosition(nuv.xy, n);
		L = length(pos - newPosition);
	}


	L = (1 - clamp(L / 20.0, 0, 1));
	//float fresnel = 1 - clamp(pow(dot(fromCamera, n), 2) * 3, 0, 1);
	float K = pow(1 - abs((nuv.x * 2 - 1) * (nuv.y * 2 - 1)), 4);
	vec4 fc;
	if (nuv.x > 0 && nuv.x < 1 && nuv.y > 0 && nuv.y < 1) {
		fc = texture(diffuse, nuv.xy) * 2.f;
		if (fc.a <= 0)
			fc = atmo(n);
	}
	else {
		fc = atmo(n);
	}
	color3 = mix(color3, pow(fc.xyz, vec3(2.2)) * _frsSchlick, (1 - roughness) * 0.7 * K);
	return max(color3, vec3(0, 0, 0));
}


// =================================
//           POST PASS
// =================================

void main() {
	vec4 ghj = texture(diffuse, pass_uv);
	if (ghj.a > 0)
	{
		vec3 color = ghj.xyz;
#if REFLECTIONS > 0
		color += reflection_pass();
#endif
		//color = vec3(1, 1, 1);
#if SSAO_SAMPLES > 0
		if (ghj.a > 0) {
			vec4 sum = ssao_pass();

			//for (int x = -1; x <= 1; x++)
			//	for (int y = -1; y <= 1; y++)
			//		sum += texture(ssao, pass_uv + vec2(x, y) / screen);
			color += sum.xyz;
			color *= sum.a;
			//color.xyz = vec3(sum.a / 9, sum.a / 9, sum.a / 9);
		}
#endif

		color = pow(color, vec3(1.0 / 2.2));

		// COLOR CORRECTION
		color *= vec3(1.02, 0.99, 1.01);

		_color = vec4(color, 1);
	}
	else {
		if (bskybox == 1) {
			_color = skybox_pass();
		}
	}
}
