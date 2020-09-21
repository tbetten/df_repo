#pragma once
#include <vector>
#include <string>
#include "ecs.h"

struct Shared_context;

struct Container
{
	void reset();
	void load (const std::string& key, Shared_context* context) {}

	std::vector<ecs::Entity_id> contents;
	int weight;  // weight of the container itself
	int total_weight;  // weight of container and contents
};