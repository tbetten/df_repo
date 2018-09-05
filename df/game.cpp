#include "stdafx.h"
#include "game.h"
#include "game_states.h"
//#include "state_intro.h"
//#include "state_main_menu.h"
#include "state_game.h"
//#include "state_paused.h"
#include <iostream>

Game::Game() : m_window{ "Theotris", sf::Vector2u(800, 600), &m_context }, m_statemanager{ &m_context }
{
	m_clock.restart();
	//std::srand(time(nullptr));

	m_context.m_wind = &m_window;
	m_context.m_event_manager = m_window.get_eventmanager();

//	m_statemanager.insert_state(state_to_int(Game_state::Intro), std::move(std::make_unique<State_intro>(&m_context)));
//	m_statemanager.insert_state(state_to_int(Game_state::Main_menu), std::move(std::make_unique<State_main_menu>(&m_context)));
	m_statemanager.insert_state(state_to_int(Game_state::Game), std::move(std::make_unique<State_game>(&m_context)));
//	m_statemanager.insert_state(state_to_int(Game_state::Paused), std::move(std::make_unique<State_paused>(&m_context)));
	m_context.m_state_manager = &m_statemanager;
	m_context.m_cache = &m_cache;

	m_statemanager.switch_to(state_to_int(Game_state::Game));
}

void Game::update()
{
	m_window.update();
	m_statemanager.update(m_elapsed);
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