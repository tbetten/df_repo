#pragma once
#include <string>
#include <memory>
#include <SFML/Graphics/Texture.hpp>

struct Shared_context;

namespace tiled
{
	class Tilesheet;
}

struct Armour
{
//	enum class Equip_slot {Torso};
	void reset () {}
	void load (const std::string& key, Shared_context* context);

	unsigned int dr { 0 };
	unsigned int crushing_dr { 0 };
//	Equip_slot equip_slot { Equip_slot::Torso };
	std::string tilesheet { "" };
	int tile_index { -1 };
};