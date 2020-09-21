#include "state_game.h"
#include "window.h"
#include "game_states.h"
#include "resource_cache.h"
#include "systems.h"
#include "renderer.h"
#include "ecs.h"
#include "drawable.h"
#include "attribute_system.h"
#include "character.h"
#include "attributes.h"
#include "tile_type.h"
#include "statemanager.h"
#include "map_data.h"
#include "character_sheet.h"
#include "utils.h"

#include <iostream>
#include <string>
//#include <filesystem>
#include <SFGUI/Widgets.hpp>
#include <SFML/Window.hpp>

//namespace fs = std::filesystem;

State_game::State_game(Shared_context* context) : State{ context }
{
	m_current_entity = 0;
	m_arrow_cursor.loadFromSystem(sf::Cursor::Arrow);
	m_hand_cursor.loadFromSystem(sf::Cursor::Hand);
	m_not_allowed_cursor.loadFromSystem(sf::Cursor::NotAllowed);
	m_current_cursor = &m_arrow_cursor;
	context->m_wind->get_renderwindow()->setMouseCursor(*m_current_cursor);

	m_font.loadFromFile("assets/fonts/arial.ttf");
	m_text.setFont(m_font);
	m_text.setString("hello world");
	m_text.setPosition(sf::Vector2f{ 100.0f, 10.0f });
}

void State_game::on_create()
{
//	auto window = m_context->m_wind->get_renderwindow();
//	auto view = sf::View{ sf::Vector2f{32 * 10, 32 * 10}, sf::Vector2f{32 * 20, 32 * 20} };
//	view.setViewport(sf::FloatRect{ 0.1f , 0.1f, 0.5f, 0.5f });
//	window->setView(view);
	m_event_mgr = m_context->m_event_manager;  // has to be done here, at construction the eventmanager is not yet added to the context
//	m_event_mgr->add_command("CMD_show_party", [this](auto data) {show_party(); });
	m_context->m_system_manager->get_messenger ()->bind ("switched_current_entity", [this] (auto val) {on_change_entity (std::any_cast<ecs::Entity_id>(val)); });
	auto win = sfg::Window::Create (sfg::Window::Style::NO_STYLE);
	win->SetRequisition (sf::Vector2f (m_context->m_wind->get_renderwindow ()->getSize ()));
	auto alignment = sfg::Alignment::Create ();
	alignment->SetScale (sf::Vector2f { 0.0f, 0.0f });
	alignment->SetAlignment (sf::Vector2f { 0.8f, 0.0f });
	//alignment->Add (sfg::Label::Create ("hidehi"));
	m_charsheet = std::make_unique<Character_sheet> (m_context);
	//m_charsheet->populate_party ();
	auto box = sfg::Box::Create (sfg::Box::Orientation::VERTICAL);
	box->Pack (m_charsheet->get_charsheet ());
	alignment->Add (box);
	win->Add (alignment);
	m_desktop.Add (win);

}

void State_game::on_destroy()
{
}

void State_game::update(const sf::Time& time)
{
	m_desktop.Update(time.asSeconds());

	auto window = m_context->m_wind->get_renderwindow();
	auto mousepos = sf::Mouse::getPosition(*window);
	auto worldpos = window->mapPixelToCoords(mousepos);
	auto& map_data = m_context->m_maps->maps[m_context->m_current_map];
	sf::Vector2i coords{ mousepos.x / map_data.m_tilesize.x, mousepos.y / map_data.m_tilesize.y };
	int tile_index{ 0 };
	try
	{
		tile_index = map_data.m_topology->tile_index(coords);
	}
	catch (std::out_of_range& e)
	{
		tile_index = 0;
	}
	m_text.setString("(" + std::to_string(coords.x) + ", " + std::to_string(coords.y) + ")");
	auto entities = map_data.get_entities_at(tile_index);
	auto em = m_context->m_entity_manager;

	auto itr = std::find_if(std::cbegin(entities), std::cend(entities), [em](const ecs::Entity_id e) {return em->has_component(e, ecs::Component_type::Sensor); });
	if (itr != std::cend(entities))
	{
		if (m_current_cursor != &m_hand_cursor)
		{
			auto pos = em->get_data<ecs::Component<Position>>(ecs::Component_type::Position, m_current_entity);
			m_current_cursor = map_data.m_topology->are_neighbours(coords, pos->coords) ? &m_hand_cursor : &m_not_allowed_cursor;
			window->setMouseCursor(*m_current_cursor);
		}
	}
	else
	{
		if (m_current_cursor != &m_arrow_cursor)
		{
			m_current_cursor = &m_arrow_cursor;
			window->setMouseCursor(*m_current_cursor);
		}
	}
	
/*	for (auto layer : entities)
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
	}*/
	auto dt = time.asMicroseconds();
	m_context->m_system_manager->update (time.asMicroseconds());
}

void State_game::on_change_entity (ecs::Entity_id entity)
{
	m_current_entity = entity;
	auto combo = find_widget<sfg::ComboBox> ("party_combobox");
	auto entity_mgr = m_context->m_entity_manager;
	auto character_comp = entity_mgr->get_data<ecs::Component<Character>> (ecs::Component_type::Character, entity);
	for (auto itr = combo->Begin (); itr != combo->End (); ++itr)
	{
		if (*itr == character_comp->name)
		{
			combo->SelectItem (std::distance(combo->Begin(), itr));
			m_charsheet->on_select ();
		}
	}
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

	if (m_charsheet == nullptr) m_charsheet = std::make_unique<Character_sheet>(m_context);
	m_charsheet->populate_party ();
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
	auto name_label = std::dynamic_pointer_cast<sfg::Label>(sfg::Widget::GetWidgetById("name_label"));
	auto cp_label = std::dynamic_pointer_cast<sfg::Label>(sfg::Widget::GetWidgetById("cp_label"));
	auto gender = character->gender == Gender::Male ? "male" : "female";
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