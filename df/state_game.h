#pragma once
#include "state.h"
#include "character_sheet.h"
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <any>
#include <memory>

namespace event
{
	struct Event_info; 
}

class State_game : public State
{
public:
	explicit State_game(Shared_context* context);

	void on_create() override;
	void on_destroy() override;
	void activate() override {}
	void deactivate() override {}
	void update(const sf::Time& time) override;
	void handle_sfml_event(sf::Event& e) override;
	void draw() override;

	void to_mainmenu();
	void pause();
	void to_chargen();
	

private:
	void show_party();
	
	void on_select(sfg::ComboBox::Ptr cb);
	sfg::Box::Ptr create_charsheet();
	void close_window();

	event::Event_manager* m_event_mgr = nullptr;
	std::unordered_map<std::string, ecs::Entity_id> m_party;
	ecs::Entity_id m_current_entity;
	sf::Font m_font;
	sf::Text m_text;
	sf::Cursor m_hand_cursor;
	sf::Cursor m_arrow_cursor;
	sf::Cursor m_not_allowed_cursor;
	sf::Cursor* m_current_cursor{ nullptr };
	std::unique_ptr<Character_sheet> m_charsheet;
	sfg::Widget::Ptr m_remove = nullptr;

	sfg::SFGUI m_sfgui;
	sfg::Desktop m_desktop;
};