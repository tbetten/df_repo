#include "character.h"

#include <vector>
#include <algorithm>

void Character::reset()
{
	name = "";
	user_controlled = true;
	race = Race::Human;
	encumbrance = Encumbrance::No;
	hitpoints = 0;
	fatigue_points = 0;
}

std::vector<std::string> races{ "Human", "Dwarf", "Halfling" };
std::vector<std::string> occupations{ "Knight", "Barbarian" };
std::vector<std::string> genders{ "Male", "Female" };

Race string_to_race(std::string race)
{
	auto itr = std::ranges::find(races, race);
	if (itr != std::cend(races))
	{
		return static_cast<Race> (std::distance(std::begin(races), itr));
	}
	return Race::Human;
}

std::string race_to_string(Race r)
{
	return races[static_cast<int>(r)];
}

Occupation string_to_occupation(const std::string& occupation)
{
	auto itr = std::ranges::find(occupations, occupation);
	{
		if (itr != std::cend(occupations))
		{
			return static_cast<Occupation>(std::distance(std::begin(occupations), itr));
		}
		return Occupation::Knight;
	}
}

Gender string_to_gender(std::string gender)
{
	auto itr = std::ranges::find(genders, gender);
	if (itr != std::cend(genders))
	{
		return static_cast<Gender>(std::distance(std::begin(genders), itr));
	}
	return Gender::Male;
}

std::string gender_to_string(Gender gender)
{
	return genders[static_cast<int>(gender)];
}