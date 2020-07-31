#include "tileset.h"
#include "tinyxml2.h"
#include "shared_context.h"
#include "resource_cache.h"

#include <stdexcept>

namespace tileset
{
	sf::IntRect Tileset::get_rect(int tile_id) const
	{
		if (tile_id > num_tiles) throw std::invalid_argument("Requested tile index " + std::to_string(tile_id) + " while tileset has only " + std::to_string(num_tiles) + " tiles");
		if (columns == 0) return sf::IntRect{ 0,0,0,0 };
		return sf::IntRect{ (tile_id % columns) * tilewidth, (tile_id / columns) * tileheight, tilewidth, tileheight };
	}

	void Tileset::set_first_gid(int first_gid) noexcept
	{
		firstgid = first_gid;
		lastgid = firstgid + num_tiles;
	}

	std::tuple<int, int> Tileset::get_gids() const noexcept
	{
		return std::make_tuple(firstgid, lastgid);
	}

	void Tileset::load(std::string_view filename)
	{
		Tileset tileset{};
		tinyxml2::XMLDocument doc;
		doc.LoadFile(filename.data());
		auto tileset_elem = doc.FirstChildElement("tileset");
		tileset.columns = tileset_elem->IntAttribute("columns");
		tileset.num_tiles = tileset_elem->IntAttribute("tilecount");
		tileset.lastgid = tileset.firstgid + tileset.num_tiles - 1;
		//tileset.lastgid = tileset.firstgid + tileset_elem->IntAttribute("tilecount") - 1;
		tileset.name = tileset_elem->Attribute("name");
		tileset.tilewidth = tileset_elem->IntAttribute("tilewidth");
		tileset.tileheight = tileset_elem->IntAttribute("tileheight");
		auto image_elem = tileset_elem->FirstChildElement("image");
		tileset.source = image_elem->Attribute("source");
		//tileset.texture = context->m_cache->get_obj(tileset.source);
		if (!tileset.texture.loadFromFile(tileset.source)) throw std::invalid_argument("Texture " + tileset.source + " not found");

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

	sf::Sprite Tile::get_sprite()
	{
		auto tileset = cache::get_val<Tileset>(tileset_resource.get());
		return sf::Sprite{ tileset->texture, tileset->get_rect(tile_index) };
	}
}