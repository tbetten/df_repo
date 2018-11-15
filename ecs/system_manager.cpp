#include "stdafx.h"
#include "system.h"
#include "attribute_system.h"
#include <algorithm>
#include <stdexcept>

System_manager::System_manager () : m_entity_mgr{ nullptr }
{
	m_systems[System::Attribute] = Attribute_system::create (this);
	m_systems[System::Attribute]->setup_events ();
}

void System_manager::update (int dt)
{
	std::for_each (m_systems.begin (), m_systems.end (), [dt](auto& system) {system.second->update (dt); });
}

void System_manager::entity_modified (Entity_id entity, Bitmask mask)
{
	for (auto& system_pair : m_systems)
	{
		auto& system = system_pair.second;
		if (system->fits_requirements (mask))
		{
			if (!system->has_entity (entity))
			{
				system->add_entity (entity);
			}
		}
		else
		{
			if (system->has_entity (entity))
			{
				system->remove_entity (entity);
			}
		}
	}
}

void System_manager::remove_entity (Entity_id entity)
{
	std::for_each (m_systems.begin (), m_systems.end (), [entity](auto& system_pair) {system_pair.second->remove_entity (entity); });
}

void System_manager::purge_entities ()
{
	std::for_each (m_systems.begin (), m_systems.end (), [](auto& system_pair) {system_pair.second->purge (); });
}

void System_manager::purge_systems ()
{
	m_systems.clear ();
}

void System_manager::register_events (System system_id, std::vector<std::string> events)
{
	for (auto event : events)
	{
		auto itr = m_events.find (event);
		if (itr == m_events.end ())
		{
			m_events[event] = system_id;
		}
		else
		{
			throw std::invalid_argument ("eventname " + event + " already exists");
		}
	}
}

System System_manager::find_event (std::string event)
{
	return m_events.at (event);
}

Dispatcher& System_manager::get_event (System system_id, std::string event)
{
	return m_systems[system_id]->get_event (event);
}