#include <algorithm>

#include "system.h"

bool System_base::add_entity(const Entity_id entity)
{
	if (has_entity(entity)) return false;
	m_entities.emplace_back(entity);
	return true;
}

bool System_base::has_entity(const Entity_id entity) const
{
	return std::find(m_entities.cbegin(), m_entities.cend(), entity) != m_entities.cend();
}

bool System_base::remove_entity(const Entity_id entity)
{
	auto ent = std::find_if(m_entities.begin(), m_entities.end(), [entity](const Entity_id& id) {return id == entity; });
	if (ent == m_entities.end()) return false;
	m_entities.erase(ent);
	return true;
}

bool System_base::fits_requirements(const Bitmask mask) const
{
	return std::find_if(m_required_components.cbegin(), m_required_components.cend(), [mask](Bitmask b) {return mask == b; }) != m_required_components.cend();
}