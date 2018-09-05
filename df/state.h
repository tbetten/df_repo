#pragma once
#include <SFML\Window.hpp>
#include "eventmanager.h"
#include "shared_context.h"

class State_manager;

class State : public Actor
{
	friend class State_manager;
public:
	explicit State(Shared_context* context) : Actor{ context }, m_transparent{ false }, m_transcendent{ false } {}
	virtual ~State() {}

	virtual void on_create() = 0;
	virtual void on_destroy() = 0;

	virtual void activate() = 0;
	virtual void deactivate() = 0;

	virtual void update(const sf::Time& time) = 0;
	virtual void draw() = 0;

	void set_transparent(const bool transparent) { m_transparent = transparent; }
	bool is_transparent() const { return m_transparent; }
	void set_transcendent(const bool transcendent) { m_transcendent = transcendent; }
	bool is_transcendent() const { return m_transcendent; }
	State_manager* get_statemanager() const { return m_context->m_state_manager; }

protected:
	//State_manager* m_state_mgr;
	bool m_transparent;
	bool m_transcendent;
};