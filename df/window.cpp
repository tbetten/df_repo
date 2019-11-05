#include "stdafx.h"
#include "window.h"
#include "statemanager.h"
#include "game_states.h"
#include <iostream>

Window::Window () : Window ("window", sf::Vector2u (640, 480)) {}

Window::Window (const std::string& title, const sf::Vector2u& size) : m_title{ title }, m_windowsize{ size }
{
	setup();
}

Window::~Window()
{
	destroy();
}

void Window::setup()
{
	m_eventmanager.add_command (Game_state::All_states, "CMD_fullscreen_toggle", [this](auto data) {toggle_fullscreen (data); });
	m_eventmanager.add_command (Game_state::All_states, "CMD_close_window", [this](auto data) {close (); });
	create ();
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
		m_state_manager->handle_event(event);
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
}

void Window::toggle_fullscreen(std::any details)
{
	std::cout << "toggle fullscreen " << std::endl;
	m_fullscreen = !m_fullscreen;
	destroy();
	create();
}

void Window::close()
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