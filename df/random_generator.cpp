#include "random_generator.h"
#include <algorithm>

std::random_device Random_generator::m_rd{};
std::mt19937 Random_generator::m_generator{ m_rd() };

int Random_generator::generate_discrete(const std::vector<int>& probabilities)
{
	std::discrete_distribution<> d {std::cbegin(probabilities), std::cend(probabilities)};
	return d(m_generator);
}

int Random_generator::generate_uniform(const int min, const int max)
{
	std::uniform_int_distribution<> d(min, max);
	return d(m_generator);
}