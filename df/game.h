#pragma once
#pragma comment (lib, "ecs.lib")

#include "window.h"
#include "shared_context.h"
#include "statemanager.h"
#include "resource_cache.h"
//#include "ecs_base.h"
//#include "system.h"
#include "ecs.h"
#include "scheduler.h"

class Game
{
public:
	Game();

	void handle_input() {}
	void update();
	void render();
	void late_update();
	Window* get_window() { return &m_window; }
	sf::Time get_elapsed() { return m_elapsed; }
	void restart_clock() { m_elapsed = m_clock.restart(); }

private:
	Shared_context m_context;
	Window m_window;
	State_manager m_statemanager;
	cache::Cache m_cache;
	sf::Clock m_clock;
	sf::Time m_elapsed;
	ecs::Entity_manager m_entity_mgr;
	ecs::System_manager m_system_mgr;
	Scheduler m_scheduler;
};