#pragma once
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
#include "ecs_types.h"

class Component_base
{
public:
	using Ptr = std::shared_ptr<Component_base>;
//	Ptr create(const Component_type type) { return std::make_shared<Component_base>(type); }
	Component_base() = default;

	virtual ~Component_base() = default;
	virtual void load_from_db(db::db_connection* db, const std::string& key) = 0;
	//virtual void load_from_db(const std::string& key) = 0;
	bool is_mutable() { return m_mutable; }
	Ptr clone() const
	{
		return std::shared_ptr<Component_base>(this->clone_impl());
	}
protected:
	bool m_mutable;
private:
	virtual Component_base* clone_impl() const = 0;
};

template <typename Derived, typename Base>
class cloneable : public Base
{
public:
	virtual ~cloneable() = default;
	std::shared_ptr<Derived> clone() const
	{
		return std::shared_ptr<Derived>(static_cast<Derived*>(this->clone_impl()));
	}
private:
	virtual cloneable* clone_impl() const override
	{
		return new Derived(static_cast<const Derived&>(*this));
	}
};

class Entity_manager;

struct Entity_context
{
	Entity_manager* m_mgr;
	Entity_id m_entity_id;
};


using Template_id = std::string;
using Component_container = std::unordered_map<Component, Component_base::Ptr>;
//using Entity_data = std::pair<Bitmask, Component_container>;
struct Entity_data
{
	Bitmask m_component_index;
	Entity_context m_context;
	Component_container m_components;
};

using Entity_container = std::unordered_map<Entity_id, Entity_data>;
using Template_container = std::map<Template_id, Entity_data>;
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
};

