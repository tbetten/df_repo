#pragma once
#include <string>
#include <variant>
#include <optional>
#include <unordered_map>
#include <vector>
#include <functional>
#include <SFML/Window.hpp>

enum class Game_state : int;

namespace event
{
	struct Modifiers
	{
		bool ctrl;
		bool alt;
		bool shift;
	};

	struct Key
	{
		sf::Keyboard::Key keycode;
		Modifiers modifiers;
	};

	struct Mousebutton
	{
		sf::Mouse::Button button;
		Modifiers modifiers;
	};

	struct Mouse_info
	{
		sf::Mouse::Button button;
		int x;
		int y;
	};

	struct Size_info
	{
		unsigned int new_width;
		unsigned int new_height;
	};

	using Event_params = std::variant<std::monostate, Key, Mousebutton>;

	struct Event_info
	{
		std::string command;
		std::variant<std::monostate, sf::Keyboard::Key, Mouse_info, Size_info> info;
	};


	class Binding
	{
	public:
		Binding(std::string command, sf::Event::EventType event_type, Event_params params) :m_command{ std::move(command) }, m_type{ event_type }, m_params{ params }{}
		std::optional<Event_info> match_binding(sf::Event event, Modifiers m) const;
	private:
		std::string m_command;
		sf::Event::EventType m_type;
		Event_params m_params;
	};

	class Event_manager
	{
	public:
		using Callback = std::function<void(Event_info)>;
		Event_manager() { load_bindings(); }
		void set_current_state(Game_state s) { m_current_state = s; }
		void add_command(const std::string& name, Callback callback);
		void handle_event(sf::Event event) const;
		void set_focus(const bool focus) { m_focus = focus; }
	private:
		void load_bindings();

		Game_state m_current_state;
		std::unordered_map<Game_state, std::vector<Binding>> m_bindings;
		std::unordered_map<std::string, Callback> m_commands;
		bool m_focus;
	};
}