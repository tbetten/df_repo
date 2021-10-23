#pragma once
#include <string>
#include "ecs_types.h"

struct Buy_skill_payload
{
	ecs::Entity_id entity;
	std::string key;
	int points;
};