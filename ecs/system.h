#pragma once
#include <vector>
#include <unordered_map>
#include <memory>

#include "ecs_types.h"
#include "messaging.h"

using Entity_list = std::vector<Entity_id>;
using Requirements = std::vector<Bitmask>;

class System_manager;
class System_base
{
public:
	using Ptr = std::unique_ptr<System_base>;
	System_base (const System type, System_manager* mgr) : m_system_type{ type }, m_system_manager{ mgr }{}
//	static Ptr create (const System system, System_manager* mgr) { std::make_unique<System_base> (system, mgr); }
	virtual ~System_base () { purge (); }

	bool add_entity (const Entity_id entity);
	bool has_entity (const Entity_id entity) const;
	bool remove_entity (const Entity_id entity);
	void purge () { m_entities.clear (); }

	System get_type () const noexcept { return m_system_type; }
	bool fits_requirements (const Bitmask reqs) const;

	virtual void update (int dt) = 0;
	virtual void setup_events () = 0;
	virtual Dispatcher& get_event (std::string event) = 0;
	//virtual void handle_event (const Entity_id entity, const Event) =0;

protected:
	System m_system_type;
	Requirements m_required_components;
	Entity_list m_entities;

	System_manager* m_system_manager;
};

using System_container = std::unordered_map<System, System_base::Ptr>;
class Entity_manager;

class System_manager
{
public:
	System_manager ();

	void set_entity_mgr (Entity_manager* mgr) { if (!m_entity_mgr) m_entity_mgr = mgr; }
	Entity_manager* get_entity_mgr () const { return m_entity_mgr; }

	template <typename T>
	T* get_system (System system)
	{
		auto system_itr = m_systems.find (system);
		return (system_itr != m_systems.end () ? dynamic_cast<T*>(system_itr->second.get ()) : nullptr);
	}
	void update (int dt);

	void entity_modified (Entity_id entity, Bitmask mask);
	void remove_entity (Entity_id entity);

	void purge_entities ();
	void purge_systems ();

	void register_events (System system_id, std::vector<std::string> events);
	System find_event (std::string event);
	Dispatcher& get_event (System system, std::string event);
	
private:
	System_container m_systems;
	Entity_manager* m_entity_mgr;
	std::unordered_map<std::string, System> m_events;
};