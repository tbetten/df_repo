#pragma once
#include <string>

enum class Race : int { Human, Dwarf, Halfling };
enum class Gender : int { Male, Female };
enum class Occupation { Knight, Barbarian };

struct Shared_context;

Race string_to_race(std::string race);
std::string race_to_string(Race r);
Occupation string_to_occupation(const std::string& occupation);
Gender string_to_gender(std::string gender);
std::string gender_to_string(Gender gender);

struct Character
{
	enum class Encumbrance { No, Light, Medium, Heavy, Extra_heavy };
	void reset();
	void load (const std::string& key, Shared_context* context) {}

	unsigned int character_points { 0 };
	std::string name;
	Gender gender { Gender::Male };
	bool user_controlled{ false };
	Race race { Race::Human };
	Occupation occupation{ Occupation::Knight };
	Encumbrance encumbrance { Encumbrance::No };
	int hitpoints { 0 };
	int fatigue_points { 0 };
};