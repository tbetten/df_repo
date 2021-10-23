#pragma once
#include "system.h"
#include "Skill.h"

#include <any>

namespace ecs
{
	template <class T>
	struct Component;
	class Entity_manager;
}

namespace systems
{
	class Skill_system : public ecs::S_base, private messaging::Sender
	{
	public:
		Skill_system (ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m);
		void update (sf::Int64 dt) override;
		void setup_events () override;
	private:
		void buy_skill (std::any payload);
		ecs::Entity_manager* m_mgr { nullptr };
		std::vector<skill::Skill> m_skills;
	};
}