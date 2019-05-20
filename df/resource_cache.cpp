#include "resource_cache.h"

namespace cache
{
	void Cache::init ()
	{
		loadfuncs["fantasyhextiles_v2.tsx"] = &Tileset_resource::load_resource;
		paths["fantasyhextiles_v2.tsx"] = "assets/maps/fantasyhextiles_v2.tsx";
		loadfuncs["fantasyhextiles_randr_4_v1.tsx"] = &Tileset_resource::load_resource;
		paths["fantasyhextiles_randr_4_v1.tsx"] = "assets/maps/fantasyhextiles_randr_4_v1.tsx";
		loadfuncs["walltiles.tsx"] = &Tileset_resource::load_resource;
		paths["walltiles.tsx"] = "d:/dfmaps/walltiles.tsx";
		loadfuncs["floortiles.tsx"] = &Tileset_resource::load_resource;
		paths["floortiles.tsx"] = "d:/dfmaps/floortiles.tsx";
		loadfuncs["floor.png"] = &Texture_resource::load_resource;
		paths["floor.png"] = "d:/dfmaps/floor.png";
		loadfuncs["wall.png"] = &Texture_resource::load_resource;
		paths["wall.png"] = "d:/dfmaps/wall.png";
		loadfuncs["big_kobold"] = &Texture_resource::load_resource;
		paths["big_kobold"] = "assets/sprite/big_kobold_new.png";
		loadfuncs["facing_indicator"] = &Texture_resource::load_resource;
		paths["facing_indicator"] = "assets/sprite/arrow_up.png";
		loadfuncs["sword"] = &Texture_resource::load_resource;
		paths["sword"] = "assets/sprite/long_sword_1_new.png";
	}

	Texture_resource::Resource (fs::path file)
	{
		val.loadFromFile (file.string ());
	}

	Tileset_resource::Resource (fs::path file)
	{
		val.loadFromFile (file.string ());
	}
}