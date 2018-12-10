#include "drawable_comp.h"
#include "resource_cache.h"

void Drawable_comp::load_from_db(db::db_connection* db, const std::string& key)
{

}

void Drawable_comp::init (const std::string& key, cache::Cache* cache, hexlib::Layout layout)
{
	auto res = cache->get_obj (key);
	m_texture = *cache::get_val<sf::Texture> (res.get ());
	m_sprite.setTexture (m_texture);
	m_layout = layout;
}

void Drawable_comp::update_position (hexlib::Hex coords)
{
	auto pixel_coords = hexlib::hex_to_pixel (m_layout, coords);
	auto size = m_texture.getSize ();
	m_screen_coords.x = static_cast<float>(pixel_coords.x - (size.x / 2.0));
	m_screen_coords.y = static_cast<float>(pixel_coords.y - (size.y / 2.0));
	m_sprite.setPosition (m_screen_coords);
}

void Drawable_comp::draw (sf::RenderWindow* win)
{
	win->draw (m_sprite);
}