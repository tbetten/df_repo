
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

/*
class Command
{
public:
	using Ptr = std::shared_ptr<Command>;
	Command() = default;
	Command(std::string name, Actor* actor) : m_name{ name }, p_actor{ actor } {}
	Command(const Command& other) = default;
	virtual ~Command() {}
	virtual void execute() = 0;
	void set_actor(Actor* actor);
	void set_details(Event_details& details);

	std::string m_name;
protected:
	Actor * p_actor;  // non-owning, can be nullptr (if there are more than one instances of the actor)
	Event_details::Ptr m_details;
};*/

using Command = std::function<void (std::any)>;

class Eventmanager;
struct Shared_context;
//using State_type = int;
const int All_states = 0;
/*
class Actor
{
public:
	using Ptr = std::shared_ptr<Actor>;
	explicit Actor(Shared_context* context) : m_context{ context } {}
	virtual ~Actor() {}
	void add_command_to_eventmanager(Game_state state, const std::string& name, Command command, Eventmanager& eventmanager);
	void remove_command_from_eventmanager(Game_state state, std::string name, Eventmanager& eventmanager);
protected:
	Shared_context * m_context;
};*/

class Commander
{
public:
	void add_command_to_queue(Command command, Eventmanager& eventmanager);
};

class Eventmanager
{
	friend Commander;
public:
	Eventmanager();
	~Eventmanager();
	void add_command(Game_state state, std::string name, Command command);
	void remove_command(Game_state state, std::string name);
	void handle_event(sf::Event);
//	void handle_command();
	void update();
	void set_focus(const bool focus);
	void set_current_state(Game_state type);
private:
	class impl;
	std::unique_ptr<impl> p_impl;
};





