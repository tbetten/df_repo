#pragma once
#include <random>
#include <vector>

class Random_generator
{
public:
	static int generate_discrete(const std::vector<int>& probabilities);
	static int generate_uniform(const int min, const int max);
private:
	static std::random_device m_rd;
	static std::mt19937 m_generator;
};