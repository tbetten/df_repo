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
#include "ecs_base.h"
#include "system.h"
#include "attribute_system.h"
#include "attribute_comp.h"

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
	add_command_to_eventmanager(Game_state::Game, menu_state, *(m_context->m_event_manager));

	auto pause_state = std::make_shared<CMD_pause>(std::string("CMD_pause"), this);
	add_command_to_eventmanager(Game_state::Game, pause_state, *(m_context->m_event_manager));
}

void State_game::on_destroy()
{
	remove_command_from_eventmanager(Game_state::Game, std::string("CMD_game_menu"), *(m_context->m_event_manager));
	remove_command_from_eventmanager(Game_state::Game, std::string("CMD_pause"), *(m_context->m_event_manager));
}

void State_game::update(const sf::Time& time)
{
	Bitmask b;
	b.set (to_number (Component::Attributes));
	auto entity_mgr = m_context->m_entity_manager;
	auto ent = entity_mgr->add_entity (b, true);
	auto comp = entity_mgr->get_component<Attribute_comp> (ent, Component::Attributes);
	auto x = comp->m_attributes[Attribute::ST]->base;
	std::cout << "strength base is " << x << std::endl;

	auto system_mgr = m_context->m_system_manager;
	auto attr_syst = system_mgr->get_system<Attribute_system> (System::Attribute);
//	attr_syst->add_entity (ent);
	attr_syst->reset (ent);
	
	attr_syst->spend_points (ent, Attribute::ST, 4);
	x = comp->m_attributes[Attribute::ST]->bought;
	std::cout << "strength bought is " << x << std::endl;

	Tilemap tm{ m_context };
//	tm.m_context = m_context;
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
	auto x_offset = tm.m_hex_side_length / 2;
	auto y_offset = static_cast<int> (std::round((std::sqrt(3) / 2.0) * tm.m_hex_side_length));
	m_sprite2.setPosition(m_tileset.m_tilesize.x - x_offset, y_offset);

	m_l = tm.m_layers[0];
}

void State_game::draw()
{
	auto window = m_context->m_wind->get_renderwindow();
	//window->draw(m_sprite);
	//window->draw(m_sprite2);
	window->draw(m_l);
	//window->draw(m_grid);
}

void State_game::to_mainmenu(const Event_details& details)
{
	m_context->m_state_manager->switch_to(Game_state::Main_menu);
}

void State_game::pause(const Event_details& details)
{
	m_context->m_state_manager->switch_to(Game_state::Paused);
}