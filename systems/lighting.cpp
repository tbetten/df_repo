#include "lighting.h"
#include "components.h"
#include "shared_context.h"
#include "lightsource.h"
#include "position.h"
#include "ecs.h"
#include "map_data.h"
#include "directions.h"
#include "tile_type.h"

#include <deque>
#include <iostream>

namespace systems
{
	Lighting::Lighting(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m) : S_base(type, mgr), m_messenger{ m }
	{
		ecs::Bitmask b;
		b.set(static_cast<int>(ecs::Component_type::Lightsource));
		m_requirements.push_back(b);

		m_entity_manager = m_system_manager->get_entity_mgr();
		m_lightsource_comp = m_entity_manager->get_component<ecs::Component<Lightsource>>(ecs::Component_type::Lightsource);
	}

	void Lighting::setup_events()
	{
		
	}

	void Lighting::update(sf::Int64 dt)
	{
		auto context = m_system_manager->get_context();
		auto current_map = context->m_current_map;
		auto& lighting_map = context->m_maps->maps[current_map].lighting_map;
		if (lighting_map.size() == 0)
		{

		}
		if (m_first)
		{
			for (auto lightsource : m_entities)
			{
				std::cout << "lightsource: " << std::to_string(lightsource) << "\n";
			}
			m_first = false;
		}
	}

	int get_neighbour(sf::Vector2u coords, int map_width, Compass direction)
	{
		auto vec = Compass_util::get_direction_vector(direction);
		auto temp = sf::Vector2i{ static_cast<int>(coords.x), static_cast<int>(coords.y) };
		auto nextpos = temp + vec;
		return nextpos.y * map_width + nextpos.x;
	}

	int get_neighbour(int tile_index, int map_width, Compass direction)
	{
		sf::Vector2i coords{ tile_index % map_width, tile_index / map_width };
		auto vec = Compass_util::get_direction_vector(direction);
		auto nextpos = coords + vec;
		return nextpos.y * map_width + nextpos.x;
	}

	bool check_tile(int index, const Map_data& map, ecs::Entity_manager* mgr)
	{
/*		auto entities = map.get_entities_at(2, static_cast<unsigned int>(index));
		for (auto entity : entities)
		{
			if (mgr->has_component(entity, ecs::Component_type::Tile_type))
			{
				auto data = mgr->get_data<ecs::Component<Tile_type>>(ecs::Component_type::Tile_type, entity);
				if (!data->transparent) return false;
			}
		}*/
		return true;
	}


	void Lighting::init_lighting_map()
	{
		auto current_map = m_system_manager->get_context()->m_current_map;
		auto& map_info = m_system_manager->get_context()->m_maps->maps[current_map];
		auto& lighting_map = map_info.lighting_map;
		auto map_size = map_info.m_mapsize.x * map_info.m_mapsize.y;
		for (auto i = 0; i < map_size; ++i)
		{
			lighting_map.push_back(0);
		}
		for (auto entity : m_entities)
		{
			auto comp_index = m_entity_manager->get_index(ecs::Component_type::Lightsource, entity);
			if (!comp_index) continue;
			auto& lightsource = m_lightsource_comp->m_data[*comp_index];
			if (lightsource.ignited)
			{
				auto position = m_entity_manager->get_data<ecs::Component<Position>>(ecs::Component_type::Position, entity);
				auto tile_index = position->coords.x + position->coords.y * map_info.m_mapsize.x;
				lighting_map[tile_index] += lightsource.brightness;
				auto dir = lightsource.direction;
				auto direction_vec = Compass_util::get_direction_vector(dir);
				//if (map_info.get_entities_at()
			}
		}
	}
}