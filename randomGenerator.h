#pragma once
#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H
#include <random>
#include <ctime>

// Objeto que genera números aleatorios entre dos números.
class randomGenerator {

	std::mt19937 gen;
	std::uniform_real_distribution<double> dist;

public:
	// Constructor.
	randomGenerator(double a, double b) : gen(std::time(0)), dist(a, b) {
		srand(time(NULL));
	}

	// Devuelve un número aleatorio antre los límites del generador.
	double get() { return dist(gen); }

};

#endif // RANDOMGENERATOR_H