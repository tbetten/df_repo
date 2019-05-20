#include "stdafx.h"
#include "eventmanager_impl.h"
#include "statemanager.h"
#include "utils.h"
#include "keynames.h"
#include "mousebutton.h"
#include "event_types.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <filesystem>
#include "messaging.h"

namespace fs = std::filesystem;

class Eventmanager::impl
{
	friend class Eventmanager;
	friend class Commander;
public:
	impl();
	~impl();

	void add_binding(Binding::Ptr binding);
	bool remove_binding(const std::string& name);
	void set_focus(const bool focus) { m_focus = focus; }
	void handle_event(sf::Event& event);
//	void handle_command();
	void update();
private:
	void load_bindings();
	Bindings create_bindings(std::string& command_name, Event_info i, bool has_alt, bool has_ctrl, bool has_shift);

	Game_state m_current_state;
	Bindings m_bindings;
	Commands m_commands;
	bool m_focus;
	std::deque<Command> m_command_queue;
};

Eventmanager::Eventmanager() :p_impl(std::make_unique<impl>()) {}
Eventmanager::~Eventmanager() = default;

Eventmanager::impl::impl() : m_focus{ true } { load_bindings(); }
Eventmanager::impl::~impl() {}

void Eventmanager::add_command(Game_state state, std::string name, Command command)
{
	auto itr = p_impl->m_commands.emplace(state, Command_container()).first;
	itr->second.emplace(name, command);
}

void Eventmanager::remove_command(Game_state state, std::string name)
{
	auto state_itr = p_impl->m_commands.find(state);
	if (state_itr != p_impl->m_commands.end())
	{
		auto command_itr = state_itr->second.find(name);
		if (command_itr != state_itr->second.end())
		{
			state_itr->second.erase(name);
		}
	}
}

void Eventmanager::handle_event(sf::Event event)
{
	p_impl->handle_event(event);
}

void Eventmanager::update()
{
	p_impl->update();
}

void Eventmanager::set_focus(const bool focus)
{
	p_impl->m_focus = focus;
}

void Eventmanager::set_current_state(Game_state state)
{
	p_impl->m_current_state = state;
}

void Commander::add_command_to_queue(Command command, Eventmanager& eventmanager)
{
	eventmanager.p_impl->m_command_queue.push_front(command);
}

void Binding::handle_event(sf::Event e)
{
	auto sfml_event = static_cast<Event_type>(e.type);
	for (auto event : m_events)
	{
		if (event.first == sfml_event)
		{
			switch (sfml_event)
			{
			case Event_type::KeyDown:
			case Event_type::KeyUp:
				if (event.second.m_code == e.key.code)
				{
					if (e.key.code == sf::Keyboard::Key::G)
					{
						std::cout << "g \n";
					}
					if (m_details.m_keycode != sf::Keyboard::Unknown)
					{
						m_details.m_keycode = event.second.m_code;
					}
					++c;
				}
				break;
			case Event_type::MButtonDown:
			case Event_type::MButtonUp:
				if (event.second.m_code == e.mouseButton.button)
				{
					m_details.m_mouse.x = e.mouseButton.x;
					m_details.m_mouse.y = e.mouseButton.y;
					if (m_details.m_keycode != sf::Keyboard::Unknown)
					{
						m_details.m_keycode = event.second.m_code;
					}
					++c;
				}
				break;
			case Event_type::MouseWheel:
				m_details.m_mousewheel_delta = e.mouseWheel.delta;
				++c;
				break;
			case Event_type::WindowResized:
				m_details.m_size.x = e.size.width;
				m_details.m_size.y = e.size.height;
				++c;
				break;
			case Event_type::TextEntered:
				m_details.m_text_entered = e.text.unicode;
				++c;
				break;
			}
		}
	}
}

Binding::Ptr Binding::clone()
{
	auto new_binding = Binding::create(m_name);
	new_binding->m_events = m_events;
	new_binding->m_details = m_details;
	new_binding->c = c;
	return new_binding;
}

void Eventmanager::impl::add_binding(Binding::Ptr binding)
{
	m_bindings.push_back(std::move(binding));
}

bool Eventmanager::impl::remove_binding(const std::string& name)
{
	auto binding = std::find_if(m_bindings.begin(), m_bindings.end(), [name](Binding::Ptr& b) {return b->m_name == name; });
	if (binding == m_bindings.end())
	{
		return false;
	}
	m_bindings.erase(binding);
	return true;
}

void Eventmanager::impl::handle_event(sf::Event& event)
{
	auto sfml_event = static_cast<Event_type> (event.type);
	for (auto& binding : m_bindings)
	{
		binding->handle_event(event);
	}
}

void Eventmanager::impl::update()
{
	if (!m_focus)
	{
		//return;
	}
	for (auto& binding : m_bindings)
	{
		for (auto& event : binding->m_events)
		{
			switch (event.first)
			{
			case Event_type::Keyboard:
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(event.second.m_code)))
				{
					if (binding->m_details.m_keycode != sf::Keyboard::Unknown)
					{
						binding->m_details.m_keycode = event.second.m_code;
					}
					++(binding->c);
				}
				break;
			case Event_type::Mouse:
				if (sf::Mouse::isButtonPressed(sf::Mouse::Button(event.second.m_code)))
				{
					if (binding->m_details.m_keycode != sf::Keyboard::Unknown)
					{
						binding->m_details.m_keycode = event.second.m_code;
					}
					++(binding->c);
					break;
				}
			}
			if (binding->m_events.size() == binding->c)
			{
				auto state_commands = m_commands.find(m_current_state);
				auto other_commands = m_commands.find(Game_state::All_states);
				if (state_commands != m_commands.end())
				{
					auto itr = state_commands->second.find(binding->m_name);
					if (itr != state_commands->second.end())
					{
						itr->second (binding->m_details);
					}
				}

				if (other_commands != m_commands.end())
				{
					auto itr = other_commands->second.find(binding->m_name);
					if (itr != other_commands->second.end())
					{
						itr->second(binding->m_details);
					}
				}
			}
			binding->c = 0;
			binding->m_details.clear();
		}
	}
}

void Eventmanager::impl::load_bindings()
{
	auto path = fs::current_path() / "keys.cfg";
	std::cout << path << std::endl;
	std::string delimiter = ":";
	std::ifstream bindings;
	bindings.open(path.string());
	if (!bindings.is_open())
	{
		std::cout << "Failed opening keys.cfg" << std::endl;
		return;
	}
	std::string line;
	while (std::getline(bindings, line))
	{
		if (line.substr(0, 1) == "#")
		{
			continue;
		}
		std::stringstream keystream(line);
		std::string commandname;
		keystream >> commandname;
		std::cout << "command " << commandname << " is gebonden aan ";
		auto bind = Binding::create(commandname);
		Bindings b{};
		bool has_shift{ false };
		bool has_ctrl{ false };
		bool has_alt{ false };
		Event_info info{ sf::Keyboard::Key::Unknown };
		while (!keystream.eof())
		{
			std::string keyval;
			keystream >> keyval;
			std::cout << keyval;
			if (keyval == "shift") has_shift = true;
			if (keyval == "ctrl") has_ctrl = true;
			if (keyval == "alt")has_alt = true;
			if (Keynames::contains(keyval))
			{
				info.m_code = Keynames::to_keycode(keyval);
			}
		}
		if (info.m_code == sf::Keyboard::Key::Unknown) continue;
		auto bindings = create_bindings(commandname, info, has_alt, has_ctrl, has_shift);
		for (auto& binding : bindings)
		{
			add_binding(std::move(binding));
		}



	/*		if (keyval == "shift")
			{
				Event_info e{ sf::Keyboard::Key::LShift };
				e.m_code = sf::Keyboard::Key::LShift;
				if (b.size() == 0)
				{
					b.emplace_back(Binding::create(commandname));
					b.back()->bind_event(Event_type::Keyboard, Event_info{ sf::Keyboard::Key::LShift });
					b.emplace_back(Binding::create(commandname));
					b.back()->bind_event(Event_type::Keyboard, Event_info{ sf::Keyboard::Key::RShift });
				}
				bind->bind_event(Event_type::Keyboard, Event_info{ sf::Keyboard::Key::LShift });
				bind->bind_event(Event_type::Keyboard, Event_info{ sf::Keyboard::Key::RShift });
				keystream >> keyval;
				std::cout << " " << keyval;
			}
			if (keyval == "ctrl")
			{
				bind->bind_event(Event_type::Keyboard, Event_info{ sf::Keyboard::Key::LControl });
			//	bind->bind_event(Event_type::Keyboard, Event_info{ sf::Keyboard::Key::RControl });
				keystream >> keyval;
				std::cout << " " << keyval;
			}
			if (keyval == "alt")
			{
				bind->bind_event(Event_type::Keyboard, Event_info{ sf::Keyboard::Key::LAlt });
	//			bind->bind_event(Event_type::Keyboard, Event_info{ sf::Keyboard::Key::RAlt });
				keystream >> keyval;
				std::cout << " " << keyval;
			}

			if (Keynames::contains(keyval))
			{
				bind->bind_event(Event_type::KeyDown, Event_info{ Keynames::to_keycode(keyval) });
			}
			if (Mousebutton::contains(keyval))
			{
				bind->bind_event(Event_type::MButtonDown, Event_info{ Mousebutton::to_buttoncode(keyval) });
			}*/

/*			size_t end = keyval.find(delimiter);
			if (end == std::string::npos)
			{
				bind = nullptr;
				break;
			}
			auto type = Event_types::to_eventcode(keyval.substr(start, end - start));
			auto code = keyval.substr(end + delimiter.length(), keyval.find(delimiter, end + delimiter.length()));
			Event_info info;
			switch (type)
			{
			case sf::Event::EventType::KeyPressed:
				info.m_code = Keynames::to_keycode(code);
				break;
			case sf::Event::EventType::MouseButtonPressed:
				info.m_code = Mousebutton::to_buttoncode(code);
				break;
			}
			bind->bind_event(static_cast<Event_type> (type), info);*/
	}
		std::cout << "\n";
	//	add_binding(std::move(bind));
	
	bindings.close();
}

void add_modifiers(Bindings& b, sf::Keyboard::Key left_key, sf::Keyboard::Key right_key)
{
	Bindings temp;
	for (auto& binding : b)
	{
		Binding q = *binding;
		auto copy = binding->clone();
		binding->bind_event(Event_type::Keyboard, Event_info{ left_key });
		copy->bind_event(Event_type::Keyboard, Event_info{ right_key });
		temp.push_back(std::move(copy));
	}
	for (auto itr = temp.begin(); itr < temp.end(); ++itr)
	{
		b.emplace_back(std::move(*itr));
	}
//	std::copy(temp.begin(), temp.end(), b.end());*/
}

Bindings Eventmanager::impl::create_bindings(std::string& command_name, Event_info i, bool has_alt, bool has_ctrl, bool has_shift)
{
	Bindings result;
	result.emplace_back(Binding::create(command_name));
	result.back()->bind_event(Event_type::KeyDown, i);
	if (has_alt)
	{
		add_modifiers(result, sf::Keyboard::Key::LAlt, sf::Keyboard::Key::RAlt);
	}
	if (has_ctrl)
	{
		add_modifiers(result, sf::Keyboard::Key::LControl, sf::Keyboard::Key::RControl);
	}
	if (has_shift)
	{
		add_modifiers(result, sf::Keyboard::Key::LShift, sf::Keyboard::Key::RShift);
	}
	return result;
/*	Bindings bindings{};
	bindings.emplace_back(Binding::create(command_name));
	bindings.back()->bind_event(Event_type::KeyDown, i);
	if (has_alt)
	{
		for (auto& binding : bindings)
		{
			Binding bind{ *binding };
			binding->bind_event(Event_type::Keyboard, Event_info{ sf::Keyboard::Key::LAlt });
			bind.bind_event(Event_type::Keyboard, Event_info{ sf::Keyboard::Key::RAlt });
			bindings.push_back(Binding::Ptr{ &bind });
		}
	}*/
}

