#pragma once
#include "system.h"
#include <SFML/Window.hpp>
#include <any>

namespace systems
{
	class Cursor_system : public ecs::S_base, private messaging::Sender
	{
	public:
		Cursor_system (ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* messenger);
		void update (sf::Int64 dt) override;
		void setup_events () override;

		void set_window (sf::Window* win);
		
	private:
		void change_cursor (std::any val);

		sf::Window* m_win;
		sf::Cursor m_cursor;
	};
}