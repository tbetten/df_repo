#pragma once
#include "ecs_base.h"
#include "hexlib.h"
#include <SFML/Graphics.hpp>

namespace cache
{
	struct Cache;
}

struct Drawable_comp : public cloneable<Drawable_comp, Component_base>
{
	using Ptr = std::shared_ptr<Drawable_comp>;
	//explicit Drawable_comp (std::string);
	static Ptr create () { return std::make_shared<Drawable_comp> (); }
	void load_from_db (db::db_connection* db, const std::string& key) override;
	void init (const std::string& key, cache::Cache* cache, hexlib::Layout layout);
	void update_position (hexlib::Hex coords);
	void draw (sf::RenderWindow* win);

	sf::Sprite m_sprite;
	sf::Texture m_texture;
	sf::Vector2f m_screen_coords;
	hexlib::Layout m_layout;
};