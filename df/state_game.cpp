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
#include "drawable_comp.h"
#include "position_comp.h"
#include "hexlib.h"

namespace fs = std::filesystem;

void State_game::on_create()
{
	auto eventmgr = m_context ->m_event_manager;
	eventmgr->add_command (Game_state::Game, "CMD_game_menu", [this](auto data) {to_mainmenu (); });
	eventmgr->add_command (Game_state::Game, "CMD_pause", [this](auto data) {pause (); });

	m_grid_layout = hexlib::Layout (hexlib::flat, hexlib::Point{ 32.0, 32.0 }, hexlib::Point{150.0, 100.0});
	auto grid = hexlib::create_map (15, 5, hexlib::flat);
	m_grid = hexlib::to_vertex_array (grid, m_grid_layout);


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
		Bitmask b;
		b.set (to_number (Component::Position));
		b.set (to_number (Component::Drawable));
		auto entity_mgr = m_context->m_entity_manager;
		auto ent = entity_mgr->add_entity (b, true);
		auto comp = entity_mgr->get_component<Drawable_comp> (ent, Component::Drawable);
		auto pos = entity_mgr->get_component<Position_comp> (ent, Component::Position);

		comp->init ("big_kobold", m_context->m_cache, m_grid_layout);
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
	window->draw(m_grid);
	m_context->m_system_manager->draw (m_context->m_wind);
}

void State_game::to_mainmenu()
{
	m_context->m_state_manager->switch_to(Game_state::Main_menu);
}

void State_game::pause()
{
	m_context->m_state_manager->switch_to(Game_state::Paused);
}