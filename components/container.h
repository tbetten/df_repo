#pragma once
#include <vector>
#include "ecs.h"

struct Container
{
	void reset();
	std::vector<ecs::Entity_id> contents;
	int weight;  // weight of the container itself
	int total_weight;  // weight of container and contents
};