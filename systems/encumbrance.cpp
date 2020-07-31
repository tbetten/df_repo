#include "encumbrance.h"
#include "ecs.h"
#include "components.h"
#include "component.h"
#include "character.h"
#include "container.h"
#include "attributes.h"

#include <SFML/System.hpp>

namespace systems
{
	Encumbrance::Encumbrance(ecs::System_type type, ecs::System_manager* mgr, messaging::Messenger* msg) : S_base{ type, mgr }, messaging::Sender{ msg }
	{
		m_entity_manager = m_system_manager->get_entity_mgr();

		ecs::Bitmask b;
		b.set(static_cast<int>(ecs::Component_type::Character));
		b.set(static_cast<int>(ecs::Component_type::Container));
		m_requirements.push_back(b);
	}

	void Encumbrance::update(sf::Int64 dt) {}

	void Encumbrance::setup_events()
	{
		m_messenger->bind("encumbrance_changed", [this](std::any val) {change_encumbrance(val); });
	}

	void Encumbrance::change_encumbrance(std::any val)
	{
		ecs::Entity_id entity = std::any_cast<ecs::Entity_id>(val);
		auto character = m_entity_manager->get_data<ecs::Component<Character>>(ecs::Component_type::Character, entity);
		auto container = m_entity_manager->get_data<ecs::Component<Container>>(ecs::Component_type::Container, entity);
		auto attributes = m_entity_manager->get_data<ecs::Component<Attributes>>(ecs::Component_type::Attributes, entity);
		auto basic_lift = attributes::get_total_value(attributes->transactions, attributes::Attrib::BL);
		auto ratio = container->total_weight / basic_lift;
		if (ratio == 0) character->encumbrance = Character::Encumbrance::No;
		if (ratio == 1) character->encumbrance = Character::Encumbrance::Light;
		if (ratio == 2) character->encumbrance = Character::Encumbrance::Medium;
		if (ratio > 2 && ratio < 6) character->encumbrance = Character::Encumbrance::Heavy;
		if (ratio >= 6 && ratio < 10) character->encumbrance = Character::Encumbrance::Extra_heavy;
		auto y = 42;
	}
}