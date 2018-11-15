#include "resource_cache.h"

void Cache::init()
{
	loadfuncs["fantasyhextiles_v2.tsx"] = &Tileset_resource::load_resource;
	paths["fantasyhextiles_v2.tsx"] = "assets/maps/fantasyhextiles_v2.tsx";
	loadfuncs["fantasyhextiles_randr_4_v1.tsx"] = &Tileset_resource::load_resource;
	paths["fantasyhextiles_randr_4_v1.tsx"] = "assets/maps/fantasyhextiles_randr_4_v1.tsx";
}

Texture_resource::Resource(fs::path file)
{
	val.loadFromFile(file.string());
}

Tileset_resource::Resource(fs::path file)
{
	val.loadFromFile(file.string());
}