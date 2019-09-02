#pragma once
#include <unordered_map>
#include "SFML/System/Vector2.hpp"

enum class Compass : int { North, North_east, East, South_east, South, South_west, West, North_west };

Compass& operator++ (Compass& c);

Compass operator++ (Compass& c, int i);

Compass& operator-- (Compass& c);

Compass operator-- (Compass& c, int i);

Compass operator+ (Compass& c, int i);

Compass& operator+= (Compass& c, int i);

Compass operator- (Compass& c, int i);

Compass operator- (Compass& c);

enum class Direction : int { Forward, Right_forward, Right, Turn_right, Right_backward, Backward, Left_backward, Left, Turn_left, Left_forward };

class Compass_util
{
public:
	static Compass find_new_facing(Direction dir, Compass facing);
	static Compass find_move_direction(Direction dir, Compass facing);
	static sf::Vector2i get_direction_vector(Compass c);
	static int get_direction_angle(Compass c);
private:
	static const std::unordered_map<Compass, sf::Vector2i> direction_vec;
	static const std::unordered_map<Compass, int> direction_angle;
};

//Compass find_new_facing (Direction dir, Compass facing);

//Compass find_move_direction (Direction dir, Compass facing);

//extern std::unordered_map<Compass, sf::Vector2i> direction_vec;