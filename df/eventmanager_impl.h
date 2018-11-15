#pragma once
#include <SFML\Graphics.hpp>
#include <vector>
#include <unordered_map>
#include <memory>
#include <deque>
#include "eventmanager.h"

enum class Event_type
{
	KeyDown = sf::Event::KeyPressed,
	KeyUp = sf::Event::KeyReleased,
	MButtonDown = sf::Event::MouseButtonPressed,
	MButtonUp = sf::Event::MouseButtonReleased,
	MouseWheel = sf::Event::MouseWheelScrolled,
	WindowResized = sf::Event::Resized,
	GainedFocus = sf::Event::GainedFocus,
	LostFocus = sf::Event::LostFocus,
	MouseEntered = sf::Event::MouseEntered,
	MouseLeft = sf::Event::MouseLeft,
	Closed = sf::Event::Closed,
	TextEntered = sf::Event::TextEntered,
	Keyboard = sf::Event::Count + 1,
	Mouse,
	Joystick
};

struct Event_info
{
	Event_info() { m_code = 0; }
	Event_info(int event) { m_code = event; }

	union
	{
		int m_code;
	};
};

using Events = std::vector<std::pair<Event_type, Event_info>>;

struct Binding
{
	using Ptr = std::unique_ptr<Binding>;
	static Ptr create(const std::string&name) { return Ptr(new Binding(name)); }  // cannot use make_unique because it can't access private constructor

	void bind_event(Event_type type, Event_info info = Event_info{})
	{
		m_events.emplace_back(type, info);
	}

	void handle_event(sf::Event e);

	Events m_events;
	std::string m_name;
	int c;
	Event_details m_details;

private:
	Binding(const std::string& name) : m_name{ name }, m_details{ name }, c{ 0 } {}
};

using Bindings = std::vector<Binding::Ptr>;

using Command_container = std::unordered_map<std::string, Command::Ptr>;

using Commands = std::unordered_map<Game_state, Command_container>;

/*class Eventmanager::impl
{
public:
impl() : p_impl(new impl) {}
~impl() {}
}
void add_binding(Binding::Ptr binding);
bool remove_binding(const std::string& name);
void set_focus(const bool focus) { m_focus = focus; }
void handle_event(sf::Event& event);
void update();
sf::Vector2i get_mouse_pos(sf::RenderWindow* wind = nullptr)
{
return (wind ? sf::Mouse::getPosition(*wind) : sf::Mouse::getPosition());
}

private:
void load_bindings();
Bindings m_bindings;
Commands m_commands;
bool m_focus;
std::deque<Command::Ptr> m_command_queue;
};*/
