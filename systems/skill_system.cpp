#include "skill_system.h"
#include "components.h"
#include "ecs.h"
#include "buy_skill_payload.h"
#include "shared_context.h"
#include "skill_comp.h"
#include "Skill.h"

#include <algorithm>
#include <array>

namespace systems
{
	Skill_system::Skill_system (ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m) : S_base { type, mgr }, messaging::Sender { m }
	{
		ecs::Bitmask b;
		b.set (static_cast<int>(ecs::Component_type::Skill));
		m_requirements.push_back (b);
		m_mgr = m_system_manager->get_context ()->m_entity_manager;
		m_skills = skill::load_skills (m_system_manager->get_context()->m_database_path);
	}

	void Skill_system::setup_events ()
	{
		m_messenger->bind ("buy_skill", [this] (std::any val){buy_skill (val); });
	}

	void Skill_system::update (sf::Int64 dt)
	{

	}



	constexpr std::array<int, 5> skill_progression { 0, 100, 50, 50, 25 };
	constexpr std::array<int, 5> start_levels { 0, 0, -1, -2, -3 };
	//constexpr std::array<std::pair<skill::Skill::Difficulty, int>, 4> start_levels { { {skill::Skill::Difficulty::Easy, 0}, {skill::Skill::Difficulty::Average, -1}, {skill::Skill::Difficulty::Hard, -2},{skill::Skill::Difficulty::Very_hard, -3} } };

	int calculate_level (int points)
	{
		int level { 0 };
		int p { points };
		for (int i = 0; i < 4; i++)
		{
			if (p-- == 0) break;
			level += skill_progression [i];
		}
		if (points >= 4)
		{
			level += skill_progression [4] * (points - 4);
		}
		return level;
	}

	void Skill_system::buy_skill (std::any val)
	{
		auto payload = std::any_cast<Buy_skill_payload>(val);
		auto& [entity, key, points] = payload;
		auto skill_itr = std::ranges::find_if (m_skills, [key] (const skill::Skill& s){return s.key == key; });
		if (skill_itr == std::cend (m_skills))
		{
			throw "unknown skill " + key;
		}
		auto diff = skill_itr->difficulty;

		auto skill_comp = m_mgr->get_data<ecs::Component<Skill_comp>> (ecs::Component_type::Skill, entity);
		auto& i = skill_comp->m_skills.front ();
		auto itr = std::ranges::find_if (skill_comp->m_skills, [&key] (const Skill_rec& s){return s.key == key; });
		if (itr == std::cend (skill_comp->m_skills))
		{
			skill_comp->m_skills.emplace_back (key, 0, 0);
			i = skill_comp->m_skills.back ();
		}
		else
		{
			i = *itr;
		}
		auto level = calculate_level (i.points_spent + points);
		i.points_spent += points;
		auto start_level = start_levels [static_cast<int>(diff)];
		i.level = level + start_level * 100;
	}

}