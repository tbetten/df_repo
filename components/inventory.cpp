#include "inventory.h"

Inventory::Inventory ()
{
	inventory.fill (std::nullopt);
}

void Inventory::load (const std::string& key, Shared_context* context) {}