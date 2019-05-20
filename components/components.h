#pragma once
#include "component.h"

#include <memory>
#include <SFML/System/Vector2.hpp>

namespace ecs
{
	enum class Component_type : size_t { Position, Drawable, Facing, Character, Item };
}

struct Shared_context;
struct Tilemap;
namespace comps
{
	void add_components (ecs::Entity_manager* mgr);
	ecs::Entity_id spawn_from_key (Shared_context* context, std::string key, sf::Vector2u coords, std::shared_ptr<Tilemap> map);
}
