#pragma once
#include <string>
#include <span>

namespace tiled
{
	struct Property;
}

struct Shared_context;

struct Sensor
{
	enum class Trigger { Invalid = -1, Use, Use_with_item, Enter };

	void reset();
	void load (std::span<tiled::Property> props);
	void load (const std::string& key, Shared_context* context) {}

	Trigger trigger;
	bool active;
	int id;
	bool state;
	std::string item;
};