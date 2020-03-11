#pragma once

#include "system.h"
#include "component.h"
#include "messaging.h"
#include "SFML/Graphics.hpp"
#include <SFML/System.hpp>
#include <unordered_map>
#include <vector>

struct Position;
struct Drawable;

namespace systems
{
	class Renderer : public ecs::S_base, private messaging::Sender
	{
		using Layers = std::unordered_map<int, std::vector<ecs::Entity_id>>;
		using Maps = std::unordered_map<std::string, Layers>;
	public:
		Renderer (ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m);
		void update (sf::Int64 dt) override;
		void setup_events () override;
//		Dispatcher& get_event (const std::string& event) override;
		void render (sf::RenderWindow* win);
	private:
		void order_entities() override;
		bool compare_entities(ecs::Entity_id e1, ecs::Entity_id e2);

		ecs::Component<Position>* m_position = nullptr;
		ecs::Component<Drawable>* m_drawable = nullptr;
		ecs::Entity_id m_current_entity = 0;
		Maps m_maps;
	};
}