#include "position.h"

void Position::reset ()
{
	map_id = "";
	coords.x = 0;
	coords.y = 0;
}

bool operator< (const Position& lhs, const Position& rhs)
{
	return lhs.map_id < rhs.map_id ? true : lhs.layer < rhs.layer;
}