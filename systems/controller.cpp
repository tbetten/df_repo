#include "controller.h"
#include "ecs.h"
#include "systems.h"
#include "components.h"
#include "shared_context.h"
#include "eventmanager.h"
#include "move_payload.h"
#include "scheduler.h"
#include "ai.h"
#include "character.h"

#include <iostream>

namespace systems
{
	Controller::Controller(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m) : S_base(type, mgr), messaging::Sender{ m }, m_messenger{ m }
	{
		ecs::Bitmask b;
		b.set (static_cast<int>(ecs::Component_type::Position));
		b.set(static_cast<int>(ecs::Component_type::Character));
		m_requirements.push_back (b);
		add_message("changed_position");
		add_message("switched_current_entity");
/*		m_dispatchers.emplace("change_position", Dispatcher{});
		m_system_manager->register_events(ecs::System_type::Controller, { "change_position" });
		m_dispatchers.emplace("switched_current_entity", Dispatcher{});
		m_system_manager->register_events(ecs::System_type::Controller, { "switched_current_entity" });*/
	}

	void Controller::setup_events ()
	{

		auto eventmgr = m_system_manager->get_context ()->m_event_manager;
		eventmgr->add_command (Game_state::Game, "CMD_move_forward", [this](auto data) {move (Direction::Forward); });
		eventmgr->add_command (Game_state::Game, "CMD_move_right_forward", [this](auto data) {move (Direction::Right_forward); });
		eventmgr->add_command (Game_state::Game, "CMD_move_right_backward", [this](auto data) {move (Direction::Right_backward); });
		eventmgr->add_command (Game_state::Game, "CMD_move_backward", [this](auto data) {move (Direction::Backward); });
		eventmgr->add_command (Game_state::Game, "CMD_move_left_backward", [this](auto data) {move (Direction::Left_backward); });
		eventmgr->add_command (Game_state::Game, "CMD_move_left_forward", [this](auto data) {move (Direction::Left_forward); });
		eventmgr->add_command (Game_state::Game, "CMD_move_left", [this](auto data) {move (Direction::Left); });
		eventmgr->add_command (Game_state::Game, "CMD_move_right", [this](auto data) {move (Direction::Right); });

		m_messenger->bind("entity_modified", [this](auto val) {register_entity(val); });
//		m_system_manager->get_entity_mgr()->get_event().bind([this](auto val) {register_entity(val); });
		
		m_messenger->bind("move", [this](auto val) {move_ai(val); });
	//	auto sys = m_system_manager->find_event("move");
	//	m_system_manager->get_event(sys, "move").bind([this](auto val) {move_ai(val); });
	}

	void Controller::register_entity(std::any val)
	{
		auto payload = std::any_cast<Modified_payload> (val);
		auto entity = payload.m_entity;
		if (this->fits_requirements(payload.m_bits))
		{
			auto scheduler = m_system_manager->get_context()->m_scheduler;
			scheduler->schedule([this, entity]() -> std::optional<int> { return take_turn(entity); });
		}
	}

	// wordt aangeroepen door scheduler
	std::optional<int> Controller::take_turn(ecs::Entity_id entity)
	{
		auto ai = m_system_manager->get_system<AI>(ecs::System_type::AI);
		m_current_entity = entity;
		auto character_comp = m_system_manager->get_entity_mgr()->get_data<ecs::Component<Character>>(ecs::Component_type::Character, entity);
		m_player_controlled = character_comp->user_controlled;
		notify("switched_current_entity", entity);
//		m_dispatchers["switched_current_entity"].notify(entity);
		if (!m_player_controlled)
		{
			return ai->take_turn(entity);
		}
		
		return std::nullopt;
	}

	void Controller::update(sf::Int64 dt)
	{}

/*	Dispatcher& Controller::get_event (const std::string& event)
	{
		return m_dispatchers[event];
	}*/

	void Controller::move_ai(std::any val)
	{
		std::cout << "AI movement\n";
		auto dir = std::any_cast<Direction> (val);
		move(dir);
	}

	void Controller::move (Direction d)
	{
		std::cout << "move " << static_cast<int>(d) << std::endl;
	//	Move_payload m;
	//	m.entity = m_current_entity;
	//	m.direction = d;
		notify("changed_position", Move_payload{m_current_entity, d});
//		m_dispatchers["change_position"].notify (m);
		if (m_player_controlled)
		{
			m_system_manager->get_context()->m_scheduler->async_response(50);
		}
	}

}