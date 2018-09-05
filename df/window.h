#pragma once
#pragma once
#include <string>
#include <SFML\Graphics.hpp>
#include "eventmanager.h"


class CMD_toggle_fullscreen : public Command
{
public:
	CMD_toggle_fullscreen(std::string name, Actor* actor) : Command(name, actor) {}
	void execute();
};

class CMD_close_window : public Command
{
public:
	CMD_close_window(std::string name, Actor* actor) : Command(name, actor) {}
	void execute();
};

class Window : public Actor
{
public:
	explicit Window(Shared_context* context);
	Window(const std::string& title, const sf::Vector2u& size, Shared_context* context);
	~Window();

	void begin_draw();
	void end_draw();
	void update();

	bool is_done() { return m_done; }
	bool is_fullscreen() { return m_fullscreen; }
	bool is_focused() { return m_focus; }

	sf::Vector2u get_windowsize() { return m_windowsize; }
	Eventmanager* get_eventmanager() { return &m_eventmanager; }
	sf::RenderWindow* get_renderwindow() { return &m_window; }

	void toggle_fullscreen(const Event_details& details);
	void close(const Event_details& details);
	void close() { m_done = true; }
	void draw(sf::Drawable& drawable) { m_window.draw(drawable); }

private:
	void setup(const std::string& title, const sf::Vector2u& size);
	void destroy();
	void create();

	sf::RenderWindow m_window;
	Eventmanager m_eventmanager;
	sf::Vector2u m_windowsize;
	std::string m_title;
	bool m_done;
	bool m_fullscreen;
	bool m_focus;
};