#pragma once
#include <string>

enum class Compass : int;

struct Shared_context;

struct Lightsource
{
	void reset();
	void load (const std::string& key, Shared_context* context) {}

	int brightness;
	Compass direction;
	bool ignited;
};