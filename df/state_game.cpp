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
#include "character.h"
#include "attributes.h"
#include "tile_type.h"
#include "statemanager.h"
#include "entity_loader.h"
#include "map_data.h"
#include "character_sheet.h"

#include <SFGUI/Widgets.hpp>

namespace fs = std::filesystem;

void State_game::on_create()
{
//	auto window = m_context->m_wind->get_renderwindow();
//	auto view = sf::View{ sf::Vector2f{32 * 10, 32 * 10}, sf::Vector2f{32 * 20, 32 * 20} };
//	view.setViewport(sf::FloatRect{ 0.1f , 0.1f, 0.5f, 0.5f });
//	window->setView(view);
	m_event_mgr = m_context ->m_event_manager;
	m_font.loadFromFile("assets/fonts/arial.ttf");
	m_text.setFont(m_font);
	m_text.setString("hello world");
	m_text.setPosition(sf::Vector2f{ 100.0f, 10.0f });
	m_event_mgr->add_command(Game_state::Game, "CMD_show_party", [this](auto data) {show_party(); });
//	auto gui_window = sfg::Window::Create(sfg::Window::Style::TOPLEVEL);
//	auto label = sfg::Label::Create("hoi pipeloi");
//	gui_window->Add(label);
//	m_desktop.Add(gui_window);
}

void State_game::on_destroy()
{
}

void State_game::update(const sf::Time& time)
{
	m_desktop.Update(time.asSeconds());
	if (m_first)
	{
		el::Entity_loader el{ m_context };
		ecs::Entity_id id{ 0 };
		ecs::Entity_id id2{ 0 };
		ecs::Entity_id id3{ 0 };
//		el.load_map("test");
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
			el.set_position(id3, sf::Vector2u{ 4,5 }, 1, "test");
		}

		std::cout << id << "\t " << id2 << "\n";
		m_first = false;
	}
	auto window = m_context->m_wind->get_renderwindow();
	auto mousepos = sf::Mouse::getPosition(*window);
	auto worldpos = window->mapPixelToCoords(mousepos);
	auto& map_data = m_context->m_maps->maps[m_context->m_current_map];
	sf::Vector2i coords{ mousepos.x / map_data.tilesize.x, mousepos.y / map_data.tilesize.y };
	auto tile_index = coords.y * map_data.mapsize.y + coords.x;
	m_text.setString("(" + std::to_string(coords.x) + ", " + std::to_string(coords.y) + ")");
	auto entities = map_data.get_entities_at(tile_index);
	auto em = m_context->m_entity_manager;
	for (auto layer : entities)
	{
		for (auto entity : layer)
		{
			if (em->has_component(entity, ecs::Component_type::Attributes))
			{
				auto data = em->get_data<ecs::Component<Attributes>>(ecs::Component_type::Attributes, entity);
				m_text.setString(std::to_string (data->data[0].bought));
			}
			if (em->has_component(entity, ecs::Component_type::Tile_type))
			{
				auto data = em->get_data<ecs::Component<Tile_type>>(ecs::Component_type::Tile_type, entity);
				m_text.setString(data->description);
			}
		}
	}
	auto dt = time.asMicroseconds();
	m_context->m_system_manager->update (time.asMicroseconds());
}

void State_game::draw()
{
	auto window = m_context->m_wind->get_renderwindow();

	auto renderer = m_context->m_system_manager->get_system<systems::Renderer> (ecs::System_type::Renderer);
	renderer->render (window);
	window->draw(m_text);
	if (m_remove != nullptr)
	{
		m_desktop.Remove(m_remove);
		m_remove = nullptr;
	}
	m_sfgui.Display(*window);
}

void State_game::show_party()
{
	std::cout << "show party\n";
	auto gui_window = sfg::Window::Create(sfg::Window::Style::BACKGROUND | sfg::Window::Style::TITLEBAR);
	gui_window->SetTitle("Party");

	m_charsheet = std::make_unique<Character_sheet>(m_context);
	auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	auto sheet = m_charsheet->get_charsheet();
	box->Pack(sheet, false);
	auto close_button = sfg::Button::Create("Close");
	close_button->GetSignal(sfg::Widget::OnLeftClick).Connect([this]() {close_window(); });
	box->Pack(close_button, false);
	gui_window->Add(box);
	m_desktop.Add(gui_window);
}

void State_game::close_window()
{
	auto widget = sfg::Context().Get().GetActiveWidget();
	m_charsheet.reset(nullptr);
	while (widget->GetName() != "Window")
	{
		widget = widget->GetParent();
	}
	m_remove = widget;
}

void State_game::on_select(sfg::ComboBox::Ptr cb)
{
	auto em = m_context->m_entity_manager;
	auto name = cb->GetSelectedText();
	auto entity = m_party[name];
	auto character = em->get_data<ecs::Component<Character>>(ecs::Component_type::Character, entity);
	auto widget = sfg::Context().Get().GetActiveWidget();
	while (widget->GetName() != "Box")
	{
		widget = widget->GetParent();
	}
	auto x = sfg::Widget::GetWidgetById("name_label");
	auto name_label = std::dynamic_pointer_cast<sfg::Label>(x);
	auto cp_label = std::dynamic_pointer_cast<sfg::Label>(sfg::Widget::GetWidgetById("cp_label"));
	auto gender = character->male ? "male" : "female";
	auto race = race_to_string(character->race);
	auto cp = character->character_points;
	name_label->SetText(name + ", " + gender + " " + race);
	cp_label->SetText("Available character points: " + std::to_string(cp));
}

sfg::Box::Ptr State_game::create_charsheet()
{
	auto outer = sfg::Box::Create(sfg::Box::Orientation::HORIZONTAL);
	auto inner = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	auto sheet = sfg::Notebook::Create();

	auto char_page = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
	char_page->SetSpacing(20.0f);
	auto name_label = sfg::Label::Create("<name>, <gender> <race>");
	name_label->SetId("name_label");
	auto cp_label = sfg::Label::Create("Available character points: <cp>");
	cp_label->SetId("cp_label");
	char_page->Pack(name_label, false);
	char_page->Pack(cp_label, false);
	sheet->AppendPage(char_page, sfg::Label::Create("Character"));
	inner->Pack(sheet, false);
	outer->Pack(inner, false);
	return outer;
}

void State_game::handle_sfml_event(sf::Event& e)
{
	m_desktop.HandleEvent(e);
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