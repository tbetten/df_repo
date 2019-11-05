#pragma once
#include <string>

enum class Race { Human, Dwarf, Halfling };
enum class Occupation {};
enum class Encumbrance {No, Light, Medium, Heavy, Extra_heavy};

Race string_to_race(std::string race);
std::string race_to_string(Race r);

struct Character
{
	void reset();
	unsigned int character_points;
	std::string name;
	bool male;
	bool user_controlled{ false };
	Race race;
	Occupation occupation;
	Encumbrance encumbrance;
	int hitpoints;
	int fatigue_points;
};