#pragma once
#include "system.h"
#include "eventmanager.h"
#include "shared_context.h"

enum class Direction { North, Northeast, Southeast, South, Southwest, Northwest };

class Controller : public System_base//, public Actor
{
public:
	using Ptr = std::unique_ptr<Controller>;
	static Ptr create (System_manager* mgr) { return std::make_unique<Controller> (mgr); }
	Controller (System_manager* mgr);
	void setup_events () override;
	void update (int dt) override;
	Dispatcher& get_event (std::string event) override;
	void set_current (Entity_id current) { m_current_entity = current; }
	void move (Direction d);
private:
	Entity_id m_current_entity;
	const std::vector<std::string> m_eventnames;
	std::unordered_map<std::string, Dispatcher> m_dispatchers;
};