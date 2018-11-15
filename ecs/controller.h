#pragma once
#include "system.h"

class Controller : public System_base
{
public:
	using Ptr = std::unique_ptr<Controller>;

	void setup_events () override;
	void update (int dt) override;
	Dispatcher& get_event (std::string event) override;
private:
	const std::vector<std::string> m_eventnames;
	std::unordered_map<std::string, Dispatcher> m_dispatchers;
};