#include "attribute_system.h"
#include "attributes.h"
#include "character.h"
#include "components.h"
#include "ecs.h"
#include "shared_context.h"

namespace systems
{
	Attribute_system::Attribute_system(ecs::System_type system, ecs::System_manager* mgr) : S_base{ system, mgr }, m_attribs{ nullptr }, m_em{ nullptr }
	{
		ecs::Bitmask b;
		b.set(static_cast<int> (ecs::Component_type::Attributes));
		b.set(static_cast<int>(ecs::Component_type::Character));
		m_requirements.push_back(b);
		m_em = m_system_manager->get_context()->m_entity_manager;
		m_attribs = m_em->get_component<ecs::Component<Attributes>>(ecs::Component_type::Attributes);
		m_char = m_em->get_component<ecs::Component<Character>>(ecs::Component_type::Character);
	}

	void Attribute_system::setup_events()
	{
		// no events
	}

/*	Dispatcher& Attribute_system::get_event(const std::string& event)
	{
		throw Messaging_exception("don't have events");
	}*/

	void Attribute_system::update(sf::Int64 time)
	{
		// no real time effects
	}

	bool Attribute_system::check_cp(int cp, ecs::Entity_id entity)
	{
		auto character_index = m_em->get_index(ecs::Component_type::Character, entity);
		auto& current_cp = m_char->m_data[*character_index].character_points;
		return static_cast<int> (current_cp) > cp;
	}

	void Attribute_system::pay_cp(int cp, ecs::Entity_id entity)
	{
		auto character_index = m_em->get_index(ecs::Component_type::Character, entity);
		auto& current_cp = m_char->m_data[*character_index].character_points;
		current_cp -= cp;
	}

	void Attribute_system::buy_attribute(Attribute a, ecs::Entity_id entity, int cp)
	{
		if (check_cp(cp, entity))
		{
			auto attrib_index = m_em->get_index(ecs::Component_type::Attributes, entity);
			auto attrib_vec = m_attribs->m_data[*attrib_index].data;
			int units_per_point = attrib_vec[static_cast<int>(a)].units_per_point;
			auto units = units_per_point * cp;
			buy_units(a, entity, units);

		}
		
	}

	void Attribute_system::buy_units(Attribute a, ecs::Entity_id entity, int units)
	{
		auto attrib_index = m_em->get_index(ecs::Component_type::Attributes, entity);
		auto& attrib_vec = m_attribs->m_data[*attrib_index].data;
		int units_per_point = m_attribs->m_data[*attrib_index].data[static_cast<int>(a)].units_per_point;
		auto cp = units / static_cast<int>(units_per_point);
		switch (a)
		{
		case Attribute::ST:
			attrib_vec[static_cast<int>(Attribute::HP)].base += units;
			break;
		case Attribute::IQ:
			attrib_vec[static_cast<int>(Attribute::Will)].base += units;
			attrib_vec[static_cast<int>(Attribute::Per)].base += units;
			break;
		case Attribute::HT:
			attrib_vec[static_cast<int>(Attribute::FP)].base += units;
		}
		attrib_vec[static_cast<int>(a)].bought += units;
		//	m_attribs->m_data[*attrib_index].data[static_cast<int>(a)].bought += units;
		pay_cp(cp, entity);
	}
}