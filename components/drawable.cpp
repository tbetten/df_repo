#include "drawable.h"
#include "component.h"
#include "components.h"
#include "resource_cache.h"
//#include "grid.h"
#include "tinyxml2.h"

Drawable::Drawable()
{
	composed_icon = std::make_unique<sf::RenderTexture>();
}
void Drawable::reset () {}
/*
void load_tile(ecs::Entity_manager* mgr, ecs::Entity_id entity, const std::string& tileset_file, cache::Cache* cache, int tile_id)
{
	auto data = mgr->get_data<ecs::Component<Drawable>>(ecs::Component_type::Drawable, entity);
	tinyxml2::XMLDocument doc;
	doc.LoadFile(tileset_file.c_str());
	auto tileset_elem = doc.FirstChildElement("tileset");
	int tilewidth = tileset_elem->IntAttribute("tilewidth");
	int tileheight = tileset_elem->IntAttribute("tileheight");
	int num_columns = tileset_elem->IntAttribute("columns");
	auto image_elem = tileset_elem->FirstChildElement("image");
	std::string image_key = image_elem->Attribute("source");
	data->m_texture_resource = cache->get_obj(image_key);
	data->m_texture = cache::get_val<sf::Texture>(data->m_texture_resource.get());
	for (auto tile_elem = tileset_elem->FirstChildElement("tile"); tile_elem != nullptr; tile_elem = tile_elem->NextSiblingElement("tile"))
	{
		if (tile_elem->IntAttribute("id") == tile_id)
		{
			auto x = tile_id % num_columns;
			auto y = tile_id / num_columns;
			auto anim_elem = tile_elem->FirstChildElement("animation");
			if (anim_elem)
			{
				data->animated = true;
				for (auto frame_elem = anim_elem->FirstChildElement("frame"); frame_elem != nullptr; frame_elem = frame_elem->NextSiblingElement("frame_elem"))
				{
					auto id = frame_elem->IntAttribute("tileid");
					auto duration = frame_elem->IntAttribute("duration");
					auto min_x = id % num_columns;
					auto min_y = id / num_columns;
					data->m_frames.push_back(sf::IntRect{ min_x, min_y, tilewidth, tileheight });
					data->m_durations.push_back(duration);
					data->current_frame = 0;
				}
			}
			data->animated = false;
			data->m_frames.push_back(sf::IntRect{ x, y, tilewidth, tileheight });
		}
	}
}*/

void init_drawable(ecs::Entity_manager* mgr, ecs::Entity_id entity, const std::string& key, cache::Cache* cache, const sf::IntRect& rect)
{
	auto res = cache->get_obj (key);
	auto data = mgr->get_data<ecs::Component<Drawable>> (ecs::Component_type::Drawable, entity);
	auto x = cache::get_val<sf::Texture>(res.get());
	data->m_texture_resource = res;
	data->texture = cache::get_val<sf::Texture> (res.get ());
	data->sprite.setTexture (*data->texture);
	//data->sprite.setTextureRect(rect);
	data->sprite.setOrigin (static_cast<sf::Vector2f> (data->texture->getSize () / 2u));
//	data->layout = layout;
}

