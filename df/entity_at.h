#pragma once

#include "ecs.h"
#include "position.h"

#include <vector>
#include <SFML/System.hpp>

std::vector<ecs::Entity_id> get_entity_at(ecs::Entity_manager* mgr, std::string map, Position::Layer layer, sf::Vector2i coords);
