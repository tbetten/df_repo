#include "icon_composer.h"
#include "components.h"
#include "drawable.h"
#include "facing.h"
#include "ecs.h"

namespace systems
{
	Icon_composer::Icon_composer(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* m) : S_base(type, mgr), m_messenger{ m }
	{
		ecs::Bitmask b;
		b.set(static_cast<int>(ecs::Component_type::Drawable));
		b.set(static_cast<int>(ecs::Component_type::Facing));
		m_requirements.push_back(b);

		m_entity_mgr = mgr->get_entity_mgr();
		m_drawable = m_entity_mgr->get_component<ecs::Component<Drawable>>(ecs::Component_type::Drawable);
	}

	void Icon_composer::setup_events()
	{
		m_messenger->bind("icon_changed", [this](auto val) {compose_icon(val); });
//		auto sys = m_system_manager->find_event("icon_changed");
//		m_system_manager->get_event(sys, "icon_changed").bind([this](auto val) {compose_icon(val); });
	}

	void Icon_composer::update(sf::Int64 dt)
	{}

/*	Dispatcher& Icon_composer::get_event(const std::string& event)
	{
		throw Messaging_exception("don't have any events");
	}*/

	void Icon_composer::compose_icon(std::any val)
	{

		auto entity = std::any_cast<ecs::Entity_id> (val);
		auto drawable_index = m_entity_mgr->get_index(ecs::Component_type::Drawable, entity);
		if (!drawable_index) return;
		auto& drawable = m_drawable->m_data[drawable_index.value()];
		auto& target = drawable.composed_icon;
		target->draw(sf::Sprite{*drawable.texture});
		if (m_entity_mgr->has_component(entity, ecs::Component_type::Facing))
		{
			auto facing = m_entity_mgr->get_data<ecs::Component<Facing>>(ecs::Component_type::Facing, entity);
			target->draw(facing->facing_indicator);
		}
		target->display();
	}
}