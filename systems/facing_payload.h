#pragma once
#include "ecs_types.h"
//#include "directions.h"

enum class Compass : int;

struct Facing_payload
{
	ecs::Entity_id entity;
	Compass new_facing;
};