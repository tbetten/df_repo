#pragma once

namespace ecs
{
	class Entity_manager;
	enum class Component_type : size_t { Position, Drawable, Facing, Character, Item, Container, Attributes, Animation, Tile_type, Lightsource, Sensor, Reactor, Armour, Inventory, Equippable, Skill };
}

struct Tilemap;
namespace comps
{
	void add_components (ecs::Entity_manager* mgr);
}
