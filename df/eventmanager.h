
#pragma once
#include <memory>
#include <string>
#include <SFML/Graphics.hpp>

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
	sf::Uint32 m_text_entered;
	sf::Vector2i m_mouse;
	int m_mousewheel_delta;
	int m_keycode;
};


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
};

class Eventmanager;
struct Shared_context;
using State_type = int;
const int All_states = 0;

class Actor
{
public:
	using Ptr = std::shared_ptr<Actor>;
	explicit Actor(Shared_context* context) : m_context{ context } {}
	virtual ~Actor() {}
	void add_command_to_eventmanager(State_type state, Command::Ptr command, Eventmanager& eventmanager);
	void remove_command_from_eventmanager(State_type state, std::string name, Eventmanager& eventmanager);
protected:
	Shared_context * m_context;
};

class Commander
{
public:
	void add_command_to_queue(Command::Ptr command, Eventmanager& eventmanager);
};

class Eventmanager
{
	friend Commander;
public:
	Eventmanager();
	~Eventmanager();
	void add_command(State_type state, std::string name, Command::Ptr command);
	void remove_command(State_type state, std::string name);
	void handle_event(sf::Event);
	void handle_command();
	void update();
	void set_focus(const bool focus);
	void set_current_state(State_type type);
private:
	class impl;
	std::unique_ptr<impl> p_impl;
};





