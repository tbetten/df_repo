#pragma once

#include "system.h"
#include "component.h"
#include "messaging.h"
#include "SFML/Graphics.hpp"
#include <SFML/System.hpp>

struct Position;
struct Drawable;

namespace systems
{
	class Renderer : public ecs::S_base, private messaging::Sender
	{
	public:
		Renderer (ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m);
		void update (sf::Int64 dt) override;
		void setup_events () override;
//		Dispatcher& get_event (const std::string& event) override;
		void render (sf::RenderWindow* win);
	private:
		ecs::Component<Position>* m_position = nullptr;
		ecs::Component<Drawable>* m_drawable = nullptr;
		ecs::Entity_id m_current_entity = 0;
	};
}