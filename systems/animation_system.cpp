#include "animation_system.h"
#include "animation.h"
#include "drawable.h"
#include "components.h"
#include "ecs.h"
#include "shared_context.h"

namespace systems
{
	Animation_system::Animation_system(ecs::System_type type, ecs::System_manager* mgr) : S_base(type, mgr), m_drawable{ nullptr }, m_animation{ nullptr }
	{
		ecs::Bitmask b;
		b.set(static_cast<int>(ecs::Component_type::Animation));
		b.set(static_cast<int>(ecs::Component_type::Drawable));
		m_requirements.push_back(b);
	//	auto m_mgr = mgr->get_context()->m_entity_manager;
	//	m_drawable = m_mgr->get_component<ecs::Component<Drawable>>(ecs::Component_type::Drawable);
	//	m_animation = m_mgr->get_component<ecs::Component<Animation>>(ecs::Component_type::Animation);
	}

	void Animation_system::setup_events()
	{
		// no events

	}

	Dispatcher& Animation_system::get_event(const std::string& event)
	{
		throw "don't have events";
	}

	void Animation_system::update(sf::Int64 dt)
	{
		auto delay = dt;// / 10.0f;
		if (m_drawable == nullptr)
		{
			m_mgr = m_system_manager->get_context()->m_entity_manager;
			m_drawable = m_mgr->get_component<ecs::Component<Drawable>>(ecs::Component_type::Drawable);
		}
		if (m_animation == nullptr)
		{
			m_animation = m_mgr->get_component<ecs::Component<Animation>>(ecs::Component_type::Animation);
		}
		for (auto entity : m_entities)
		{
			auto anim_index = m_mgr->get_index(ecs::Component_type::Animation, entity);
			auto drawable_index = m_mgr->get_index(ecs::Component_type::Drawable, entity);
			auto drawable = &m_drawable->m_data[*drawable_index];
			auto animation = &m_animation->m_data[*anim_index];
			auto duration = animation->frames[animation->current_frame].duration;
			if (animation->time_in_frame + delay > duration)
			{
				animation->time_in_frame = animation->time_in_frame + delay - duration;
				if (animation->current_frame == animation->frames.size() - 1)
				{
					animation->current_frame = 0;
				}
				else
				{
					animation->current_frame++;
				}
				auto rect1 = drawable->sprite.getTextureRect();
				auto rect2 = animation->frames[animation->current_frame].frame_rect;
				drawable->sprite.setTextureRect(animation->frames[animation->current_frame].frame_rect);
			}
			else
			{
				animation->time_in_frame += delay;
			}
		}
	}
}