/*#pragma once
#include <sstream>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <type_traits>

#include "db.h"
#include "ecs_types_old.h"

class Component_base
{
public:
	using Ptr = std::shared_ptr<Component_base>;
//	Ptr create(const Component_type type) { return std::make_shared<Component_base>(type); }
	Component_base() = default;

	virtual ~Component_base() = default;
	virtual void add_entity (Entity_id entity) = 0;
	virtual bool has_entity (Entity_id entity) = 0;
	virtual void load_from_db(db::db_connection* db, const std::string& key) = 0;
};

class Entity_manager;

using Template_id = std::string;
using Component_container = std::unordered_map<Component, Component_base::Ptr>;
//using Entity_data = std::pair<Bitmask, Component_container>;

using Component_factory = std::unordered_map<Component, std::function<Component_base::Ptr(void)>>;

struct Not_found_exception : public std::runtime_error
{
	Not_found_exception(const std::string& message) : std::runtime_error(message) {}
};

template <typename T>
inline std::string to_string(T id)
{
	if (!std::is_enum<T>::value)
	{
		throw std::invalid_argument("only enums allowed!");
	}
	return std::to_string(static_cast<int>(id));
}

class System_manager;

using Template_id = std::string;

class Entity_manager
{
public:
	explicit Entity_manager (System_manager* systems);
	Entity_id add_entity (const Bitmask mask);
	bool remove_entity (const Entity_id id);

	bool add_component_to_entity (const Entity_id entity, const Component component);

	template <typename T>
	std::shared_ptr<T> get_component (const Component component)
	{
		if (m_components.count (component) > 0)
		{
			return std::dynamic_pointer_cast<T>(m_components[component]);
		}
		else
		{
			return nullptr;
		}
	}
	bool remove_component_from_entity (const Entity_id entity, const Component component);
	inline bool has_component (const Entity_id entity, const Component component) const;
	inline bool has_entity (const Entity_id entity) const;
private:
	template <typename T>
	Component_base::Ptr create_component ()
	{
		return T::create ();
	}
	inline Entity_id generate_entity_id ();

	std::map<Component, Component_base::Ptr> m_components;
	std::map<Entity_id, Bitmask> m_index;
	std::map<Template_id, Bitmask> m_template_index;
	System_manager* m_systems;
	std::vector<Entity_id> m_recycle_bin;
	Entity_id m_max_entity_id = 0;
};
/*
class Entity_manager
{
public:
	explicit Entity_manager(System_manager* systems);

	Entity_id add_entity(const Bitmask mask, bool fill_default = false);
	bool remove_entity(const Entity_id id);
	Entity_id spawn_from_template(const Template_id& template_id);

	bool add_component(const Entity_id entity, const Component component);
	bool add_component(Entity_data& entity, const Component component);

	template <class T>
	std::shared_ptr<T> get_component(const Entity_id entity, const Component component)
	{
		auto itr = m_entities.find(entity);
		if (itr == m_entities.end()) { return nullptr; }  // not found
		if (!itr->second.m_component_index.test(to_number (component))) { return nullptr; }  // entity doesn't have this component
		auto& container = itr->second.m_components;
		auto cmp_itr = container.find(component);
		auto comp_ptr = cmp_itr->second;
		return (cmp_itr != container.end() ? std::dynamic_pointer_cast<T>(cmp_itr->second) : nullptr);
	}
	bool remove_component(const Entity_id entity, const Component component);
	bool has_component(const Entity_id entity, const Component component);
private:
	void load_templates();
	Entity_data create_entity(Entity_id id, const Bitmask mask, bool fill_default);

	template <class T>
	void add_component_type(const Component id)
	{
		m_cfactory[id] = []()->Component_base::Ptr {return T::create(); };
	}

	unsigned int m_max_id;
	Entity_container m_entities;
	Component_factory m_cfactory;
	Template_container m_templates;
	System_manager* m_systems;
};*/

