#pragma once
#include "ecs_types.h"
#include <SFML/System/Vector2.hpp>

struct Pickup_payload
{
	ecs::Entity_id entity;
	sf::Vector2i coords;
};