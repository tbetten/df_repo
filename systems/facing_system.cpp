#include "facing_system.h"
#include "ecs.h"
#include "components.h"
#include "position.h"
#include "drawable.h"
#include "facing.h"
#include "directions.h"
#include "facing_payload.h"
#include "shared_context.h"
#include "resource_cache.h"
#include "systems.h"
#include "db_queries.h"
#include <iostream>
#include <algorithm>

namespace systems
{
	Facing_system::Facing_system (ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m) : S_base (type, mgr), messaging::Sender { m }
	{
		m_entity_manager = m_system_manager->get_entity_mgr ();
		m_facing_comp = m_entity_manager->get_component<ecs::Component<Facing>> (ecs::Component_type::Facing);
		ecs::Bitmask b;
		b.set (static_cast<int>(ecs::Component_type::Drawable));
		b.set (static_cast<int>(ecs::Component_type::Position));
		b.set (static_cast<int>(ecs::Component_type::Facing));
		m_requirements.push_back (b);

		add_message ("icon_changed");
		auto table = DB_queries::query_tiles ("facing");
		for (auto [tile_name, tile_index] : table)
		{
			m_tile_indices.emplace (tile_name, tile_index);
		}
	}

	void Facing_system::setup_events ()
	{
		m_messenger->bind ("changed_facing", [this] (auto val) {change_facing (val); });
	}

	void Facing_system::update (sf::Int64 dt)
	{}

//	constexpr std::array<std::pair<Compass, const char*>, 8> facing { { {Compass::North, "facing_n"}, {Compass::North_east, "facing_ne"}, {Compass::East, "facing_e"},
//	{Compass::South_east, "facing_se"}, {Compass::South, "facing_s"}, {Compass::South_west, "facing_sw"}, {Compass::West, "facing_w"}, {Compass::North_west, "facing_nw"} } };

		

	const std::unordered_map<Compass, std::string> facing = { {Compass::North, "facing_n"}, {Compass::North_east, "facing_ne"}, {Compass::East, "facing_e"},
	{Compass::South_east, "facing_se"}, {Compass::South, "facing_s"}, {Compass::South_west, "facing_sw"}, {Compass::West, "facing_w"}, {Compass::North_west, "facing_nw"} };

	void Facing_system::change_facing(std::any val)
	{
		std::cout << "facing system\n";
		const auto [entity, new_facing] = std::any_cast<Facing_payload>(val);
		auto cache = m_system_manager->get_context()->m_cache;
//		auto key = std::lower_bound (std::cbegin (facing), std::cend (facing), new_facing)->second;
		auto key = facing.at(new_facing);
		int tile_index = m_tile_indices[key];
		fill_icon_part(m_system_manager->get_entity_mgr(), cache, "facing", tile_index, "facing_indicator", entity);
		notify("icon_changed", entity);
	}

}