#pragma once

class PseudoRandom {
public:
	double chance;
	double cr;
	double current;
	PseudoRandom(double chance);
	double calculatePFromC(double c);
	double calculateCFromP(double p);
	bool next();
};