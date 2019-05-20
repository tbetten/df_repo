#pragma once
#include "state.h"

#include <memory>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>

class State_character_generator : public State
{
public:
	State_character_generator(Shared_context* context) : State{ context } {}

	void on_create() override;
	void on_destroy() override;
	void activate() override {}
	void deactivate() override {}
	void update(const sf::Time& time) override;
	void draw() override;

	void to_game();

private:
	sfg::SFGUI m_sfgui;
	sfg::Window::Ptr m_gui_window;
};