#ifndef SERVER

#include "BlurShader.h"
#include "global.h"
float* ComputeGaussianKernel(const int inRadius, const float inWeight)
{
	int mem_amount = (inRadius * 2) + 1;
	float* gaussian_kernel = (float*)malloc(mem_amount * sizeof(float));

	float twoRadiusSquaredRecip = 1.0 / (2.0 * inRadius * inRadius);
	float sqrtTwoPiTimesRadiusRecip = 1.0 / (sqrt(2.0 * M_PI) * inRadius);
	float radiusModifier = inWeight;

	// Create Gaussian Kernel
	int r = -inRadius;
	float sum = 0.0f;
	for (int i = 0; i < mem_amount; i++)
	{
		float x = r * radiusModifier;
		x *= x;
		float v = sqrtTwoPiTimesRadiusRecip * exp(-x * twoRadiusSquaredRecip);
		gaussian_kernel[i] = v;

		sum += v;
		r++;
	}

	// Normalize distribution
	float div = sum;
	for (int i = 0; i < mem_amount; i++)
		gaussian_kernel[i] /= div;

	return gaussian_kernel;
}
BlurShader::BlurShader(std::string v, std::string f) :
	Shader(v, f), radius(0) {
}
void BlurShader::setBlurRadius(int radius) {
	if (BlurShader::radius != radius && radius && radius % 2 && radius < 15) {
		BlurShader::radius = radius;
		float* d = ComputeGaussianKernel(radius, 2);
		loadArray("kernel", std::vector<float>(d, d + radius * 2 + 1));
		loadInt("kSize", radius);
		delete[] d;
	}
}

#endif