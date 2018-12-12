#include "stdafx.h"
#include "statemanager.h"
#include "eventmanager.h"
#include <iostream>

State_manager::State_manager(Shared_context* context) : m_shared{ context }
{}

State_manager::~State_manager()
{
	for (auto& state : m_states)
	{
		if (state.second != nullptr)
		{
			state.second->on_destroy();
		}
	}
}

void State_manager::insert_state(Game_state id, State_ptr state)
{
	m_states.emplace(id, std::move(state));
}

void State_manager::draw()
{
	if (m_state_stack.empty())
	{
		return;
	}
	if (m_states[m_state_stack.back()]->is_transparent() && m_state_stack.size() > 1)
	{
		auto r_itr = std::find_if_not(m_state_stack.crbegin(), m_state_stack.crend(), [this](Game_state id) {return m_states[id]->is_transparent(); });
		for (; r_itr != m_state_stack.crbegin(); --r_itr)
		{
			m_states[*r_itr]->draw();
		}
		m_states[*m_state_stack.crbegin()]->draw();
	}
	else
	{
		m_states[m_state_stack.back()]->draw();
	}
}

void State_manager::update(const sf::Time time)
{
	if (m_state_stack.empty())
	{
		return;
	}
	if (m_states[m_state_stack.back()]->is_transcendent() && m_state_stack.size() > 1)
	{
		auto r_itr = std::find_if_not(m_state_stack.crbegin(), m_state_stack.crend(), [this](Game_state id) {return m_states[id]->is_transcendent(); });
		for (; r_itr != m_state_stack.crbegin(); --r_itr)
		{
			m_states[*r_itr]->update(time);
		}
		m_states[*m_state_stack.crbegin()]->update(time);
	}
	else
	{
		m_states[m_state_stack.back()]->update(time);
	}
}

bool State_manager::has_state(Game_state  id) const
{
	auto state = std::find(m_state_stack.cbegin(), m_state_stack.cend(), id);
	auto removed = std::find(m_to_remove.cbegin(), m_to_remove.cend(), id);
	if (state != m_state_stack.cend() && removed == m_to_remove.cend())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void State_manager::remove(Game_state id)
{
	m_to_remove.push_back(id);
}

void State_manager::process_requests()
{
	while (m_to_remove.begin() != m_to_remove.end())
	{
		remove_state(*m_to_remove.begin());
		m_to_remove.erase(m_to_remove.begin());
	}
}


void State_manager::switch_to(Game_state id)
{
	m_shared->m_event_manager->set_current_state(id);
	auto state = std::find(m_state_stack.cbegin(), m_state_stack.cend(), id);//std::find_if(m_states.begin(), m_states.end(), [type](State_elem& e) {return e.first == type; });
	if (state != m_state_stack.end())
	{
		m_states[m_state_stack.back()]->deactivate();
		m_state_stack.erase(state);
		m_state_stack.push_back(id);
		m_states[id]->activate();
		return;
	}
	else
	{
		if (!m_state_stack.empty())
		{
			m_states[m_state_stack.back()]->deactivate();
		}
		create_state(id);
		m_states[m_state_stack.back()]->activate();
	}
}

void State_manager::create_state(Game_state id)
{
	m_state_stack.push_back(id);
	m_states[id]->on_create();
}

void State_manager::remove_state(Game_state id)
{
	auto state = std::find(m_state_stack.cbegin(), m_state_stack.cend(), id);
	if (state != m_state_stack.cend())
	{
		m_states[*state]->on_destroy();
		m_states[*state] = nullptr;
		m_state_stack.erase(state);
	}
}