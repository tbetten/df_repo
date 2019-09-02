#include "stdafx.h"
#include "game.h"
#include "game_states.h"
//#include "state_intro.h"
//#include "state_main_menu.h"
#include "state_game.h"
//#include "state_paused.h"
#include "state_character_generator.h"
#include "components.h"
#include "systems.h"
#include "scheduler.h"

#include <iostream>

Game::Game () : m_window{ "Theotris", sf::Vector2u (1600, 600) }, m_statemanager{ &m_context }//, m_system_mgr{&m_entity_mgr}
{
	restart_clock();

	m_context.m_wind = &m_window;
	m_context.m_event_manager = m_window.get_eventmanager();

	m_statemanager.insert_state(Game_state::Game, std::move(std::make_unique<State_game>(&m_context)));
//	m_statemanager.insert_state(Game_state::Character_generator, std::move(std::make_unique<State_character_generator>(&m_context)));
	m_context.m_state_manager = &m_statemanager;
	m_context.m_cache = &m_cache;
	
	comps::add_components (&m_entity_mgr);
	m_context.m_entity_manager = &m_entity_mgr;
	m_system_mgr.add_entity_manager (&m_entity_mgr);
	m_system_mgr.setup_events ();
	systems::add_systems (&m_system_mgr);
	
	m_system_mgr.set_context (&m_context);
	m_context.m_system_manager = &m_system_mgr;
	m_system_mgr.setup_events ();
	m_statemanager.switch_to (Game_state::Game);

	m_context.m_scheduler = &m_scheduler;
}

void Game::update()
{
	
	m_scheduler.pass_time();
	m_window.update();
	m_statemanager.update(m_elapsed);
	restart_clock();
}

void Game::render()
{
	m_window.begin_draw();
	m_statemanager.draw();
	m_window.end_draw();
}

void Game::late_update()
{
	m_statemanager.process_requests();
	restart_clock();
}