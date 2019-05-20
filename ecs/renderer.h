/*#pragma once

#include "system.h"

class Window;
class Renderer : public System_base
{
public:
	using Ptr = std::unique_ptr<Renderer>;
	explicit Renderer (System_manager* mgr) : System_base (System::Renderer, mgr)
	{
		Bitmask b;
		b.set (to_number (Component::Drawable));
		b.set (to_number (Component::Position));
		m_required_components.push_back (b);
	}
	static Ptr create (System_manager* mgr) { return std::make_unique<Renderer> (mgr); }
	void setup_events () override;
	void update (int dt) override;
	Dispatcher& get_event (std::string event) override;
	void render (Window* win);
private:
	std::unordered_map<std::string, Dispatcher> m_dispatchers;
};*/