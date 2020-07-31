#pragma once
#include <string>

struct Sensor
{
	enum class Trigger { Invalid = -1, Use, Use_with_item, Enter };

	void reset();

	Trigger trigger;
	bool active;
	int id;
	bool state;
	std::string item;
};