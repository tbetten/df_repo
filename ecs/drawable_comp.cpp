/*#include "drawable_comp.h"
#include "resource_cache.h"

void Drawable_comp::load_from_db(db::db_connection* db, const std::string& key)
{

}

void Drawable_comp::init (Entity_id entity, const std::string& key, cache::Cache* cache, hexlib::Layout layout)
{
	auto res = cache->get_obj (key);
	auto data = m_data[entity];
	data.m_texture = *cache::get_val<sf::Texture> (res.get ());
	data.m_sprite.setTexture (data.m_texture);
	data.m_layout = layout;
}

void Drawable_comp::update_position (Entity_id entity, hexlib::Hex coords)
{
	auto data = m_data[entity];
	auto pixel_coords = hexlib::hex_to_pixel (data.m_layout, coords);
	auto size = data.m_texture.getSize ();
	data.m_screen_coords.x = static_cast<float>(pixel_coords.x - (size.x / 2.0));
	data.m_screen_coords.y = static_cast<float>(pixel_coords.y - (size.y / 2.0));
	data.m_sprite.setPosition (data.m_screen_coords);
}

void Drawable_comp::draw (Entity_id entity, sf::RenderWindow* win)
{
	win->draw (m_data[entity].m_sprite);
}*/