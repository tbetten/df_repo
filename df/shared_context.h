#pragma once

class Window;
class Eventmanager;
class State_manager;
struct Cache;
class Entity_manager;
class System_manager;

struct Shared_context
{
	Shared_context () : m_wind{ nullptr }, m_event_manager{ nullptr }, m_state_manager{ nullptr }, m_cache{ nullptr }, m_entity_manager{ nullptr }, m_system_manager{ nullptr } {}

	Window * m_wind;
	Eventmanager* m_event_manager;
	State_manager* m_state_manager;
	Cache* m_cache;
	Entity_manager* m_entity_manager;
	System_manager* m_system_manager;
};