#pragma once
#include "ecs_base.h"
#include "hexlib.h"
#include <SFML/Graphics.hpp>

namespace cache
{
	struct Cache;
}

struct Drawable_data
{
	sf::Sprite m_sprite;
	sf::Texture m_texture;
	sf::Vector2f m_screen_coords;
	hexlib::Layout m_layout;
};

struct Drawable_comp : public Component_base
{
	using Ptr = std::shared_ptr<Drawable_comp>;
	//explicit Drawable_comp (std::string);
	static Ptr create () { return std::make_shared<Drawable_comp> (); }
	void load_from_db (db::db_connection* db, const std::string& key) override;
	void add_entity (Entity_id entity) override { m_data.emplace (entity, Drawable_data{}); }
	bool has_entity (Entity_id entity) override { return m_data.count (entity); }
	Drawable_data& get_data (Entity_id entity) { return m_data[entity]; }
	void init (Entity_id entity, const std::string& key, cache::Cache* cache, hexlib::Layout layout);
	void update_position (Entity_id entity, hexlib::Hex coords);
	void draw (Entity_id entity, sf::RenderWindow* win);

	std::unordered_map<Entity_id, Drawable_data> m_data;
//	sf::Sprite m_sprite;
//	sf::Texture m_texture;
//	sf::Vector2f m_screen_coords;
//	hexlib::Layout m_layout;
};