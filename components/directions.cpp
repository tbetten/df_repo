#include "directions.h"

#include <algorithm>
#include <cctype>

Compass& operator++ (Compass& c)
{
	if (c == Compass::North_west)
	{
		c = Compass::North;
	}
	else
	{
		int i = static_cast<int> (c);
		c = static_cast<Compass>(++i);
	}
	return c;
}

Compass operator++ (Compass& c, int i)
{
	Compass res{ c };
	++c;
	return res;
}

Compass& operator-- (Compass& c)
{
	if (c == Compass::North)
	{
		c = Compass::North_west;
	}
	else
	{
		int i = static_cast<int>(c);
		c = static_cast<Compass>(--i);
	}
	return c;
}

Compass operator-- (Compass& c, int i)
{
	Compass res{ c };
	--c;
	return res;
}

Compass operator+ (Compass& c, int i)
{
	Compass res{ c };
	int j = static_cast<int> (c);
	j += i;
	j %= 8;
	if (j < 0)
	{
		j = 8 + j;
	}
	res = static_cast<Compass> (j);
	return res;
}

Compass& operator+= (Compass& c, int i)
{
	c = c + i;
	return c;
}

Compass operator- (Compass& c, int i)
{
	return c + (-i);
}

Compass operator- (Compass& c)
{
	Compass res{ c };
	res = res + 4;
	return res;
}

Compass Compass_util::find_new_facing(Direction dir, Compass facing)
{
	switch (dir)
	{
	case Direction::Right_forward: [[fallthrough]];
	case Direction::Turn_right:
		return ++facing;
		break;
	case Direction::Left_forward: [[fallthrough]];
	case Direction::Turn_left:
		return --facing;
	default:
		return facing;
	}
}

Compass Compass_util::find_move_direction(Direction dir, Compass facing)
{
	switch (dir)
	{
	case Direction::Forward:
		return facing;
	case Direction::Backward:
		return -facing;
	case Direction::Left_forward:
		return --facing;
	case Direction::Right_forward:
		return ++facing;
	case Direction::Right:
		return facing + 2;
	case Direction::Left:
		return facing - 2;
	case Direction::Left_backward:
		return facing - 3;
	case Direction::Right_backward:
		return facing + 3;
	default:
		return facing;
	}
}

sf::Vector2i Compass_util::get_direction_vector(Compass c)
{
	return direction_vec.at(c);
}

int Compass_util::get_direction_angle(Compass c)
{
	return direction_angle.at(c);
}

Compass Compass_util::from_string(const std::string& s)
{
	return string_to_compass.at(s);
}

const std::unordered_map<std::string, Compass> Compass_util::string_to_compass = { {"north", Compass::North}, {"northeast", Compass::North_east}, {"east", Compass::East}, {"southeast", Compass::South_east},
{"south", Compass::South}, {"southwest", Compass::South_west}, {"west", Compass::West}, {"northwest", Compass::North_west} };


const std::unordered_map<Compass, sf::Vector2i> Compass_util::direction_vec{ {Compass::North, {0, -1}}, {Compass::North_east, {1, -1}}, {Compass::East, {1, 0}}, {Compass::South_east, {1, 1}},
																			 {Compass::South, {0, 1}}, {Compass::South_west, {-1, 1}}, {Compass::West, {-1, 0}}, {Compass::North_west, {-1, -1}} };

const std::unordered_map<Compass, int> Compass_util::direction_angle{ {Compass::North, 0}, { Compass::North_east, 45 }, { Compass::East, 90 }, { Compass::South_east, 135 }, { Compass::South, 180 },
																	  {Compass::South_west, 225}, {Compass::West, 270}, {Compass::North_west, 315} };