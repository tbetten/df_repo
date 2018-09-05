#include "stdafx.h"
#include "ecs_base.h"
#include "item.h"
#include "db.h"

#include <iostream>
#include <filesystem>

Entity_manager::Entity_manager() : m_max_id{ 0 }
{
	add_component_type<item::Item_shared>(Component_type::Item_shared);
	add_component_type<item::Projectile>(Component_type::Projectile);
	load_templates();
	//add_component_type<
}

Entity_id Entity_manager::add_entity(const Bitmask mask, bool fill_default)
{
	auto entity = m_max_id;
	Bitmask new_mask;
	fill_default ? new_mask.reset() : new_mask = mask;
	Entity_context context;
	context.m_entity_id = entity;
	context.m_mgr = this;
	Entity_data d;
	d.m_component_index = new_mask;
	d.m_components = Component_container();
	d.m_context = context;
	if (!m_entities.emplace(entity, d).second)
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

Entity_data Entity_manager::create_entity(Entity_id id, const Bitmask mask, bool fill_default)
{
	Entity_context context;
	context.m_entity_id = id;
	context.m_mgr = this;
	Entity_data entity;
	entity.m_component_index = mask;
	entity.m_context = context;
	entity.m_components = Component_container();
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
	return entity;
}

bool Entity_manager::remove_entity(const Entity_id id)
{
	return m_entities.erase(id);
}

namespace fs = std::filesystem;
db::db_connection_ptr init_database()
{
	fs::path dbasepath;
	try
	{
		dbasepath = fs::canonical(fs::current_path() / "../db/dfdata.db");
	}
	catch (fs::filesystem_error& e)
	{
		std::cout << e.what() << std::endl;
	}
	if (!fs::is_regular_file(dbasepath))
	{
		std::cout << "database not found at path " << dbasepath.string() << std::endl;
		return nullptr;
	}
	return db::db_connection::create(dbasepath.string());
}

void Entity_manager::load_templates()
{
	std::string entity_sql = "select key, c_index from entity";
	auto database = init_database();
	auto entity_stmt = database->prepare(entity_sql);
	auto result = entity_stmt->fetch_table();
	for (auto& row : result)
	{
		auto entity_key = row["key"].string_value;
		std::string component_index = row["c_index"].string_value;
		std::cout << entity_key << "\t" << component_index << std::endl;
		//Bitmask index( "00011000000000000000000000000000" );
		std::reverse(component_index.begin(), component_index.end());
		Bitmask index(component_index);
		std::cout << index.to_string() << std::endl;
		auto entity_data = create_entity(0, index, true);
		for (auto component : entity_data.m_components)
		{
			component.second->load_from_db(database.get(), entity_key);
		}
		m_templates[entity_key] = entity_data;
	}

/*	Ecs_db db{};
	auto entities = db.load_entities();
	for (auto entity : entities)
	{
		std::cout << "a " << entity << std::endl;
		db.load_components(entity);
	}*/

	// om te testen, eigenlijk uit db laden
/*	auto shared = item::Item_shared::create("Arrow", "Standard arrow", 2, 0.1f);
	
	auto proj = item::Projectile::create(item::Projectile::Point::Default);
	Template_id id = "ARROW_01";
	Component_container cc;
	Bitmask m;
	m.set(to_number(Component_type::Item_shared));
	m.set(to_number(Component_type::Projectile));
	cc[Component_type::Item_shared] = shared;
	cc[Component_type::Projectile] = proj;
	Entity_data ed;
	ed.m_components = cc;
	ed.m_component_index = m;
	m_templates[id] = std::move(ed);*/
}

Entity_id Entity_manager::spawn_from_template(const Template_id& t_id)
{
	auto mask = m_templates[t_id].m_component_index;
	auto ent = add_entity(mask);
	m_entities[ent].m_component_index = mask;
	auto components = m_templates[t_id].m_components;
	auto target = m_entities[ent].m_components.begin();

	for (auto&[component_type, component_ptr] : components)
	{
		if (component_ptr->is_mutable())   // if the component is mutable, the entity needs a private copy
		{
			m_entities[ent].m_components[component_type] = component_ptr->clone();
		}
		else    // if the component is not mutable, just point into the template
		{
			m_entities[ent].m_components[component_type] = component_ptr;
		}
	}
	return ent;
}

bool Entity_manager::add_component(const Entity_id entity, const Component_type component)
{
	auto ent_itr = m_entities.find(entity);
	if (ent_itr == m_entities.end()) { return false; }
	if (ent_itr->second.m_component_index.test(to_number(component))) { return false; }
	auto cf_itr = m_cfactory.find(component);
	if (cf_itr == m_cfactory.end()) { return false; }
	auto cmp = cf_itr->second();
	ent_itr->second.m_components[component] = cmp;
	ent_itr->second.m_component_index.set(to_number(component));
//	m_systems->entity_modified(entity, itr->second.first);
	return true;
}

bool Entity_manager::add_component(Entity_data& entity, const Component_type component)
{
	auto cf_itr = m_cfactory.find(component);
	if (cf_itr == m_cfactory.end()) { return false; }
	auto cmp = cf_itr->second();
	entity.m_components.emplace(std::make_pair(component, cmp));
//	entity.m_components[component] = cmp;
	return true;
}

bool Entity_manager::remove_component(const Entity_id entity, const Component_type component)
{
	auto ent_itr = m_entities.find(entity);
	if (ent_itr == m_entities.end()) { return false; }
	if (!ent_itr->second.m_component_index.test(to_number (component))) { return false; }
	auto& container = ent_itr->second.m_components;
	auto cmp_itr = container.find(component);
	if (cmp_itr == container.end()) { return false; }
	container.erase(cmp_itr);
	ent_itr->second.m_component_index.reset(to_number(component));
	//m_systems->entity_modified(entity, itr->second.first);
	return true;
}

bool Entity_manager::has_component(const Entity_id entity, const Component_type component)
{
	auto ent_itr = m_entities.find(entity);
	if (ent_itr == m_entities.end()) { return false; }
	return ent_itr->second.m_component_index[to_number(component)];
}