#include "character.h"

void Character::reset()
{
	name = "";
	user_controlled = true;
	race = Race::Human;
	encumbrance = Encumbrance::No;
	hitpoints = 0;
	fatigue_points = 0;
}