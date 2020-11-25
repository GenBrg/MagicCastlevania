#pragma once

#include <engine/Singleton.hpp>

#include <random>

class Random : public Singleton<Random> {
public:
	/**
	 * Generate a random floating point number between [0.0, 1.0)
	 */
	float Generate()
	{
		return dist(mt);
	}

private:
	std::random_device rd {};
	std::mt19937 mt {rd()};
	std::uniform_real_distribution<float> dist { 0.0f, 1.0f };
};