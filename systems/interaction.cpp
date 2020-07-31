#include "interaction.h"
#include "components.h"
#include "component.h"
#include "position.h"
#include "sensor.h"
#include "animation.h"
#include "drawable.h"
#include "item.h"
#include "sensor_payload.h"
#include "reactor.h"
#include "pickup_payload.h"
#include "shared_context.h"
#include "map_data.h"
#include "ecs.h"
#include "db_queries.h"

#include <iostream>

namespace systems
{
	Interaction::Interaction(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* messenger) : S_base(type, mgr), messaging::Sender{ messenger }
	{
		ecs::Bitmask b;
		//b.set(ecs::Component_type::)
		m_requirements.push_back(b);
		add_message("sensor_triggered");
		add_message("icon_changed");
	}

	void Interaction::setup_events()
	{
		m_messenger->bind("use", [this](auto val) {use(val); });
		m_messenger->bind("sensor_triggered", [this](auto val) {trigger(val); });
	}

	void Interaction::update(sf::Int64 dt)
	{

	}

	void Interaction::use(std::any val)
	{
		auto [entity, coords] = std::any_cast<Pickup_payload>(val);
		std::cout << "interaction " << coords.x << " " << coords.y << "\n";
		auto entity_manager = m_system_manager->get_entity_mgr();
		auto pos = entity_manager->get_data<ecs::Component<Position>>(ecs::Component_type::Position, entity);
		auto context = m_system_manager->get_context();
		auto& map = context->m_maps->maps[context->m_current_map];
		if (!map.m_topology->are_neighbours(coords, pos->coords))
		{
			std::cout << "too far\n";
			return;
		}
		auto entities = map.m_map_index[map.m_topology->tile_index(coords)][Position::Layer::Interactive_objects];
		ecs::Entity_list sensors;
		std::copy_if(std::cbegin(entities), std::cend(entities), std::back_inserter(sensors), [entity_manager](const ecs::Entity_id e) {return entity_manager->has_component(e, ecs::Component_type::Sensor); });
		auto entity_itr = std::find_if(std::cbegin(entities), std::cend(entities), [entity_manager](const ecs::Entity_id e) {return entity_manager->has_component(e, ecs::Component_type::Sensor); });
	//	if (entity_itr != std::cend(entities))
	//	{
		for (auto entity : sensors)
		{
			auto sensor = entity_manager->get_data<ecs::Component<Sensor>>(ecs::Component_type::Sensor, entity);
			if (sensor->active && sensor->trigger == Sensor::Trigger::Use)
			{
				sensor->state = !sensor->state;
				std::cout << std::boolalpha << "sensor triggered: state is " << sensor->state << "\n";
				notify("sensor_triggered", Sensor_payload{ sensor->id, sensor->state });
			}
			if (sensor->active && sensor->trigger == Sensor::Trigger::Use_with_item && context->m_active_object.has_value())
			{
				auto obj = context->m_active_object.value();
				std::cout << "sensor triggered with object " << obj << "\n";
				auto item = entity_manager->get_data<ecs::Component<Item>>(ecs::Component_type::Item, obj);
				auto x = item->item_name;
				if (item->item_name == sensor->item)
				{
					std::cout << "correct item \n";
					context->m_active_object = std::nullopt;
					sensor->state = !sensor->state;
					notify("sensor_triggered", Sensor_payload{ sensor->id, sensor->state });
				}
			}
		}
	}

/*	int get_tile_index(std::string tilesheet, std::string tile)
	{
		db::DB_connection db{ "./assets/database/gamedat.db" };
		db::Prepared_statement stmt = db.prepare("select tile_index from tile_catalog where sheet = ? and key = ?");
		stmt.bind(1, tilesheet);
		stmt.bind(2, tile);
		stmt.execute_row();
		auto row = stmt.fetch_row();
		auto i = std::get<int>(row["tile_index"]);
		return i;
	}*/

	void activate_sensor(ecs::Entity_manager* mgr, ecs::Entity_id entity)
	{
		if (mgr->has_component(entity, ecs::Component_type::Sensor))
		{
			auto sensor = mgr->get_data<ecs::Component<Sensor>>(ecs::Component_type::Sensor, entity);
			sensor->active = true;
		}
	}

	void change_tile(ecs::Entity_manager* mgr, Shared_context* context, Reactor* reactor, bool state)
	{
		if (mgr->has_component(reactor->entity, ecs::Component_type::Animation))
		{
			auto anim = mgr->get_data<ecs::Component<Animation>>(ecs::Component_type::Animation, reactor->entity);
			anim->active = !anim->active;
		}
		if (mgr->has_component(reactor->entity, ecs::Component_type::Drawable))
		{
			auto drawable = mgr->get_data<ecs::Component<Drawable>>(ecs::Component_type::Drawable, reactor->entity);
			auto icon = &drawable->icon_parts["base_icon"];
			auto tile = state ? reactor->tile_true : reactor->tile_false;
			auto index = DB_queries::query_tile_index(icon->key, tile);
			change_icon_index(mgr, "base_icon", reactor->entity, index);
			mgr->get_messenger()->notify("icon_changed", reactor->entity);
		}
		if (reactor->make_accessible && mgr->has_component(reactor->entity, ecs::Component_type::Position))
		{
			auto position = mgr->get_data<ecs::Component<Position>>(ecs::Component_type::Position, reactor->entity);
			auto& map_data = context->m_maps->maps[context->m_current_map];
			auto tile_index = map_data.m_topology->tile_index(position->coords);
			map_data.m_pathfinding[tile_index] = state ? 0 : -1;
		}

	}

	void Interaction::trigger(std::any val)
	{
		auto [sensor_id, state] = std::any_cast<Sensor_payload>(val);
		std::cout << "sensor " << sensor_id << " was triggered; state is: " << state << "\n";
		auto entity_manager = m_system_manager->get_entity_mgr();
		auto context = m_system_manager->get_context();
		auto reactor_comp = entity_manager->get_component<ecs::Component<Reactor>>(ecs::Component_type::Reactor)->m_data;
		auto itr = std::cbegin(reactor_comp);
		while (itr != std::cend(reactor_comp))
		{
			itr = std::find_if(itr, std::cend(reactor_comp), [sensor_id](const Reactor& r) {return r.triggered_by == sensor_id; });
			if (itr == std::cend(reactor_comp)) break;
			auto reactor = *itr;
			auto action = reactor.action;
			auto ent = reactor.entity;
			switch (action)
			{
			case Reactor::On_trigger::Activate:
				activate_sensor(entity_manager, ent);
				break;
			case Reactor::On_trigger::Change_tile:
				change_tile(entity_manager, context, &reactor, state);
				break;
			case Reactor::On_trigger::No_action: 
				[[fallthrough]];
			default:
				return;
			}
			++itr;
		}
	}
}