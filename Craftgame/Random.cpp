#include "Random.h"

int Random::nextInt()
{
	return rand();
}

int Random::nextInt(int max)
{
	return nextInt() % max;
}

float Random::nextFloat()
{
	return nextInt(10000) / 10000.f;
}

bool Random::nextBool()
{
	return bool(nextInt(2));
}

double Random::nextDouble()
{
	return nextInt(10000) / 10000.0;
}

glm::vec2 Random::nextVec2()
{
	return { nextFloat(), nextFloat() };
}
glm::vec3 Random::nextVec3()
{
	return { nextFloat(), nextFloat(), nextFloat() };
}

glm::vec4 Random::nextVec4()
{
	return { nextFloat(), nextFloat(), nextFloat(), nextFloat() };
}

