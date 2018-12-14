#include "movement.h"
#include "move_payload.h"
#include "hexlib.h"
#include "position_comp.h"
#include <iostream>

Movement::Movement (System_manager* mgr) : System_base{ System::Movement, mgr }
{
	Bitmask b;
	b.set (to_number (Component::Position));
	m_required_components.push_back (b);
}

void Movement::setup_events ()
{
	auto sys = m_system_manager->find_event ("change_position");
	m_system_manager->get_event (sys, "change_position").bind ([this](auto val) {move (val); });
}

void Movement::update(int dt)
{}

Dispatcher& Movement::get_event(std::string name)
{
	Dispatcher d;
	return d;
}

void Movement::move (std::any val)
{
	auto d = std::any_cast<Move_payload>(val);
	std::cout << "move " << static_cast<int>(d.direction) << std::endl;
	auto ent = d.entity;
	auto pos = m_system_manager->get_entity_mgr ()->get_component<Position_comp> (ent, Component::Position);
	auto coords = pos->coords;
	auto x = hexlib::hex_neighbour (coords, static_cast<int>(d.direction));
	std::cout << x << std::endl;
	pos->coords = x;
}