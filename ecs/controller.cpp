#include "controller.h"
#include "move_payload.h"
#include <iostream>

Controller::Controller (System_manager* mgr) : System_base(System::Controller, mgr)
{
	Bitmask b;
	b.set (to_number (Component::Position));
	m_required_components.push_back (b);
}

void Controller::setup_events ()
{
	Dispatcher d;
	m_dispatchers["change_position"] = d;
	m_system_manager->register_events (System::Controller, { "change_position" });
	auto eventmgr = m_system_manager->get_context ()->m_event_manager;
	eventmgr->add_command (Game_state::Game, "CMD_move_north", [this](auto data) {move (Direction::North); });
	eventmgr->add_command (Game_state::Game, "CMD_move_northeast", [this](auto data) {move (Direction::Northeast); });
	eventmgr->add_command (Game_state::Game, "CMD_move_southeast", [this](auto data) {move (Direction::Southeast); });
	eventmgr->add_command (Game_state::Game, "CMD_move_south", [this](auto data) {move (Direction::South); });
	eventmgr->add_command (Game_state::Game, "CMD_move_southwest", [this](auto data) {move (Direction::Southwest); });
	eventmgr->add_command (Game_state::Game, "CMD_move_northwest", [this](auto data) {move (Direction::Northwest); });
}

void Controller::move (Direction d)
{
	std::cout << "move " << static_cast<int>(d) << std::endl;
	Move_payload m;
	m.entity = m_current_entity;
	m.direction = d;
	m_dispatchers["change_position"].notify (m);
}

void Controller::update (int dt)
{

}

Dispatcher& Controller::get_event (std::string event)
{
	return m_dispatchers[event];
}