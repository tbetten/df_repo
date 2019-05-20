#pragma once
#include "ecs_types.h"
#include "directions.h"
//enum class Move_direction { Forward, Right_forward, Southeast, Backward, Southwest, Left_forward };

struct Move_payload
{
	ecs::Entity_id entity;
	Direction direction;
};