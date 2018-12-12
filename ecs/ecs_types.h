#pragma once
#include <bitset>

using Entity_id = unsigned int;
using Component_type = unsigned int;

constexpr int max_components = 32;
using Bitmask = std::bitset<max_components>;

enum class Component {Position, SpriteSheet, State, Item_shared, Projectile, Attributes, Drawable};
enum class System {Renderer, Movement, Collision, Attribute, Controller};
enum class Entity_event{Spawned, Despawned, };

inline unsigned int to_number (Component cmp)
{
	return static_cast<int>(cmp);
}

inline Component to_comp_type (unsigned int type)
{
	return static_cast<Component>(type);
}

enum class Direction { North, Northeast, Southeast, South, Southwest, Northwest };