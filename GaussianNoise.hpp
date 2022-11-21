#ifndef _GAUSSIANNOISE_HPP_
#define _GAUSSIANNOISE_HPP_

#include "pch.h"
#include <random>

class GaussianNoise
{
public:
	GaussianNoise()
	{
	}

	~GaussianNoise()
	{
	}

	double noise()
	{
		return dist(generator);
	}

	double brownian()
	{
		counter += dist(generator);
		return counter;
	}

	void init()
	{
		seed = std::rand() + std::rand() + std::time(NULL);
		generator = std::default_random_engine(seed);
		counter = 0.0;
		const double mean = 0.0;
		const double stddev = 1.4;
		dist = std::normal_distribution<double>(mean, stddev);
	}

private:
	unsigned int seed;
	double counter;
	std::default_random_engine generator;
	std::normal_distribution<double> dist;
};


#endif