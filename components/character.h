#pragma once
#include <string>

enum class Race { Human, Dwarf, Halfling };
enum class Occupation {};
enum class Encumbrance {No, Light, Medium, Heavy, Extra_heavy};

struct Character
{
	void reset();
	std::string name;
	bool user_controlled{ false };
	Race race;
	Occupation occupation;
	Encumbrance encumbrance;
	int hitpoints;
	int fatigue_points;
};