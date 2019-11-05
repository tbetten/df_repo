#pragma once
#include "state.h"
#include <memory>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

class State_main_menu : public State
{
public:
	State_main_menu(Shared_context* context) : State{ context } {}

	void on_create() override;
	void on_destroy() override;
	void activate() override;
	void deactivate() override;
	void update(const sf::Time& time) override;
	void handle_sfml_event(sf::Event& e) override;
	void draw() override;

	void to_game();
private:
	sfg::SFGUI m_sfgui;
	sfg::Window::Ptr m_gui_window;
	sfg::Box::Ptr m_root_widget;
	sfg::Desktop m_desktop;
	bool m_done{ false };
	void on_click();
};