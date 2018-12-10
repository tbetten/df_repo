#include "resource_cache.h"

namespace cache
{
	void Cache::init ()
	{
		loadfuncs["fantasyhextiles_v2.tsx"] = &Tileset_resource::load_resource;
		paths["fantasyhextiles_v2.tsx"] = "assets/maps/fantasyhextiles_v2.tsx";
		loadfuncs["fantasyhextiles_randr_4_v1.tsx"] = &Tileset_resource::load_resource;
		paths["fantasyhextiles_randr_4_v1.tsx"] = "assets/maps/fantasyhextiles_randr_4_v1.tsx";
		loadfuncs["big_kobold"] = &Texture_resource::load_resource;
		paths["big_kobold"] = "assets/sprite/big_kobold_new.png";
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