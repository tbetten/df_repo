#include "stdafx.h"
#include "state_game.h"
#include "window.h"
#include "game_states.h"
#include "keynames.h"
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
#include "statemanager.h"
#include "entity_loader.h"

namespace fs = std::filesystem;

void State_game::on_create()
{
	auto window = m_context->m_wind->get_renderwindow();
	auto view = sf::View{ sf::Vector2f{32 * 10, 32 * 10}, sf::Vector2f{32 * 20, 32 * 20} };
	view.setViewport(sf::FloatRect{ 0.1f , 0.1f, 0.5f, 0.5f });
	window->setView(view);
	auto m_event_mgr = m_context ->m_event_manager;
	m_event_mgr->add_command (Game_state::Game, "CMD_game_menu", [this](auto data) {to_mainmenu (); });
	m_event_mgr->add_command (Game_state::Game, "CMD_pause", [this](auto data) {pause (); });
	m_event_mgr->add_command(Game_state::Game, "CMD_character_generator", [this](auto data) {to_chargen(); });

	//m_map = std::make_shared<Tilemap> (m_context);
	//m_map->load_from_file ("d:/dfmaps/test.tmx");
	//m_context->m_current_map = m_map.get ();
	auto cache = m_context->m_cache;
	auto em = m_context->m_entity_manager;
	auto bit = static_cast<int>(ecs::Component_type::Drawable);
	ecs::Bitmask b;
	b.set(bit);
	auto entity = em->add_entity(b);
	
//	m_font_resource = cache->get_obj("arial");
//	m_font = cache::get_val<sf::Font>(m_font_resource.get());
//	m_font.loadFromFile("assets/fonts/arial.ttf");
//	m_text.setFont(*m_font);
}

void State_game::on_destroy()
{
	//auto eventmgr = m_state_mgr->get_context ()->m_event_manager;
	if (m_event_mgr)
	{
		m_event_mgr->remove_command(Game_state::Game, std::string("CMD_game_menu"));
		m_event_mgr->remove_command(Game_state::Game, std::string("CMD_pause"));
	}
}

void State_game::update(const sf::Time& time)
{
	if (m_first)
	{
		el::Entity_loader el{ m_context };
		ecs::Entity_id id{ 0 };
		ecs::Entity_id id2{ 0 };
		ecs::Entity_id id3{ 0 };
		el.load_map("test");
		if (auto opt = el.load_entity("big_kobold"))
		{
			id = *opt;
			el.set_position(id, sf::Vector2u{ 1,1 }, 2, "test");
			el.set_player_controlled(id, true);
		}
		if (auto opt = el.load_entity("big_kobold"))
		{
			id2 = *opt;
			el.set_position(id2, sf::Vector2u{ 5,5 }, 2, "test");
		}
		if (auto opt = el.load_entity("sword"))
		{
			id3 = *opt;
			el.set_position(id3, sf::Vector2u{ 4,5 }, 2, "test");
		}

		std::cout << id << "\t " << id2 << "\n";
		m_first = false;
	}
	auto window = m_context->m_wind->get_renderwindow();
	auto mousepos = sf::Mouse::getPosition(*window);
	auto worldpos = window->mapPixelToCoords(mousepos);

//	m_text.setString(std::to_string(worldpos.x) + "\t" + std::to_string(worldpos.y));
//	m_text.setPosition(sf::Vector2f{200, 100});
//	window->draw(m_text);
	auto dt = time.asMicroseconds();
	m_context->m_system_manager->update (time.asMicroseconds());
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
	//window->draw(m_text);
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