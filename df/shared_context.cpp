#include "shared_context.h"
#include "map_data.h"
//#include "window.h"
Shared_context::Shared_context() : m_wind{ nullptr }, m_event_manager{ nullptr }, m_state_manager{ nullptr }, m_cache{ nullptr }, m_entity_manager{ nullptr }, m_system_manager{ nullptr }, m_current_map{}
{
	m_maps = std::make_unique<Maps>();
}

Shared_context::~Shared_context() = default;