/*#include "stdafx.h"
#include "attribute_system.h"
#include "ecs_base.h"
#include "attribute_comp.h"

#include <algorithm>
#include <iostream>

void Attribute_system::setup_events ()
{
	Dispatcher bought;
	m_dispatchers["attribute_bought"] = bought;
	Dispatcher changed;
	m_dispatchers["attribute_changed"] = changed;
	m_system_manager->register_events (System::Attribute, m_eventnames);
	auto x = m_system_manager->find_event ("attribute_bought");
	auto y = m_system_manager->get_event (System::Attribute, "attribute_bought");
	m_system_manager->get_event (System::Attribute, "attribute_bought").bind ([this](auto val) {on_attrib_bought (val); });

}

void Attribute_system::update (int dt)
{
	auto data = m_component->m_data;
	for (auto entity : data)
	{
		for (auto& attrib : entity.second)
		{
			int index{ 0 };
			for (auto& bonus : attrib.second->bonuses)
			{
				if (bonus.timed)
				{
					bonus.remaining_time -= dt;
					if (bonus.remaining_time <= 0)
					{
						attrib.second->bonuses.erase (attrib.second->bonuses.begin () + index);
						--index;
					}
				}
				++index;
			}
		}

	}
}

void Attribute_system::reset (Entity_id entity)
{
	if (has_entity (entity))
	{

		auto& attribs = m_component->m_data[entity];
		for (auto& attrib : attribs)
		{
			attrib.second->bought = 0;
			attrib.second->base = 1000;
			attrib.second->units_per_point = 10;
		}
		attribs[Attribute::BL]->base = 2000;
		attribs[Attribute::IQ]->units_per_point = 5;
		attribs[Attribute::DX]->units_per_point = 5;
		attribs[Attribute::HP]->units_per_point = 50;
		attribs[Attribute::Will]->units_per_point = 20;
		attribs[Attribute::Per]->units_per_point = 20;
		attribs[Attribute::FP]->units_per_point = 30;
	}
}

void Attribute_system::spend_points (Entity_id entity, Attribute attrib, int amount)
{
	if (m_component->m_data.count(entity))
	auto attribs = m_component->m_data[entity];
	auto units = m_component->m_data[entity][attrib]->units_per_point * amount;
	m_component->m_data[entity][attrib]->bought += units;
	auto payload = std::make_shared<Attrib_payload> ();
	payload->entity = entity;
	payload->units = units;
	payload->attribute = attrib;
	m_dispatchers.at ("attribute_bought").notify (payload);
	m_dispatchers.at ("attribute_changed").notify (payload);
}

int Attribute_system::get_value (Entity_id entity, Attribute attrib, Value_type value)
{
	if (has_entity (entity))
	{
		auto& data = m_component->m_data[entity][attrib];
	//	auto& x = attrib_data[attrib];
	//	auto& data = *comp->m_attributes[attrib];
		switch (value)
		{
		case Value_type::Base:
			return data->base;
			break;
		case Value_type::Bought:
			return data->bought;
			break;
		case Value_type::Natural:
			return data->base + data->received + data->bought;
		case Value_type::Bonus:
			return calculate_bonus (*data);
			break;
		case Value_type::Effective:
			return data->base + data->bought + calculate_bonus (*data);
			break;
		default:
			break;
		}
	}
}

int Attribute_system::calculate_bonus (const Attribute_data & data) const
{
	int direct{ 0 };
	int perc{ 0 };
	for (auto& bonus : data.bonuses)
	{
		switch (bonus.type)
		{
		case Bonus::Type::Direct:
			direct += bonus.amount;
			break;
		case Bonus::Type::Percent:
			perc += bonus.amount;
			break;
		default:
			break;
		}
	}
	return static_cast<int> (direct + (1 + (perc / 100.0)) * (data.base + data.bought));
}

Dispatcher& Attribute_system::get_event (std::string event)
{
	return m_dispatchers[event];
}

std::string format_units (int units)
{
	std::ostringstream ss;
	ss.precision (2);
	ss << std::showpos << units / 100.0f;
	return ss.str ();
}

void Attribute_system::on_attrib_bought (std::any value)  // add notification of attribute changed event
{
	auto payload = std::any_cast<Attrib_payload>(value);
	//auto payload = dynamic_cast<Attrib_payload*>(value.get ());
	auto attrib = payload.attribute;
	int units = payload.units;
	auto entity = payload.entity;
	switch (attrib)
	{
	case Attribute::ST:

		m_component->m_data[entity][Attribute::HP]->received += payload.units;
		m_component->m_data[entity][Attribute::HP]->notes.emplace_back (format_units (payload.units) + " points from bought ST");
		{
			int strength = m_component->get_natural(entity, Attribute::ST);
			int new_value = (strength * strength) / 500;
			int old = m_component->get_natural (entity, Attribute::BL);
			m_component->m_data[entity][Attribute::BL]->received += new_value - old ;
		}
		
		break;
	case Attribute::DX:
		break;
	case Attribute::IQ:
		m_component->m_data[entity][Attribute::Will]->received += payload.units;
		m_component->m_data[entity][Attribute::Per]->received += payload.units;
		m_component->m_data[entity][Attribute::Will]->notes.emplace_back (format_units (payload.units) + "points from bought IQ");
		m_component->m_data[entity][Attribute::Per]->notes.emplace_back (format_units (payload.units) + "points from bought IQ");
		break;
	case Attribute::HT:
		m_component->m_data[entity][Attribute::FP]->received += payload.units;
		m_component->m_data[entity][Attribute::FP]->notes.emplace_back (format_units (payload.units) + "points from bought HT");
	default:
		break;
	}

}*/