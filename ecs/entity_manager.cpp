#include "stdafx.h"
#include "ecs_base.h"
#include "item.h"


Entity_manager::Entity_manager() : m_max_id{ 0 }
{
	add_component_type<item::Item_shared>(Component_type::Item_shared);
	add_component_type<item::Projectile>(Component_type::Projectile);
	load_templates();
	//add_component_type<
}

int Entity_manager::add_entity(const Bitmask mask, bool fill_default)
{
	auto entity = m_max_id;
	Bitmask new_mask;
	fill_default ? new_mask.reset() : new_mask = mask;
	if (!m_entities.emplace(entity, Entity_data(new_mask, Component_container())).second)
	{
		return -1;
	}
	++m_max_id;
	if (fill_default)
	{
		for (unsigned int i = 0; i < max_components; ++i)
		{
			if (mask.test(i))
			{
				add_component(entity, to_comp_type(i));
			}
		}
	}
//	m_systems->entity_modified(entity, mask);
//	m_systens->add_event(entity, static_cast<Event_id>(Entity_event::Spawned));
	return entity;
}

bool Entity_manager::remove_entity(const Entity_id id)
{
	return m_entities.erase(id);
}

void Entity_manager::load_templates()
{
	// om te testen, eigenlijk uit db laden
	auto shared = item::Item_shared::create("Arrow", "Standard arrow", 2, 0.1f);
	
	auto proj = item::Projectile::create(item::Projectile::Point::Default);
	Template_id id = "ARROW_01";
	Component_container cc;
	Bitmask m;
	m.set(to_number(Component_type::Item_shared));
	m.set(to_number(Component_type::Projectile));
	cc[Component_type::Item_shared] = shared;
	cc[Component_type::Projectile] = proj;
	Entity_data ed = std::make_pair(m, std::move(cc));
	m_templates[id] = std::move(ed);
}

int Entity_manager::spawn_from_template(const Template_id& t_id)
{
	auto mask = m_templates[t_id].first;
	auto ent = add_entity(mask);
	m_entities[ent].first = mask;
	auto components = m_templates[t_id].second;
	auto target = m_entities[ent].second.begin();

	for (auto&[component_type, component_ptr] : components)
	{
		if (component_ptr->is_mutable())   // if the component is mutable, the entity needs a private copy
		{
			m_entities[ent].second[component_type] = component_ptr->clone();
		}
		else    // if the component is not mutable, just point into the template
		{
			m_entities[ent].second[component_type] = component_ptr;
		}
	}
	return ent;
}

bool Entity_manager::add_component(const Entity_id entity, const Component_type component)
{
	auto ent_itr = m_entities.find(entity);
	if (ent_itr == m_entities.end()) { return false; }
	if (ent_itr->second.first.test(to_number(component))) { return false; }
	auto cf_itr = m_cfactory.find(component);
	if (cf_itr == m_cfactory.end()) { return false; }
	auto cmp = cf_itr->second();
	ent_itr->second.second[component] = cmp;
	ent_itr->second.first.set(to_number(component));
//	m_systems->entity_modified(entity, itr->second.first);
	return true;
}

bool Entity_manager::remove_component(const Entity_id entity, const Component_type component)
{
	auto ent_itr = m_entities.find(entity);
	if (ent_itr == m_entities.end()) { return false; }
	if (!ent_itr->second.first.test(to_number (component))) { return false; }
	auto& container = ent_itr->second.second;
	auto cmp_itr = container.find(component);
	if (cmp_itr == container.end()) { return false; }
	container.erase(cmp_itr);
	ent_itr->second.first.reset(to_number(component));
	//m_systems->entity_modified(entity, itr->second.first);
	return true;
}

bool Entity_manager::has_component(const Entity_id entity, const Component_type component)
{
	auto ent_itr = m_entities.find(entity);
	if (ent_itr == m_entities.end()) { return false; }
	return ent_itr->second.first[to_number(component)];
}