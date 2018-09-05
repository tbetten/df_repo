#pragma once

class Window;
class Eventmanager;
class State_manager;
struct Cache;
struct Shared_context
{
	Shared_context() : m_wind{ nullptr }, m_event_manager{ nullptr }, m_state_manager{ nullptr }, m_cache{ nullptr } {}

	Window * m_wind;
	Eventmanager* m_event_manager;
	State_manager* m_state_manager;
	Cache* m_cache;
};