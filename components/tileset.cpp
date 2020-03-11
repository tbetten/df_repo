#include "tileset.h"
#include "tinyxml2.h"
#include "shared_context.h"

namespace tileset
{
	sf::IntRect Tileset::get_rect(int tile_id)
	{
		auto x = tile_id % columns;
		auto y = tile_id / columns;
		auto pixel_x = x * tilewidth;
		auto pixel_y = y * tileheight;
		return sf::IntRect{ (tile_id % columns) * tilewidth, (tile_id / columns) * tileheight, tilewidth, tileheight };
	}

	void load_tileset(std::string_view filename, Tileset& tileset, Shared_context* context)
	{
		tinyxml2::XMLDocument doc;
		doc.LoadFile(filename.data());
		auto tileset_elem = doc.FirstChildElement("tileset");
		tileset.columns = tileset_elem->IntAttribute("columns");
		tileset.lastgid = tileset.firstgid + tileset_elem->IntAttribute("tilecount") - 1;
		tileset.name = tileset_elem->Attribute("name");
		tileset.tilewidth = tileset_elem->IntAttribute("tilewidth");
		tileset.tileheight = tileset_elem->IntAttribute("tileheight");
		auto image_elem = tileset_elem->FirstChildElement("image");
		tileset.source = image_elem->Attribute("source");
		tileset.texture = context->m_cache->get_obj(tileset.source);

		for (auto tile_elem = tileset_elem->FirstChildElement("tile"); tile_elem != nullptr; tile_elem = tile_elem->NextSiblingElement("tile"))
		{
			auto id = tile_elem->IntAttribute("id");
			if (tile_elem->Attribute("type"))
			{
				std::string type = tile_elem->Attribute("type");
				tileset.tile_types.push_back(std::make_pair(id, type));
			}
			auto animation_elem = tile_elem->FirstChildElement("animation");
			if (animation_elem)
			{
				Animation a{};
				for (auto frame_elem = animation_elem->FirstChildElement("frame"); frame_elem != nullptr; frame_elem = frame_elem->NextSiblingElement("frame"))
				{
					Animation_frame  af;
					af.tile_id = frame_elem->IntAttribute("tileid");
					af.duration = frame_elem->IntAttribute("duration");
					a.push_back(af);
				}
				tileset.animations.push_back(a);
			}
		}
	}
}