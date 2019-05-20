/*#include "movement.h"
#include "move_payload.h"
#include "hexlib.h"
#include "position_comp.h"
#include "attribute_comp.h"
#include <iostream>

Movement::Movement (System_manager* mgr) : System_base{ System::Movement, mgr }
{
	Bitmask b;
	b.set (to_number (Component::Position));
	b.set (to_number (Component::Attributes));
	m_required_components.push_back (b);
}

void Movement::setup_events ()
{
	auto sys = m_system_manager->find_event ("change_position");
	m_system_manager->get_event (sys, "change_position").bind ([this](auto val) {move (val); });
}

void Movement::update (int dt)
{}

Dispatcher& Movement::get_event (std::string name)
{
	Dispatcher d;
	return d;
}

void Movement::move (std::any val)
{
	auto d = std::any_cast<Move_payload>(val);
	std::cout << "move " << static_cast<int>(d.direction) << std::endl;
	auto ent = d.entity;
	auto entity_mgr = m_system_manager->get_entity_mgr ();
	auto pos = entity_mgr->get_component<Position_comp> (Component::Position)->get_data(ent);
	int bm;
	if (m_move_cache.count (ent) > 0)
	{
		bm = m_move_cache[ent];
	}
	else
	{
		auto attribs = entity_mgr->get_component<Attribute_comp> (Component::Attributes);
		auto bm = attribs->get_natural (ent, Attribute::BM) / 100;
		m_move_cache[ent] = bm;
	}
	auto time = 1000 / bm;
	std::cout << bm << "\t" << std::endl;
	auto coords = pos.coords;
	//auto x = attribs->m_attributes.
	auto x = hexlib::hex_neighbour (coords, static_cast<int>(d.direction));
	std::cout << x << std::endl;
	pos.coords = x;
}*/