#include "cursor_system.h"
#include "ecs.h"
#include "shared_context.h"
#include "eventmanager.h"
#include "cursor_payload.h"

#include <any>

namespace systems
{
	Cursor_system::Cursor_system (ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m) : S_base { type, mgr }, messaging::Sender { m } 
	{
		m_cursor.loadFromSystem (sf::Cursor::Type::Arrow);
	}

	void Cursor_system::setup_events () 
	{
		m_messenger->bind ("change_cursor", [this] (std::any val){change_cursor (val); });
	}

	void Cursor_system::update (sf::Int64 dt) {}

	void Cursor_system::set_window (sf::Window* win)
	{
		m_win = win;
	}

	void Cursor_system::change_cursor (std::any val)
	{
		auto payload = std::any_cast<Cursor_payload>(val);
		if (std::holds_alternative<sf::Cursor::Type> (payload.m_payload))
		{
			m_cursor.loadFromSystem (std::get<sf::Cursor::Type> (payload.m_payload));
		}
		m_win->setMouseCursor (m_cursor);
	}
}