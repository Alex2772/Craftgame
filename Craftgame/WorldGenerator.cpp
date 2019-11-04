#include "WorldGenerator.h"
#include "Chunk.h"
#include "World.h"
#include "GameEngine.h"
# include <random>


WorldGenerator::WorldGenerator()
{
}


double WorldGenerator::getNoise(int x, int z)
{
	return pn.noise0_1(x / 128.0, z / 128.0);
}

double WorldGenerator::getSmoothNoise(int x, int z)
{
	const int SMOOTH = 2;

	const int SMOOTH_SQ = (SMOOTH * 2 + 1) * (SMOOTH * 2 + 1);
	double sum = 0.f;
	for (int px = x - SMOOTH; px <= x + SMOOTH; ++px)
		for (int pz = z - SMOOTH; pz <= z + SMOOTH; ++pz)
			sum += getNoise(px, pz);
	sum /= SMOOTH_SQ;
	return sum;
}
double interpolate(double a, double b, double blend)
{
	double theta = blend * glm::pi<double>();
	double f = 1.f - cos(theta) * 0.5;
	return a * (1 - f) + b * f;
}
double WorldGenerator::getInterpolatedNoise(double x, double z)
{
	int intX = int(floor(x));
	int intZ = int(floor(z));

	double fracX = x - intX;
	double fracZ = z - intZ;
	double v1 = getSmoothNoise(intX, intZ);
	double v2 = getSmoothNoise(intX + 1, intZ);
	double v3 = getSmoothNoise(intX, intZ + 1);
	double v4 = getSmoothNoise(intX + 1, intZ + 1);
	double i1 = interpolate(v1, v2, fracX);
	double i2 = interpolate(v3, v4, fracX);
	return interpolate(i1, i2, fracZ);
}
int WorldGenerator::getHeight(int x, int z)
{
	const double AMPLITUDE = 132.0;
	
	return getInterpolatedNoise(x, z) * AMPLITUDE + 40.0;
}

Block* WorldGenerator::getBlock(Pos p, int height)
{

	//p.y -= sin(p.x / 10.f) * cos(p.z / 10.f) * 10.f;
	if (p.y < (height - 5) || (height > 120 && p.y == height && pn.noise0_1(p.x / 64.0, p.y / 64.0, p.z / 64.0) > glm::clamp(60.0 / double(height), 0.0, 1.0)))
	{
		static Block* b = CGE::instance->goRegistry->getBlock(_R("craftgame:blocks/stone"));
		return b;
	}
	else if (p.y < height || (p.y == height && height < 64)) {
		static Block* b = CGE::instance->goRegistry->getBlock(_R("craftgame:blocks/dirt"));
		return b;
	}
	else if (p.y == height)
	{
		static Block* b = CGE::instance->goRegistry->getBlock(_R("craftgame:blocks/grass"));
		return b;
	}
	static Block* b = CGE::instance->goRegistry->getBlock(_R("craftgame:blocks/air"));

	return b;
}

/**
 * \brief Генерирует чанк
 * \param pos Координаты чанка в сетке чанков
 */
Chunk* WorldGenerator::generate(Pos& pos)
{
	Chunk* c = new Chunk(worldObj, pos);
	for (unsigned char x = 0; x < 16; x++)
		for (unsigned char z = 0; z < 16; z++) {
			Pos p = World::convertBS2WS(pos, Pos(x, 0, z));
			int height = getHeight(p.x, p.z);
			for (unsigned char y = 0; y < 16; y++) {
				c->setBlock(getBlock({ p.x, p.y + y, p.z}, height), Pos(x, y, z));
			}
		}
	c->modified = false;
	return c;
}
