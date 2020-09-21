#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct General
{
	std::string key;
	std::string name;
	std::string description;
	size_t weight;
	size_t value;
};

enum class Damage_type {};

struct Split_DR
{
	bool valid;
	Damage_type damage_type;
	int value;
};

using DR = std::variant<int, std::vector<Split_DR>>;

enum class Humanoid_hitlocation { Eye, Skull, Face, Right_leg, Left_leg, Right_arm, Left_arm, Torso, Groin, Left_hand, Right_hand, Left_foot, Right_foot, Neck, Vitals };
enum class Winged_humanoid_hitlocation { Eye, Skull, Face, Right_leg, Left_leg, Right_arm, Left_arm, Torso, Groin, Left_hand, Right_hand, Left_foot, Right_foot, Neck, Vitals, Left_wing, Right_wing };
using Hitlocation = std::variant<Humanoid_hitlocation, Winged_humanoid_hitlocation>;

enum class Armour_slot { Head, Torso, Hands, Feet, Amulet, Left_ring, Right_ring };

struct Armour
{
	enum class size { Human, Halfling, Dwarf };
	DR dr;
	std::vector<Hitlocation> hitlocations;
	Armour_slot armour_slot;
};

struct Item_data
{
	explicit Item_data (const std::string& key);
	std::unique_ptr<General> general;
	std::unique_ptr<Armour> armour;
};