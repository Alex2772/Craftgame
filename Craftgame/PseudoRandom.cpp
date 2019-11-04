#include <random>
#include "PseudoRandom.h"
#include <glm/glm.hpp>

PseudoRandom::PseudoRandom(double _c) :
	chance(_c) {
	cr = calculateCFromP(chance / 100.0) * 100;
	current = cr;
}

double PseudoRandom::calculatePFromC(double c)
{
	double procOnN = 0;
	double procByN = 0;
	double sum = 0;
	double dd = 1.0 / c;
	int maxFails = glm::ceil(dd);

	for (int n = 0; n < maxFails; n++) {
		procOnN = glm::min(1.0, n * c) * (1.0 - procByN);
		procByN += procOnN;
		sum += n * procOnN;
	}
	return 1.0 / sum;
}

double PseudoRandom::calculateCFromP(double p)
{
	double Cu = p;
	double Cl = 0;
	double Cm;

	double p1;
	double p2 = 1;

	while (1) {
		Cm = (Cu + Cl) / 2.0;
		p1 = calculatePFromC(Cm);
		if (abs(p1 - p2) <= 0.0)
			break;
		if (p1 > p)
			Cu = Cm;
		else
			Cl = Cm;
		p2 = p1;
	}
	return Cm;
}

bool PseudoRandom::next()
{
	double f = double(rand() % 100);
	if (current > f) {
		current = cr;
		return true;
	}
	else {
		current += cr;
		return false;
	}
}
