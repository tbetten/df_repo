#include "stdafx.h"
#include "state_game.h"
#include "window.h"
#include "game_states.h"
#include "keynames.h"
#include "tileset.h"
#include "tilemap.h"
#include "grid.h"
#include <iostream>
#include <string>
#include <filesystem>
#include "resource_cache.h"
//#include "ecs_base.h"
//#include "system.h"
#include "systems.h"
#include "renderer.h"
#include "ecs.h"
#include "drawable.h"
#include "attribute_system.h"
#include "attribute_comp.h"
#include "drawable_comp.h"
#include "position_comp.h"
#include "hexlib.h"
#include "statemanager.h"

namespace fs = std::filesystem;

void State_game::on_create()
{
	auto eventmgr = m_context ->m_event_manager;
	eventmgr->add_command (Game_state::Game, "CMD_game_menu", [this](auto data) {to_mainmenu (); });
	eventmgr->add_command (Game_state::Game, "CMD_pause", [this](auto data) {pause (); });
	eventmgr->add_command(Game_state::Game, "CMD_character_generator", [this](auto data) {to_chargen(); });

	m_map = std::make_shared<Tilemap> (m_context);
	m_map->load_from_file ("d:/dfmaps/test.tmx");
	m_context->m_current_map = m_map.get ();
}

void State_game::on_destroy()
{
	auto eventmgr = m_state_mgr->get_context ()->m_event_manager;
	eventmgr->remove_command (Game_state::Game, std::string ("CMD_game_menu"));
	eventmgr->remove_command (Game_state::Game, std::string ("CMD_pause"));
}

void State_game::update(const sf::Time& time)
{
	if (m_first)
	{
		auto id = comps::spawn_from_key (m_context, "big_kobold", sf::Vector2u{ 3, 6 }, m_map);
		auto id2 = comps::spawn_from_key(m_context, "big_kobold", sf::Vector2u{ 5, 6 }, m_map);
		auto id3 = comps::spawn_from_key(m_context, "sword", sf::Vector2u{ 4,5 }, m_map);
		std::cout << id << "\t " << id2 << "\n";
		m_first = false;
	}
	m_context->m_system_manager->update (1);
}

void State_game::draw()
{
	auto window = m_context->m_wind->get_renderwindow();
	//window->draw(m_sprite);
	//window->draw(m_sprite2);
	//window->draw(m_l);
//	window->draw (*m_grid);
//	window->draw (*m_map);
//	window->draw(m_grid);
	auto renderer = m_context->m_system_manager->get_system<systems::Renderer> (ecs::System_type::Renderer);
	renderer->render (window);
	//m_context->m_system_manager->draw (m_context->m_wind);
}

void State_game::to_mainmenu()
{
	m_context->m_state_manager->switch_to(Game_state::Main_menu);
}

void State_game::to_chargen()
{
	m_context->m_state_manager->switch_to(Game_state::Character_generator);
}

void State_game::pause()
{
	m_context->m_state_manager->switch_to(Game_state::Paused);
}