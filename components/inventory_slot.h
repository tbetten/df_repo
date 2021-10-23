#pragma once
#include <string>
#include <utility>
#include <vector>

namespace inventory
{
	enum class Inventory_slot { None, Head, Necklace, Torso, Hands, Ring_left, Ring_right, Cloak, Feet, Left_hand, Right_hand };

	using slot = std::pair<std::string, Inventory_slot>;
	inline static std::vector<slot> string_to_slot { {"None", Inventory_slot::None}, {"Head", Inventory_slot::Head},
			{"Necklace", Inventory_slot::Necklace}, { "Torso", Inventory_slot::Torso }, {"Hands", Inventory_slot::Hands}, {"Ring_left", Inventory_slot::Ring_left},
			{"Ring_right", Inventory_slot::Ring_right}, {"Cloak", Inventory_slot::Cloak}, {"Feet", Inventory_slot::Feet}, {"Left_hand", Inventory_slot::Left_hand},
			{"Right_hand", Inventory_slot::Right_hand} };
}