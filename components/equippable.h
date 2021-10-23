#pragma once
#include <string>
#include "inventory_slot.h"

struct Shared_context;

struct Equippable
{
	void reset () {}
	void load (const std::string& key, Shared_context* context);

	std::string tilesheet;
	int tile_index;
	inventory::Inventory_slot slot { inventory::Inventory_slot::None };
};