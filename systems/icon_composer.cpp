#include "icon_composer.h"
#include "components.h"
#include "drawable.h"
#include "facing.h"
#include "ecs.h"

#include <iostream>

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
	}

	void Icon_composer::update(sf::Int64 dt)
	{}

	void Icon_composer::compose_icon(std::any val)
	{
		//std::cout << "icon composer\n";
		auto entity = std::any_cast<ecs::Entity_id> (val);
		auto drawable_index = m_entity_mgr->get_index(ecs::Component_type::Drawable, entity);
		if (!drawable_index) return;
		auto& drawable = m_drawable->m_data[drawable_index.value()];
		auto& target = drawable.composed_icon;
		target->clear(sf::Color::Transparent);
		for (auto& part : drawable.locations)
		{
			auto texture = drawable.icon_parts.at(part).texture;
			if (texture) target->draw(drawable.icon_parts.at(part).sprite);
		}

/*		target->draw(sf::Sprite{*drawable.texture});
		if (m_entity_mgr->has_component(entity, ecs::Component_type::Facing))
		{
			auto facing = m_entity_mgr->get_data<ecs::Component<Facing>>(ecs::Component_type::Facing, entity);
			target->draw(facing->facing_indicator);
		}*/
		target->display();
	}
}