#include "util_types.h"

using Underlying = typename std::underlying_type_t<Direction>;

Direction& operator++(Direction& d)
{
	if (d == Direction::Northwest)
	{
		d = Direction::North;
	}
	else
	{
		d = static_cast<Direction>(static_cast<Underlying>(d) + 1);
	}
	return d;
}

Direction operator++ (Direction& d, int)
{
	Direction result = d;
	++d;
	return result;
}

Direction& operator--(Direction& d)
{
	if (d == Direction::North)
	{
		d = Direction::Northwest;
	}
	else
	{
		d = static_cast<Direction>(static_cast<Underlying>(d) - 1);
	}
	return d;
}

Direction operator-- (Direction& d, int)
{
	Direction result = d;
	--d;
	return result;
}