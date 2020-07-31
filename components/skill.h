#pragma once
#include "attributes.h"

#include <string>
#include <vector>

struct Default_skill
{
	std::string name;
	int modifier;
};

struct Skill
{
	enum class Skill_type { Skill, Attribute };
	enum class Difficulty { Easy, Average, Hard, Very_hard };
	std::string name;
	attributes::Attrib base_attribute;
	Difficulty difficulty;
	int level;
	int points_spent;
	std::vector<Default_skill> defaults;
};