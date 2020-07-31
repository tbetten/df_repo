#pragma once
#include <string>

enum class Race : int { Human, Dwarf, Halfling };
enum class Gender : int { Male, Female };
enum class Occupation {};


Race string_to_race(std::string race);
std::string race_to_string(Race r);

struct Character
{
	enum class Encumbrance { No, Light, Medium, Heavy, Extra_heavy };
	void reset();
	unsigned int character_points;
	std::string name;
	Gender gender;
	bool user_controlled{ false };
	Race race;
	Occupation occupation;
	Encumbrance encumbrance;
	int hitpoints;
	int fatigue_points;
};