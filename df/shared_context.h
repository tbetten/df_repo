#pragma once
#include <string>

class Window;
class Eventmanager;
class State_manager;
class Scheduler;
struct Maps;

namespace cache
{
	struct Cache;
}
namespace ecs
{
	class Entity_manager;
	class System_manager;
}

struct Shared_context
{
	Shared_context();
	~Shared_context();

	Window * m_wind;
	Eventmanager* m_event_manager;
	State_manager* m_state_manager;
	cache::Cache* m_cache;
	ecs::Entity_manager* m_entity_manager;
	ecs::System_manager* m_system_manager;
	std::string m_current_map;
	Scheduler* m_scheduler{ nullptr };
	std::unique_ptr<Maps> m_maps{ nullptr };
};