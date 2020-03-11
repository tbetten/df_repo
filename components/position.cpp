#include "position.h"

void Position::reset ()
{
	map_id = "";
	coords.x = 0;
	coords.y = 0;
}

bool operator< (const Position& lhs, const Position& rhs)
{
	if (lhs.current_map == rhs.current_map)
	{
		if (lhs.layer == rhs.layer)
		{
			if (lhs.coords.y == rhs.coords.y)
			{
				return lhs.coords.x < rhs.coords.x;
			}
			else
			{
				return lhs.coords.y < rhs.coords.y;
			}
		}
		else
		{
			return lhs.layer < rhs.layer;
		}
	}
	else
	{
		return lhs.current_map < rhs.current_map;
	}
	//return lhs.map_id < rhs.map_id ? true : lhs.layer < rhs.layer;
}