#pragma once
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include "game_states.h"

struct Shared_context;
class State;

using State_ptr = std::unique_ptr<State>;
using State_container = std::unordered_map<Game_state, State_ptr>;
using Type_container = std::vector<Game_state>;

class State_manager
{
public:
	explicit State_manager(Shared_context* shared);
	~State_manager();

	void insert_state(Game_state id, State_ptr state);
	void update(const sf::Time time);
	void handle_event(sf::Event& e);
	void draw();

	void process_requests();

	Shared_context* get_context() const { return m_shared; }
	bool has_state(Game_state id) const;
	void switch_to(Game_state id);
	void remove(Game_state id);
private:
	void create_state(Game_state id);
	void remove_state(Game_state id);

	State_container m_states;
	std::vector<Game_state> m_state_stack;
	Type_container m_to_remove;
	Shared_context* m_shared;
};
