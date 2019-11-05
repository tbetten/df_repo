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

Race string_to_race(std::string race)
{
	auto itr = std::find(std::cbegin(races), std::cend(races), race);
	if (itr != std::cend(races))
	{
		return static_cast<Race> (std::distance(std::cbegin(races), itr));
	}
	return Race::Human;
}

std::string race_to_string(Race r)
{
	return races[static_cast<int>(r)];
}