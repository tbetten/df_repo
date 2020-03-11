#pragma once
#include <optional>
#include <SFML/System/Vector2.hpp>
#include "db.h"
#include "component.h"

struct Shared_context;

namespace el
{
	std::vector<std::string> split(const std::string& s, const std::string& delims);

	class Entity_loader
	{
	public:
		explicit Entity_loader(Shared_context* context);
		std::optional<ecs::Entity_id> load_entity(const std::string& key);
		void load_map(const std::string& map_handle);
		bool set_position(ecs::Entity_id entity, sf::Vector2i coords, int layer, std::string map);
		bool set_player_controlled(ecs::Entity_id entity, bool player_controlled);
	private:
		std::string get_filename(const std::string& handle) ;
		db::DB_connection m_db;
		db::Prepared_statement m_entity_stmt;
		db::Prepared_statement m_filename_stmt;
		Shared_context* m_context;
	};
}