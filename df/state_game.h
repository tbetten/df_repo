#pragma once
#include "state.h"
#include "resource_cache.h"
#include "character_sheet.h"
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

#include <memory>

struct Tilemap;
//class Character_sheet;

class State_game : public State
{
public:
	State_game(Shared_context* context) : State{ context } {}

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

	bool m_first{ true };
	std::shared_ptr<Tilemap> m_map;
	Eventmanager* m_event_mgr = nullptr;
	std::unordered_map<std::string, ecs::Entity_id> m_party;
//	std::shared_ptr<cache::Resource_base> m_font_resource;
	sf::Font m_font;
	sf::Text m_text;
	std::unique_ptr<Character_sheet> m_charsheet;
	sfg::Widget::Ptr m_remove = nullptr;

	sfg::SFGUI m_sfgui;
	sfg::Desktop m_desktop;
};