#pragma once
#include "system.h"
#include <unordered_map>

class Movement : public System_base
{
public:
	using Ptr = std::unique_ptr<Movement>;
	explicit Movement (System_manager* mgr);
	static Ptr create (System_manager* mgr) { return std::make_unique<Movement> (mgr); }
	void setup_events () override;
	void update (int dt) override;
	Dispatcher& get_event (std::string event) override;

	void move (std::any val);

private:
	std::unordered_map<Entity_id, int> m_move_cache;
};