#pragma once
#include <string>

enum class Compass : int;

struct Lightsource
{
	void reset();
	int brightness;
	Compass direction;
	bool ignited;
};