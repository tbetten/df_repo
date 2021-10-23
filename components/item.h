#pragma once
#include <string>
#include "inventory.h"

struct Shared_context;

struct Item
{
	void reset();
	void load (const std::string& key, Shared_context* context);

	std::string item_name = "";
	std::string description = "";
	int weight = 0;
	int value = 0;
//	bool equippable = false;
	bool useable = false;
//	Inventory::Inventory_slot slot = Inventory::Inventory_slot::None;
};