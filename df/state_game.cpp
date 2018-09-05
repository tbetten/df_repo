#include "stdafx.h"
#include "state_game.h"
#include "window.h"
#include "game_states.h"
#include "keynames.h"
#include "tileset.h"
#include "tilemap.h"
#include <iostream>
#include <string>
#include <filesystem>
#include "resource_cache.h"

namespace fs = std::filesystem;

void CMD_game_menu::execute()
{
	auto state_game = dynamic_cast<State_game*>(p_actor);
	state_game->to_mainmenu(*m_details);
}

void CMD_pause::execute()
{
	auto state_game = dynamic_cast<State_game*>(p_actor);
	state_game->pause(*m_details);
}

void State_game::on_create()
{
	auto menu_state = std::make_shared<CMD_game_menu>(std::string("CMD_game_menu"), this);
	add_command_to_eventmanager(state_to_int(Game_state::Game), menu_state, *(m_context->m_event_manager));

	auto pause_state = std::make_shared<CMD_pause>(std::string("CMD_pause"), this);
	add_command_to_eventmanager(state_to_int(Game_state::Game), pause_state, *(m_context->m_event_manager));
}

void State_game::on_destroy()
{
	remove_command_from_eventmanager(state_to_int(Game_state::Game), std::string("CMD_game_menu"), *(m_context->m_event_manager));
	remove_command_from_eventmanager(state_to_int(Game_state::Game), std::string("CMD_pause"), *(m_context->m_event_manager));
}

void State_game::update(const sf::Time& time)
{
	Tilemap tm;
	tm.load_from_file("assets/maps/wilderness.tmx");
	auto ts = m_context->m_cache;
	auto tileset_obj = ts->get_obj("fantasyhextiles_v2.tsx");
	auto tileset = get_val<Tileset>(tileset_obj.get());
	m_tileset = *tileset;
	m_text = m_tileset.m_tiles;
	m_sprite.setTexture(m_tileset.m_tiles);
	m_sprite.setTextureRect(m_tileset.get_tile(1));
	m_sprite2.setTexture(m_tileset.m_tiles);
	m_sprite2.setTextureRect(m_tileset.get_tile(32));
	m_sprite2.setPosition(m_tileset.m_tilesize.x, 0);
}

void State_game::draw()
{
	auto window = m_context->m_wind->get_renderwindow();
	window->draw(m_sprite);
	window->draw(m_sprite2);
	//window->draw(m_grid);
}

void State_game::to_mainmenu(const Event_details& details)
{
	m_context->m_state_manager->switch_to(state_to_int(Game_state::Main_menu));
}

void State_game::pause(const Event_details& details)
{
	m_context->m_state_manager->switch_to(state_to_int(Game_state::Paused));
}