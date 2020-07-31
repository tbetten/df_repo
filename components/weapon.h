#pragma once
#include <string>

struct Weapon
{
	enum class Weapon_type { Melee, Ranged };
	std::string name;
};