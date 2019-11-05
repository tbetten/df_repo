#pragma once
#include "system.h"
#include <SFML/System.hpp>

struct Drawable;
struct Animation;

namespace ecs
{
	template <class T>
	struct Component;
	class Entity_manager;
}

namespace systems
{
	class Animation_system : public ecs::S_base
	{
	public:
		Animation_system(ecs::System_type type, ecs::System_manager* mgr);
		void update(sf::Int64 dt) override;
		void setup_events() override;
	//	Dispatcher& get_event(const std::string& event) override;
	private:
		int m_i;
		ecs::Component<Drawable>* m_drawable;
		ecs::Component<Animation>* m_animation;
		ecs::Entity_manager* m_mgr;
	};
}