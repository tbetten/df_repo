#include "eventmanager.h"
#include "db.h"
#include "game_states.h"

#include <iostream>
#include <algorithm>

namespace event
{
	using namespace std::string_literals;

	Modifiers check_modifiers()
	{
		// Right alt will normally be equivalent to ctrl + alt. This is a windows feature and can be dependent on the chosen keyboard layout
		Modifiers m{ false, false, false };
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::RControl)) m.ctrl = true;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt)) m.alt = true;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) m.shift = true;
		return m;
	}

	bool is_modifier(sf::Keyboard::Key k)
	{
		return k == sf::Keyboard::LControl || k == sf::Keyboard::RControl || k == sf::Keyboard::LAlt || k == sf::Keyboard::RAlt || k == sf::Keyboard::LShift || k == sf::Keyboard::RShift;
	}

	std::optional<Event_info> Binding::match_binding(sf::Event event, Modifiers m) const
	{
		if (event.type != m_type) return std::nullopt;
		switch (event.type)
		{
		case sf::Event::EventType::KeyPressed: [[fallthrough]];
		case sf::Event::EventType::KeyReleased:
			if (is_modifier(event.key.code)) return std::nullopt;
			auto keyparams = std::get<Key>(m_params);
			if (event.key.code != keyparams.keycode) return std::nullopt;

			if (m.ctrl == keyparams.modifiers.ctrl && m.alt == keyparams.modifiers.alt && m.shift == keyparams.modifiers.shift)
			{
				return Event_info{ m_command, event.key.code };
			}
			return std::nullopt;
			break;
		case sf::Event::MouseButtonPressed: [[fallthrough]];
		case sf::Event::MouseButtonReleased:
			auto mouse_params = std::get<Mousebutton>(m_params);
			if (event.mouseButton.button != mouse_params.button) return std::nullopt;
			if (m.ctrl == mouse_params.modifiers.ctrl && m.alt == mouse_params.modifiers.alt && m.shift == mouse_params.modifiers.shift) return Event_info{ m_command, Mouse_info{event.mouseButton.button, event.mouseButton.x, event.mouseButton.y} };
			break;
		case sf::Event::Resized:

			return Event_info{ m_command, Size_info{event.size.width, event.size.height} };
		case sf::Event::Closed:
			return Event_info{ m_command, std::monostate{} };
		default:
			return std::nullopt;
		}
		return std::nullopt;
	}

	bool int2bool(int i)
	{
		return i != 0;
	}

	std::unordered_map <Game_state, std::vector<Binding>> make_bindings(db::table_t data, sf::Event::EventType event_type)
	{
		std::unordered_map <Game_state, std::vector<Binding>> result;
		for (auto row : data)
		{
			auto state = std::get<int>(row["game_state"]);
			auto code = std::get<int>(row["code"]);
			auto ctrl = int2bool(std::get<int>(row["ctrl"]));
			auto alt = int2bool(std::get<int>(row["alt"]));
			auto shift = int2bool(std::get<int>(row["shift"]));
			auto name = std::get<std::string>(row["name"]);
			switch (event_type)
			{
			case sf::Event::KeyPressed:
				//Binding b{ name, event_type, Key{static_cast<sf::Keyboard::Key>(code), ctrl, alt, shift} };
				result[static_cast<Game_state>(state)].push_back(Binding{ name, event_type, Key{static_cast<sf::Keyboard::Key>(code), ctrl, alt, shift} });
				break;
			case sf::Event::MouseButtonPressed:
				result[static_cast<Game_state>(state)].push_back(Binding{ name, event_type, Mousebutton{static_cast<sf::Mouse::Button>(code), ctrl, alt, shift} });
				break;
			}
		}
		return result;
	}

	void Event_manager::load_bindings()
	{
		try
		{
			db::DB_connection db{ "assets/database/gamedat.db" };
			auto key_bindings_stmt = db.prepare("select g.code as game_state, b.ctrl, b.alt, b.shift, k.keycode as code, b.name from key_binding b inner join keys k on k.keyname = b.keyname inner join game_states g on g.name = b.game_state;"s);
			auto table = key_bindings_stmt.fetch_table();

			auto bindings = make_bindings(table, sf::Event::KeyPressed);
			for (auto [game_state, binding_vec] : bindings)
			{
				m_bindings[game_state].insert(m_bindings[game_state].cbegin(), binding_vec.cbegin(), binding_vec.cend());
			}
			auto mouse_bindings_stmt = db.prepare("select g.code as game_state, b.name, b.ctrl, b.alt, b.shift, m.code from mouse_binding b inner join mousebutton m on m.button = b.mousebutton inner join game_states g on g.name = b.game_state;"s);
			table = mouse_bindings_stmt.fetch_table();

			bindings = make_bindings(table, sf::Event::MouseButtonPressed);
			for (auto [game_state, binding_vec] : bindings)
			{
				m_bindings[game_state].insert(m_bindings[game_state].cbegin(), binding_vec.cbegin(), binding_vec.cend());
			}
		}
		catch (db::db_exception & e)
		{
			std::cout << e.what() << "\n";
		}
		m_bindings[Game_state::All_states].push_back(Binding{ "CMD_close", sf::Event::Closed, std::monostate{} });
		m_bindings[Game_state::All_states].push_back(Binding{ "CMD_resize", sf::Event::Resized, std::monostate{} });
	}

	void Event_manager::add_command(const std::string& name, Event_manager::Callback callback)
	{
		m_commands.emplace(name, callback);
	}

	std::optional<Event_info> find_binding(sf::Event event, Modifiers m, std::vector<Binding>& bindings)
	{
		auto itr = std::find_if(std::cbegin(bindings), std::cend(bindings), [event, m](Binding b) {return b.match_binding(event, m); });
		if (itr != std::cend(bindings))
		{
			Binding x = *itr;
			return x.match_binding(event, m);
		}
		return std::nullopt;
	}

	void Event_manager::handle_event(sf::Event event) const
	{
		auto m = check_modifiers();
		if (!m_bindings.contains(m_current_state)) return;
		auto bindings = m_bindings.at(m_current_state);
		auto info = find_binding(event, m, bindings);
		bindings = m_bindings.at(Game_state::All_states);
		if (!info) info = find_binding(event, m, bindings);
		if (info)
		{
			auto ev = *info;
			auto itr = m_commands.find(ev.command);
			if (itr != std::cend(m_commands))
			{
				auto callback = (*itr).second;
				callback(ev);
			}
		}
	}
}