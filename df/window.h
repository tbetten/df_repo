#pragma once
#pragma once
#include <string>
#include <SFML\Graphics.hpp>
#include "eventmanager.h"

class State_manager;

class Window 
{
public:
	Window();
	Window(const std::string& title, const sf::Vector2u& size);
	~Window();

	void set_statemanager(State_manager* sm) { m_state_manager = sm; }

	void begin_draw();
	void end_draw();
	void update();

	bool is_done() { return m_done; }
	bool is_fullscreen() { return m_fullscreen; }
	bool is_focused() { return m_focus; }

	sf::Vector2u get_windowsize() { return m_windowsize; }
	Eventmanager* get_eventmanager() { return &m_eventmanager; }
	sf::RenderWindow* get_renderwindow() { return &m_window; }

	void toggle_fullscreen(std::any details);
//	void close(const Event_details& details);
	void close ();// { m_done = true; }
	void draw(sf::Drawable& drawable) { m_window.draw(drawable); }

private:
	void setup();
	void destroy();
	void create();

	
	Eventmanager m_eventmanager;
	State_manager* m_state_manager;
	sf::Vector2u m_windowsize;
	std::string m_title;
	bool m_done = false;
	bool m_fullscreen = false;
	bool m_focus = true;
	sf::RenderWindow m_window;
};