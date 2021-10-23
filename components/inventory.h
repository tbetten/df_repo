#pragma once
#include <array>
#include <map>
#include <optional>
#include "ecs_types.h"
#include "container.h"

struct Inventory
{
	Inventory ();
	void reset () {}
	void load (const std::string& key, Shared_context* context);

/*	enum class Inventory_slot { None, Head, Necklace, Torso, Hands, Ring_left, Ring_right, Cloak, Feet, Left_hand, Right_hand };
	inline static std::map<std::string, Inventory_slot> string_to_slot { {"None", Inventory_slot::None}, {"Head", Inventory_slot::Head}, 
		{"Necklace", Inventory_slot::Necklace}, { "Torso", Inventory_slot::Torso }, {"Hands", Inventory_slot::Hands}, {"Ring_left", Inventory_slot::Ring_left},
		{"Ring_right", Inventory_slot::Ring_right}, {"Cloak", Inventory_slot::Cloak}, {"Feet", Inventory_slot::Feet}, {"Left_hand", Inventory_slot::Left_hand},
		{"Right_hand", Inventory_slot::Right_hand} };*/
	std::array<std::optional<ecs::Entity_id>, 10> inventory;
	Container bag;
};