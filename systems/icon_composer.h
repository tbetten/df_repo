#pragma once
#include "system.h"

namespace ecs
{
	template <class T>
	struct Component;
	class Entity_manager;
}

struct Drawable;

namespace systems
{
	class Icon_composer : public ecs::S_base
	{
	public:
		Icon_composer(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m);
		void update(sf::Int64 dt) override;
		void setup_events() override;
//		Dispatcher& get_event(const std::string& event) override;
	private:
		ecs::Entity_manager* m_entity_mgr{ nullptr };
		ecs::Component<Drawable>* m_drawable{ nullptr };
		messaging::Messenger* m_messenger;

		void compose_icon(std::any val);
	};
}