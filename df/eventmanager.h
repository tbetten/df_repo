
#pragma once
#include <memory>
#include <string>
#include <any>
#include <functional>
#include <SFML/Graphics.hpp>
#include "game_states.h"
//#include "messaging.h"


class Actor;

struct Event_details
{
	using Ptr = std::unique_ptr<Event_details>;
	Event_details(const std::string& bind_name) : m_name{ bind_name }
	{
		clear();
	}

	void clear()
	{
		m_size = sf::Vector2i{ 0,0 };
		m_text_entered = 0;
		m_mouse = sf::Vector2i{ 0,0 };
		m_mousewheel_delta = 0;
		m_keycode = sf::Keyboard::Unknown;
	}

	std::string m_name;
	sf::Vector2i m_size;
	sf::Uint32 m_text_entered = 0;
	sf::Vector2i m_mouse;
	int m_mousewheel_delta = 0;
	int m_keycode = 0;
};

using Command = std::function<void (std::any)>;

class Eventmanager;
struct Shared_context;
const int All_states = 0;

class Eventmanager
{
public:
	Eventmanager();
	~Eventmanager();
	void add_command(Game_state state, std::string name, Command command);
	void remove_command(Game_state state, std::string name);
	void handle_event(sf::Event);
	void update();
	void set_focus(const bool focus);
	void set_current_state(Game_state type);
private:
	class impl;
	std::unique_ptr<impl> p_impl;
};





