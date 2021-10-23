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
	int weight { 0 };  // weight of the container itself
	int total_weight { 0 };  // weight of container and contents
};