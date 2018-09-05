#include "stdafx.h"
#include "window.h"
#include "statemanager.h"
#include "game_states.h"
#include <iostream>

void CMD_toggle_fullscreen::execute()
{
	//auto actor = p_actor.get();
	auto window = dynamic_cast<Window*> (p_actor);
	window->toggle_fullscreen(*m_details);
	//auto win = dynamic_cast<std::shared_ptr<Window>> (*p_actor);
	//win.toggle_fullscreen();
}

void CMD_close_window::execute()
{
	//auto actor = p_actor.get();
	auto window = dynamic_cast<Window*>(p_actor);
	window->close(*m_details);
}

Window::Window(Shared_context *context) : Actor{ context }
{
	setup("window", sf::Vector2u(640, 480));
}

Window::Window(const std::string& title, const sf::Vector2u& size, Shared_context* context) : Actor{ context }
{
	setup(title, size);
}

Window::~Window()
{
	destroy();
}

void Window::setup(const std::string& title, const sf::Vector2u& size)
{
	m_title = title;
	m_windowsize = size;
	m_fullscreen = false;
	m_done = false;
	m_focus = true;
	create();
	//std::string name = "CMD_toggle_fullscreen";
	auto fullscreen_cmd = std::make_shared<CMD_toggle_fullscreen>(std::string("CMD_fullscreen_toggle"), this);
	//fullscreen_cmd->set_actor(this);
	//auto x = std::make_shared<CMD_toggle_fullscreen>(fullscreen_cmd);
	add_command_to_eventmanager(state_to_int(Game_state::All_states), fullscreen_cmd, m_eventmanager);
	auto close_cmd = std::make_shared<CMD_close_window>(std::string{ "CMD_close_window" }, this);
	//close_cmd->set_actor(this);
	add_command_to_eventmanager(state_to_int(Game_state::All_states), close_cmd, m_eventmanager);
}

void Window::create()
{
	auto style = (m_fullscreen ? sf::Style::Fullscreen : sf::Style::Default);
	m_window.create({ m_windowsize.x, m_windowsize.y, 32 }, m_title, style);
}

void Window::destroy()
{
	m_window.close();
}

void Window::update()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::LostFocus)
		{
			m_focus = false;
			m_eventmanager.set_focus(false);
		}
		else if (event.type == sf::Event::GainedFocus)
		{
			m_focus = true;
			m_eventmanager.set_focus(true);
		}
		m_eventmanager.handle_event(event);
	}
	m_eventmanager.update();
	m_eventmanager.handle_command();
}

void Window::toggle_fullscreen(const Event_details& details)
{
	std::cout << "toggle fullscreen " << std::endl;
	m_fullscreen = !m_fullscreen;
	destroy();
	create();
}

void Window::close(const Event_details& details)
{
	std::cout << "close command" << std::endl;
	m_done = true;
}

void Window::begin_draw()
{
	m_window.clear(sf::Color::Black);
}

void Window::end_draw()
{
	m_window.display();
}