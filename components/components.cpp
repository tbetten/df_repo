#include "components.h"
#include "position.h"
#include "drawable.h"
#include "facing.h"
#include "character.h"
#include "item.h"
#include "tilemap.h"
#include "shared_context.h"

void comps::add_components (ecs::Entity_manager* mgr)
{
	mgr->add_component (ecs::Component_type::Position, std::unique_ptr<ecs::C_base> (new  ecs::Component<Position> ()));
	mgr->add_component (ecs::Component_type::Drawable, std::unique_ptr<ecs::C_base> (new ecs::Component<Drawable> ()));
	mgr->add_component (ecs::Component_type::Facing, std::unique_ptr<ecs::C_base> (new ecs::Component<Facing> ()));
	mgr->add_component(ecs::Component_type::Character, std::unique_ptr<ecs::C_base>(new ecs::Component<Character>()));
	mgr->add_component(ecs::Component_type::Item, std::unique_ptr<ecs::C_base>(new ecs::Component<Item>()));
}

ecs::Entity_id comps::spawn_from_key (Shared_context* context, std::string key, sf::Vector2u coords, std::shared_ptr<Tilemap> map)
{   // temp implementation
	if (key == "big_kobold")
	{
		auto mgr = context->m_entity_manager;
		ecs::Bitmask b;
		b.set (static_cast<int>(ecs::Component_type::Position));
		b.set (static_cast<int>(ecs::Component_type::Drawable));
		b.set (static_cast<int>(ecs::Component_type::Facing));
		b.set(static_cast<int>(ecs::Component_type::Character));
		ecs::Entity_id id = mgr->add_entity (b);

/*		grid::Layout l;
		l.gridsize = sf::Vector2u{ 15, 10 };
		l.origin = sf::Vector2f{ 0.0f, 0.0f };
		l.tilesize = sf::Vector2f{ 32.0f, 32.0f };*/
		init_drawable (mgr, id, "big_kobold", context->m_cache);// , l);

		auto postion = mgr->get_data<ecs::Component<Position>> (ecs::Component_type::Position, id);
		postion->coords = coords;
		postion->current_map = map;
		return id;
	}
	if (key == "sword")
	{
		auto mgr = context->m_entity_manager;
		ecs::Bitmask b;
		b.set(static_cast<int>(ecs::Component_type::Position));
		b.set(static_cast<int>(ecs::Component_type::Drawable));
		b.set(static_cast<int>(ecs::Component_type::Item));

		ecs::Entity_id id = mgr->add_entity(b);
		init_drawable(mgr, id, "sword", context->m_cache);
		auto position = mgr->get_data<ecs::Component<Position>>(ecs::Component_type::Position, id);
		position->coords = coords;
		position->current_map = map;
		return id;
	}
	return 0;
}